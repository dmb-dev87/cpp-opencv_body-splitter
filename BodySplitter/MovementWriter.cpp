#include "MovementWriter.h"

#include "Settings.h"
#include "Line.h"
#include "PrintBed.h"
#include "iSlicable.h"
#include "Layer.h"
/*
using GCodeLinePair = std::pair<std::string, std::shared_ptr<GCodeLine>>;

MovementWriter::MovementWriter(std::shared_ptr<Layer> layer) : currentLayer(layer)
{
}


MovementWriter::~MovementWriter()
{
}

void MovementWriter::writeZTravelTo(GCodeLinePath &result, double z, double f)
{
	if (lastZ != z)
	{
		if (Settings::getSingleton().getBoolValue("retractOnLayerChange"))
		{
			writeRetract(result,RETRACTION_TYPE::TRAVEL_MOVE);
		}
		GCodeLine zTravel(lastTool, lastColour);
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
			f = Settings::getSingleton().getDoubleValue("travelSpeed") * 60;

		zTravel.feedrate = f;

		result.push_back(GCodeLinePair("",std::make_shared<GCodeLine>(zTravel)));
		lastFeedrate = f;
	}
}

void MovementWriter::writeTravelXY(GCodeLinePath &result, double x,
	double y, double f, bool dontRetract, unsigned int lineType)
{
	writeTravelTo(result,x, y, lastZ, f, dontRetract, lineType);
}

void MovementWriter::writeTravelTo(GCodeLinePath &result, double x, double y, double z, double f, bool dontRetract, unsigned int lineType)
{
	//x += xOffset; y += yOffset;
	//writeMixerSpeed(Settings::getSingleton().getDoubleValue("travelMixerSpeed"), false, "Infill mixer");
	double distanceFromLastSqrd = pow(x - lastX, 2) + pow(y - lastY, 2) + pow(z - lastZ, 2);
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("travelSpeed") * 60;
	if (distanceFromLastSqrd < MIN_MOVE)//Ignore move
		return ;
	if (!dontRetract && GET_BOOL("retractOnlyPerimeterCrossing"))
	{
		if (lineCrossesPerim(lastX, lastY, x, y))
		{
			//bufferLine("", "Perimter crossing retract");
			if (distanceFromLastSqrd >
				pow(Settings::GetSingleton().getDoubleValue("travelDistanceWithoutRetractT" + std::to_string(lastTool)), 2))
			{
				writeRetract(result,RETRACTION_TYPE::TRAVEL_MOVE);
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
				writeRetract(result,RETRACTION_TYPE::TRAVEL_MOVE);
			}
		}
	}
	else if (dontRetract == false && distanceFromLastSqrd >
		pow((Settings::GetSingleton().getDoubleValue("travelDistanceWithoutRetractT" + std::to_string(lastTool))), 2))
	{
		//bufferLine("","Travel retract");
		writeRetract(result,RETRACTION_TYPE::TRAVEL_MOVE);
	}
	GCodeLine travelLine(lastTool, lastColour);
	travelLine.addParameter(Line::LineType::TRAVEL);
	travelLine.pt1.x = quantizeMove(lastX, AXIS::X);
	travelLine.pt1.y = quantizeMove(lastY, AXIS::Y);
	travelLine.pt1.z = quantizeMove(lastZ, AXIS::Z);

	travelLine.feedrate = f;
	travelLine.e1 = quantizeMove(lastE, AXIS::E);
	travelLine.e2 = quantizeMove(lastE, AXIS::E);

	travelLine.pt2.x = quantizeMove(x, AXIS::X);
	travelLine.pt2.y = quantizeMove(y, AXIS::Y);
	if (retracted && Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool))>0)
		z += (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)));
	travelLine.pt2.z = quantizeMove(z, AXIS::Z);

	//if (GET_BOOL("quantizeMovement"))
	//{
	//	if ((lastX != travelLine.pt2.x || lastY != travelLine.pt2.y || lastZ != travelLine.pt2.z))
	//		bufferLine(travelLine);
	//}
	//else
	result.push_back(GCodeLinePair("",std::make_shared<GCodeLine>(travelLine)));
	//Update values
	lastX = x; lastY = y; lastZ = z;
	lastFeedrate = f;

	return;
}

void MovementWriter::writeExtrudeTo(GCodeLinePath &result, double x, double y, double z, double e_multiplier, double extWidth, double f)
{
	

	x += xOffset; y += yOffset;
	if (fabs(x - lastX) < MIN_MOVE && fabs(y - lastY) < MIN_MOVE && fabs(z - lastZ) < MIN_MOVE)
		return;
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		writeIssue(result);
	//Start calculating extrusion volume
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("printSpeed");
	if (currentLayer == 0)
	{
		e_multiplier *= GET_DOUBLE("firsyLayerExtrusion") / 100.0;
		//writeMixerSpeed(Settings::GetSingleton().getDoubleValue("firstLayerMixerSpeed"), false, "First layer");
		f = Settings::GetSingleton().getDoubleValue("firstLayerSpeed")*60.0;
	}
	//else
		//writeMixerSpeed(GET_DOUBLE("mixerSpeed"), false, "Normal mixer");
	double volumeToExtruder = sqrt(pow((lastX - x), 2) + pow((lastY - y), 2) + pow((lastZ - z), 2));


	volumeToExtruder *=
		((extWidth*e_multiplier) - currentLayer->getThickness())*currentLayer->getThickness() +
		3.141592654*pow(currentLayer->getThickness() / 2.0, 2);
	volumeToExtruder *= Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "extMulti");

	if (Settings::getSingleton().getBoolValue("useConstantExtrusionSpeed"))
	{
		//Figure out time needed to extude said volume
		double time = volumeToExtruder / Settings::GetSingleton().getDoubleValue("extrusionSpeed");
		//Need distance to determine feedrate
		double length = sqrt(pow(x - lastX, 2) + pow(y - lastY, 2));
		f = length / time;
	}

	//Divide by area of filament to get length to run extruder
	volumeToExtruder /= 3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "filamentD") / 2.0, 2);
	printDistanceSinceRetract += volumeToExtruder;
	//writeComment("Print distance since retract: " + std::to_string(printDistanceSinceRetract));
	GCodeLine writeLine(lastTool, lastColour);
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

	result.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(writeLine)));

	lastX = x; lastY = y; lastZ = z; lastE = eOut;
	lastFeedrate = f;

	return;
}

double MovementWriter::getExtrudeLengthTo(double x1, double y1, double x2, double y2, double z1, double z2, double e_multiplier, double extWidth) const
{
	double volumeToExtruder = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2) + pow((z1 - z2), 2));
	volumeToExtruder *=
		((extWidth*e_multiplier) - currentLayer->getThickness())*currentLayer->getThickness() +
		3.141592654*pow(currentLayer->getThickness() / 2.0, 2);
	volumeToExtruder /= 3.141592654*pow(Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "filamentD") / 2.0, 2);
	volumeToExtruder *= Settings::GetSingleton().getDoubleValue("T" + std::to_string(lastTool) + "extMulti");
	//if (m_currentLayer == 0)
		//volumeToExtruder *= GET_DOUBLE("firsyLayerExtrusion") / 100.0;
	return volumeToExtruder;
}

void MovementWriter::writeManualMove(GCodeLinePath &result, double x, double y, double z, double e, double f)
{
	x += xOffset; y += yOffset;
	// If this is an out printing move and we're retracted, unretract
	if (e + lastE > lastE && toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		writeIssue();
	if (f < 0)
		f = Settings::getSingleton().getDoubleValue("printSpeed");
	GCodeLine writeLine(lastTool, lastColour);
	writeLine.addParameter(Line::LineType::MANUAL_MOVE);
	writeLine.pt1.x = quantizeMove(lastX, AXIS::X);
	writeLine.pt1.y = quantizeMove(lastY, AXIS::Y);
	writeLine.pt1.z = quantizeMove(lastZ, AXIS::Z);

	writeLine.pt2.x = quantizeMove(x, AXIS::X);
	writeLine.pt2.y = quantizeMove(y, AXIS::Y);
	writeLine.pt2.z = quantizeMove(z, AXIS::Z);
	writeLine.e1 = quantizeMove(lastE, AXIS::E);
	double eOut = e + lastE;
	printDistanceSinceRetract += e;
	writeLine.e2 = quantizeMove(eOut, AXIS::E);
	writeLine.feedrate = f;
	result.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(writeLine)));
	lastX = x; lastY = y; lastZ = z; lastE = eOut;
	lastFeedrate = f;
}

void MovementWriter::writeRetract(GCodeLinePath &result, RETRACTION_TYPE retractType)
{
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
		return;
	if (printDistanceSinceRetract < GET_DOUBLE("PrintDistanceBeforeRetractT" + std::to_string(lastTool)))
		return;
	printDistanceSinceRetract = 0;
	// Dont retract on unset tool
	if (lastTool == std::numeric_limits<uint16_t>::max())
		return;
	//if (retractType == RETRACTION_TYPE::TOOL_CHANGE)
	//	writeComment("Tool change retraction\n");
	if (Settings::GetSingleton().getStringValue("onRetractGCode").length() > 1)
		result.push_back(GCodeLinePair(Settings::GetSingleton().getStringValue("onRetractGCode"),nullptr));
	if (Settings::GetSingleton().getBoolValue("useSoftwareRetraction"))
	{
		//writeComment("E is at: " + std::to_string(lastE));
		//writeCoast();
		if (GET_BOOL("fanOffDuringRetract"))
		{
			//retractFanSpeedCache = (double)lastFanSpeed / 255.0*100.0;
			//writeFanOff();
		}
		//if (Settings::GetSingleton().getBoolValue("useServo"))
		//	writeSyringeLift();
		GCodeLine retractLine(lastTool, lastColour);
		retractLine.addParameter(Line::LineType::RETRACT);
		retractLine.e1 = quantizeMove(lastE, AXIS::E);
		retractLine.pt1.z = retractLine.pt2.z = quantizeMove(lastZ, AXIS::Z);
		double retractAmount;
		if (retractType == RETRACTION_TYPE::TOOL_CHANGE)
			retractAmount = Settings::getSingleton().getDoubleValue("toolChangeRetractDistanceT" + std::to_string(lastTool));
		else
			retractAmount = Settings::getSingleton().getDoubleValue("retractAmountT" + std::to_string(lastTool));
		lastE -= retractAmount;
		retractLine.e2 = quantizeMove(lastE, AXIS::E);
		retractLine.feedrate = Settings::getSingleton().getDoubleValue("retractFeedrateT" + std::to_string(lastTool));
		//retractLine.comment = "Retraction";

		if (Settings::GetSingleton().getDoubleValue("wipeDistanceT" + std::to_string(lastTool)) > 0.0 && retractType != RETRACTION_TYPE::DONTWIPE)
		{
			//auto wipePath = getWipePath();
			//if (wipePath.size() > 1)
			//{
			//	//retractLine.addParameter(Line::LineType::RETRACT_WIPE);
			//	resultPath.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(retractLine)));
			//	for (auto it : wipePath)
			//		resultPath.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(it)));
			//	//bufferLine("G4 E", "wait for wipe");
			//	lastRetractWasWipe = true;
			//}
			//else
				result.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(retractLine)));
			//std::stringstream simulRetract;
			//std::fixed(simulRetract);
			//simulRetract << std::setprecision(GCODE_PRECISION_E);
			//simulRetract << "G10 E" << retractAmount;
			//simulRetract << std::setprecision(GCODE_PRECISION_F);
			//simulRetract << " F" << Settings::getSingleton().getDoubleValue("retractFeedrateT" + std::to_string(lastTool));
			//bufferLine(simulRetract.str(), "wipe retract");
		}
		else
			result.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(retractLine)));

		if (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)) > 0)
		{
			GCodeLine lift(lastTool, lastColour);
			lift.addParameter(Line::LineType::Z_MOVE);
			lift.pt1.x = quantizeMove(lastX, AXIS::X);
			lift.pt1.y = quantizeMove(lastY, AXIS::Y);
			lift.pt1.z = quantizeMove(lastZ, AXIS::Z);
			lift.e1 = lift.e2 = quantizeMove(lastE, AXIS::E);
			lift.feedrate = Settings::GetSingleton().getDoubleValue("zLiftFeedT" + std::to_string(lastTool)) * 60;
			lift.comment = "zLift";

			lift.pt2.x = quantizeMove(lastX, AXIS::X);
			lift.pt2.y = quantizeMove(lastY, AXIS::Y);
			lift.pt2.z = quantizeMove(lastZ + Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)), AXIS::Z);
			lastZ += Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool));
			result.push_back(GCodeLinePair("", std::make_shared<GCodeLine>(lift)));
		}
	}
	else
		result.push_back(GCodeLinePair("G10",nullptr));
	toolRetractionState[lastTool] = retractType;
	retracted = true;

	return;
}

void MovementWriter::writeIssue(GCodeLinePath &result)
{
	if (toolRetractionState[lastTool] != RETRACTION_TYPE::NO_RETRACTION)
	{
		//writeComment("E is at: " + std::to_string(lastE));
		//if (toolRetractionState[lastTool] == RETRACTION_TYPE::TOOL_CHANGE)
		//	writeComment("Tool change issue");
		//else
		//	writeComment("Other issue");
		double issueAmt = 0;
		if (toolRetractionState[lastTool] == RETRACTION_TYPE::TOOL_CHANGE)
			issueAmt = Settings::getSingleton().getDoubleValue("toolChangeIssueDistanceT" + std::to_string(lastTool));
		else
			issueAmt = Settings::getSingleton().getDoubleValue("issueAmountT" + std::to_string(lastTool));
		if (Settings::GetSingleton().getStringValue("onIssueGCode").length() > 1)
			result.push_back(GCodeLinePair(Settings::GetSingleton().getStringValue("onIssueGCode"),nullptr));
		if (Settings::GetSingleton().getBoolValue("useSoftwareRetraction"))
		{
			if (Settings::GetSingleton().getBoolValue("useServo"))
				writeSyringeIssue(result);
			if (Settings::GetSingleton().getDoubleValue("zLiftT" + std::to_string(lastTool)) > 0)
			{
				GCodeLine zUnlift(lastTool, lastColour);
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
				result.push_back(GCodeLinePair(std::string(), std::make_shared<GCodeLine>(zUnlift)));
			}
			if (GET_BOOL("fanOffDuringRetract"))
			{
				//writeFanSpeed(retractFanSpeedCache);
			}
			GCodeLine issue(lastTool, lastColour);
			issue.addParameter(Line::LineType::ISSUE);
			issue.e1 = quantizeMove(lastE, AXIS::E);
			lastE += issueAmt;
			issue.e2 = quantizeMove(lastE, AXIS::E);
			issue.feedrate = Settings::getSingleton().getDoubleValue("issueFeedrateT" + std::to_string(lastTool));
			//writeComment("Issue line E1: " + std::to_string(issue.e1) + " E2: " + std::to_string(issue.e2));
			if (lastRetractWasWipe)
			{
				lastRetractWasWipe = false;
				//	issue.addParameter(Line::LineType::RETRACT_WIPE);
				result.push_back(GCodeLinePair(std::string(), std::make_shared<GCodeLine>(issue)));
				//	bufferLine("G4 E", "wait for issue");
			}
			else
				result.push_back(GCodeLinePair(std::string(), std::make_shared<GCodeLine>(issue)));
			//writeColourReturn();
		}
		else
			result.push_back(GCodeLinePair("G11", nullptr));
		retracted = false;
		toolRetractionState[lastTool] = RETRACTION_TYPE::NO_RETRACTION;
	}
}

bool MovementWriter::lineCrossesPerim(double x1, double y1, double x2, double y2)
{
	for (auto it : PrintBed::GetSingleton().getDrawables())
	{
		std::shared_ptr<iSlicable> slicableIter = std::dynamic_pointer_cast<iSlicable>(it);
		if (!slicableIter)
			continue;
		Line testLine(Vertex(x1 - slicableIter->getOffsetX(), y1 - slicableIter->getOffsetY()),
			Vertex(x2 - slicableIter->getOffsetX(), y2 - slicableIter->getOffsetY()), 0);
		for (auto PolyIT : currentLayer->getRSPolies())
		{
			if (!PolyIT->getPerimTree()->getContour()->empty())
			{
				// Inflate the perimeter, we need to optimze this
				RoVaClipperLib::ClipperOffset offsetter;
				offsetter.AddPath(RoVaClipperLib::BodySplitterToClipperPath(*PolyIT->getPerimTree()->getContour()), RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
				RoVaClipperLib::Paths offsetPaths;
				offsetter.Execute(offsetPaths, 1);
				for (auto &offsetPathIT : offsetPaths)
				{
					auto inRsFormat = RoVaClipperLib::ClipperPathToBodySplitter(offsetPathIT, 0);
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
					offsetter.Execute(offsetPaths, -1);
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

double MovementWriter::quantizeMove(double in, const AXIS axis) const
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

*/