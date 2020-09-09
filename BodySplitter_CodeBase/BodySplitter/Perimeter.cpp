#include <spdlog/spdlog.h>
#include <fstream>
#include "Perimeter.h"
#include "Settings.h"
#ifdef _DEBUG
#include <wx/log.h>
#endif
#include <stdio.h>
#include <unordered_set>
#include "RSPolygon.h"
#include "colour.h"
#include "BodySplitterTexture.h"


#include "LineToLoopSolver.h"


 Perimeter::Perimeter(Lines &lines, bool hasTextureMap, bool forceClose) : holeCalculated(false), perimColour(-1.0, -1.0, -1.0)
 {
	 if (lines.empty())
		 return;
	 Lines tempLines(lines);

	 if (createStrongPerimeter(tempLines))
	 {
		 lines = tempLines;
		 return;
	 }
	 else
	 {
		 auto logger = spdlog::get("DEBUG_FILE");
		logger->info("Failed to close perimeter with strong perimeter looping");

		 tempLines = lines;
		if (createDijikstraPerimeters(tempLines))
		{
			lines = tempLines;
			return;
		}
		else
		{
			auto logger = spdlog::get("DEBUG_FILE");
			logger->info("Failed to close perimeter with dijiskstra looping");
			m_lines.clear();
			tempLines = lines;
		}
	 }
	 
	 m_lines.clear();
	 auto logger = spdlog::get("DEBUG_FILE");
	 logger->info("Failed to close perimeter with strict ordering");
	 createWeakPerimeter(lines, hasTextureMap, forceClose);
 }
 bool Perimeter::createDijikstraPerimeters(Lines &lines)
 {
	 Lines solution;
	 Line backLine = lines.back();
	 lines.pop_back();
	 bool loopGenerated = LineToLoopSolver::findLoop(solution, lines, backLine.pt1, backLine.pt2);
	 if (solution.empty())
		 return false;
	 m_lines = solution;
	 m_lines.push_back(backLine);

	 //This bit is really slow, should hash it
	 for (auto lineIT = lines.begin(); lineIT != lines.end();)
	 {
		 bool foundMatch = false;
		 for (auto solutionIT : solution)
		 {
			 if ((*lineIT).matchesLocation(solutionIT))
			 {
				 lineIT = lines.erase(lineIT);
				 foundMatch = true;
				 break;
			 }
		 }
		 if (!foundMatch)
			 lineIT++;
	 }
	 return true;
 }

 //More prone to failure but also correctness...
 
bool Perimeter::createStrongPerimeter(Lines &lines)
{
  //Start with the first line
	if (lines.empty())
		return false;
  m_lines.push_back(lines.front());
  lines.erase(lines.begin());
  bool isClosed = false;
  while(!isClosed)
  {
	  if (lines.empty())
		  return false;	//Cant close the perimeter with this method
    //Find next closest point
    double closestPtDistance = std::min(m_lines.back().pt2.distanceToSquared(lines.front().pt1),
					m_lines.back().pt2.distanceToSquared(lines.front().pt2));
    auto closestIT = lines.begin();
	if (lines.size() > 1)
	{
		for (auto lineTestIT = lines.begin() + 1; lineTestIT != lines.end(); ++lineTestIT)
		{
			double testDistance = std::min(m_lines.back().pt2.distanceToSquared(lineTestIT->pt1),
				m_lines.back().pt2.distanceToSquared(lineTestIT->pt2));
			if (testDistance < closestPtDistance)
			{
				closestPtDistance = testDistance;
				closestIT = lineTestIT;
			}
		}
	}
	// At some point we need to check if there is > 1 possible next line, if so we need djikstra ordering
    //Now have the closest, check if we need to reverse it
    if(m_lines.back().pt2.distanceToSquared(closestIT->pt1) > m_lines.back().pt2.distanceToSquared(closestIT->pt2))
    {
      closestIT->reverse();
    }
    if(!m_lines.back().pt2.MatchesLocation(closestIT->pt1) && m_lines.size() == 1) // If this happens, that means this line belonged on the last perimeter
      m_lines.clear();
    m_lines.push_back(*closestIT);
    lines.erase(closestIT);
    //Check if its closed
    if(m_lines.front().pt1.MatchesLocation(m_lines.back().pt2))
      isClosed = true;
  }
  return isClosed;
}



Perimeter::Perimeter(std::vector<cv::Point_<int>> &cvPoints) : perimColour(-1.0, -1.0, -1.0)
{
	Lines outlines;
	double scaler = Settings::getSingleton().getDoubleValue("coasterSize") / 100.0;
	cv::Point lastPoint = cvPoints.front();
	for (uint i = 1; i < cvPoints.size(); i++)
	{
		Vertex point1(lastPoint.x*scaler, lastPoint.y*scaler);
		Vertex point2(cvPoints[i].x*scaler, cvPoints[i].y*scaler);

		outlines.push_back(Line(point1, point2, 0));
		lastPoint = cvPoints[i];
	}
	Vertex point1(cvPoints.back().x*scaler, cvPoints.back().y*scaler);
	Vertex point2(cvPoints.front().x*scaler, cvPoints.front().y*scaler);
	outlines.push_back(Line(point1, point2, 0));
	createWeakPerimeter(outlines, false, true);
}

Perimeter::Perimeter(const BodySplitter::vert2DPoly& inPoly)
{
	Vertex pt1 = inPoly.poly.front();
	for (std::size_t i = 1; i < inPoly.poly.size(); i++)
	{
		Vertex pt2(inPoly.poly[i]);
		m_lines.push_back(Line(pt1, pt2, 0));
		pt1 = pt2;
	}
	m_lines.push_back(Line(m_lines.back().pt2, m_lines.front().pt1, 0));
	Line::setLineSetToColour(m_lines, inPoly.colour);
}

Perimeter::Perimeter(const std::vector<cv::Point_<int> > &cvPoints, unsigned int xSize, unsigned int ySize, double maxX, double maxY) : perimColour(-1.0, -1.0, -1.0)
{
	Lines outlines;
	cv::Point lastPoint = cvPoints.front();
	for (uint i = 1; i < cvPoints.size(); i++)
	{
		Vertex point1((double)lastPoint.x / (double)xSize*maxX, (double)lastPoint.y / (double)ySize*maxY);
		Vertex point2((double)cvPoints[i].x / (double)xSize*maxX, (double)cvPoints[i].y / (double)ySize*maxY);

		outlines.push_back(Line(point1, point2, Line::LineType::PERIM));
		lastPoint = cvPoints[i];
	}
	Vertex point1((double)cvPoints.back().x / (double)xSize*maxX, (double)cvPoints.back().y / (double)ySize*maxY);
	Vertex point2((double)cvPoints.front().x / (double)xSize*maxX, (double)cvPoints.front().y / (double)ySize*maxY);
	outlines.push_back(Line(point1, point2, Line::LineType::PERIM));
	createWeakPerimeter(outlines, false, true);
}

void Perimeter::createWeakPerimeter(Lines &lines, bool hasTextureMap, bool forceClose)
{
#ifndef NDEBUG
	std::vector<Line> lineDebugCopy(lines);
#endif
	if (lines.empty())
		return;
	Line lastLine = lines.front();
	lines.erase(lines.begin());
	//Lines loop1;
	//loop1.push_back(lastLine);
	////Find all the lines with a point that matches an existing point...

	m_lines.push_back(lastLine);
	bool loopIsClosed = false;
	bool hasAdded = false;
	while (!loopIsClosed)
	{
		hasAdded = false;
		for (auto it = lines.begin(); it != lines.end(); ++it)
		{
			if (Vertex::MatchesLocation(it->pt1, lastLine.pt2)) //it->pt1 == lastLine.pt2)
			{
				hasAdded = true;
				//First point matches last point on previous line
				m_lines.push_back(*it);
				lastLine = (*it);


				if (Vertex::MatchesLocation(it->pt2, m_lines.front().pt1)) //(it->pt2 == m_lines.front().pt1))
				{
					//Line finishes the loop
					loopIsClosed = true;
				}
				it = lines.erase(it);
				//if (it == lines.end())
				break;
			}
			else if (Vertex::MatchesLocation(it->pt2, lastLine.pt2)) //it->pt2 == lastLine.pt2)
			{
				if ((*it) != lastLine)
				{
					it->reverse();
					hasAdded = true;
					//First point matches last point on previous line
					m_lines.push_back(*it);
					lastLine = (*it);


					if (Vertex::MatchesLocation(it->pt2, m_lines.front().pt1))
					{
						//Line finishes the loop
						loopIsClosed = true;
					}
					it = lines.erase(it);
					//if (it == lines.end())
					break;
					//wxLogDebug("Line matches with a reverse line");
				}
			}
		}
		if (hasAdded == false && forceClose == false)
		{
			//Cant close the loop
#ifdef _DEBUG
			wxLogDebug("Cant close the loop");
#endif
			//This should never happen, but it does... often
			//Check if matched with reverse points
			if (!lines.empty())
				lines.erase(lines.begin());
			return;
		}
		else if (forceClose == true && hasAdded == false && lines.size() > 1)
		{
			lines.push_back(Line(lines.back().pt2, lines.front().pt1, lines.front().getParam()));//Close the perimeter by force
			loopIsClosed = true;
		}

		if (lines.empty())
			break;
	}
	//Check for duplicates
	for (Lines::iterator inputLines = lines.begin(); inputLines != lines.end(); ++inputLines)
	{
		for (Lines::const_iterator outputLines = m_lines.begin(); outputLines != m_lines.end(); ++outputLines)
		{
			if ((*inputLines) == (*outputLines))
			{
				inputLines = lines.erase(inputLines);
				break;
			}
		}
		if (inputLines == lines.end())
			break;
	}

#ifdef DUMP_TO_TEST_FILES
	if (output == nullptr)
	{
		if (output == nullptr)
		{
			output = fopen("test.loops", "w");
		}
	}
	fprintf(output, "Input\n");
	fprintf(output, "loop0\n");
	int point = 0;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		fprintf(output, "P%d X%d Y%d R%2.2f G%2.2f B%2.2f\n",
			point, (int)round(it->pt1.x*100.0), (int)round(it->pt1.y*100.0), it->pt1.r, it->pt1.g, it->pt1.b);
		point++;
		//fprintf(output, "P%d X%d Y%d R%2.2f G%2.2f B%2.2f\n",
		//	point, (int)round(it->pt2.x*100.0), (int)round(it->pt2.y*100.0), it->pt2.r, it->pt2.g, it->pt2.b);
	}
#endif
	rovaClipPath = RoVaClipperLib::BodySplitterToClipperPath(m_lines);
}

void Perimeter::writeContour(std::fstream &file)
{
	file << "NEWCONTOUR\n";

	if (Area() < 0)
		Reverse();

	for (auto it : m_lines)
	{
		file << it.pt1.x << " " << it.pt1.y << " " << it.colour.getR<uint8_t>() << " " << it.colour.getG<uint8_t>() << " " << it.colour.getB<uint8_t>() << "\n";
	}
}

double Perimeter::Area() const
{
	RoVaClipperLib::Path temp = RoVaClipperLib::BodySplitterToClipperPath(m_lines);

	return -RoVaClipperLib::Area(temp) / (DEDOUBLER*DEDOUBLER);
}

void Perimeter::Reverse()
{
	//Reverse each line, reverse order of the lines
	for (uint i = 0; i < m_lines.size(); i++)
	{
		m_lines[i].reverse();
	}
	std::reverse(m_lines.begin(), m_lines.end());
}

#if USE_INT_COORDS
auto Perimeter::width() const
{
	micrometers minX = m_lines.front().pt1.x;
	micrometers maxX = minX;
#else
double Perimeter::width() const
{
	double minX = m_lines.front().pt1.x;
	double maxX = minX;
#endif
	for (uint i = 0; i < m_lines.size(); i++)
	{
		minX = std::min(minX, std::min(m_lines[i].pt1.x, m_lines[i].pt2.x));
		maxX = std::max(maxX, std::max(m_lines[i].pt1.x, m_lines[i].pt2.x));
	}
	return maxX - minX;
}
#if USE_INT_COORDS
auto Perimeter::height() const
{
	micrometers minY = m_lines.front().pt1.y;
	micrometers maxY = minY;
#else
double Perimeter::height() const
{
	double minY = m_lines.front().pt1.y;
	double maxY = minY;
#endif
	for (uint i = 0; i < m_lines.size(); i++)
	{
		minY = std::min(minY, std::min(m_lines[i].pt1.y, m_lines[i].pt2.y));
		maxY = std::max(maxY, std::max(m_lines[i].pt1.y, m_lines[i].pt2.y));
	}
	return maxY - minY;
}
#if USE_INT_COORDS
auto Perimeter::minX() const
#else
double Perimeter::minX() const
#endif
{
	auto minX = m_lines.front().pt1.x;
	for (uint i = 0; i < m_lines.size(); i++)
	{
		minX = std::min(minX, std::min(m_lines[i].pt1.x, m_lines[i].pt2.x));
	}
	return minX;
}
#if USE_INT_COORDS
auto Perimeter::minY() const
#else
double Perimeter::minY() const
#endif
{
	auto minY = m_lines.front().pt1.y;
	for (uint i = 0; i < m_lines.size(); i++)
	{
		minY = std::min(minY, std::min(m_lines[i].pt1.y, m_lines[i].pt2.y));
	}
	return minY;
}

bool Perimeter::isWithin(const Perimeter &bigger) const
{

	if (this == &bigger)
		return false;
	//if (fabs(bigger.Area()) >= fabs(this->Area())) // Temp TEST
	//	return true;
	//return false;
	for (auto &inLine : this->m_lines)
	{
		if (BodySplitter::isPointInPoly(inLine.pt2, bigger.m_lines) == false)
			return false;
	}
	return true;
}

bool Perimeter::pointIn(const Vertex &pt) const
{
	return BodySplitter::isPointInPoly(pt, this->m_lines);
	//return RoVaClipperLib::PointInPolygon(RoVaClipperLib::IntPoint(pt.x*DEDOUBLER, pt.y*DEDOUBLER), rovaClipPath)>0;
}

bool Perimeter::isBigger(const Perimeter &test) const
{
	if (this == &test)
		return false;

	if (fabs(test.Area()) > fabs(this->Area()))
		return false;

	return (this->minX() < test.minX() &&
		this->minY() < test.minY()) &&
		(this->width() > test.width() &&
			this->height() > test.height());
}

void Perimeter::setColour(double r, double b, double g)
{
	for (uint i = 0; i < m_lines.size(); i++)
	{
		m_lines[i].colour = Colour(r, g, b);
	}
}

void Perimeter::setColour(const Colour &in)
{
	perimColour = in;
	for (uint i = 0; i < m_lines.size(); i++)
	{
		m_lines[i].setColour(in);
	}
}

struct PolyNode {
	PolyNode *parent;
	std::vector<PolyNode*> children;
	Perimeter perim;
	PolyNode(PolyNode* par, const Perimeter &perm) : parent(par), perim(perm) {}
	~PolyNode()
	{
		for (auto it : children)
			delete it;
	}
	int getDepth()
	{
		int depth = 0;
		PolyNode* tracer = parent;
		while (tracer != nullptr)
		{
			depth++;
			tracer = tracer->parent;
		}
		return depth;
	}
};

bool fixChildren(PolyNode* rootNode)
{
	bool result = false;
	bool madeChange = true;
	while (madeChange)
	{
		madeChange = false;
		for (auto childIT = rootNode->children.begin(); childIT != rootNode->children.end(); ++childIT)
		{
			for (auto secondChildIT = rootNode->children.begin(); secondChildIT != rootNode->children.end(); ++secondChildIT)
			{
				if (childIT == secondChildIT)
					continue;

				if ((*secondChildIT)->perim.isWithin((*childIT)->perim))
				{
					(*secondChildIT)->parent = (*childIT);
					(*childIT)->children.push_back(*secondChildIT);
					madeChange = true;
					result = true;
					rootNode->children.erase(std::find(rootNode->children.begin(), rootNode->children.end(), *secondChildIT));
					break;
				}
			}
			if (madeChange)
				break;
		}
	}
	return result;
}

void fixTree(PolyNode *node)
{
	bool madeChange = true;
	while (madeChange)
	{
		madeChange = fixChildren(node);
	}
	for (auto childIT = node->children.begin(); childIT != node->children.end(); ++childIT)
	{
		fixTree(*childIT);
	}
}

void turnTreeToExRSPolys(std::list<std::shared_ptr<ExRSPolygon>> &polyList, PolyNode* node)
{
	if (node == nullptr)
		return;
	if (node->getDepth() % 2 == 0) // 1 is a hole, 0 is not a hole
	{
		polyList.push_back(std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR));
		polyList.back()->setContour(node->perim);
		for (auto childrenIT : node->children)
		{
			polyList.back()->addHole(childrenIT->perim);
		}
		//std::shared_ptr<ExRSPolygon> newPoly = std::make_shared<ExRSPolygon>();
	}
	for (auto childrenIT : node->children)
	{
		turnTreeToExRSPolys(polyList, childrenIT);
	}
}

void Perimeter::generateExRSPolygons(std::list<std::shared_ptr<ExRSPolygon>> &out, std::vector<Perimeter> &lines, iSlicable::ToolType toolIn)
{
	if (lines.empty())
		return;
	if (lines.size() == 1) //Only one lineset, we can save time
	{
		out.push_back(std::make_shared<ExRSPolygon>(toolIn));
		out.back()->setContour(lines.front());
		return;
	}
	//PolyNode* rootNode = new PolyNode(nullptr, Perimeter());
	// DOES NOT SUPPORT SIBLING NODES ON TOP PARENT
	while (lines.empty() == false)
	{
		std::vector<Perimeter>::iterator largestLoop = lines.begin();
		for (auto lineIT = lines.begin() + 1; lineIT != lines.end(); ++lineIT)
		{
			if (largestLoop->isWithin(*lineIT))
			{
				largestLoop = lineIT;
			}
		}
		Perimeter largestPerim = *largestLoop;
		lines.erase(largestLoop);
		PolyNode* rootNode = new PolyNode(nullptr, largestPerim);
		//TODO:Optimize this mess
		bool foundMatch = true;
		while (foundMatch)
		{
			foundMatch = false;
			for (auto lineIT = lines.begin(); lineIT != lines.end(); ++lineIT)
			{
				if (lineIT->isWithin(largestPerim))
				{
					rootNode->children.push_back(new PolyNode(rootNode, *lineIT));
					lines.erase(lineIT);
					foundMatch = true;
					break;
				}
			}
		}
		bool madeChange = true;
		while (madeChange)
		{
			madeChange = false;
			for (auto node : rootNode->children)
			{
				for (auto testNode : rootNode->children)
				{
					if (node == testNode)
						continue;
					if (testNode->perim.isWithin(node->perim))
					{
						node->children.push_back(testNode);
						testNode->parent = node;
						rootNode->children.erase(std::find(rootNode->children.begin(), rootNode->children.end(), testNode));
						madeChange = true;
						break;
					}
				}
				if (madeChange)
					break;
			}
		}
		fixTree(rootNode);
		// Transverse the tree
		turnTreeToExRSPolys(out, rootNode);
		delete rootNode;
	}
	return;
}

void Perimeter::simplify()
{
	Line::removeZeroLengthLines(m_lines);
	if (Settings::GetSingleton().getDoubleValue("removeShortLines") > 0.0001)
		Line::removeShortLines(m_lines, Settings::GetSingleton().getDoubleValue("removeShortLines"));
	Line::removePointlessPoints(m_lines);
	return;
}

std::vector<Lines> Perimeter::offset(double amt) const
{
	//Convert the contour to clipper
	RoVaClipperLib::Path path_contour;
	path_contour = RoVaClipperLib::BodySplitterToClipperPath(m_lines);
	RoVaClipperLib::ClipperOffset offsetter;
	offsetter.AddPath(path_contour, RoVaClipperLib::jtSquare, RoVaClipperLib::etClosedPolygon);
	RoVaClipperLib::Paths offsetPaths;

	offsetter.Execute(offsetPaths, amt);

	std::vector<Lines> outputSets;
	for (auto it = offsetPaths.begin(); it != offsetPaths.end(); ++it)
	{
		outputSets.push_back(RoVaClipperLib::ClipperPathToBodySplitter((*it), Line::LineType::PERIM));
	}
	return outputSets;
}

std::vector<Lines> Perimeter::cut(const std::vector<Line> &cutline)
{
	Colour cachedColour;
	if (m_lines.size() > 0)
		cachedColour = m_lines.front().getColour();

	RoVaClipperLib::Path path_contour;
	path_contour = RoVaClipperLib::BodySplitterToClipperPath(m_lines);

	RoVaClipperLib::Path cut_line;
	cut_line = RoVaClipperLib::BodySplitterToClipperPath(cutline);

	RoVaClipperLib::Clipper clipper;
	clipper.AddPath(path_contour, RoVaClipperLib::ptSubject, true);
	clipper.AddPath(cut_line, RoVaClipperLib::ptClip, true);
	RoVaClipperLib::Paths sol;
	clipper.Execute(RoVaClipperLib::ctDifference, sol);

	std::vector<Lines> output;
	for (std::size_t i = 0; i < sol.size(); i++)
	{
		output.push_back(RoVaClipperLib::ClipperPathToBodySplitter(sol[i], Line::LineType::PERIM));
	}

	for (std::size_t i = 0; i < output.size(); i++)
	{
		for (std::size_t j = 0; j < output[i].size(); j++)
		{
			output[i][j].setColour(cachedColour);
		}
	}
	return output;
}

Colour Perimeter::getColour() const
{
	return m_lines.front().getColour();
}

void Perimeter::breakLineIntoPixelColours(const BodySplitterTexture *ptrText)
{
	std::vector<Line> outputLines;
	for (std::vector<Line>::size_type i = 0; i < m_lines.size(); i++)
	{
		std::vector<Line> pixelLines = m_lines[i].breakLineToPixels(ptrText);
		outputLines.insert(outputLines.end(), pixelLines.begin(), pixelLines.end());
	}
	m_lines = outputLines;
}

void Perimeter::travisSimplification()
{
	// Yes we insert to the front of a vector, rarely and this saves writing a bunch of code
	std::vector<std::vector<Line>> colourLineGroupen;
	colourLineGroupen.push_back(std::vector<Line>());
	colourLineGroupen.back().push_back(m_lines.front());
	Colour currentWorkingColour = m_lines.front().getColour();

	for (std::size_t i = 1; i < m_lines.size(); i++)
	{
		if (m_lines[i].getColour().distanceTo(currentWorkingColour) < GET_DOUBLE("colourChangeThreshold"))//pow(GET_DOUBLE("colourChangeThreshold"), 2))
		{
			colourLineGroupen.back().push_back(m_lines[i]);
		}
		else
		{
			// 			if (Line::getSetLength(colourLineGroupen.back()) < GET_DOUBLE("minLineLengthTravisSimplification"))
			// 			{
			// 				//Merge with this one
			// 				colourLineGroupen.push_back(std::vector<Line>());
			// 				colourLineGroupen.back().push_back(m_lines[i]);
			// 			}
			// 			else
			{
				colourLineGroupen.push_back(std::vector<Line>());
				colourLineGroupen.back().push_back(m_lines[i]);
			}
			currentWorkingColour = m_lines[i].getColour();
		}
	}

	//Strip out the short line segemnts
	bool madeChange = true;
	while (madeChange)
	{
		madeChange = false;
		for (std::size_t i = 0; i < colourLineGroupen.size(); i++)
		{
			if (Line::getSetLength(colourLineGroupen[i]) < GET_DOUBLE("minLineLengthTravisSimplification"))
			{
				std::cerr << "Line removed due to length\n";
				madeChange = true;
				//Join either before or after depending on colour distanceTo
				std::size_t beforeIndx;
				std::size_t afterIndx;
				if (i + 1 == colourLineGroupen.size())
					afterIndx = 0;
				else
					afterIndx = i + 1;
				if (i == 0)
					beforeIndx = colourLineGroupen.size() - 1;
				else
					beforeIndx = i - 1;
				if (colourLineGroupen[i].front().colour.distanceTo(colourLineGroupen[beforeIndx].back().colour) <
					colourLineGroupen[i].back().colour.distanceTo(colourLineGroupen[afterIndx].front().colour))
				{
					// Before index is closer to colour
			// 		for(auto &it : colourLineGroupen[i])
			// 		  it.setColour(colourLineGroupen[beforeIndx].front().colour);
					colourLineGroupen[beforeIndx].insert(colourLineGroupen[beforeIndx].end(), colourLineGroupen[i].begin(), colourLineGroupen[i].end());
				}
				else
				{
					// After index is closer to colour
			// 		for(auto &it : colourLineGroupen[i])
			// 		  it.setColour(colourLineGroupen[afterIndx].front().colour);
					// Slow enough?
					colourLineGroupen[afterIndx].insert(colourLineGroupen[afterIndx].begin(), colourLineGroupen[i].begin(), colourLineGroupen[i].end());
				}
				colourLineGroupen.erase(colourLineGroupen.begin() + i);
				break;
			}
		}
	}
	//Average Colour
	for (auto &it : colourLineGroupen)
	{
		//Average colour??
		double avgColourR = 0.0, avgColourG = 0.0, avgColourB = 0.0, avgColourA = 0;// = it.front().getColour();
		for (auto &lineIT : it)
		{
			avgColourR += ((lineIT.getColour().getR<double>())*lineIT.length());
			avgColourG += ((lineIT.getColour().getG<double>())*lineIT.length());
			avgColourB += ((lineIT.getColour().getB<double>())*lineIT.length());
			avgColourA += ((lineIT.getColour().getA<double>())*lineIT.length());
		}
		avgColourR /= Line::getSetLength(it);
		avgColourG /= Line::getSetLength(it);
		avgColourB /= Line::getSetLength(it);
		avgColourA /= Line::getSetLength(it);
		for (auto &lineIT : it)
		{
			lineIT.setColour(Colour(avgColourR, avgColourG, avgColourB, avgColourA));
		}
	}

	std::vector<Colour> uniqueColours;
	for (auto &it : colourLineGroupen)
	{
		if (uniqueColours.empty())
		{
			uniqueColours.push_back(it.front().getColour());
			continue;
		}
		//Check if this set matches an existing colour set
		bool foundMatch = false;
		for (auto colourIT : uniqueColours)
		{
			if (it.front().getColour().distanceTo(colourIT) < GET_DOUBLE("colourBinThreshold"))//pow(GET_DOUBLE("colourChangeThreshold"), 2))
			{
				Line::setLineSetToColour(it, colourIT);
				foundMatch = true;
				break;
			}
		}
		if (!foundMatch)
			uniqueColours.push_back(it.front().getColour());
	}
	//Repopulate m_lines with the new set
	m_lines.clear();
	for (auto lineIT : colourLineGroupen)
		m_lines.insert(m_lines.end(), lineIT.begin(), lineIT.end());
}

InnerPerims::InnerPerims(InnerPerims *parent) : m_ptrParent(parent)
{
	if (parent)
	{
		colour = parent->colour;
	}
}

InnerPerims::InnerPerims(InnerPerims *parent, const std::vector<Vertex2D> &path, const std::vector<std::vector<Vertex2D>> &holePath, int perimDepth, double offset) : m_ptrParent(parent)
{
	m_path = new std::vector<Vertex2D>(path);
	if(holePath.empty() == false)
		m_holePaths = new std::vector<std::vector<Vertex2D>>(holePath);
	if (parent)
	{
		colour = parent->colour;
	}
	generateInnerPerims(offset, perimDepth);
}

InnerPerims::~InnerPerims()
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		delete (*it);
	}
	if (m_path)
		delete m_path;
	if (m_holePaths)
		delete m_holePaths;
}

void InnerPerims::setPerimeterLoop(const Perimeter &loop)
{
	//Transfer colours
	colour = loop.m_lines.front().getColour();
	m_path = new std::vector<Vertex2D>();

	m_path->push_back(Vertex2D(loop.m_lines.front().pt1));
	for (std::size_t i = 0; i < loop.m_lines.size(); i++)
	{
		m_path->push_back(Vertex2D(loop.m_lines[i].pt2));
	}
}

//SEL  Change path order to start closest to middle-rear or print bed.   I am hard coding this to X110 Y210
//     Maybe some day this should be randomized or set in the settings.ini
//     This code isn't pretty, but neither am I
void SetStartOfPath(std::vector<Vertex2D> &duOldPath)
{
	std::vector<Vertex2D>  duNewPath;
	double dSqrdDistance, dMinDistance;
	int nMinIndex = -1;
	int n;

	// Find the vertex closest to X110 Y210
	for (n = 0; n < duOldPath.size(); n++)
	{
		dSqrdDistance = (duOldPath[n].x - 110.) * (duOldPath[n].x - 110.) + (duOldPath[n].y - 210.) * (duOldPath[n].y - 210.);
		if (nMinIndex < 0)
		{
			nMinIndex = n;
			dMinDistance = dSqrdDistance;
		}
		else if (dSqrdDistance < dMinDistance)
		{
			nMinIndex = n;
			dMinDistance = dSqrdDistance;
		}
	}

	// Now make a new path starting at the vertex closest to the desired point
	for (n = 0; n < duOldPath.size()-1; n++)
	{
		duNewPath.push_back(duOldPath[nMinIndex++]);
		if(nMinIndex >= duOldPath.size())
		{ 
			nMinIndex = 1;  // skip over the repeated point (previous closed loop)
		}
	}
	duNewPath.push_back(duNewPath[0]);  // close the loop

	// clear the current path and over write it with the new order
	duOldPath.clear();
	duOldPath = duNewPath;
}

void InnerPerims::generateInnerPerims(double amt, int perimDepth)
{
	if (perimDepth <= 0)
		return;
	perimDepth--;

	//SEL  We have the outer path.   Maybe we should change the order of the points now??
	SetStartOfPath(*m_path);

	RoVaClipperLib::Path oPath = RoVaClipperLib::BodySplitterToClipperPath(*m_path);


	RoVaClipperLib::ClipperOffset offsetter;
	RoVaClipperLib::Paths holes;
	if (m_holePaths)
	{
		for (auto &holeSet : *m_holePaths)
		{
			holes.push_back(RoVaClipperLib::BodySplitterToClipperPath(holeSet));
		}
	}
	offsetter.AddPaths(holes, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	offsetter.AddPath(oPath, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	RoVaClipperLib::PolyTree out;
	offsetter.Execute(out, amt);
	for(int i = 0; i < out.ChildCount(); i++)
	{
		//RoVaClipperLib::ClipperPathToBodySplitter(out.Childs[i]->Contour);
		RoVaClipperLib::Path nextPath = out.Childs[i]->Contour;
		//SEL   How about do the reverse right her and we will ignore holes the following lines were turned off !!! This is what Chris asked for
		if (RoVaClipperLib::Area(nextPath) > 0)
		{
			RoVaClipperLib::ReversePath(nextPath);
		}
		//SEL end

		std::vector<Vertex2D> innerPath = RoVaClipperLib::ClipperPathToBodySplitter(nextPath);



		innerPath.push_back(innerPath.front());

		SetStartOfPath(innerPath);  //SEL reorder path

		std::vector<std::vector<Vertex2D>> holePath;
		for (int j = 0; j < out.Childs[i]->ChildCount(); j++)
		{
			auto nextHole = RoVaClipperLib::ClipperPathToBodySplitter(out.Childs[i]->Childs[j]->Contour);
			nextHole.push_back(nextHole.front()); //Needed because perim is closed loop
			holePath.push_back(nextHole);
		}
		m_children.push_back(new InnerPerims(this, innerPath, holePath, perimDepth, amt));
		// Holes

	}
}

void InnerPerims::fillTheInnerestOfPerims(std::vector<InnerPerims*> &perims)
{
	if (m_children.empty())
	{
		perims.push_back(this);
	}
	else
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			m_children[i]->fillTheInnerestOfPerims(perims);
		}
	}
}

void InnerPerims::addInfillLine(const std::vector<Line> &lineSet, bool isFull)
{
	if (isFull)
	{
		fullInfillPath.push_back(std::vector<Vertex2D>());
		fullInfillPath.back().push_back(Vertex2D(lineSet.front().pt1.x, lineSet.front().pt1.y));
		for (int i = 0; i < lineSet.size(); i++)
			fullInfillPath.back().push_back(Vertex2D(lineSet[i].pt2.x, lineSet[i].pt2.y));
	}
	else
	{
		infillPath.push_back(std::vector<Vertex2D>());
		infillPath.back().push_back(Vertex2D(lineSet.front().pt1.x, lineSet.front().pt1.y));
		for (int i = 0; i < lineSet.size(); i++)
			infillPath.back().push_back(Vertex2D(lineSet[i].pt2.x, lineSet[i].pt2.y));
	}
}

void InnerPerims::addHole(const std::shared_ptr<Perimeter> &perim)
{
	if (perim->m_lines.empty())
		return;
	if (m_holePaths == nullptr)
		m_holePaths = new std::vector<std::vector<Vertex2D>>;
	m_holePaths->push_back(std::vector<Vertex2D>());
	//Hole is already reveresed, dont revserseit again
	m_holePaths->back().push_back(Vertex2D(perim->m_lines.front().pt1));
	for (auto &it : perim->m_lines)
	{
		m_holePaths->back().push_back(Vertex2D(it.pt2));
	}
}

void InnerPerims::addHole(const std::vector<Vertex2D> &hole)
{
	if (m_holePaths == nullptr)
		m_holePaths = new std::vector<std::vector<Vertex2D>>;
	m_holePaths->push_back(hole);
}

std::vector<std::vector<Vertex2D>> InnerPerims::getAllInfillLines() const
{
	std::vector<std::vector<Vertex2D>> result;

	for (auto it : m_children)
	{
		std::vector<std::vector<Vertex2D>> childLines;
		childLines = it->getAllInfillLines();
		result.insert(result.end(), childLines.begin(), childLines.end());
	}
	result.insert(result.end(), infillPath.begin(), infillPath.end());
	return result;
}