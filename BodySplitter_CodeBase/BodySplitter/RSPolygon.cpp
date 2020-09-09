#include <opencv2/core/core.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include "RSPolygon.h"
#include "Settings.h"
#include "Infill.h"
#include "colour.h"
#include "colourGrid.h"

//Source is from CoasterPrint
RSPolygon::RSPolygon(const std::vector<cv::Point> &pointSource, RSPolygon *inParent) : parent(inParent)
{
	if (parent)
	{
		isHole = !parent->isHole;
	}
	else
		isHole = false;	//No parent, orphans are never holes
	double scaler = Settings::getSingleton().getDoubleValue("coasterSize") / 100.0;
	Vertex pt1((double)pointSource.front().x*scaler, (double)pointSource.front().y*scaler, 0);
	Vertex pt2((double)pointSource[1].x*scaler, (double)pointSource[1].y*scaler, 0);
	minX = std::min((double)pointSource[0].x*scaler, (double)pointSource[1].x*scaler);
	maxX = std::max((double)pointSource[0].x*scaler, (double)pointSource[1].x*scaler);

	minY = std::min((double)pointSource[0].y*scaler, (double)pointSource[1].y*scaler);
	maxY = std::max((double)pointSource[0].y*scaler, (double)pointSource[1].y*scaler);
	Line line1(pt1, pt2, 0);
	m_lines.push_back(line1);
	for (uint i = 2; i < pointSource.size(); i++)
	{
		pt1 = pt2;
		pt2 = Vertex((double)pointSource[i].x*scaler, (double)pointSource[i].y*scaler, 0);
		minX = std::min(minX, (double)pointSource[i].x*scaler); maxX = std::max(minX, (double)pointSource[i].x*scaler);
		minY = std::min(minY, (double)pointSource[i].y*scaler); maxY = std::max(maxY, (double)pointSource[i].y*scaler);
		m_lines.push_back(Line(pt1, pt2, 0));
	}
	//Complete the loop
	m_lines.push_back(Line(pt2, Vertex((double)pointSource.front().x*scaler, (double)pointSource.front().y*scaler, 0), 0));

	RoVaClipperLib::Path tempPoly;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		RoVaClipperLib::IntPoint pt1(it->pt1.x, it->pt1.y);
		RoVaClipperLib::IntPoint pt2(it->pt2.x, it->pt2.y);

		tempPoly.push_back(pt1);
		tempPoly.push_back(pt2);
	}
	m_area = RoVaClipperLib::Area(tempPoly);
}

void RSPolygon::reverseWinding()
{
	for (uint i = 0; i < m_lines.size(); i++)
	{
		std::swap(m_lines[i].pt1, m_lines[i].pt2);
		std::reverse(m_lines.begin(), m_lines.end());
	}
	m_area *= -1.0;
}

void RSPolygon::setColour(float inR, float inG, float inB)
{
	r = inR;
	g = inG;
	b = inB;
	for (uint i = 0; i < m_lines.size(); i++)
	{
		m_lines[i].colour = Colour(inR, inG, inB);
	}
	for (uint i = 0; i < children.size(); i++)
		children[i]->setColour(inR, inG, inB);
}

void RSPolygon::addChild(RSPolygon* child)
{
	children.push_back(child);
}

void RSPolygon::resisterChild()
{
	if (parent)
		parent->addChild(this);
}

RSPolygon::RSPolygon(RSPolygon *inParent) : parent(inParent)
{
	if (parent)
	{
		parent->addChild(this);
		isHole = !parent->isHole;
	}
	else
		isHole = false;
}

RSPolygon::~RSPolygon()
{
	for (uint i = 0; i < children.size(); i++)
	{
		if (children[i] != nullptr)
		{
			delete children[i];
			children[i] = nullptr;
		}
	}
}


void ExRSPolygon::addHole(const Perimeter &perim)
{
	std::shared_ptr<Perimeter> newHole = std::make_shared<Perimeter>(perim);
	if (newHole->Area() > 0)
		newHole->Reverse();
	m_holes.push_back(newHole);
	perimeterTree->addHole(newHole);
}

void ExRSPolygon::addHole(const std::shared_ptr<Perimeter> &perim)
{
	std::shared_ptr<Perimeter> newHole = (perim);
	if (newHole->Area() > 0)
		newHole->Reverse();
	m_holes.push_back(newHole);
	perimeterTree->addHole(newHole);
}

void ExRSPolygon::setContour(const Perimeter &perim)
{
	if (perim.m_lines.empty())
		return;
	m_contour = std::make_shared<Perimeter>(perim);
	if (m_contour->Area() < 0)
		m_contour->Reverse();
	m_contour->parent = m_contour;
	perimeterTree = new InnerPerims(nullptr);
	perimeterTree->setPerimeterLoop(*m_contour);
}

void ExRSPolygon::setContour(const std::shared_ptr<Perimeter> &cont)
{
	m_contour = cont;
	
	if (m_contour->Area() < 0)
		m_contour->Reverse();
	m_contour->parent = m_contour;
	perimeterTree = new InnerPerims(nullptr);
	perimeterTree->setPerimeterLoop(*m_contour);
}

ExRSPolygon::ExRSPolygon(const std::shared_ptr<Perimeter> &contour, ExRSPolygon* par)
{
	parent = par;
	thisTool = par->thisTool;
	setContour(contour);
}

//Unoptimzed coasterprint compatible - Infill source path doesnt exist in this case
void ExRSPolygon::generateInfill(double angle, bool dir, double density)
{
	Infill masterInfill(density,
		m_contour->width(), m_contour->height(), angle, m_contour->minX(), m_contour->minY(), dir, RoVaClipperLib::Paths(), false);
	std::vector<InnerPerims*> innerestOfPerims;
	// Follow the perimeter tree
	perimeterTree->fillTheInnerestOfPerims(innerestOfPerims);

	for (int i = 0; i < innerestOfPerims.size() - 1; i++)
	{
		//Copy the master infill
		Infill thisPiece(masterInfill);
		thisPiece.completeCut(innerestOfPerims[i]->getContour(), innerestOfPerims[i]->getHoles());

		thisPiece.orderLines(*innerestOfPerims[i]->getContour(), innerestOfPerims[i]->getHoles());
		// Now transfer these sorted lines to the perimeter piece...
		for (auto it : thisPiece.sortedLines)
		{
			innerestOfPerims[i]->addInfillLine(it, false);
		}
		for (auto it : thisPiece.fullInfillLinesSorted)
		{
			innerestOfPerims[i]->addInfillLine(it, true);
		}
	}
	masterInfill.completeCut(innerestOfPerims.back()->getContour(), innerestOfPerims.back()->getHoles());
	masterInfill.orderLines(*innerestOfPerims.back()->getContour(), innerestOfPerims.back()->getHoles());

	// Now transfer these sorted lines to the perimeter piece...
	for (auto it : masterInfill.sortedLines)
	{
		innerestOfPerims.back()->addInfillLine(it,false);
	}
	for (auto it : masterInfill.fullInfillLinesSorted)
	{
		innerestOfPerims.back()->addInfillLine(it,true);
	}
}

void ExRSPolygon::generateInfill(double angle, bool dir, double density, const RoVaClipperLib::Paths &notFullInfill,
	bool skipTopLogic, const RoVaClipperLib::Paths *sources, bool isTop)
{
	if (density < 0)
		density = Settings::getSingleton().getDoubleValue("infillDensity");
	// Follow the perimter tree to the childest child (This one cuts the infill)
	//if (isTop)
	//	infillExtWidth = Settings::GetSingleton().getDoubleValue("T" + std::to_string((int)thisTool) + "extrusionWidth");
	//else
	infillExtWidth = Settings::GetSingleton().getDoubleValue("T" + std::to_string((int)thisTool) + "extrusionWidth");
	std::vector<InnerPerims*> innerestOfPerims;
	// Follow the perimeter tree
	perimeterTree->fillTheInnerestOfPerims(innerestOfPerims);
	double area = m_contour->Area();
	//Copy the base infill pattern rather than regenerating it each island
	Infill masterInfill(angle, sources, notFullInfill, skipTopLogic, isTop);

	for (int i = 0; i < innerestOfPerims.size() - 1; i++)
	{
		//Copy the master infill
		Infill thisPiece(masterInfill);
		thisPiece.completeCut(innerestOfPerims[i]->getContour(), innerestOfPerims[i]->getHoles());

		thisPiece.orderLines(*innerestOfPerims[i]->getContour(), innerestOfPerims[i]->getHoles());
		// Now transfer these sorted lines to the perimeter piece...
		for (auto it : thisPiece.sortedLines)
		{
			innerestOfPerims[i]->addInfillLine(it,false);
		}
		for (auto it : thisPiece.fullInfillLinesSorted)
		{
			innerestOfPerims[i]->addInfillLine(it,true);
		}
	}
	{	//By cutting the source material we can save a copy operation
		masterInfill.completeCut(innerestOfPerims.back()->getContour(), innerestOfPerims.back()->getHoles());
		masterInfill.orderLines(*innerestOfPerims.back()->getContour(), innerestOfPerims.back()->getHoles());
		// Now transfer these sorted lines to the perimeter piece...
		for (auto it : masterInfill.sortedLines)
		{
			innerestOfPerims.back()->addInfillLine(it,false);
		}
		for (auto it : masterInfill.fullInfillLinesSorted)
		{
			innerestOfPerims.back()->addInfillLine(it,true);
		}
	}
}

Colour ExRSPolygon::getColour() const
{
	//Get colour whereever we can
	if (m_contour)
		return m_contour->getColour();
	else if (perimeterTree)
	{
		return perimeterTree->colour;
	}
	else if (basicLinePaths.empty() == false)
		return basicLinePaths.front().front().getColour();

	return Colour::InfillColour();
}

ExRSPolygon::ExRSPolygon(iSlicable::ToolType toolIn)
{
	thisTool = toolIn;
	extWidth = GET_DOUBLE("T"+std::to_string((int)thisTool)+"extrusionWidth");
	infillExtWidth = extWidth;
}


ExRSPolygon::~ExRSPolygon()
{
	if (perimeterTree)
		delete perimeterTree;
}

void ExRSPolygon::registerChild()
{
	if (parent)
	{
		parent->addHole(this->m_contour);
	}
}

void ExRSPolygon::generateInnerPerimeters(int innerPerimCount)
{
	if(innerPerimCount == -1)
		perimeterTree->generateInnerPerims(-extWidth*DEDOUBLER, GET_DOUBLE("perimeterCount") - 1);
	else
		perimeterTree->generateInnerPerims(-extWidth*DEDOUBLER, innerPerimCount - 1);
	//If only one perimeter, this function can be skipped
	//if (Settings::getSingleton().getDoubleValue("perimeterCount") == 1)
	//{
	//	innerestPerims.push_back(m_contour);
	//	//perimeterTree = new InnerPerims(nullptr);
	//	//perimeterTree->setPerimeterLoop(*m_contour);
	//}
	return;
}

//SEL this function is not actually called
std::vector<std::shared_ptr<Perimeter>> ExRSPolygon::getInnerPerims(std::shared_ptr<Perimeter> perim) const
{
	std::vector<std::shared_ptr<Perimeter>> output;
	std::vector<Lines> newPerims = perim->offset(-(extWidth));

	for (uint i = 0; i < newPerims.size(); i++)
	{
		output.push_back(std::make_shared<Perimeter>(newPerims[i], false, false));
		output.back()->parent = perim;
		perim->children.push_back(output.back());
		output.back()->setColour(perim->getColour());
	}
	for (auto &it : output)
		it->Reverse();
	return output;
}

double ExRSPolygon::getSquaredDistanceNearest(const Vertex &inVert) const
{
	//Ideally find the closest point so the perim can be printed from here
	double shortestDistance = std::numeric_limits<double>::max();
	if (m_contour)
	{
		shortestDistance = m_contour->m_lines.front().pt1.distanceToSquared(inVert);
		for (auto it = m_contour->m_lines.begin(); it != m_contour->m_lines.end(); ++it)
		{
			double testDistance = it->pt1.distanceToSquared(inVert);
			if (testDistance < shortestDistance)
				shortestDistance = testDistance;
		}
	}
	if (perimeterTree)
	{
		shortestDistance = std::min(Vertex2D::distanceR2Sqrd(perimeterTree->getContour()->front(), inVert),shortestDistance);
		for (auto it = perimeterTree->getContour()->begin(); it != perimeterTree->getContour()->end(); ++it)
		{
			double testDistance = Vertex2D::distanceR2Sqrd((*it), inVert);// it->pt1.distanceToSquared(inVert);
			if (testDistance < shortestDistance)
				shortestDistance = testDistance;
		}
	}
	if (basicLinePaths.empty() == false)
	{
		//Line paths also go in order, cause they just do
		shortestDistance = std::min(basicLinePaths.front().front().pt1.distanceToSquared(inVert),shortestDistance);
	}
	return shortestDistance;
}

void ExRSPolygon::addParamToLines(unsigned int param)
{
	//Perims
	Line::addParamToSet(m_contour->m_lines, param);
}

void ExRSPolygon::addParamToLines(unsigned int param, const RoVaClipperLib::Paths &paths)
{
	//Follow all the lines ORing with the existing param
	for (auto polyIT : paths)
	{
		for (uint i = 0; i < m_contour->m_lines.size(); i++)
		{

			if (!RoVaClipperLib::PointInPolygon(RoVaClipperLib::IntPoint(m_contour->m_lines[i].pt1.x*DEDOUBLER,
				m_contour->m_lines[i].pt1.y*DEDOUBLER), polyIT) || !RoVaClipperLib::PointInPolygon(RoVaClipperLib::IntPoint(m_contour->m_lines[i].pt2.x*DEDOUBLER,
					m_contour->m_lines[i].pt2.y*DEDOUBLER), polyIT))
			{
				m_contour->m_lines[i].addParameter(param);
			}

		}
	}
}

extern int gl_nSEL;   //SEL debug - verbose if true

//SEL added most of the comments
//
// SEL this method will take the split lines of the contour and create a colourGrid object.
// The colour grid is used to seperate the one polygon into polygons for each colour island
//
std::list< std::shared_ptr< ExRSPolygon > > ExRSPolygon::straightSkeletonColours(bool isUVMap)
{
	static int islandCounter = 0;
	
	//This isnt straight skeleton anymore, but the name remains
	// Really Robert?   How much does a find and replace cost?

	//SEL  debug
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{

		std::cout << "****break up the polygons if colour changes along perim ****\n\n";
		std::cout << "Lines for this contour = " << m_contour->m_lines.size() << " and lines after trimming zero length lines = ";
	}
	Line::removeZeroLengthLines(m_contour->m_lines);  //remove all lines of zero length.  I think this should be done in the crossescontour check before the line is created. 
													  // I have now implemented that check in LineCrossesContourcheck Aug 6, 2019 SEL
													  // but I will continue to check for zero length lines for safety

	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{
		std::cout << m_contour->m_lines.size() << "\n";
	}

	std::list<std::shared_ptr<ExRSPolygon>> output;
	bool shortCircuit = true;

	if (m_contour->m_lines.size() < 2)
	{
		if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
		{
			std::cout << "Less than 2 Lines ????\n"; //SEL

		}
		return output;
	}

	//go through all the lines in the contour to see if we are only dealing with one colour
	//
	// !!! SEL  This doesn't test holes
	Colour testColour = m_contour->getColour();
	for (int i = 1; i < m_contour->m_lines.size(); i++)
	{
		if (m_contour->m_lines[i].colour != testColour)
		{
			shortCircuit = false;
			break;
		}
	}
	//If there is only one colour we can skip a bunch of stuff
	if (shortCircuit)
	{
		output = offsetPolygon(isUVMap);
		if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
		{
			std::cout << "Only one colour in this contour so we can leave now\n";  //SEL
		}
		return output;
	}


	// Create the colour grid for colour inset TODO: simplify the interface for creation
	islandCounter++;
	//std::ofstream lineFile;
	//lineFile.open("insetlinefile" + std::to_string(islandCounter) + ".rova");
	//Line::writeLinesWithColourToFile(lineFile, m_contour->m_lines);
	//lineFile << "holeCount:" << m_holes.size() << "\n";
	//for (auto holeIT : m_holes)
	//{
	//	Line::writeLinesWithColourToFile(lineFile, holeIT->m_lines);
	//}
	//lineFile.close();

	// This is where the colourGrid object is created
	// It is large enough to box the original polygon
	BodySplitter::colourGrid gridObject;
	//SEL gridObject.setGridSize(GET_DOUBLE("mmPerPixel"));  // lets' change this to be a factor * ExtrusionWidth
	gridObject.setGridSize(Settings::getSingleton().getDoubleValue("ColourGridFactorOfExtrusionWidth") *
		Settings::getSingleton().getDoubleValue("T0extrusionWidth"));

	gridObject.setMinX(m_contour->minX()); //min X
	gridObject.setMinY(m_contour->minY()); //min Y
	gridObject.setMaxX(m_contour->width() + m_contour->minX()); //Max X
	gridObject.setMaxY(m_contour->height() + m_contour->minY()); //max Y

	// this only references the line vector in the gridObject
	gridObject.addLineSet(m_contour->m_lines);

	
	// HOLES NOT CURRENTLY WORKING - Why?  Yeah Robert??  Why?  SEL will have to check
 	for (auto holeIT : m_holes)
 		gridObject.addHoleSet(holeIT->m_lines);


	// Initialize grid should
	// This will create a two dimensional grid of pixels that will be marked with either empty, 
	//inside polygon but not inside holes and whether the polygon line
	// lies upon this pixel
	// If it returns a 0 then the colours have been reduced down to one because the regions would have been too small to print
	if(! gridObject.initializeGrid())
	{
		// we need to change the polygon to the same colour as the lines because it may be different
		m_contour->setColour(gridObject.m_ColourRemaining);
		output = offsetPolygon(isUVMap);
		if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
		{
			std::cout << "This contour is reduced to one colour so we can leave now\n";  //SEL
		}
		return output;
	}

	if (gl_nSEL && Settings::getSingleton().getBoolValue("logInitializedGrid"))
	{ 
		std::cout << "Initialized Grid\n";
		gridObject.printGrid(); 
	} //SEL debug

	//
	// Solve Grid
	// This will go through the grid and in a clockwise pattern and replicate the colour pixels towards the center, one
	// layer at a time until it reaches the center.   This will form colour regions into the depth of the model.
	//
	// I have adjusted the software to support even a single colour pixel
	//
	gridObject.solveGrid();
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logSolvedGrid"))
	{
		std::cout << "Solved Grid\n";
		gridObject.printGrid();
	} //SEL debug

	//
	// createBorderLines
	//
	// This section will find the boundaries between the colour grid regions, create a set of lines for each colour
	// join these broken lines into a complete path, smooth the lines, and then form
	// polygon paths of 2D vertices joining the outer perimeter lines to the inner boundary lines
	gridObject.createBorderLines();
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logBoundaryGrid"))
	{
		std::cout << "Created Border Lines Grid\n";
		gridObject.printGrid();
	} //SEL debug

	auto results = gridObject.getResult();
	
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{
		std::cout << "Island Counter " << islandCounter << " perimeters = " << gridObject.getResult().size();  //SEL DEBUG
	}

	//BodySplitter::writePolygonsToIPE(results, "SSresult" + std::to_string(islandCounter) + ".ipe");
	std::vector<Perimeter> perimSet;
	for (auto &it : results)
	{
		perimSet.push_back(Perimeter(it));
	}

	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{
		// Let's log all the perimeters 
		for (int i = 0; i < perimSet.size(); i++)
		{
			std::cout << "\nPerimeter Set " << i << "\n";
			for (int j = 0; j < perimSet[i].m_lines.size(); j++)
			{
				std::cout << perimSet[i].m_lines[j].getColour() << ", " << perimSet[i].m_lines[j].pt1.x << ", " << perimSet[i].m_lines[j].pt1.y;
				std::cout << ", " << perimSet[i].m_lines[j].pt2.x << ", " << perimSet[i].m_lines[j].pt2.y << "\n";
			}
		}
	}


	//std::list<std::shared_ptr<ExRSPolygon>> generatedRSPolies;
	Perimeter::generateExRSPolygons(output, perimSet,thisTool);

#if 0
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{
		std::cout << " Polygons = " << output.size() << "\n"; //SEL debug

		// Let's log all the perimeters 
		for (int i = 0; i < perimSet.size(); i++)
		{
			std::cout << "Perimeter Set " << i;
			for (int j = 0; j < perimSet[i].m_lines.size(); j++)
			{
				std::cout << perimSet[i].m_lines[j].getColour() << ", " << perimSet[i].m_lines[j].pt1.x << ", " << perimSet[i].m_lines[j].pt1.y;
				std::cout << ", " << perimSet[i].m_lines[j].pt2.x << ", " << perimSet[i].m_lines[j].pt2.y;
			}
		}


		// Let's log the polygons
		for (int i = 0; i < output.size(); i++)
		{
			std::cout << "Polygon Set " << i;
			for (int j = 0; j < output[i]); j++)
			{
				std::cout << perimSet[i].m_lines[j].getColour() << ", " << perimSet[i].m_lines[j].pt1.x << ", " << perimSet[i].m_lines[j].pt1.y;
				std::cout << ", " << perimSet[i].m_lines[j].pt2.x << ", " << perimSet[i].m_lines[j].pt2.y;
			}
		}
	}
#endif

	std::list<std::shared_ptr<ExRSPolygon>> offsetOutput;
	for (auto &it : output)
	{
		auto offsetPolies = it->offsetPolygon();
		offsetOutput.splice(offsetOutput.end(), offsetPolies);
	}

	if (gl_nSEL && Settings::getSingleton().getBoolValue("logColourGridProcessing"))
	{
		std::cout << " Offset Polygons = " << offsetOutput.size() << "\n"; //SEL debug
	}

	return offsetOutput;
}

std::list<std::shared_ptr<ExRSPolygon>> ExRSPolygon::offsetPolygon(bool hasUVMap)
{
	std::list<std::shared_ptr<ExRSPolygon>> output;
	auto RovaClipPath = RoVaClipperLib::BodySplitterToClipperPath(this->m_contour->m_lines);
	RoVaClipperLib::CleanPolygon(RovaClipPath);
	RoVaClipperLib::ClipperOffset offsetter;
	
	offsetter.AddPath(RovaClipPath, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	// offsetter.Execute()
	RoVaClipperLib::PolyTree out;

	for (auto holeIT : this->m_holes)
	{
		auto tempHole = RoVaClipperLib::BodySplitterToClipperPath(holeIT->m_lines);
		//ReversePath(tempHole);
		offsetter.AddPath(tempHole, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	}

	//SEL 2020  if we are exporting polygons for use by the program bodySlicer, then let's not offset the polygons by the extruder width divide by 2.
	if (Settings::getSingleton().getBoolValue("exportPolygons"))
	{
		this->extWidth = 0.002;   // I am hard coding this to 2 microns
	}//SEL End

	if (!hasUVMap)
		offsetter.Execute(out, ((-this->extWidth / 2.0) + GET_DOUBLE("XYOffset"))*DEDOUBLER);
	else
		offsetter.Execute(out, ((-this->extWidth / 2.0))*DEDOUBLER);
	auto polyNode = out.GetFirst();
	if (polyNode == nullptr && GET_DOUBLE("T" + std::to_string((int)thisTool) + "minExtrusionWidth") < GET_DOUBLE("T"+std::to_string((int)thisTool)+"extrusionWidth")) // Node collapsed.. too small
	{
		//Try again with a smaller extWidth if it is less
		this->extWidth = GET_DOUBLE("T" + std::to_string((int)thisTool) + "minExtrusionWidth");

		if (!hasUVMap)
			offsetter.Execute(out, ((-this->extWidth / 2.0) + GET_DOUBLE("XYOffset"))*DEDOUBLER);
		else
			offsetter.Execute(out, ((-this->extWidth / 2.0))*DEDOUBLER);
		polyNode = out.GetFirst();
		if (polyNode == nullptr)
			return output;
	}
	else if (polyNode == nullptr) // Node has collapsed and we cant try a smaller node
		return output;
	assert(!polyNode->IsHole());	//Not sure what it means if the first is a hole...
	ClipperLibPolytreeToRSPolygon(&out, output, this->getColour());
	for (auto &it : output)
	{
		it->setExtWidth(this->extWidth);
	}

	return output;
}

double ExRSPolygon::getPolytreeHolelessArea(const RoVaClipperLib::PolyNode *polyOut)
{
	double area = 0;
	if (polyOut->Contour.empty() == false && !polyOut->IsHole())
	{
		std::shared_ptr<ExRSPolygon> newExRSPoly = std::make_shared<ExRSPolygon>(thisTool);
		BodySplitter::vert2DPoly newPoly;
		RoVaClipperLib::Path cleanedPath;
		RoVaClipperLib::CleanPolygon(polyOut->Contour, cleanedPath);
		if (cleanedPath.empty())
			return area;
		area += RoVaClipperLib::Area(polyOut->Contour);
	}
	for (int i = 0; i < polyOut->ChildCount(); i++)
	{
		area += getPolytreeHolelessArea(polyOut->Childs[i]);
	}
	return area;
}

void ExRSPolygon::ClipperLibPolytreeToRSPolygon(const RoVaClipperLib::PolyNode *polyOut, std::list<std::shared_ptr<ExRSPolygon>> &output, const Colour &colour)
{
	//Recursively follow the clipperlib tree
	if (polyOut->Contour.empty() == false && !polyOut->IsHole())
	{
		std::shared_ptr<ExRSPolygon> newExRSPoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
		BodySplitter::vert2DPoly newPoly;
		RoVaClipperLib::Path cleanedPath;
		RoVaClipperLib::CleanPolygon(polyOut->Contour, cleanedPath);
		if (cleanedPath.empty())
			return;
		newPoly.poly = RoVaClipperLib::ClipperPathToBodySplitter(cleanedPath);
		newPoly.colour = colour;
		newExRSPoly->setContour(Perimeter(newPoly));
		for (int i = 0; i < polyOut->ChildCount(); i++)
		{
			if (polyOut->Childs[i]->IsHole())
			{
				RoVaClipperLib::Path cleanedPath;
				RoVaClipperLib::CleanPolygon(polyOut->Childs[i]->Contour, cleanedPath);
				auto contour = RoVaClipperLib::ClipperPathToBodySplitter(cleanedPath, 0);
				newExRSPoly->addHole(Perimeter(contour));
			}
		}
		output.push_back(newExRSPoly);
	}
	for (int i = 0; i < polyOut->ChildCount(); i++)
	{
		ClipperLibPolytreeToRSPolygon(polyOut->Childs[i], output, colour);
	}
}

void ExRSPolygon::setExtWidth(double extW)
{
	extWidth = extW;
}