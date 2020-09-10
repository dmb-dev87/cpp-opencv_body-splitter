#include "GCodeWriter.h"
#include <iomanip>
#include <map>
#include <memory>
#include "Settings.h"
#include "Layer.h"
#include "iSlicable.h"
#include "PrimeTowerHandler.h"
#include "colour.h"
#include "RSPolygon.h"
#include "Surface.h"
#include "machineControl.h"
#include "PrintBed.h"
#include "SupportManager.h"

const double GCodeWriter::MAX_E_RESET = 80000.0;
#define MIN_MOVE 1e-12
#define TIME_ESTIMATE Settings::GetSingleton().getBoolValue("estimateTime") || GET_BOOL("enableGCodeSplitup")

GCodeWriter::GCodeWriter(const std::string &outputFileName) : lastToolPosition(0, 0, 0),
lastX(0), lastY(0), lastZ(0), lastFeedrate(0), lastE(0), lastR(-100), lastG(-100), lastB(-100), retracted(false), m_ptrMachine(nullptr)
{
	if (GET_BOOL("enableGCodeSplitup"))
	{
		outputFileNameSplit = outputFileName.substr(0, outputFileName.length() - 6);
		outputStream.open(outputFileNameSplit + std::to_string(filePartCounter++) + ".gcode");
	}
	else
		outputStream.open(outputFileName);
	outputStream.setf(std::ios::fixed, std::ios::floatfield);
	xOffset = (Settings::getSingleton().getDoubleValue("printXOffset"));
	yOffset = (Settings::getSingleton().getDoubleValue("printYOffset"));

	timeEstimate.reset();
	filamentUsageEstimate.reset();
	lineBuffer.clear();
	recommendPurgeExcess = false;
}

GCodeWriter::GCodeWriter(machineControl *machine) : lastToolPosition(0, 0, 0),
lastX(0), lastY(0), lastZ(0), lastFeedrate(0), lastE(0), lastR(-100), lastG(-100), lastB(-100), retracted(false), m_ptrMachine(machine)
{
	xOffset = (Settings::getSingleton().getDoubleValue("printXOffset"));
	yOffset = (Settings::getSingleton().getDoubleValue("printYOffset"));

	timeEstimate.reset();
	filamentUsageEstimate.reset();
	lineBuffer.clear();
	recommendPurgeExcess = false;
}

void GCodeWriter::startWriter() {
	recommendPurgeExcess = false;
	for (uint8_t i = 0; i < 16; ++i)
		toolRetractionState.push_back(RETRACTION_TYPE::NO_RETRACTION);
	writeStartGCode();
	writeStartTemperatures();
	if (GET_BOOL("mixerOnlyUse") == false && Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
	{
		writeMixerSpeed(GET_DOUBLE("mixerSpeed"));
	}
	if (GET_BOOL("primeToolPurgeBin"))
	{
		//Goto purge bin at start of print
		double targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") * ((double)rand() / (double)(RAND_MAX)) - xOffset;
		double targetY = GET_DOUBLE("purgeLocationY") + GET_DOUBLE("purgeHeight") / 2.0 - yOffset;
		writeComment("TravelToPurgeBin:(" + std::to_string(targetX) + "," + std::to_string(targetY) + ")");
		writeTravelXY(targetX, targetY, -1.0,true);
	}
}

GCodeWriter::~GCodeWriter()
{
	if (outputStream)
	{
		writeAllBufferedLines();

		outputStream.flush();
		outputStream.close();
	}
}

std::size_t GCodeWriter::getLayerCount() const {
	unsigned int maxLayer = 0;
	for (auto &it : slicables)
	{
		maxLayer = std::max(maxLayer, it->getLayerCount());
	}
	return maxLayer;
}

void GCodeWriter::bufferLine(const std::string &text, const std::string &comment)
{
	if (text.length() == 0 && comment.length() == 0)
		return;
	std::string bufferText = text;
	if (comment.empty() == false && GET_BOOL("addDebugComments"))
		bufferText += "; " + comment;
	if (bufferText.back() != '\n')
		bufferText += '\n';
	if (isUsingLayerLineBuffer)
	{
		layerLineBuffer.push_back(linePair(bufferText, nullptr));
	}
	else
	{
		lineBuffer.push_back(linePair(bufferText, nullptr));
	}
}

void GCodeWriter::bufferLine(const GCodeLine &line)
{
	if (isUsingLayerLineBuffer)
		layerLineBuffer.push_back(linePair("", std::make_shared<GCodeLine>(line)));
	else
		lineBuffer.push_back(linePair("", std::make_shared<GCodeLine>(line)));
	return;
}

double GCodeWriter::writeInlineColourChange(const Colour &in, double distance)
{
	double remainingDistance = writeInlineColourChange(getColourString(in), distance);
	lastColour = in;
	return remainingDistance;
}

double GCodeWriter::writeInlineColourChange(uint16_t toolID, double distance)
{
	double remainingDistance = writeInlineColourChange("T" + std::to_string((int)toolID), distance);
	lastTool = toolID;
	return remainingDistance;
}

double GCodeWriter::writeInlineColourChange(const std::string &colourCmd, double distance)
{
	std::string colourCmdText = colourCmd;
	if (GET_BOOL("addDebugComments"))
		colourCmdText += "; Inline colour chnage";
	colourCmdText += "\n";
	std::deque<linePair> *activeLineBuffer;
	if (isUsingLayerLineBuffer)
		activeLineBuffer = &layerLineBuffer;
	else
		activeLineBuffer = &lineBuffer;
	auto currentLine = activeLineBuffer->rbegin();
	while (distance > 0)
	{
		if (currentLine->second)
		{
			if ((currentLine->second->e2 - currentLine->second->e1) > distance)
			{
				//Gotta split this line and insert the colour change cmd
				GCodeLine firstHalf(lastTool,lastColour);
				GCodeLine secondHalf(lastTool, lastColour);
				currentLine->second->splitLine(distance, firstHalf, secondHalf);
				firstHalf.feedrate = currentLine->second->feedrate;
				secondHalf.feedrate = currentLine->second->feedrate;
				firstHalf.e1 = currentLine->second->e1;
				firstHalf.e2 = currentLine->second->e1 + distance;
				secondHalf.e1 = firstHalf.e2;
				secondHalf.e2 = currentLine->second->e2;

				firstHalf.comment = "Inline split first half";
				secondHalf.comment = "Inline split second half";
				auto insertPoint = activeLineBuffer->erase(currentLine.base()-1);
				insertPoint = activeLineBuffer->insert(insertPoint,
					linePair("", std::make_shared<GCodeLine>(secondHalf)));
				
				insertPoint = activeLineBuffer->insert(insertPoint,
					linePair(colourCmdText, nullptr));
				insertPoint = activeLineBuffer->insert(insertPoint,
					linePair("", std::make_shared<GCodeLine>(firstHalf)));
				
				return 0;
			}
			else if (currentLine->second->e2 - currentLine->second->e1 == distance)
			{
				// Neat it falls exactly on the line end, doubt this code will ever run
				activeLineBuffer->insert(currentLine.base()-1,
					linePair(colourCmdText, nullptr));
				return 0;
			}
			else
			{
				distance -= (currentLine->second->e2 - currentLine->second->e1);
			}
		}
		currentLine++;
		if (currentLine == activeLineBuffer->rend())
		{
			//outa buffer space
			activeLineBuffer->insert(currentLine.base(),
				std::pair<std::string, std::shared_ptr<GCodeLine>>(colourCmdText, nullptr));
			return distance;
		}
	}
	// No in line colour change needed
	activeLineBuffer->push_back(std::pair<std::string, std::shared_ptr<GCodeLine>>(colourCmdText, nullptr));
	return 0;
}

void GCodeWriter::writeAllBufferedLines()
{
	//Find colinear lines
	while (lineBuffer.empty() == false)
	{
		if (lineBuffer.front().second)
		{
			writeFormattedLine(lineBuffer.front().second);
		}
		else
		{
#ifndef _CONSOLE
			if (m_ptrMachine)
			{
				m_ptrMachine->queueLines(lineBuffer.front().first);
			}
			else
#endif
			{
				outputStream << lineBuffer.front().first;
			}
		}
		lineBuffer.pop_front();
	}
}

void GCodeWriter::writeFormattedLine(const std::shared_ptr<GCodeLine> &line)
{
	static double lastF = -1;
	static double lastEWrote = 0;
	std::stringstream output;
	std::fixed(output);
	if (line->isLineOfType(Line::LineType::ISSUE | Line::LineType::RETRACT | Line::LineType::RETRACT_WIPE))
	{
		output << std::setprecision(GCODE_PRECISION_E);
		if (line->e2 != line->e1)
		{
			if (TIME_ESTIMATE)
			{
				timeEstimate.addPrintMove(line->e2 - line->e1, line->feedrate);
				if ((line->tool == 0 || line->tool == ~0) && !GET_BOOL("rova5Compat"))
					filamentUsageEstimate.addColourUsage(line->colour, line->e2 - line->e1);
				else
					filamentUsageEstimate.addOtherTool(line->tool, line->e2 - line->e1);
			}
			
			if (line->isLineOfType(Line::LineType::RETRACT_WIPE))
				output << "G10 E" << (line->e2 - line->e1);
			else
			{
				lastEWrote += line->e2 - line->e1;
				output << "G1 E" << lastEWrote;
			}
			if (lastF != line->feedrate)
			{
				output << std::setprecision(GCODE_PRECISION_F);
				output << " F" << line->feedrate;
			}
			lastF = line->feedrate;
			//lastEWrote = line->e2;
		}
	}
	else
	{
		if (line->isLineOfType(Line::LineType::Z_MOVE))
		{
			output << std::setprecision(GCODE_PRECISION_XY);
			output << "G1 Z" << (line->pt2.z);
			if (TIME_ESTIMATE)
				timeEstimate.addPrintMove(line->pt2.z - line->pt1.z, line->feedrate);
		}
		else
		{
			output << std::setprecision(GCODE_PRECISION_XY);
			output << "G1 X" << (line->pt2.x) << " Y" << (line->pt2.y);// << " Z" << line->pt2.z;
			if (TIME_ESTIMATE)
				timeEstimate.addPrintMove(line->length(), line->feedrate);
		}
		if (line->isLineOfType(Line::LineType::TRAVEL))
		{

		}
		else
		{
			if (line->e1 != line->e2)
			{
				output << std::setprecision(GCODE_PRECISION_E);
				lastEWrote += (line->e2 - line->e1);
				output << " E" << lastEWrote;
				if (line->tool == 0 && !GET_BOOL("rova5Compat") || line->tool == 65535)
					filamentUsageEstimate.addColourUsage(line->colour, line->e2 - line->e1);
				else
					filamentUsageEstimate.addOtherTool(line->tool, line->e2 - line->e1);
				//	lastE = line->e2;
			}
		}
		if (lastF != line->feedrate)
		{
			output << std::setprecision(GCODE_PRECISION_F);
			output << " F" << line->feedrate;
			lastF = line->feedrate;
		}
	}
	if (Settings::GetSingleton().getBoolValue("addDebugComments") && line->comment.empty() == false)
		output << ";" << line->comment;
	//outputStream << "\n";
#ifndef _CONSOLE
	if (m_ptrMachine)
	{
		m_ptrMachine->queueLines(output);
	}
	else
#endif
	{
		if(output.str().length()>0)
			outputStream << output.str() << "\n";
	}

	if (lastEWrote > MAX_E_RESET && Settings::GetSingleton().getBoolValue("resetEAxis"))
	{
		//Reset E
#ifndef _CONSOLE
		if (m_ptrMachine)
			m_ptrMachine->queueLines("G92 E0");
		else
#endif
			outputStream << "G92 E0\n";
		lastEWrote = 0;
	}

}

void GCodeWriter::writeCoast()
{
	double distanceNeeded = Settings::GetSingleton().getDoubleValue("coastLengthT" + std::to_string(lastTool));
	std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>> *activeLineBuffer;
	if (isUsingLayerLineBuffer)
		activeLineBuffer = &layerLineBuffer;
	else
		activeLineBuffer = &lineBuffer;

	auto lineIT = activeLineBuffer->rbegin();
	for (; lineIT != activeLineBuffer->rend(); ++lineIT)
	{
		if (distanceNeeded <= 0.0)
			break;

		if (lineIT->second && lineIT->second->hasELength() != 0 && !lineIT->second->isLineOfType(Line::LineType::TRAVEL | Line::LineType::Z_MOVE))
		{
			if (lineIT->second->length() > distanceNeeded)
			{
				double percent = distanceNeeded / lineIT->second->length();
				//We must split the line
				GCodeLine firstHalf(lastTool, lastColour);
				GCodeLine secondHalf(lastTool, lastColour);
				lineIT->second->splitLine(distanceNeeded, firstHalf, secondHalf);
				secondHalf.e1 = secondHalf.e2 = lineIT->second->e2;
				secondHalf.addParameter(Line::LineType::COAST);
				firstHalf.e1 = lineIT->second->e1;
				firstHalf.pt1.z = secondHalf.pt1.z = lineIT->second->pt1.z;
				firstHalf.pt2.z = secondHalf.pt2.z = lineIT->second->pt2.z;
				firstHalf.e2 = (lineIT->second->e2 - lineIT->second->e1)*percent + firstHalf.e1;
				firstHalf.feedrate = secondHalf.feedrate = lineIT->second->feedrate;
				//Lines split and reformed, now insert into original buffer
				secondHalf.comment = "Coast Second half";
				firstHalf.comment = "Coast split line, not coast";
				//Lines not dividing proper
				if (firstHalf.pt2.x > 0.01 && firstHalf.pt2.y > 0.01 && secondHalf.pt2.x > 0.01 && secondHalf.pt2.y > 0.01)
				{
					auto insertIT = activeLineBuffer->erase(lineIT.base() - 1);//Base is reverse plus 1
					bufferLine(firstHalf);
					bufferLine(secondHalf);
					/*
					lineBuffer.insert(insertIT, std::pair<std::string, Line>("Line", secondHalf));
					lineBuffer.insert(insertIT, std::pair<std::string, Line>("Line", firstHalf));
					*/

					break;
				}

			}
			else
			{
				distanceNeeded -= lineIT->second->length();
				lineIT->second->addParameter(Line::LineType::COAST);
				lineIT->second->e2 = lineIT->second->e1;
				lineIT->second->comment = "Coast";
			}
		}
	}
	writeAllBufferedLines();
}

void GCodeWriter::writeStartTemperatures()
{
	if (Settings::getSingleton().getDoubleValue("bedTemp") > 0)
	{
		std::stringstream lineFormat;
		lineFormat << "M140 S";	//Heat and go
		lineFormat.precision(0);//No decimal places for temperatures
		lineFormat << Settings::getSingleton().getDoubleValue("bedTemp");
		bufferLine(lineFormat.str(),"Heat and go Bed");
	}
	//Need tools used
	std::set<uint16_t> toolsUsed;
	if (Settings::GetSingleton().getBoolValue("rova5Compat"))
	{
		auto toolColourMap = Settings::GetSingleton().getColourToolMap();
		for (auto it : toolColourMap)
		{
			toolsUsed.insert(it.second);
		}
		for (auto it : slicables)
		{
			// IF the slicable is colour, its already accounted for in the toolColourMap
			if (!it->isColour())
			{
				auto temp = it->getToolsUsed();
				toolsUsed.insert(temp.begin(), temp.end());
			}
		}
		// In rova compat mode, all tools get the colour temperature
		for (auto it : toolsUsed)
		{
			std::stringstream tempCmd; // = "M109 T" + std::to_string((int)it) + " S" + std::to_string(Settings::getSingleton().getDoubleValue("T0")) + "\n";
			tempCmd << std::setprecision(0);
			tempCmd << "M104 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T0"));
			bufferLine(tempCmd.str(),"Heat and go tool");
		}
		//Now we can do heat and wait if enabled
		if (Settings::getSingleton().getBoolValue("heatAndWait"))
		{
			for (auto it : toolsUsed)
			{
				std::stringstream tempCmd; // = "M109 T" + std::to_string((int)it) + " S" + std::to_string(Settings::getSingleton().getDoubleValue("T0")) + "\n";
				tempCmd << std::setprecision(0);
				tempCmd << "M109 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T0"));
				bufferLine(tempCmd.str(),"Heat and wait");
			}
			if (Settings::getSingleton().getDoubleValue("bedTemp") > 0)
			{
				std::stringstream lineFormat;
				lineFormat << "M190 S";	//Heat and go
				lineFormat.precision(0);//No decimal places for temperatures
				lineFormat << Settings::getSingleton().getDoubleValue("bedTemp");
				bufferLine(lineFormat.str());
			}
		}
	}
	else
	{
		for (auto it : slicables)
		{
			auto tempSet = it->getToolsUsed();
			toolsUsed.insert(tempSet.begin(), tempSet.end());
		}
		if (PrintBed::getSingleton().hasSupport())
		{
			toolsUsed.insert((uint16_t)GET_DOUBLE("mainSupportExtruder"));
			toolsUsed.insert((uint16_t)GET_DOUBLE("supportInterfaceExtruder"));
		}
		for (auto it : toolsUsed)
		{
			std::stringstream tempCmd;
			tempCmd << std::setprecision(0);
			if((int)it < 3)
				tempCmd << "M104 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T" + std::to_string((int)it)));
			else
				tempCmd << "M104 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T0"));
			bufferLine(tempCmd.str(),"Heat and go");
		}
		//Now we can do heat and wait if enabled
		if (Settings::getSingleton().getBoolValue("heatAndWait"))
		{
			for (auto it : toolsUsed)
			{
				std::stringstream tempCmd;
				tempCmd << std::setprecision(0);
				if ((int)it < 3)
					tempCmd << "M109 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T" + std::to_string((int)it)));
				else
					tempCmd << "M109 T" << std::to_string((int)it) << " S" << (Settings::getSingleton().getDoubleValue("T0"));
				bufferLine(tempCmd.str(),"Heat and wait");
			}
			if (Settings::getSingleton().getDoubleValue("bedTemp") > 0)
			{
				std::stringstream lineFormat;
				lineFormat << "M190 S";	//Heat and go
				lineFormat.precision(0);//No decimal places for temperatures
				lineFormat << Settings::getSingleton().getDoubleValue("bedTemp");
				bufferLine(lineFormat.str());
			}
		}
	}
}

void GCodeWriter::writeStartGCode()
{
	writeAllBufferedLines();
	bufferLine(Settings::GetSingleton().getStringValue("onStartGCode"));
}

void GCodeWriter::writeEndGCode()
{
	writeAllBufferedLines();
	bufferLine(Settings::GetSingleton().getStringValue("onEndGCode"));
}

void GCodeWriter::writeLayerCode(double z)
{
	if(Settings::GetSingleton().getStringValue("onBeforeZGCode").length() > 1)
		bufferLine(Settings::GetSingleton().getStringValue("onBeforeZGCode"));
	if (GET_BOOL("enableGCodeSplitup") && timeEstimate.needGCodeSplit())
	{
		writeSuspend();
		writeResume();
	}
	if (Settings::getSingleton().getBoolValue("addDebugComments"))
	{
		std::stringstream command;
		command.precision(GCODE_PRECISION_XY);
		command << "New Z=" << z + Settings::GetSingleton().getDoubleValue("layerThickness");
		bufferLine("",command.str());
	}
}

void GCodeWriter::writeLayer(unsigned int layer)
{
	isUsingLayerLineBuffer = true;
	//We have polys on each layer representing Perim+inner Perims+infill
	//Order of the islands:
	//1. Matched Colour -> next closest colour(what is this??) - LAB colour space? Cartesian distances?
	//2. Closest? or closest without crossing perimeters?
	//Earse slicables that are now too short
	for (auto it = slicables.begin(); it != slicables.end();)
	{
		if ((*it)->getLayerCount() <= layer)
			it = slicables.erase(it);
		else
			++it;
	}
	if (slicables.empty())
	{
		return;
	}
	double eStarting = lastE;
	currentLayer = layer;
	m_currentLayer = layer;
	double bottomOfLayer = slicables.front()->getLayer(layer)->getLayerZ();
	writeLayerCode(bottomOfLayer);
	double layerThickness = slicables.front()->getLayer(layer)->getThickness();
	writeZTravelTo(bottomOfLayer + layerThickness);
	if(Settings::GetSingleton().getStringValue("onAfterZGCode").length() > 1)
		bufferLine(Settings::GetSingleton().getStringValue("onAfterZGCode"));
	std::unordered_map<Colour, std::vector<PolyWithOffset>> islandsByColour;
	std::unordered_map<uint16_t, std::vector<PolyWithOffset>> islandsByTool;
	for (auto it : slicables)
	{
		auto drawablePtr = std::dynamic_pointer_cast<iDrawable>(it);
		if (drawablePtr->getTool() == iDrawable::ToolType::FULL_COLOUR)
		{
			auto currentLayer = it->getLayer(layer);
			for (auto PolyIT : currentLayer->getRSPolies())
			{
				islandsByColour[PolyIT->getColour()].push_back(PolyWithOffset(Vertex2D(it->getOffsetX(), it->getOffsetY()), PolyIT));
			}
			//Add surface RS Polys
			for (auto layerTT : currentLayer->m_Surfaces)
			{
				islandsByColour[layerTT->m_ptrSurfacePoly->m_contour->getColour()].push_back(PolyWithOffset(Vertex2D(it->getOffsetX(), it->getOffsetY()), layerTT->m_ptrSurfacePoly));
			}
		}
		else
		{
			auto currentLayer = it->getLayer(layer);
			for (auto PolyIT : currentLayer->getRSPolies())
			{
				islandsByTool[(uint16_t)drawablePtr->getTool()].push_back(PolyWithOffset(Vertex2D(it->getOffsetX(), it->getOffsetY()), PolyIT));
			}
			//Add surface RS Polys
			for (auto layerTT : currentLayer->m_Surfaces)
			{
				islandsByTool[(uint16_t)drawablePtr->getTool()].push_back(PolyWithOffset(Vertex2D(it->getOffsetX(), it->getOffsetY()), layerTT->m_ptrSurfacePoly));
			}
		}
	}
	// Need to estimate the time for the layer so we can adjust speed for auto cooling
	if (lastTool == (uint16_t)iDrawable::ToolType::FULL_COLOUR && !GET_BOOL("rova5Compat"))
	{
		writeIslandsByColour(islandsByColour, layer);
		writeIslandsByTool(islandsByTool, layer);
	}
	else
	{
		writeIslandsByTool(islandsByTool, layer);
		writeIslandsByColour(islandsByColour, layer);
	}
	//Check print time for layer
	// Autocooling
	double layerPrintTime = 0;
	double layerTotalTime = 0;
	double layerLength = 0;
	for (auto it : layerLineBuffer)
	{
		if (it.second)
		{
			if (isnan(it.second->length()))
			{

			}
			else
			{
				layerTotalTime += it.second->length() / (it.second->feedrate / 60.0);
				layerLength += it.second->length();
				// Is printing move? Not travel, not retract/issue
				if (!it.second->pt1.MatchesLocation(it.second->pt2) && it.second->e1 != it.second->e2)
				{
					layerPrintTime+= it.second->length() / (it.second->feedrate / 60.0);
				}
			}
		}
	}
	bufferLine("","Layer Total Time: " + std::to_string(layerTotalTime) + " seconds");
	bufferLine("","Layer Printing Time: " + std::to_string(layerPrintTime) + " seconds");
	bufferLine("", "Layer extrusion Amount: " + std::to_string(lastE - eStarting) + " mm");
	if (!GET_BOOL("keepFanAlwaysOn"))
	{
		isUsingLayerLineBuffer = false; // Force the fan speed to buffer at the start of the layer
		// Figure out the fan speed
		if (layerTotalTime < GET_DOUBLE("minLayerTime"))
			writeFanSpeed(GET_DOUBLE("maxFanSpeed"));
		else if (layerTotalTime > GET_DOUBLE("minLayerTimeFan"))
			writeFanSpeed(GET_DOUBLE("minFanSpeed"));
		else
		{
			writeFanSpeed(std::max(0.0, std::min(100.0, 100 - (layerTotalTime - GET_DOUBLE("minLayerTime")) / (GET_DOUBLE("minLayerTimeFan") - GET_DOUBLE("minLayerTime")))));
		}
		isUsingLayerLineBuffer = true;
	}
	else
	{
		isUsingLayerLineBuffer = false;
		writeFanSpeed(GET_DOUBLE("maxFanSpeed"));
		isUsingLayerLineBuffer = true;
	}
	if (GET_BOOL("reducePrintSpeedSmallLayers"))
	{
		if (layerTotalTime < GET_DOUBLE("minLayerTime"))
		{
			double timeToAdd = GET_DOUBLE("minLayerTime") - layerTotalTime;
			scaleBufferedLayer(layerPrintTime/(layerPrintTime + timeToAdd));
		}
	}
	if (isUsingLayerLineBuffer && layerLineBuffer.empty() == false)
	{
		lineBuffer.insert(lineBuffer.end(), layerLineBuffer.begin(), layerLineBuffer.end());
		layerLineBuffer.clear();
	}
	isUsingLayerLineBuffer = false;
}

void GCodeWriter::scaleBufferedLayer(double amt)
{
	for (auto it : layerLineBuffer)
	{
		if (it.second)
		{
			if (isnan(it.second->length()))
			{

			}
			else
			{
				// Is printing move? Not travel, not retract/issue
				if (!it.second->pt1.MatchesLocation(it.second->pt2) && it.second->e1 != it.second->e2)
				{
					it.second->feedrate = std::max(amt*it.second->feedrate, GET_DOUBLE("minSlowDownFeedrate"));
				}
			}
		}
	}
}

void GCodeWriter::registerSupportManager(std::shared_ptr<SupportManager> sman)
{
	m_ptrSupportManager = sman;
}

double GCodeWriter::getInfillLength(std::vector<PolyWithOffset> islands, std::shared_ptr<Layer> layer, bool includeFullInfill) const
{
	double infillLength = 0;
	for (auto &it : islands)
	{
		infillLength += getInfillLength(it.second, layer, includeFullInfill);
	}
	return infillLength;
}

double GCodeWriter::getInnerPerimTreeInfillLength(const InnerPerims *parent, bool includeFullInfil, double extWidth, std::shared_ptr<Layer> layer) const
{
	double result = 0.0;
	if (parent == nullptr)
		return 0;
	for (auto it : parent->getChildren())
	{
		result += getInnerPerimTreeInfillLength(it, includeFullInfil,extWidth,layer);
	}
	for (auto &it : parent->getInfillLines())
	{
		Vertex2D firstPoint = it.front();
		for (int i = 1; i < it.size(); ++i)
		{
			Vertex2D secondPoint = it[i];
			result += getExtrudeLengthTo(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, 1, 1, GET_DOUBLE("infillExtrusionMultiplier")/100.0, extWidth, layer);
			firstPoint = secondPoint;
		}
	}
	// Why wouldnt we want to include full infill here? I'll tell you, if we are using this for purging transition colours,
	// fullinfill is typically top or bottom solid layers, which we dont want the wrong colour showing
	if (includeFullInfil)
	{
		for (auto &it : parent->getFullInfillLines())
		{
			Vertex2D firstPoint = it.front();
			for (int i = 1; i < it.size(); ++i)
			{
				Vertex2D secondPoint = it[i];
				result += getExtrudeLengthTo(firstPoint.x, firstPoint.y, secondPoint.x, secondPoint.y, 1, 1, GET_DOUBLE("infillExtrusionMultiplier")/100.0, extWidth, layer);
				firstPoint = secondPoint;
			}
		}
	}
	return result;
}

double GCodeWriter::getInfillLength(std::shared_ptr<ExRSPolygon> poly, std::shared_ptr<Layer> layer, bool includeFullInfill) const
{
	double infillLength = 0;
	if (poly->getPerimTree() == nullptr)
	{
		// no perimeter tree, its probably support
		for (auto &linePath : poly->basicLinePaths)
		{
			for (auto &lines : linePath)
			{
				infillLength += getExtrudeLengthTo(lines.pt1.x, lines.pt1.y, lines.pt2.x, lines.pt2.y, 1, 1, GET_DOUBLE("infillExtrusionMultiplier"), poly->getExtWidth(), layer);///
					//(3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "filamentD") / 2.0, 2));
			}
		}
	}
	else
	{
		infillLength += getInnerPerimTreeInfillLength(poly->getPerimTree(), includeFullInfill, poly->getExtWidth(), layer);///
			//(3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "filamentD") / 2.0, 2));
	}
	return infillLength;
}



void GCodeWriter::writeIslands(std::vector<PolyWithOffset> &islands, unsigned int layer, double feedrateMultiplier, const Colour const* nextColour, int nextTool)
{
	bool infillOnly = GET_BOOL("infillBeforePerimeter");
	// Used to print all perims first, then pass with infill/vice versa
	if (!GET_BOOL("printAllPerimsFirst"))
	{
		while (!islands.empty())
		{
			//We have the colour, we have polys to do, find the next poly based on the current position
			std::vector<PolyWithOffset>::iterator closest = islands.begin();

			double closestDistance = closest->second->getSquaredDistanceNearest(Vertex(lastX-xOffset,lastY-yOffset));
			for (std::vector<PolyWithOffset>::iterator islandIT = islands.begin() + 1;
				islandIT != islands.end(); ++islandIT)
			{
				double testDistance = (islandIT)->second->getSquaredDistanceNearest(Vertex(lastX-xOffset, lastY-yOffset));
				if (testDistance < closestDistance)
				{
					closest = islandIT;
					closestDistance = testDistance;
				}
			}
			xOffset = closest->first.x;
			yOffset = closest->first.y;
			// If this is the last island in this colour, find if the infill is long enough to transition in, purge if not
			if (islands.size() == 1 && (nextColour != nullptr || nextTool > -1))
			{
				double infillLength = getInfillLength(closest->second, slicables.front()->getLayer(layer), false);
				if (infillLength > GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition"))
				{
					// Do the perims in the same colour
					writeComment("Perimeter of mixed poly original tool: " + std::to_string(lastTool));
					writePoly(closest->second, slicables.front()->getLayer(layer), false, false, false);
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, true);
					if (nextColour)
					{
						writeColour(*nextColour);
					}
					else if (nextTool > -1)
					{
						writeComment("Mixed poly colour chabnge, new Tool: " + std::to_string(nextTool));
						writeTool(nextTool);
					}
					// Do the infill in the next colour
					writeComment("Infill of mixed poly");
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, false);
				}
				else if (infillLength > GET_DOUBLE("distanceOfTransition"))
				{
					writePoly(closest->second, slicables.front()->getLayer(layer), false, false, false);
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, true);
					double purgeAmt;
					if (nextTool > -1)
						purgeAmt = writeInlineColourChange(nextTool, GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition") - infillLength);
					else
						purgeAmt = writeInlineColourChange(*nextColour, GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition") - infillLength);
					if (purgeAmt > 0.1 && GET_BOOL("purgeExcess"))
						writePurge(purgeAmt,-1,-1);
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, false);
				}
				else
				{
					writePoly(closest->second, slicables.front()->getLayer(layer), false, false, false);
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, true);
					double purgeAmt;
					if (nextTool > -1)
						purgeAmt = writeInlineColourChange(nextTool, GET_DOUBLE("distanceBeforeTransition"));
					else
						purgeAmt = writeInlineColourChange(*nextColour, GET_DOUBLE("distanceBeforeTransition"));
					if (purgeAmt > 0.1 && GET_BOOL("purgeExcess"))
						writePurge(purgeAmt + (GET_DOUBLE("distanceOfTransition") - infillLength),-1,-1);
					writePoly(closest->second, slicables.front()->getLayer(layer), false, true, false);
				}
			}
			else
				writePoly(closest->second, slicables.front()->getLayer(layer), true, false, false);
			islands.erase(closest);
		}
	}
	else
	{
		//Do all perimters first
		std::vector<PolyWithOffset> secondPass = islands;
		while (!islands.empty())
		{
			std::vector<PolyWithOffset>::iterator closest = islands.begin();

			double closestDistance = closest->second->getSquaredDistanceNearest(Vertex(lastX-xOffset, lastY-yOffset));
			for (std::vector<PolyWithOffset>::iterator islandIT = islands.begin() + 1;
				islandIT != islands.end(); ++islandIT)
			{
				double testDistance = (islandIT)->second->getSquaredDistanceNearest(Vertex(lastX-xOffset, lastY-yOffset));
				if (testDistance < closestDistance)
				{
					closest = islandIT;
					closestDistance = testDistance;
				}
			}
			xOffset = closest->first.x;
			yOffset = closest->first.y;
			// Perim
			writePoly(closest->second, slicables.front()->getLayer(layer), false, false, false);
			// Solid infill
			writePoly(closest->second, slicables.front()->getLayer(layer), false, true, true);
			islands.erase(closest);
		}
		// Find the infill length to determine if a purge is needed
		if (nextTool > -1 || nextColour)
		{
			double infillLength = 0;
			for (auto &it : secondPass)
			{
				infillLength += getInfillLength(it.second, slicables.front()->getLayer(layer), false);
			}
			if (infillLength > GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition"))
			{
				if (nextTool > -1)
					writeTool(nextTool);
				if (nextColour)
					writeColour(*nextColour);
			}
			else if (infillLength > GET_DOUBLE("distanceOfTransition"))
			{
				double purgeAmt;
				if (nextTool > -1)
					purgeAmt = writeInlineColourChange(nextTool, GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition") - infillLength);
				else
					purgeAmt = writeInlineColourChange(*nextColour, GET_DOUBLE("distanceOfTransition") + GET_DOUBLE("distanceBeforeTransition") - infillLength);
				if (purgeAmt > 0.1 && GET_BOOL("purgeExcess"))
					writePurge(purgeAmt,-1,-1);
				if (nextTool > -1)
					writeTool(nextTool);
				if (nextColour)
					writeColour(*nextColour);
			}
			else
			{
				double purgeAmt;
				if (nextTool > -1)
					purgeAmt = writeInlineColourChange(nextTool, GET_DOUBLE("distanceBeforeTransition"));
				else
					purgeAmt = writeInlineColourChange(*nextColour, GET_DOUBLE("distanceBeforeTransition"));
				if (purgeAmt > 0.1 && GET_BOOL("purgeExcess"))
					writePurge(purgeAmt + (GET_DOUBLE("distanceOfTransition") - infillLength),-1,-1);
				if (nextTool > -1)
					writeTool(nextTool);
				if (nextColour)
					writeColour(*nextColour);
			}
		}
		while (!secondPass.empty())
		{
			std::vector<PolyWithOffset>::iterator closest = secondPass.begin();

			double closestDistance = closest->second->getSquaredDistanceNearest(Vertex(lastX-xOffset, lastY-yOffset));
			for (std::vector<PolyWithOffset>::iterator islandIT = secondPass.begin() + 1;
				islandIT != secondPass.end(); ++islandIT)
			{
				double testDistance = (islandIT)->second->getSquaredDistanceNearest(Vertex(lastX-xOffset, lastY-yOffset));
				if (testDistance < closestDistance)
				{
					closest = islandIT;
					closestDistance = testDistance;
				}
			}
			xOffset = closest->first.x;
			yOffset = closest->first.y;
			// Perim
			writePoly(closest->second, slicables.front()->getLayer(layer), false, true, false);
			secondPass.erase(closest);
		}
	}
	if (nextTool > -1)
		writeTool(nextTool);
	if (nextColour)
		writeColour(*nextColour);
		
}

void GCodeWriter::writeIslandsByTool(std::unordered_map<uint16_t, std::vector<PolyWithOffset>> &islandsByTool, unsigned int layer, double feedratemultiplier)
{
	//Start with the last tool if its used again
	int nextTool = -1;
	while (islandsByTool.empty() == false)
	{
		nextTool = -1;
		if (islandsByTool.count(lastTool) > 0)
		{
			if (islandsByTool[lastTool].empty())
			{
				islandsByTool.erase(lastTool);
			}
			else
			{
				//We have the colour, we have polys to do, find the next poly based on the current position
				if (islandsByTool.size() > 1 && GET_BOOL("lookAheadPurge"))
				{
					for (auto &it : islandsByTool)
					{
						if (it.first == lastTool)
							continue;
						else if(!it.second.empty())
						{
							nextTool = it.first;
							break;
						}
					}
				}
				else
					nextTool = -1;
				int cachedTool = lastTool;
				writeIslands(islandsByTool[lastTool], layer, feedratemultiplier, nullptr, nextTool);
				islandsByTool[cachedTool].clear();
				islandsByTool.erase(cachedTool);
				if (nextTool >-1)
					lastTool = nextTool;
			}
		}
		else
		{
			writeTool(islandsByTool.begin()->first);
		}
	}
}

double GCodeWriter::writeNonTransition(std::vector<PolyWithOffset> &islands, unsigned int layer, Colour const &nextColour, double feedratemultiplier)
{

	while (!islands.empty())
	{
		std::vector<PolyWithOffset>::iterator closest = islands.begin();

		double closestDistance = closest->second->getSquaredDistanceNearest(Vertex(lastX - xOffset, lastY - yOffset));
		for (std::vector<PolyWithOffset>::iterator islandIT = islands.begin() + 1;
			islandIT != islands.end(); ++islandIT)
		{
			double testDistance = (islandIT)->second->getSquaredDistanceNearest(Vertex(lastX - xOffset, lastY - yOffset));
			if (testDistance < closestDistance)
			{
				closest = islandIT;
				closestDistance = testDistance;
			}
		}
		xOffset = closest->first.x;
		yOffset = closest->first.y;
		// Perim
		writeComment("Write perimeter");
		writePoly(closest->second, slicables.front()->getLayer(layer), false, false, false);
		// Solid infill
		writeComment("Write infill poly");
		writePoly(closest->second, slicables.front()->getLayer(layer), false, true, true);
		islands.erase(closest);
	}
	if (lastColour == nextColour)
		return 0;
	return writeInlineColourChange(nextColour, GET_DOUBLE("distanceBeforeTransition"));
}

double GCodeWriter::writeInfillPurge(std::vector<std::vector<Vertex2D>> &purges, double amt, double extWidth, std::shared_ptr<Layer> layer)
{
	writeComment("Write infill purge");
	// Find the closest lines to start purging on
	while (amt > 0)
	{
		if (purges.empty())
			return amt;
		std::vector<std::vector<Vertex2D>>::iterator closest = purges.begin();
		double distance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), closest->front());
		for (std::vector<std::vector<Vertex2D>>::iterator it = purges.begin(); it != purges.end(); ++it)
		{
			if (Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), it->front()) < distance)
			{
				distance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), it->front());
				closest = it;
			}
		}
		double z = layer->getLayerZ() + layer->getThickness();
		writeTravelTo(closest->front().x, closest->front().y, z);
		for (std::vector<Vertex2D>::iterator points = closest->begin(); points != closest->end(); )
		{
			amt -= getExtrudeLengthTo(lastX+xOffset, lastY+yOffset, points->x, points->y, z, z, 1.0, extWidth, layer);
			writeExtrudeTo(points->x, points->y, z, 1, extWidth, layer, GET_DOUBLE("infillSpeed")*60.0);
			points = closest->erase(points);
			if (amt <= 0)
				break;
		}
		if (closest->empty())
			purges.erase(closest);
	}
	return 0;
}

void GCodeWriter::writeRemainingInfill(std::vector<std::vector<Vertex2D>> &purges, double extWidth, std::shared_ptr<Layer> layer)
{
	writeComment("Writing remaining infill");
	while (purges.empty() == false)
	{
		std::vector<std::vector<Vertex2D>>::iterator closest = purges.begin();
		double distance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), closest->front());
		for (std::vector<std::vector<Vertex2D>>::iterator it = purges.begin(); it != purges.end(); ++it)
		{
			if (Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), it->front()) < distance)
			{
				distance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), it->front());
				closest = it;
			}
		}
		double z = layer->getLayerZ() + layer->getThickness();
		writeTravelTo(closest->front().x, closest->front().y, z);
		for (std::vector<Vertex2D>::iterator points = closest->begin(); points != closest->end(); ++points)
		{
			writeExtrudeTo(points->x, points->y, z, 1, extWidth, layer, GET_DOUBLE("infillSpeed")*60.0);
		}
		purges.erase(closest);
	}
}

void GCodeWriter::writeIslandsByColourLookahead(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour, unsigned int layer, double feedratemultiplier)
{
	writeComment("Write islands by colour 2");

	std::unordered_map<std::shared_ptr<ExRSPolygon>, double> purgeAmtInEachPoly;
	using IslandTuple = std::tuple<std::vector<std::vector<Vertex2D>>, double, Vertex2D>;
	std::list<IslandTuple> islandsAvailableToPurgeTuple;
	std::list <std::pair<std::vector<std::vector<Vertex2D>>,double>> islandsAvailableToPurge;
	double totalPurgeDistanceAvailable = 0;
	int purgeIslandCount = 0;
	for (auto it : islandsByColour)
	{
		for (auto polyIT : it.second)
		{
			double islandInfill = getInfillLength(polyIT.second, slicables.front()->getLayer(layer), false);
			//double perimeterForPreChange = 
			totalPurgeDistanceAvailable += islandInfill;
			if (islandInfill >= GET_DOUBLE("distanceOfTransition"))
				++purgeIslandCount;
			purgeAmtInEachPoly[polyIT.second] = islandInfill;
		}
	}
	int colourChangesReq = islandsByColour.size();
	if (islandsByColour.count(lastColour) == 0)
	{
		// Last colour isnt used
		writeComment("Last colour isnt used on this layer, purge to begin");
		if (GET_BOOL("purgeExcess"))
			writeSoftwareColourPurge(islandsByColour.begin()->first);
		else
			writeColour(islandsByColour.begin()->first); // should also start with the infill because of this
	}
	else
		colourChangesReq--;

	// Best case, each colour island can infill the change
	if (purgeIslandCount >= colourChangesReq)
	{
		// We can fit each colour purge inside its own island - the simple case
		while (!islandsByColour.empty())
		{
			xOffset = islandsByColour[lastColour].front().first.x;
			yOffset = islandsByColour[lastColour].front().first.y;
			for (auto polyIT : islandsByColour[lastColour])
			{
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, false, true);
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, true, true);
			}
			Colour colourCache = lastColour;
			if (islandsByColour.size() > 1 && GET_BOOL("lookAheadPurge"))
			{
				Colour nextColour;
				for (auto &it : islandsByColour)
				{
					if (it.first == lastColour)
						continue;
					else if (!it.second.empty())
					{
						nextColour = it.first;
						break;
					}
				}
				writeColour(nextColour);
			}
			for (auto polyIT : islandsByColour[colourCache])
			{
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, true, false);
			}
			islandsByColour.erase(colourCache);
		}
	}
	else
	{
		for (auto colourIT : islandsByColour)
		{
			for (auto polyIT : colourIT.second)
			{
				double infillLength = getInfillLength(polyIT.second, slicables.front()->getLayer(layer), false);
				islandsAvailableToPurgeTuple.push_back(IslandTuple(polyIT.second->getPerimTree()->getAllInfillLines(), infillLength,
					polyIT.first));
				for (auto &lineSet : std::get<0>(islandsAvailableToPurgeTuple.back()))
				{
					Line::translateLines(lineSet, std::get<2>(islandsAvailableToPurgeTuple.back()).x, std::get<2>(islandsAvailableToPurgeTuple.back()).y);
				}
			}
		}
		while (!islandsByColour.empty())
		{
			for (auto polyIT : islandsByColour[lastColour])
			{
				xOffset = islandsByColour[lastColour].front().first.x;
				yOffset = islandsByColour[lastColour].front().first.y;
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, false, true);
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, true, true);
				//double infillLength = getInfillLength(polyIT.second, slicables.front()->getLayer(layer), false);
				//islandsAvailableToPurge.push_back(std::pair<PolyWithOffset, double>(polyIT, infillLength));
			}
			islandsByColour.erase(lastColour);
			double purgeAmtStillRequired = GET_DOUBLE("distanceOfTransition");
			if (islandsByColour.size() >= 1 && GET_BOOL("lookAheadPurge"))
			{
				Colour nextColour;
				for (auto &it : islandsByColour)
				{
					if (it.first == lastColour)
						continue;
					else if (!it.second.empty())
					{
						nextColour = it.first;
						break;
					}
				}
				if(GET_DOUBLE("distanceBeforeTransition")>0.001)
					purgeAmtStillRequired+=writeInlineColourChange(nextColour, GET_DOUBLE("distanceBeforeTransition"));
				else
					writeColour(nextColour);
			}
			writeComment("Write purge infill");
			for (std::list <IslandTuple>::iterator infillIT = 
				islandsAvailableToPurgeTuple.begin(); infillIT != islandsAvailableToPurgeTuple.end();)
			{
				if (std::get<0>(*infillIT).empty())
				{
					infillIT = islandsAvailableToPurgeTuple.erase(infillIT);
					if (infillIT == islandsAvailableToPurgeTuple.end())
						break;
				}
				if (std::get<1>(*infillIT) > purgeAmtStillRequired)
				{
					// Partial infill purge available
					std::vector<std::vector<Vertex2D>> writeSet;
					std::vector<std::vector<Vertex2D>> remainSet;
					std::vector<std::vector<Vertex2D>>::iterator infillSetLine;
					for (infillSetLine = std::get<0>(*infillIT).begin(); infillSetLine != std::get<0>(*infillIT).end(); ++infillSetLine)
					{
						std::vector<Vertex2D> tempSet;
						Vertex2D lastPoint = infillSetLine->front();
						for (std::vector<Vertex2D>::iterator lineSetIT = infillSetLine->begin(); 
							lineSetIT != infillSetLine->end();)
						{
							purgeAmtStillRequired -= getExtrudeLengthTo(lastPoint.x, lastPoint.y, lineSetIT->x, lineSetIT->y, 0, 0,
								GET_DOUBLE("infillExtrusionMultiplier") / 100.0, 0.6,
								slicables.front()->getLayer(layer));
							tempSet.push_back(*lineSetIT);
							lastPoint = *lineSetIT;
							lineSetIT = infillSetLine->erase(lineSetIT);
							if (purgeAmtStillRequired <= 0)
								break;
							if (lineSetIT == infillSetLine->end())
								break;
							
						}
						if (infillSetLine->empty())
						{
							infillSetLine = std::get<0>(*infillIT).erase(infillSetLine);
							if (infillSetLine == std::get<0>(*infillIT).end())
							{
								writeSet.push_back(tempSet);
								break;
							}
						}
						writeSet.push_back(tempSet);
					}
					xOffset = 0;// std::get<2>(*infillIT).x;
					yOffset = 0;// std::get<2>(*infillIT).y;
					writeInfillPath(writeSet,
						slicables.front()->getLayer(layer), 0.6, false);
					
				}
				else
				{
					// Fill the entire island
					if (std::get<1>(*infillIT) > 0.01)
					{
						xOffset = 0;// std::get<2>(*infillIT).x;
						yOffset = 0;// std::get<2>(*infillIT).y;
						purgeAmtStillRequired -= std::get<1>(*infillIT);
						writeInfillPath(std::get<0>(*infillIT),
							slicables.front()->getLayer(layer), 0.6, false);
					}
					//writePoly(infillIT->first.second, slicables.front()->getLayer(layer), false, true, false);
					infillIT = islandsAvailableToPurgeTuple.erase(infillIT);
				}
				if (purgeAmtStillRequired <= 0)
					break;
				if (infillIT == islandsAvailableToPurgeTuple.end())
					break;
			}
			if (purgeAmtStillRequired > 0)
				writePurge(purgeAmtStillRequired, -1, -1);
			else
				writeComment("Avoided a purge!");
		}
	}
	// Write all unused purge infills
	writeComment("Unused purge infill");
	for (auto infillIT : islandsAvailableToPurgeTuple)
	{
		xOffset = 0;// std::get<2>(infillIT).x;
		yOffset = 0;// std::get<2>(infillIT).y;
		writeInfillPath(std::get<0>(infillIT),
			slicables.front()->getLayer(layer), 0.6, false);
	}
}

/*
void GCodeWriter::writeIslandsByColourLookahead(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour, unsigned int layer, double feedratemultiplier)
{
	writeComment("Write islands by colour");
	// Get each island purge area
	std::unordered_map <std::shared_ptr<ExRSPolygon>, std::pair<double,std::vector<std::vector<Vertex2D>>>> polyToPurgeMap;


	double possiblePurgeDistance = 0;
	Colour biggestColour = islandsByColour.begin()->first;
	double biggestAmt = 0;
	std::vector<std::vector<Vertex2D>> infillPurges;
	int purgeCount = islandsByColour.size();
	for (auto it : islandsByColour)
	{
		double thisColourAmt = 0;
		for (auto polyIT : it.second)
		{
			double islandAmt = getInfillLength(polyIT.second, slicables.front()->getLayer(layer), false);
			possiblePurgeDistance += islandAmt;
			thisColourAmt += islandAmt;
			auto infillSet = polyIT.second->getPerimTree()->getAllInfillLines();
			polyToPurgeMap[polyIT.second] = std::pair<double,std::vector<std::vector<Vertex2D>>>(islandAmt,infillSet);
			infillPurges.insert(infillPurges.end(), infillSet.begin(),infillSet.end());
		}
		if (thisColourAmt > biggestAmt)
		{
			biggestColour = it.first;
			biggestAmt = thisColourAmt;
		}
	}
	if (islandsByColour.count(lastColour) == 0)
	{
		// The last colour isnt used on this layer
		writeComment("Last colour isnt used on this layer, purge to begin");
		if (GET_BOOL("purgeExcess"))
			writeSoftwareColourPurge(biggestColour);
		else
			writeColour(biggestColour); // should also start with the infill because of this
	}
	else
	{
		// Colour is repeated
		purgeCount--;
	}
	
	int purgesAvailable = 0;
	for (auto it : polyToPurgeMap)
	{
		if (it.second.first > GET_DOUBLE("distanceOfTransition"))
		{
			++purgesAvailable;
		}
	}
	bool canPurgeEachColourSeperateIsland = (purgesAvailable >= purgeCount);
	Colour nextColour = lastColour;
	// Rough approximation for extrusion width
	double extWidth = islandsByColour.begin()->second.front().second->getExtWidth();
	while (islandsByColour.empty() == false)
	{
		if (islandsByColour.size() > 1 && GET_BOOL("lookAheadPurge"))
		{
			for (auto &it : islandsByColour)
			{
				if (it.first == lastColour)
					continue;
				else if (!it.second.empty())
				{
					nextColour = it.first;
					break;
				}
			}
		}
		Colour cCache = lastColour;

		// Write purge, if possible fill the current island infill
		if (canPurgeEachColourSeperateIsland)
		{
			for (auto polyIT : islandsByColour[cCache])
			{
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, false, false);
			}
			writeColour(nextColour);
			for (auto polyIT : islandsByColour[cCache])
			{
				//polyIT.second;
				writePoly(polyIT.second, slicables.front()->getLayer(layer), false, true, false);
			}
		}
		else
		{
			// Write Perimeters and Solid Infill
			double purgeAmt = writeNonTransition(islandsByColour[lastColour], layer, nextColour);
			if (cCache != lastColour)
				purgeAmt += GET_DOUBLE("distanceOfTransition");
			purgeAmt = writeInfillPurge(infillPurges, purgeAmt, extWidth, slicables.front()->getLayer(layer));
			if (purgeAmt > 0 && GET_BOOL("purgeExcess"))
				writePurge(purgeAmt, -1, -1);
		}
		islandsByColour.erase(cCache);
	}
	if (infillPurges.empty() == false)
	{
		writeRemainingInfill(infillPurges, extWidth,
			slicables.front()->getLayer(layer));
	}
}
*/
void GCodeWriter::writeIslandsByColour(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour, unsigned int layer, double feedratemultiplier)
{
	if (islandsByColour.empty())
		return;
	if (lastTool != COLOUR_TOOL)
		writeTool(COLOUR_TOOL);
	if (lastColour.isSet() == false)
	{
		writeColour(islandsByColour.begin()->first);
	}
	if (GET_BOOL("lookAheadPurge")) {
		writeIslandsByColourLookahead(islandsByColour, layer, feedratemultiplier);
		return;
	}
	const Colour *nextColour = nullptr;
	while (islandsByColour.empty() == false)
	{
		nextColour = nullptr;
		if (islandsByColour.count(lastColour) > 0)
		{
			//Last colour used, is used again
			if (islandsByColour[lastColour].empty())
			{
				islandsByColour.erase(lastColour);
			}
			else
			{
				if (islandsByColour.size() > 1 && GET_BOOL("lookAheadPurge"))
				{
					for (auto &it : islandsByColour)
					{
						if (it.first == lastColour)
							continue;
						else if(!it.second.empty())
						{
							nextColour = &it.first;
							break;
						}
					}
				}
				else
					nextColour = nullptr;
				Colour colourChache = lastColour;
				writeIslands((islandsByColour[lastColour]), layer, feedratemultiplier, nextColour);
				islandsByColour[colourChache].clear();
				islandsByColour.erase(colourChache);
				if(nextColour != nullptr)
					lastColour = *nextColour;
			}
		}
		else
		{
			if(!islandsByColour.empty())
				writeColour(islandsByColour.begin()->first);
		}
	}
}

void GCodeWriter::writePoly(std::shared_ptr<ExRSPolygon> &in, std::shared_ptr<Layer> layer, bool infillAndPerims, bool infillPass, bool fullInfill)
{
	double z = layer->getLayerZ() + layer->getThickness();
	if (GET_BOOL("perimetersInsideOut"))
	{
		if (infillAndPerims)
		{
			if (Settings::GetSingleton().getBoolValue("infillBeforePerimeter"))
			{
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, true);
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, false);
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
			}
			else
			{
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, true);
				writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, false);
			}
		}
		else
			writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
	}
	else
	{
		if (infillAndPerims)
		{
			if (Settings::GetSingleton().getBoolValue("infillBeforePerimeter"))
			{
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, true);
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, false);
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
			}
			else
			{
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, true);
				writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, true, false);
			}
		}
		else
			writePerimeterTreeReversed(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), false, infillPass, fullInfill);
		//writePerimeterTree(in->getPerimTree(), layer, in->getExtWidth(), in->getFullInfillExtWidth(), infillAndPerims, infillPass, fullInfill);
	}
	if (infillAndPerims || infillPass)
	{
		if (Settings::GetSingleton().getBoolValue("infillBeforePerimeter"))
		{
			writeBasicLine(in->basicLinePaths, layer, in->getExtWidth());
		}
		else
		{
			writeBasicLine(in->basicLinePaths, layer, in->getExtWidth());
		}
	}
}

void GCodeWriter::writeInfillPath(const std::vector<std::vector<Vertex2D>> &paths, std::shared_ptr<Layer> layer, double extWidth, bool isFullInfill,double f)
{
	// Find closest Path
	std::vector<std::vector<Vertex2D>> workingPaths = paths;
	if(currentLayer != 0)
		writeMixerSpeed(Settings::getSingleton().getDoubleValue("infillMixerSpeed"));
	double e_multiplier = 1.0;
	if (!isFullInfill)
		e_multiplier = GET_DOUBLE("infillExtrusionMultiplier") / 100.0;
	while (workingPaths.empty() == false)
	{
		bool reversedSet = false;
		double shortestDistanceSqrd = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), workingPaths.front().front());
		std::vector<std::vector<Vertex2D>>::iterator closestSet = workingPaths.begin();
		double z = layer->getLayerZ() + layer->getThickness();
		if (!reversedSet)
		{
			writeTravelTo(closestSet->front().x, closestSet->front().y, z, -1, false, Line::LineType::INFILL);
			for (auto &points : (*closestSet))
			{
				writeExtrudeTo(points.x, points.y, z, e_multiplier,extWidth,layer,f);
			}
		}
		else
		{
			writeTravelTo(closestSet->back().x, closestSet->back().y, z,-1,false,Line::LineType::INFILL);
			for (auto points = closestSet->rbegin(); points != closestSet->rend(); ++points)
			{
				writeExtrudeTo(points->x, points->y, z, e_multiplier,extWidth,layer, f);
			}
		}
		workingPaths.erase(closestSet);
	}
}
// Write outer perimeters first
void GCodeWriter::writePerimeterTree(const InnerPerims* perimTree, std::shared_ptr<Layer> layer,double extWidth, double fullInfillExtWidth, bool infillAndPerims, bool infillPass, bool fullInfill)
{
	// Write perim path
	if (perimTree)
	{
		if ((!perimTree->getInfillLines().empty() || !perimTree->getFullInfillLines().empty()) && GET_BOOL("infillBeforePerimeter") && (infillAndPerims || infillPass))
		{
			if (infillAndPerims || !fullInfill)
			{
				if (!perimTree->getInfillLines().empty())
					writeInfillPath(perimTree->getInfillLines(), layer, extWidth, false, GET_DOUBLE("infillSpeed")*60.0);
			}
			if (infillAndPerims || fullInfill)
			{
				if (!perimTree->getFullInfillLines().empty())
					writeInfillPath(perimTree->getFullInfillLines(), layer, fullInfillExtWidth,true, GET_DOUBLE("infillSpeed")*60.0);
			}
		}
		if (infillAndPerims || !infillPass)
		{
			writePath(*perimTree->getContour(), layer, extWidth, GET_DOUBLE("perimeterSpeed")*60.0);
			if (perimTree->hasHole())
				writePaths(*perimTree->getHoles(), layer, extWidth, GET_DOUBLE("perimeterSpeed")*60.0);
		}
		if ((!perimTree->getInfillLines().empty() || !perimTree->getFullInfillLines().empty()) && !GET_BOOL("infillBeforePerimeter") && (infillAndPerims || infillPass))
		{
			if (infillAndPerims || !fullInfill)
			{
				if (!perimTree->getInfillLines().empty())
					writeInfillPath(perimTree->getInfillLines(), layer, extWidth, false,GET_DOUBLE("infillSpeed")*60.0);
			}
			if (infillAndPerims || fullInfill)
			{
				if (!perimTree->getFullInfillLines().empty())
					writeInfillPath(perimTree->getFullInfillLines(), layer, fullInfillExtWidth, true,GET_DOUBLE("infillSpeed")*60.0);
			}
		}
		for (auto it : perimTree->getChildren())
		{
			writePerimeterTree(it, layer, extWidth, fullInfillExtWidth,infillAndPerims,infillPass,fullInfill);
		}
	}
}
// Write inner perimeters first
void GCodeWriter::writePerimeterTreeReversed(const InnerPerims* perimTree, std::shared_ptr<Layer> layer, double extWidth, double fullInfillExtWidth,bool infillAndPerims,bool infillPass, bool fullInfill)
{
	if (perimTree)
	{
		for (auto it : perimTree->getChildren())
		{
			writePerimeterTreeReversed(it, layer, extWidth, fullInfillExtWidth, infillAndPerims, infillPass,fullInfill);
		}
		// Write perim path
		if ((!perimTree->getInfillLines().empty() || !perimTree->getFullInfillLines().empty()) && GET_BOOL("infillBeforePerimeter") && (infillAndPerims || infillPass))
		{
			if (infillAndPerims || !fullInfill)
			{
				if (!perimTree->getInfillLines().empty())
					writeInfillPath(perimTree->getInfillLines(), layer, extWidth, false,GET_DOUBLE("infillSpeed")*60.0);
			}
			if(infillAndPerims || fullInfill)
			{
				if (!perimTree->getFullInfillLines().empty())
					writeInfillPath(perimTree->getFullInfillLines(), layer, fullInfillExtWidth, true,GET_DOUBLE("infillSpeed")*60.0);
			}
		}
		if (infillAndPerims || !infillPass)
		{
			if (perimTree->hasHole())
			{
				writePaths(*perimTree->getHoles(), layer, extWidth, GET_DOUBLE("perimeterSpeed")*60.0);
			}
			writePath(*perimTree->getContour(), layer, extWidth, GET_DOUBLE("perimeterSpeed")*60.0);
		}
		if ((!perimTree->getInfillLines().empty() || !perimTree->getFullInfillLines().empty()) && !GET_BOOL("infillBeforePerimeter") && (infillAndPerims || infillPass))
		{
			if (infillAndPerims || !fullInfill)
			{
				if (!perimTree->getInfillLines().empty())
					writeInfillPath(perimTree->getInfillLines(), layer, extWidth, false,GET_DOUBLE("infillSpeed")*60.0);
			}
			if (infillAndPerims || fullInfill)
			{
				if (!perimTree->getFullInfillLines().empty())
					writeInfillPath(perimTree->getFullInfillLines(), layer, fullInfillExtWidth,true, GET_DOUBLE("infillSpeed")*60.0);
			}
		}
	}
}

void GCodeWriter::writePath(const std::vector<Vertex2D> &path, std::shared_ptr<Layer> layer, double extWidth, double f)
{
	if (currentLayer != 0)
		writeMixerSpeed(Settings::getSingleton().getDoubleValue("perimeterMixerSpeed"),false,"Perimeter");
	double z = layer->getLayerZ() + layer->getThickness();
	writeTravelTo(path.front().x, path.front().y, z);
	for (int i = 1; i < path.size(); i++)
	{
		writeExtrudeTo(path[i].x, path[i].y, z, 1,extWidth,layer, f);
	}
}

void GCodeWriter::writePaths(const std::vector<std::vector<Vertex2D>> &path, std::shared_ptr<Layer> layer, double extWidth, double f)
{
	std::vector<std::vector<Vertex2D>> pathsToGo(path);
	while (!pathsToGo.empty())
	{
		double closestDistance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), pathsToGo.front().front());
		std::vector<std::vector<Vertex2D>>::iterator closestLoop = pathsToGo.begin();
		for (auto setIT = pathsToGo.begin() + 1; setIT != pathsToGo.end(); ++setIT)
		{
			if (closestDistance < Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), setIT->front()))
			{
				closestDistance = Vertex2D::distanceR2Sqrd(Vertex2D(lastX, lastY), setIT->front());
				closestLoop = setIT;
			}
		}
		writePath(*closestLoop, layer,extWidth, f);
		pathsToGo.erase(closestLoop);
	}
}

void GCodeWriter::writeBasicLine(const std::vector<Lines> &in, std::shared_ptr<Layer> layer, double extWidth, double f)
{
	if (in.empty())
		return;
	double z = layer->getLayerZ() + layer->getThickness();
	for (auto lineSetIT : in)
	{
		writeTravelTo(lineSetIT.front().pt1.x, lineSetIT.front().pt1.y, z);
		for (auto it : lineSetIT)
		{
			writeExtrudeTo(it.pt2.x, it.pt2.y, z, 1,extWidth,layer, f);
		}
	}
}

void GCodeWriter::writeZTravelTo(double z, double f)
{
	if (lastZ != z)
	{
		if (Settings::getSingleton().getBoolValue("retractOnLayerChange"))
		{
			writeRetract(RETRACTION_TYPE::TRAVEL_MOVE);
		}
		GCodeLine zTravel(lastTool,lastColour);
		zTravel.pt1.x = quantizeMove(lastX, AXIS::X);
		zTravel.pt1.y = quantizeMove(lastY, AXIS::Y);
		zTravel.pt1.z = quantizeMove(lastZ, AXIS::Z);
		zTravel.addParameter(Line::LineType::TRAVEL | Line::LineType::Z_MOVE);
		zTravel.e1 = zTravel.e2 = quantizeMove(lastE, AXIS::E);
		if (retracted && Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)) > 0)
		{
			lastZ = z + Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool));
		}
		else
			lastZ = z;
		zTravel.pt2.x = quantizeMove(lastX, AXIS::X);
		zTravel.pt2.y = quantizeMove(lastY, AXIS::Y);
		zTravel.pt2.z = quantizeMove(lastZ, AXIS::Z);

		if (f < 0)
			f = Settings::getSingleton().getDoubleValue("travelSpeed")*60;

		zTravel.feedrate = f;

		lastToolPosition.x = lastX;
		lastToolPosition.y = lastY;
		lastToolPosition.z = lastZ;
		bufferLine(zTravel);
		lastFeedrate = f;
	}
}

void GCodeWriter::writeTool(uint16_t tool)
{
	if (tool == lastTool)
		return;
	if (lastTool == std::numeric_limits<uint16_t>::max())
		lastTool = tool;
	if (GET_BOOL("mixerOnlyUse") && Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
	{
		if (tool != 0)
			writeMixerSpeed(0,false,"Not using full colour, disable mixer");
		else
			writeMixerSpeed(GET_DOUBLE("mixerSpeed"),false,"Tool switch");
	}
	toolsUsed.insert(tool);
	if (GET_BOOL("changeToolOverPurgeBin"))
	{
		double targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") * ((double)rand() / (double)(RAND_MAX)) - xOffset;
		double targetY = GET_DOUBLE("purgeLocationY") + GET_DOUBLE("purgeHeight") / 2.0 - yOffset;
		writeRetract(RETRACTION_TYPE::TOOL_CHANGE);
		writeComment("TravelToPurgeBin Tool Change:(" + std::to_string(targetX) + "," + std::to_string(targetY) + ")");
		writeTravelTo(targetX, targetY, lastZ);
		//writeTravelTo(GET_DOUBLE("purgeLocationX"), GET_DOUBLE("purgeLocationY"), lastZ);
	}
	bufferLine("T" + std::to_string(static_cast<int>(tool)));
	lastTool = tool;
	if (GET_BOOL("primeToolPurgeBin") && !GET_BOOL("rova5Compat") && m_primedToolSet.count(tool) == 0)
	{
		writeComment("Tool prime purge\n");
		writePurge(GET_DOUBLE("primeToolPurgeLength"), -1, -1);
		m_primedToolSet.insert(tool);
	}
	else if (GET_BOOL("purgeOnToolChange") && !GET_BOOL("rova5Compat"))
	{
		writeComment("Tool switch purge\n");
		writePurge(GET_DOUBLE("toolChangePurgeDistance"), -1, -1);
	}
}

void GCodeWriter::writeColour(const Colour &in)
{
	//Debug checks
	assert(lastTool == (uint16_t)iDrawable::ToolType::FULL_COLOUR);
	uint8_t r = in.getR<uint8_t>();
	uint8_t g = in.getG<uint8_t>();
	uint8_t b = in.getB<uint8_t>();

	if (fabs(r - lastR) > Settings::getSingleton().getDoubleValue("colourChangeThreshold") ||
		fabs(g - lastG) > Settings::getSingleton().getDoubleValue("colourChangeThreshold") ||
		fabs(b - lastB) > Settings::getSingleton().getDoubleValue("colourChangeThreshold"))
	{
		if (GET_BOOL("alwaysPurge"))
		{
			writeSoftwareColourPurge(in);
		}
		else
		{
			std::string colourCommand = getColourString(in);
			bufferLine(colourCommand, " R:" + std::to_string((int)in.getR<uint8_t>()) + " G:" + std::to_string((int)in.getG<uint8_t>()) +
				" B:" + std::to_string((int)in.getB<uint8_t>()));
		}
		lastColour = in;
	}
	if (GET_BOOL("mixerOnlyUse"))
	{
		if (in.isSingleColour())
		{
			writeMixerSpeed(0,false,"Switched to single colour");
		}
	}
}

void GCodeWriter::writeSoftwareColourPurge(const Colour &in)
{
	if (GET_BOOL("useSoftwarePurge") == false)
	{
		bufferLine(getColourString(in, !GET_BOOL("useSoftwarePurge")));
		lastColour = in;
		return;
	}
	if (GET_BOOL("purgeForceRetract"))
		writeRetract(RETRACTION_TYPE::TRAVEL_MOVE);
	writeAllBufferedLines(); // avoid wipe and other maddness here
	// Procedure:
	// 0. Set Mixer speed
	// 1. Travel to centre of purge bin
	// 2. Dwell?
	// 2.5 Switch colours
	// 3. Purge while moving from one extreme to the other extreme of the bin
	// 4a. Force Retract
	// 4b. Travel to wipe area
	// 5. Force a Y only move down accross the wipe area?
	// 6. Return
	// Subtract the offsets from all printing moves, since purge is in absolution positoins
	double targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") * ((double)rand() / (double)(RAND_MAX)) - xOffset;
	double targetY = GET_DOUBLE("purgeLocationY") + GET_DOUBLE("purgeHeight") / 2.0 - yOffset;
	writeComment("TravelToPurgeBin:(" + std::to_string(targetX) + "," + std::to_string(targetY) + ")");
	writeTravelTo(targetX, targetY, lastZ);
	if (GET_BOOL("pauseOnFullPurgeBin") && purgeBinAmount + GET_DOUBLE("purgeLength") > GET_DOUBLE("purgeBinCapacity"))
	{
		bufferLine("@pause", "Purge bin pause");
		purgeBinAmount = 0;
	}
	writeIssue();
	bufferLine(getColourString(in, !GET_BOOL("useSoftwarePurge")));
	lastColour = in;
	writePurge(GET_DOUBLE("purgeLength"),targetX,targetY);
	//Resume print
}

std::string GCodeWriter::getColourString(const Colour &in, bool hardwarePurge) const
{
	if (GET_BOOL("rova5Compat"))
	{
		uint16_t toolId = Settings::getSingleton().getToolByColour(in);
		return "T" + std::to_string((int)toolId);
	}
	else if (Settings::getSingleton().getStringValue("headChoice")  == "Diamond Hotend - CMY" || Settings::getSingleton().getStringValue("firmware") == "Marlin" ||
		Settings::getSingleton().getStringValue("headChoice") == "Diamond Full Colour - CMYKW")
	{
		std::stringstream command;
		if (Settings::getSingleton().getStringValue("headChoice") == "Diamond Full Colour - CMYKW")
		{
			auto cmykw = in.getCMYKW();
			if (Settings::GetSingleton().hasColourSubs())
			{
				cmykw.c = Settings::GetSingleton().getColourSub(in).c;
				cmykw.m = Settings::GetSingleton().getColourSub(in).m;
				cmykw.y = Settings::GetSingleton().getColourSub(in).y;
				cmykw.w = Settings::getSingleton().getColourSub(in).w;
				cmykw.k = Settings::GetSingleton().getColourSub(in).k;
			}
			if (Settings::getSingleton().getStringValue("firmware") == "Marlin")
			{
				command.precision(2);//marlin direct mixing values add to 1.0 instead of 100
				command << "G1 A" << cmykw.c / 100.0 << " B" << cmykw.m / 100.0 << " C" << cmykw.y / 100.0
					<< " D"<<cmykw.k/100.0 << " H"<<cmykw.w/100.0;
			}
			else
			{
				command.precision(4);//marlin direct mixing values add to 1.0 instead of 100
				if (hardwarePurge)
					command << "M3016 ";
				else
					command << "M3015 ";
				command << "C" << cmykw.c << " M" << cmykw.m << " Y" << cmykw.y << " K" << cmykw.k << " W" << cmykw.w;
			}
		}
		else
		{
			auto cmykw = in.getCMY();
			if (Settings::GetSingleton().hasColourSubs())
			{
				cmykw.c = Settings::GetSingleton().getColourSub(in).c;
				cmykw.m = Settings::GetSingleton().getColourSub(in).m;
				cmykw.y = Settings::GetSingleton().getColourSub(in).y;
			}
			if (Settings::getSingleton().getStringValue("firmware") == "Marlin")
			{
				command.precision(2);//marlin direct mixing values add to 1.0 instead of 100
				command << "G1 A" << cmykw.c / 100.0 << " B" << cmykw.m / 100.0 << " C" << cmykw.y / 100.0;
			}
			else
			{
				command.precision(4);//marlin direct mixing values add to 1.0 instead of 100
				if (hardwarePurge)
					command << "M3016 ";
				else
					command << "M3015 ";
				command << "C" << cmykw.c << " M" << cmykw.m << " Y" << cmykw.y;
			}
		}
		return command.str();
	}
	else
	{
		std::stringstream command;
		std::fixed(command);
		command << std::setprecision(2);
		if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
		{
			auto cmykw = in.getCMYKW();
			if (Settings::getSingleton().hasColourSubs())
			{
				cmykw.c = Settings::GetSingleton().getColourSub(in).c;
				cmykw.m = Settings::GetSingleton().getColourSub(in).m;
				cmykw.y = Settings::GetSingleton().getColourSub(in).y;
				cmykw.k = Settings::GetSingleton().getColourSub(in).k;
				cmykw.w = Settings::GetSingleton().getColourSub(in).w;
			}
			if (hardwarePurge)
				command << "M3016 ";
			else
				command << "M3015 ";
			command << "C" << cmykw.c << " M" << cmykw.m << " Y" << cmykw.y
				<< " K" << cmykw.k << " W" << cmykw.w;
			if (Settings::GetSingleton().hasColourSubs())
				command << " T" << Settings::GetSingleton().getColourSub(in).t;
			else
				command << " T0.00";
			return command.str();
		}
		else
		{
			auto cmykw = in.getCMY();
			if (Settings::getSingleton().hasColourSubs())
			{
				cmykw.c = Settings::GetSingleton().getColourSub(in).c;
				cmykw.m = Settings::GetSingleton().getColourSub(in).m;
				cmykw.y = Settings::GetSingleton().getColourSub(in).y;
			}
			if (hardwarePurge)
				command << "M3016 ";
			else
				command << "M3015 ";
			command << "C" << cmykw.c << " M" << cmykw.m << " Y" << cmykw.y;
			return command.str();
		}
	}
}

void GCodeWriter::writePurge(double purgeAmt, double targetX, double targetY)
{
	// Skip all these moves if the purge amt is super small
	if (purgeAmt <= 0.01)
		return;
	if (GET_BOOL("useSoftwarePurge") == false)
		return;
	if (targetX < 0 && targetY < 0)
	{
		targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") * ((double)rand() / (double)(RAND_MAX)) - xOffset;
		targetY = GET_DOUBLE("purgeLocationY") + GET_DOUBLE("purgeHeight") / 2.0 - yOffset;
		writeComment("TravelToPurgeBin:(" + std::to_string(targetX) + "," + std::to_string(targetY) + ")");
		writeTravelTo(targetX, targetY, lastZ);
		if (GET_BOOL("pauseOnFullPurgeBin") && purgeBinAmount + GET_DOUBLE("purgeLength") > GET_DOUBLE("purgeBinCapacity"))
		{
			bufferLine("@pause", "Purge bin pause");
			purgeBinAmount = 0;
		}
	}
	writeComment("Software purge of " + std::to_string(purgeAmt) + "mm");
	static bool lastDir = false;
	if(lastTool == 0)
		writeMixerSpeed(GET_DOUBLE("purgeMixerSpeed"), true,"Purging");
	double eLength;
	double purgedAmt = 0;
	while (purgedAmt < purgeAmt)
	{
		if (lastTool == 0)
			writeMixerSpeed(GET_DOUBLE("purgeMixerSpeed"), true,"Purging");
		double xDist;
		if (lastDir == true)
			xDist = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") - xOffset - targetX;
		else
			xDist = targetX - (GET_DOUBLE("purgeLocationX") - xOffset);
		double eAmt = purgeAmt / 2.0 * (xDist / (GET_DOUBLE("purgeWidth")));
		purgedAmt += eAmt;
		if (purgedAmt > purgeAmt)
			eAmt -= (purgedAmt - purgeAmt);
		if (lastDir)
			targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") -xOffset;	//Right
		else
			targetX = GET_DOUBLE("purgeLocationX") -xOffset;								//Left
		// Adjust feedrate so extruder feedrate is the deciding factor
		double eNormalFeed = sqrt(pow((targetX+xOffset) - lastX, 2) + pow((targetY+yOffset) - lastY, 2) + pow(eAmt, 2)) / (eAmt / GET_DOUBLE("purgeFeedrate"));

		writeManualMove(targetX, targetY, lastZ, eAmt, eNormalFeed);
		lastDir = !lastDir;
	}
	purgeBinAmount += purgeAmt;
	//Goto Wipe
	writePause(GET_DOUBLE("purgeWait"));
	writeRetract(RETRACTION_TYPE::DONTWIPE);
	
	writeTravelTo(GET_DOUBLE("purgeWipeX") + GET_DOUBLE("purgeWipeWidth") / 2.0 - xOffset, targetY, lastZ,-1.0f,true);
	writeTravelTo(GET_DOUBLE("purgeWipeX") + GET_DOUBLE("purgeWipeWidth") / 2.0 - xOffset, GET_DOUBLE("purgeLocationY") - 20.0 - yOffset, lastZ, GET_DOUBLE("purgeWipeSpeed"), true);
	writeTravelTo(GET_DOUBLE("purgeWipeX") + GET_DOUBLE("purgeWipeWidth") / 2.0 - xOffset, GET_DOUBLE("purgeLocationY") - 50.0 - yOffset, lastZ, -1.0f, true);
}

void GCodeWriter::writePause(uint32_t milliseconds, std::string comment)
{
	if (milliseconds == 0)
		return;
	std::stringstream command;
	command << "G4 P" << (int)milliseconds;
	if(TIME_ESTIMATE)
		timeEstimate.addTime(milliseconds);
	bufferLine(command.str(), comment);
}

void GCodeWriter::writeManualMove(double x, double y, double z, double e, double f)
{
	x += xOffset; y += yOffset;
	// If this is an out printing move and we're retracted, unretract
	if(e + lastE > lastE && toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		writeIssue();
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("printSpeed");
	GCodeLine writeLine(lastTool,lastColour);
	writeLine.addParameter(Line::LineType::MANUAL_MOVE);
	writeLine.pt1.x = quantizeMove(lastX, AXIS::X);
	writeLine.pt1.y = quantizeMove(lastY, AXIS::Y);
	writeLine.pt1.z = quantizeMove(lastZ, AXIS::Z);

	writeLine.pt2.x = quantizeMove(x, AXIS::X);
	writeLine.pt2.y = quantizeMove(y, AXIS::Y);
	writeLine.pt2.z = quantizeMove(z, AXIS::Z);
	writeLine.e1 = quantizeMove(lastE, AXIS::E);
	double eOut = e+lastE;
	printDistanceSinceRetract += e;
	writeLine.e2 = quantizeMove(eOut, AXIS::E);
	writeLine.feedrate = f;
	if (GET_BOOL("quantizeMovement"))
	{
		if ((fabs(lastX - writeLine.pt2.x) >  1.0 / GET_DOUBLE("stepsPerMMX")) ||
			(fabs(lastY - writeLine.pt2.y) >  1.0 / GET_DOUBLE("stepsPerMMY")) ||
			(fabs(lastZ - writeLine.pt2.z) >  1.0 / GET_DOUBLE("stepsPerMMZ")) ||
			(fabs(lastE - writeLine.e2) >  1.0 / GET_DOUBLE("stepsPerMME")))
			bufferLine(writeLine);
	}
	else
		bufferLine(writeLine);
	lastX = x; lastY = y; lastZ = z; lastE = eOut;
	lastToolPosition.x = x; lastToolPosition.y = y; lastToolPosition.z = z;
	lastFeedrate = f;
	writeLinesWaiting();
}

void GCodeWriter::writeMixerSpeed(uint16_t mixerSpeed, bool isPurge, std::string comment)
{

	if (GET_BOOL("mixerOnlyUse") && !isPurge)
	{
		if (lastTool != 0)
			mixerSpeed = 0;
		if (lastColour.isSingleColour())
			mixerSpeed = 0;
	}
	if (Settings::getSingleton().getStringValue("headChoice") == "RoVa4D - CMYK+WT")
	{
		if ((int)mixerSpeed == lastMixerSpeed)
			return;
		std::stringstream command;
		command << "M106 M" << (int)mixerSpeed;
		if (isPurge)
			bufferLine(command.str(), "Mixer Purge:"+comment);
		else if (lastColour.isSingleColour())
			bufferLine(command.str(), "Single colour:"+comment);
		else
			bufferLine(command.str(), "Set mixer state:"+comment);
		lastMixerSpeed = mixerSpeed;
	}
}

void GCodeWriter::writeFanSpeed(double speed)
{
	assert(speed <= 100);
	assert(speed >= 0);
	if (Settings::getSingleton().getDoubleValue("firstLayerFanSpeed") < 1 && currentLayer == 0 ||
		!Settings::getSingleton().getBoolValue("useCoolingFan"))
		return;
	else if (currentLayer == 0)
	{
		speed = GET_DOUBLE("firstLayerFanSpeed");
	}
	if (lastFanSpeed != 255.0*speed / 100.0)
	{
		std::stringstream command;
		command << "M106 S" << (int)(255.0*speed / 100.0);
		bufferLine(command.str(),"Layer fan speed");
	}
	lastFanSpeed = 255.0*speed / 100.0;
}

void GCodeWriter::writeFanOff()
{
	if (lastFanSpeed > 0)
		bufferLine("M107", "Turn off layer fan");
	lastFanSpeed = 0;
}

void GCodeWriter::writeExtrudeTo(double x, double y, double z, double e_multiplier,double extWidth, std::shared_ptr<Layer> layer, double f)
{
	x += xOffset; y += yOffset;
	if (fabs(x - lastX) < MIN_MOVE && fabs(y - lastY) < MIN_MOVE && fabs(z - lastZ) < MIN_MOVE)
		return;
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		writeIssue();
	//Start calculating extrusion volume
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("printSpeed");
	if (currentLayer == 0)
	{
		e_multiplier *= GET_DOUBLE("firsyLayerExtrusion")/100.0;
		writeMixerSpeed(Settings::GetSingleton().getDoubleValue("firstLayerMixerSpeed"),false,"First layer");
		f = Settings::GetSingleton().getDoubleValue("firstLayerSpeed")*60.0;
	}
	else
		writeMixerSpeed(GET_DOUBLE("mixerSpeed"),false,"Normal mixer");
	double volumeToExtruder = sqrt(pow((lastX - x), 2) + pow((lastY - y), 2) + pow((lastZ - z), 2));


	volumeToExtruder *=
		((extWidth*e_multiplier) - layer->getThickness())*layer->getThickness() +
		3.141592654*pow(layer->getThickness()/ 2.0, 2);
	volumeToExtruder *= Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool)+ "extMulti");

	if (Settings::getSingleton().getBoolValue("useConstantExtrusionSpeed"))
	{
		//Figure out time needed to extude said volume
		double time = volumeToExtruder / Settings::GetSingleton().getDoubleValue("extrusionSpeed");
		//Need distance to determine feedrate
		double length = sqrt(pow(x - lastX, 2) + pow(y - lastY, 2));
		f = length / time;
	}

	//Divide by area of filament to get length to run extruder
	volumeToExtruder /= 3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) +"filamentD") / 2.0, 2);
	printDistanceSinceRetract += volumeToExtruder;
	//writeComment("Print distance since retract: " + std::to_string(printDistanceSinceRetract));
	GCodeLine writeLine(lastTool,lastColour);
	writeLine.pt1.x = quantizeMove(lastX, AXIS::X);
	writeLine.pt1.y = quantizeMove(lastY, AXIS::Y);
	writeLine.pt1.z = quantizeMove(lastZ, AXIS::Z);

	writeLine.pt2.x = quantizeMove(x, AXIS::X);
	writeLine.pt2.y = quantizeMove(y, AXIS::Y);
	writeLine.pt2.z = quantizeMove(z, AXIS::Z);
	writeLine.e1 = quantizeMove(lastE, AXIS::E);
	double eOut = lastE + volumeToExtruder;
	
	writeLine.e2 = quantizeMove(eOut, AXIS::E);

	writeLine.feedrate = f;
	if (GET_BOOL("quantizeMovement"))
	{
		if ((fabs(lastX - writeLine.pt2.x) >  1.0 / GET_DOUBLE("stepsPerMMX")) ||
			(fabs(lastY - writeLine.pt2.y) >  1.0 / GET_DOUBLE("stepsPerMMY")) ||
			(fabs(lastZ - writeLine.pt2.z) >  1.0 / GET_DOUBLE("stepsPerMMZ")) ||
			(fabs(lastE - writeLine.e2) >  1.0 / GET_DOUBLE("stepsPerMME")))
			bufferLine(writeLine);
	}
	else
		bufferLine(writeLine);
	lastX = x; lastY = y; lastZ = z; lastE = eOut;
	lastToolPosition.x = x; lastToolPosition.y = y; lastToolPosition.z = z;
	lastFeedrate = f;
	writeLinesWaiting();
}

double GCodeWriter::getExtrudeLengthTo(double x1, double y1, double x2, double y2, double z1, double z2, double e_multiplier, double extWidth, std::shared_ptr<Layer> layer) const
{
	double volumeToExtruder = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2) + pow((z1 - z2), 2));
	volumeToExtruder *=
		((extWidth*e_multiplier) - layer->getThickness())*layer->getThickness() +
		3.141592654*pow(layer->getThickness() / 2.0, 2);
	volumeToExtruder /= 3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "filamentD") / 2.0, 2);
	volumeToExtruder *= Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "extMulti");
	if(m_currentLayer == 0)
		volumeToExtruder *= GET_DOUBLE("firsyLayerExtrusion") / 100.0;
	return volumeToExtruder;
}

void GCodeWriter::writeResetE()
{
	bufferLine("G92 E0");
	lastE = 0;
}

void GCodeWriter::writeTravelXY(double x, double y, double f, bool dontRetract, unsigned int lineType)
{
	writeTravelTo(x, y, lastZ, f, dontRetract, lineType);
}

void GCodeWriter::writeTravelTo(double x, double y, double z, double f, bool dontRetract, unsigned int lineType)
{
	x += xOffset; y += yOffset;
	writeMixerSpeed(Settings::getSingleton().getDoubleValue("travelMixerSpeed"),false,"Infill mixer");
	double distanceFromLastSqrd = pow(x - lastX, 2) + pow(y - lastY, 2) + pow(z - lastZ, 2);
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("travelSpeed")*60;
	if (distanceFromLastSqrd < MIN_MOVE)//Ignore move
		return;
	if (!dontRetract && GET_BOOL("retractOnlyPerimeterCrossing"))
	{
		if (lineCrossesPerim(lastX, lastY, x, y))
		{
			//bufferLine("", "Perimter crossing retract");
			if (distanceFromLastSqrd >
				pow(Settings::GetSingleton().getDoubleValue("travelDistanceWithoutRetractT"+std::to_string(lastTool)), 2))
			{
				writeRetract(RETRACTION_TYPE::TRAVEL_MOVE);
			}
		}
	}
	else if (!dontRetract && lineType == Line::LineType::INFILL && GET_BOOL("retactInfillOnlyCrossingPerim"))
	{
		if (lineCrossesPerim(lastX, lastY, x, y))
		{
			//bufferLine("", "Perimter crossing retract");
			if (distanceFromLastSqrd >
				pow((Settings::GetSingleton().getDoubleValue("travelDistanceWithoutRetractT" + std::to_string(lastTool))), 2))
			{
				writeRetract(RETRACTION_TYPE::TRAVEL_MOVE);
			}
		}
	}
	else if (dontRetract == false && distanceFromLastSqrd >
		pow((Settings::GetSingleton().getDoubleValue("travelDistanceWithoutRetractT" + std::to_string(lastTool))), 2))
	{
		//bufferLine("","Travel retract");
		writeRetract(RETRACTION_TYPE::TRAVEL_MOVE);
	}
	GCodeLine travelLine(lastTool,lastColour);
	travelLine.addParameter(Line::LineType::TRAVEL);
	travelLine.pt1.x = quantizeMove(lastX, AXIS::X);
	travelLine.pt1.y = quantizeMove(lastY, AXIS::Y);
	travelLine.pt1.z = quantizeMove(lastZ, AXIS::Z);

	travelLine.feedrate = f;
	travelLine.e1 = quantizeMove(lastE, AXIS::E);
	travelLine.e2 = quantizeMove(lastE, AXIS::E);

	travelLine.pt2.x = quantizeMove(x, AXIS::X);
	travelLine.pt2.y = quantizeMove(y, AXIS::Y);
	if (retracted && Settings::GetSingleton().getDoubleValue("zLiftT"+std::to_string(lastTool))>0)
		z += (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)));
	travelLine.pt2.z = quantizeMove(z, AXIS::Z);

	//if (GET_BOOL("quantizeMovement"))
	//{
	//	if ((lastX != travelLine.pt2.x || lastY != travelLine.pt2.y || lastZ != travelLine.pt2.z))
	//		bufferLine(travelLine);
	//}
	//else
	bufferLine(travelLine);
	//Update values
	lastX = x; lastY = y; lastZ = z;
	lastToolPosition.x = lastX;
	lastToolPosition.y = lastY;
	lastToolPosition.z = lastZ;
	lastFeedrate = f;
}

bool GCodeWriter::lineCrossesPerim(double x1, double y1, double x2, double y2)
{
	for (auto it : slicables)
	{
		Line testLine(Vertex(x1-it->getOffsetX(), y1-it->getOffsetY()),
			Vertex(x2-it->getOffsetX(), y2 - it->getOffsetY()),0);
		auto currentLayer = it->getLayer(m_currentLayer);
		for (auto PolyIT : currentLayer->getRSPolies())
		{
			if (!PolyIT->getPerimTree()->getContour()->empty())
			{
				// Inflate the perimeter, we need to optimze this
				RoVaClipperLib::ClipperOffset offsetter;
				offsetter.AddPath(RoVaClipperLib::BodySplitterToClipperPath(*PolyIT->getPerimTree()->getContour()), RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
				RoVaClipperLib::Paths offsetPaths;
				offsetter.Execute(offsetPaths, 2);
				for (auto &offsetPathIT : offsetPaths)
				{
					auto inRsFormat = RoVaClipperLib::ClipperPathToBodySplitter(offsetPathIT,0);
					if (Line::lineCrossesContour(testLine, inRsFormat))
						return true;
				}
			}
			if (PolyIT->getPerimTree()->hasHole())
			{
				for (auto holeIT : *PolyIT->getPerimTree()->getHoles())
				{
					// Inflate the perimeter, we need to optimze this
					RoVaClipperLib::ClipperOffset offsetter;
					offsetter.AddPath(RoVaClipperLib::BodySplitterToClipperPath(holeIT), RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
					RoVaClipperLib::Paths offsetPaths;
					offsetter.Execute(offsetPaths, -2);
					for (auto &offsetPathIT : offsetPaths)
					{
						auto inRsFormat = RoVaClipperLib::ClipperPathToBodySplitter(offsetPathIT, 0);
						if (Line::lineCrossesContour(testLine, inRsFormat))
							return true;
					}
				}
			}
		}
	}
	return false;
}

void GCodeWriter::writeRetract(RETRACTION_TYPE retractType)
{
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		return;
	if (printDistanceSinceRetract < GET_DOUBLE("PrintDistanceBeforeRetractT"+ std::to_string(lastTool)))
		return;
	printDistanceSinceRetract = 0;
	// Dont retract on unset tool
	if (lastTool == std::numeric_limits<uint16_t>::max())
		return;
	if (retractType == RETRACTION_TYPE::TOOL_CHANGE)
		writeComment("Tool change retraction\n");
	if(Settings::GetSingleton().getStringValue("onRetractGCode").length() > 1)
		bufferLine(Settings::GetSingleton().getStringValue("onRetractGCode"));
	if (Settings::GetSingleton().getBoolValue("useSoftwareRetraction"))
	{
		writeComment("E is at: " + std::to_string(lastE));
		//writeCoast();
		if (GET_BOOL("fanOffDuringRetract"))
		{
			retractFanSpeedCache = (double)lastFanSpeed / 255.0*100.0;
			writeFanOff();
		}
		GCodeLine retractLine(lastTool, lastColour);
		retractLine.addParameter(Line::LineType::RETRACT);
		retractLine.e1 = quantizeMove(lastE, AXIS::E);
		retractLine.pt1.z = retractLine.pt2.z = quantizeMove(lastZ, AXIS::Z);
		double retractAmount;
		if(retractType == RETRACTION_TYPE::TOOL_CHANGE)
			retractAmount = Settings::getSingleton().getDoubleValue("toolChangeRetractDistanceT" + std::to_string(lastTool));
		else
			retractAmount = Settings::getSingleton().getDoubleValue("retractAmountT" + std::to_string(lastTool));
		lastE -= retractAmount;
		retractLine.e2 = quantizeMove(lastE, AXIS::E);
		retractLine.feedrate = Settings::getSingleton().getDoubleValue("retractFeedrateT" + std::to_string(lastTool));
		//retractLine.comment = "Retraction";
		// TESTED - SYRINGE LIFT BEFORE RETRACTION IS BEST 10/26/17 jk
		//if (Settings::GetSingleton().getBoolValue("useServo"))
		//	writeSyringeLift();
		if (Settings::GetSingleton().getDoubleValue("wipeDistanceT" + std::to_string(lastTool)) > 0.0 && retractType != RETRACTION_TYPE::DONTWIPE)
		{
			auto wipePath = getWipePath();
			if (wipePath.size() > 1)
			{
				//retractLine.addParameter(Line::LineType::RETRACT_WIPE);
				bufferLine(retractLine);
				for (auto it : wipePath)
					bufferLine(it);
				//bufferLine("G4 E", "wait for wipe");
				lastRetractWasWipe = true;
				
			}
			else
			{
				bufferLine(retractLine);
			}
			//std::stringstream simulRetract;
			//std::fixed(simulRetract);
			//simulRetract << std::setprecision(GCODE_PRECISION_E);
			//simulRetract << "G10 E" << retractAmount;
			//simulRetract << std::setprecision(GCODE_PRECISION_F);
			//simulRetract << " F" << Settings::getSingleton().getDoubleValue("retractFeedrateT" + std::to_string(lastTool));
			//bufferLine(simulRetract.str(), "wipe retract");
		}
		else
			bufferLine(retractLine);

		if (Settings::GetSingleton().getBoolValue("useServo"))
			writeSyringeLift();

		if (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)) > 0)
		{
			GCodeLine lift(lastTool,lastColour);
			lift.addParameter(Line::LineType::Z_MOVE);
			lift.pt1.x = quantizeMove(lastX, AXIS::X);
			lift.pt1.y = quantizeMove(lastY, AXIS::Y);
			lift.pt1.z = quantizeMove(lastZ, AXIS::Z);
			lift.e1 = lift.e2 = quantizeMove(lastE, AXIS::E);
			lift.feedrate = Settings::GetSingleton().getDoubleValue("zLiftFeedT" + std::to_string(lastTool))*60;
			lift.comment = "zLift";

			lift.pt2.x = quantizeMove(lastX, AXIS::X);
			lift.pt2.y = quantizeMove(lastY, AXIS::Y);
			lift.pt2.z = quantizeMove(lastZ + Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)), AXIS::Z);
			lastZ += Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool));
			bufferLine(lift);
		}
	}
	else
		bufferLine("G10");
	toolRetractionState[lastTool] = retractType;
	retracted = true;
}

void GCodeWriter::writeIssue()
{
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
	{
		writeComment("E is at: " + std::to_string(lastE));
		if (toolRetractionState[lastTool] == RETRACTION_TYPE::TOOL_CHANGE)
			writeComment("Tool change issue");
		else
			writeComment("Other issue");
		double issueAmt = 0;
		if(toolRetractionState[lastTool] == RETRACTION_TYPE::TOOL_CHANGE)
			issueAmt = Settings::getSingleton().getDoubleValue("toolChangeIssueDistanceT" + std::to_string(lastTool));
		else
			issueAmt = Settings::getSingleton().getDoubleValue("issueAmountT" + std::to_string(lastTool));
		if(Settings::GetSingleton().getStringValue("onIssueGCode").length() > 1)
			bufferLine(Settings::GetSingleton().getStringValue("onIssueGCode"));
		if (Settings::GetSingleton().getBoolValue("useSoftwareRetraction"))
		{

			if (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)) > 0)
			{
				GCodeLine zUnlift(lastTool,lastColour);
				zUnlift.comment = "Z unlift";
				zUnlift.e1 = zUnlift.e2 = lastE;
				zUnlift.pt1.x = quantizeMove(lastX, AXIS::X);
				zUnlift.pt1.y = quantizeMove(lastY, AXIS::Y);
				zUnlift.pt1.z = quantizeMove(lastZ, AXIS::Z);
				zUnlift.addParameter(Line::LineType::Z_MOVE);
				zUnlift.pt2.x = quantizeMove(lastX, AXIS::X);
				zUnlift.pt2.y = quantizeMove(lastY, AXIS::Y);
				zUnlift.pt2.z = quantizeMove(lastZ - Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)), AXIS::Z);
				lastZ -= Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool));
				zUnlift.feedrate = Settings::GetSingleton().getDoubleValue("zLiftFeedT" + std::to_string(lastTool)) * 60.0;
				bufferLine(zUnlift);
			}
			if (Settings::GetSingleton().getBoolValue("useServo"))
				writeSyringeIssue();
			if (GET_BOOL("fanOffDuringRetract"))
			{
				writeFanSpeed(retractFanSpeedCache);
			}
			GCodeLine issue(lastTool,lastColour);
			issue.addParameter(Line::LineType::ISSUE);
			issue.e1 = quantizeMove(lastE, AXIS::E);
			lastE += issueAmt;
			issue.e2 = quantizeMove(lastE, AXIS::E);
			issue.feedrate = Settings::getSingleton().getDoubleValue("issueFeedrateT" + std::to_string(lastTool));
			writeComment("Issue line E1: " + std::to_string(issue.e1) + " E2: " + std::to_string(issue.e2));
			if (lastRetractWasWipe)
			{
				lastRetractWasWipe = false;
			//	issue.addParameter(Line::LineType::RETRACT_WIPE);
				bufferLine(issue);
			//	bufferLine("G4 E", "wait for issue");
			}
			else
				bufferLine(issue);
			writeColourReturn();
		}
		else
			bufferLine("G11");
		retracted = false;
		toolRetractionState[lastTool] = RETRACTION_TYPE::NO_RETRACTION;
	}
}

void GCodeWriter::writeSyringeIssue()
{
	if (Settings::GetSingleton().getBoolValue("useServo"))
	{
		bufferLine("M280 P" + std::to_string(GET_DOUBLE("syringeServoIndex")) + " S" + std::to_string((int)GET_DOUBLE("servoIssue")), "Syringe movemnet");
		if (GET_DOUBLE("servoIssueTime") >= 1.0)
			writePause(((int)GET_DOUBLE("servoIssueTime")), "Wait for servo issue");
	}
}

void GCodeWriter::writeSyringeLift()
{
	if (Settings::GetSingleton().getBoolValue("useServo"))
	{
		bufferLine("M280 P" + std::to_string(GET_DOUBLE("syringeServoIndex")) + " S" + std::to_string((int)GET_DOUBLE("servoLift")),"Syringe movemnet");
		if (GET_DOUBLE("servoLiftTime") >= 1.0)
			writePause((int)GET_DOUBLE("servoLiftTime"), "Wait for servo lift");
	}
}

void GCodeWriter::writeColourEven()
{
	if (Settings::getSingleton().getBoolValue("retractAllActive"))
	{
		std::string command = "M3021";
		bufferLine(command, "Set all extruders to retract equal");
	}
}

void GCodeWriter::writeColourReturn()
{
	if (Settings::getSingleton().getBoolValue("retractAllActive"))
	{
		std::string command = "M3022";
		bufferLine(command,"Return to the unequal colour");
	}
}

void GCodeWriter::writeComment(const std::string &comment)
{
	if (Settings::GetSingleton().getBoolValue("addDebugComments"))
	{
		bufferLine(" ", comment);
	}
}

std::vector<Line> GCodeWriter::arrangeLineLoop(const std::vector<Line> &inputLoop, const Vertex &closestPos) const
{
	std::vector<Line> output;

	//Find starting point in the loop
	double closestDistanceSquared = inputLoop.front().pt1.distanceToSquared(closestPos);
	unsigned int closestLineIndex = 0;
	for (unsigned int i = 1; i < inputLoop.size(); i++)
	{
		if (inputLoop[i].pt1.distanceToSquared(closestPos) < closestDistanceSquared)
		{
			closestLineIndex = i;
			closestDistanceSquared = inputLoop[i].pt1.distanceToSquared(closestPos);
		}
	}
	//Now we have the starting position
	for (int i = 0; i < inputLoop.size(); i++)
	{
		output.push_back(inputLoop[closestLineIndex++]);
		if (closestLineIndex == inputLoop.size())
			closestLineIndex = 0;
	}
	return output;
}

std::vector<Line> GCodeWriter::arrangeInfill(const std::vector<Line> &fillLines, const Vertex &closestPt) const
{
	std::vector<Line> output;
	std::vector<Line> input = fillLines;
	Vertex lastPt = closestPt;
	while (input.size() > 0)
	{
		double closestPtDistance = input.front().pt1.distanceToSquared(lastPt);
		unsigned int closestIndex = 0;
		//output.push_back(input.front());

		for (unsigned int i = 1; i < input.size(); i++)
		{
			if (input[i].pt1.distanceToSquared(lastPt) < closestPtDistance)
			{
				closestPtDistance = input[i].pt1.distanceToSquared(lastPt);
				closestIndex = i;
			}
		}
		output.push_back(input[closestIndex]);
		lastPt = output.back().pt2;
		input.erase(input.begin() + closestIndex);
	}
	return output;
}

std::deque<GCodeLine> GCodeWriter::getWipePath()
{
	std::deque<std::pair<std::string,std::shared_ptr<GCodeLine>>> *activeLineBuffer;
	if (isUsingLayerLineBuffer)
		activeLineBuffer = &layerLineBuffer;
	else
		activeLineBuffer = &lineBuffer;
	if (activeLineBuffer->size() == 0)
		return std::deque<GCodeLine>();
	double distanceToWipe = Settings::GetSingleton().getDoubleValue("wipeDistanceT" + std::to_string(lastTool));
	double distanceFound = 0;
	//double lastE;
	//Find the last extrusion lines to wipe alon
	
	// Check if the wipe is possible
	for (auto lastLineIT = activeLineBuffer->rbegin(); lastLineIT != activeLineBuffer->rend(); ++lastLineIT)
	{
		if ((*lastLineIT).second == nullptr)	//Travel or existing retract breaking things
			continue;
		if ((*lastLineIT).second->isLineOfType(Line::LineType::TRAVEL) || (*lastLineIT).second->isLineOfType(Line::LineType::MANUAL_MOVE))
			break;
		//if (distanceFound == 0)
		//	lastE = lastLineIT->second->e2;
		distanceFound += lastLineIT->second->length();
		if (distanceFound >= distanceToWipe)
		{
			break;
		}
	}
	//if (distanceFound < distanceToWipe)
	//{	// Wipe not possible
	//	return;
	//}
	distanceFound = 0;
	double retractDistanceToGo = GET_DOUBLE("retractAmountT" + std::to_string(lastTool));
	std::deque<GCodeLine> wipePath;
	int i = 0;
	for (auto lastLineIT = activeLineBuffer->rbegin(); lastLineIT != activeLineBuffer->rend(); ++lastLineIT)
	{
		if ((*lastLineIT).second == nullptr)	//Travel or existing retract breaking things
			continue;
		if ((*lastLineIT).second->isLineOfType(Line::LineType::TRAVEL) || (*lastLineIT).second->isLineOfType(Line::LineType::MANUAL_MOVE))
			break;
		GCodeLine tempLine(*(*lastLineIT).second);
		
		tempLine.reverse();
		tempLine.comment = "wipe line: "+std::to_string(i++);
		tempLine.feedrate = Settings::GetSingleton().getDoubleValue("wipeFeedrateT" + std::to_string(lastTool));
		tempLine.addParameter(Line::LineType::COAST);
		//double lineTime = tempLine.length() / tempLine.feedrate;
		//double possibleRetractionDistance = lineTime * GET_DOUBLE("retractFeedrate");
		//if (possibleRetractionDistance > retractDistanceToGo && retractDistanceToGo > 0.1)
		//{
		//	Line outA, outB;
		//	tempLine.splitLine(retractDistanceToGo / possibleRetractionDistance*tempLine.length(), outA, outB);
		//	GCodeLine outGCodeA(outA);
		//	GCodeLine outGCodeB(outB);
		//	outGCodeA.feedrate = tempLine.feedrate;
		//	outGCodeB.feedrate = tempLine.feedrate;
		//	outGCodeA.e1 = lastE;
		//	outGCodeA.e2 = lastE - retractDistanceToGo;
		//	lastE -= retractDistanceToGo;
		//	retractDistanceToGo = 0;
		//	outGCodeB.e1 = lastE;
		//	outGCodeB.e2 = lastE;
		//	outGCodeA.comment = "WipeAndRetractA, wipe line: " + std::to_string(i - 1);
		//	outGCodeB.comment = "WipeAndRetractB";
		//	wipePath.push_back(outGCodeA);
		//	wipePath.push_back(outGCodeB);
		//}
		//else
		{
			tempLine.e1 = lastE;
			//if (retractDistanceToGo > 0.1)
			//{
			//	tempLine.e2 = lastE - possibleRetractionDistance;
			//	lastE -= possibleRetractionDistance;
			//	retractDistanceToGo -= possibleRetractionDistance;
			//	tempLine.feedrate *= tempLine.length() / tempLine.fullLength();
			//}
			//else
				tempLine.e2 = lastE;
			wipePath.push_back(tempLine);
		}
		distanceFound += tempLine.length();
		if (distanceFound >= distanceToWipe)
			break;
	}
	//if(retractDistanceToGo > 0)
	//{
	//	GCodeLine retractLine(lastTool, lastColour);
	//	retractLine.addParameter(Line::LineType::RETRACT);
	//	retractLine.e1 = quantizeMove(lastE, AXIS::E);
	//	retractLine.pt1.z = retractLine.pt2.z = quantizeMove(lastZ, AXIS::Z);
	//	lastE -= retractDistanceToGo;
	//	retractLine.e2 = quantizeMove(lastE, AXIS::E);
	//	retractLine.feedrate = Settings::getSingleton().getDoubleValue("retractFeedrate");
	//	wipePath.push_back(retractLine);
	//}
	//activeLineBuffer->back().second->comment = "Start of wipe";
	//activeLineBuffer->insert(activeLineBuffer->end(), wipePath.begin(), wipePath.end());
	return wipePath;
	/*
	for (auto it : wipePath)
		bufferLine(it);
	if(i > 1)
		bufferLine("G4 E", "wait for wipe");
		*/
}

void GCodeWriter::writeLinesWaiting()
{
	return;
	double lengthToEx = 0;
	for (auto it : lineBuffer)
	{

		if (it.second)
		{
			if (!it.second->isLineOfType(Line::LineType::TRAVEL))
			{
				lengthToEx += it.second->length();
			}
		}
	}
	if (lengthToEx > Settings::getSingleton().getDoubleValue("coastLengthT" + std::to_string(lastTool)) * 2)
	{
		//We can coast!
		double lengthExtruded = 0;
		while (lengthExtruded < Settings::getSingleton().getDoubleValue("coastLengthT" + std::to_string(lastTool)))
		{
			if (lineBuffer.front().second)
			{
				writeFormattedLine(lineBuffer.front().second);
				lengthExtruded += lineBuffer.front().second->length();
			}
			else
			{
#ifndef _CONSOLE
				if (m_ptrMachine)
				{
					m_ptrMachine->queueLines(lineBuffer.front().first);
				}
				else
#endif
					outputStream << lineBuffer.front().first;
			}
			lineBuffer.pop_front();
		}
	}
}

double GCodeWriter::quantizeMove(double in, const AXIS axis) const
{
	if (!GET_BOOL("quantizeMovement"))
		return in;

	double smallestStep = 1.0;
	if (axis == AXIS::X)
		smallestStep /= GET_DOUBLE("stepsPerMMX");
	else if (axis == AXIS::Y)
		smallestStep /= GET_DOUBLE("stepsPerMMY");
	else if (axis == AXIS::Z)
		smallestStep /= GET_DOUBLE("stepsPerMMZ");
	else if (axis == AXIS::E)
		smallestStep /= GET_DOUBLE("stepsPerMME");
	else
		assert(1 == 2);

	double remainderVal = fmod(in, smallestStep);
	if (remainderVal > smallestStep / 2.0) //poor mans rounding
	{
		return in += (smallestStep - remainderVal);
	}
	else
		return in -= remainderVal;
}

void GCodeWriter::writeSuspend()
{

	if (GET_BOOL("travelToPurgeOnSuspend"))
	{
		double targetX = GET_DOUBLE("purgeLocationX") + GET_DOUBLE("purgeWidth") * ((double)rand() / (double)(RAND_MAX)) - xOffset;
		double targetY = GET_DOUBLE("purgeLocationY") + GET_DOUBLE("purgeHeight") / 2.0 - yOffset;
		writeTravelTo(targetX, targetY, lastZ, -1.0, true);
	}
	bufferLine(Settings::GetSingleton().getStringValue("onSuspend"));
	if (isUsingLayerLineBuffer && layerLineBuffer.empty() == false)
	{
		lineBuffer.insert(lineBuffer.end(), layerLineBuffer.begin(), layerLineBuffer.end());
		layerLineBuffer.clear();
	}
	if (outputStream)
	{
		writeAllBufferedLines();

		outputStream.flush();
		outputStream.close();
	}
	timeEstimate.resetGCodeSplit();
}

void GCodeWriter::writeResume()
{
	outputStream.open(outputFileNameSplit+ std::to_string(filePartCounter++)+".gcode");
	bufferLine(Settings::GetSingleton().getStringValue("onResume"));
	bufferLine("T" + std::to_string(lastTool));
	bufferLine(getColourString(lastColour));
	bufferLine("G92 Z" + std::to_string(lastZ));
	if (GET_BOOL("purgeOnResume"))
	{
		if (GET_BOOL("useSoftwarePurge") == false ||
			Settings::getSingleton().getStringValue("headChoice") != "RoVa4D - CMYK+WT")
			return;
		writePurge(GET_DOUBLE("purgeLength"), -1, -1);
	}
}