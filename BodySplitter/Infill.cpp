#include "Infill.h"
#include "Perimeter.h"
#include "Settings.h"
#include "Vertex.h"
#include "Line.h"
#include "RSPolygon.h"
#include "colour.h"
#include "MotionPlanner.h"

Infill::Infill(float density, double width, double height, double angle, double xoff, double yoff, bool lastDir, const RoVaClipperLib::Paths &notFullInfill, bool skipTopLogic) : m_fLineAngle(angle)
{
	//Figure out line spacing...
	m_density = density;
	//Spacing at 100% density is extrusion width
	RoVaClipperLib::PolyTree yes100ClippedLines;
	RoVaClipperLib::PolyTree not100ClippedLines;

	RoVaClipperLib::Clipper partialInfillClipper;

	RoVaClipperLib::Paths notFullInfillOffset(notFullInfill);
	RoVaClipperLib::ClipperOffset notFullShrinker;
	notFullShrinker.AddPaths(notFullInfillOffset, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	notFullShrinker.Execute(notFullInfillOffset, -Settings::getSingleton().getDoubleValue("extrusionWidth")*DEDOUBLER);

	double dimMax = std::max(width, height);
	double spacing = (Settings::getSingleton().getDoubleValue("extrusionWidth"));
	spacing = spacing / (density / 100.0);
		//One set of regular infill lines for each notfullinfill path please
	m_lines.push_back(Lines());
	Line::generateLines(m_lines.back(), spacing, -dimMax*2.0, dimMax*2.0, -dimMax*2.0, dimMax*2.0);
	Line::rotateLines(m_lines.back(), angle);
	Line::translateLines(m_lines.back(), xoff, yoff);

	partialInfillClipper.AddPaths(notFullInfillOffset, RoVaClipperLib::ptClip, true);
	RoVaClipperLib::Paths infillPoly;
	for (auto it : m_lines)
	{
		for (auto lineIT : it)
		{
			RoVaClipperLib::Path newPath;
			newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt1.x*DEDOUBLER, lineIT.pt1.y*DEDOUBLER));
			newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt2.x*DEDOUBLER, lineIT.pt2.y*DEDOUBLER));
			infillPoly.push_back(newPath);
		}
	}
	partialInfillClipper.AddPaths(infillPoly, RoVaClipperLib::ptSubject, false);
	RoVaClipperLib::PolyTree out;
	m_lines.clear();
	partialInfillClipper.Execute(RoVaClipperLib::ctIntersection, out);
	RoVaClipperLib::Paths outPaths;
	RoVaClipperLib::OpenPathsFromPolyTree(out, outPaths);
	for (auto it : outPaths)
	{
		m_lines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
	}
	//Full infill lines

	if (!skipTopLogic)
	{
		double spacing = (Settings::getSingleton().getDoubleValue("topSolidExtrusionWidth"));

		m_fullInfillLines.push_back(Lines());
		Line::generateLines(m_fullInfillLines.back(), spacing, -dimMax*2.0, dimMax*2.0, -dimMax*2.0, dimMax*2.0);
		Line::rotateLines(m_fullInfillLines.back(), angle);
		Line::translateLines(m_fullInfillLines.back(), xoff, yoff);

		RoVaClipperLib::Clipper fullInfillClipper;


		fullInfillClipper.AddPaths(notFullInfillOffset, RoVaClipperLib::ptClip, true);
		RoVaClipperLib::Paths infillPoly;
		for (auto it : m_fullInfillLines)
		{
			//infillPoly.push_back(RoVaClipperLib::BodySplitterToClipperPath(it));
			for (auto lineIT : it)
			{
				RoVaClipperLib::Path newPath;
				newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt1.x*DEDOUBLER, lineIT.pt1.y*DEDOUBLER));
				newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt2.x*DEDOUBLER, lineIT.pt2.y*DEDOUBLER));
				infillPoly.push_back(newPath);
			}
		}
		RoVaClipperLib::PolyTree out;
		m_fullInfillLines.clear();
		fullInfillClipper.AddPaths(infillPoly, RoVaClipperLib::ptSubject, false);
		fullInfillClipper.Execute(RoVaClipperLib::ctDifference, out);
		RoVaClipperLib::Paths outPaths;
		RoVaClipperLib::OpenPathsFromPolyTree(out, outPaths);
		for (auto it : outPaths)
		{
			m_fullInfillLines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
		}
	}

}
// This is the prefered constructor, as the source lines do not need to be generated
/*
angle - alternates between the two angle sources in sources
sources - generated per mesh in slicer.cpp
notFullInfill - A path containing the area that is_NOT_full infill, outside this, could mean top solid/bottom solid
skipTopLogic - Set to true to ignore full infill methd
*/
Infill::Infill(double angle, const RoVaClipperLib::Paths *sources, const RoVaClipperLib::Paths &notFullInfill, bool skipTopLogic, bool isTop)
{
	//Figure out line spacing...
	//Spacing at 100% density is extrusion width
	RoVaClipperLib::PolyTree yes100ClippedLines;
	RoVaClipperLib::PolyTree not100ClippedLines;

	RoVaClipperLib::Clipper partialInfillClipper;

	RoVaClipperLib::Paths notFullInfillOffset(notFullInfill);
	RoVaClipperLib::ClipperOffset notFullShrinker;
	notFullShrinker.AddPaths(notFullInfillOffset, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	notFullShrinker.Execute(notFullInfillOffset, -Settings::getSingleton().getDoubleValue("extrusionWidth")*DEDOUBLER);
	partialInfillClipper.AddPaths(notFullInfillOffset, RoVaClipperLib::ptClip, true);
	if (angle < 50)
		partialInfillClipper.AddPaths(sources[0], RoVaClipperLib::ptSubject, false);
	else
		partialInfillClipper.AddPaths(sources[1], RoVaClipperLib::ptSubject, false);
	RoVaClipperLib::PolyTree out;
	m_lines.clear();
	if(!skipTopLogic)
		partialInfillClipper.Execute(RoVaClipperLib::ctIntersection, out);
	else
		partialInfillClipper.Execute(RoVaClipperLib::ctUnion, out);
	RoVaClipperLib::Paths outPaths;
	RoVaClipperLib::OpenPathsFromPolyTree(out, outPaths);
	for (auto it : outPaths)
	{
		m_lines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
	}

	//Full infill lines
	if (!skipTopLogic)
	{
		RoVaClipperLib::Clipper fullInfillClipper;
		fullInfillClipper.AddPaths(notFullInfillOffset, RoVaClipperLib::ptClip, true);
		RoVaClipperLib::Paths infillPoly;
		RoVaClipperLib::PolyTree out;
		m_fullInfillLines.clear();
		if (isTop)
		{
			if (angle < 50)
				fullInfillClipper.AddPaths(sources[4], RoVaClipperLib::ptSubject, false);
			else
				fullInfillClipper.AddPaths(sources[5], RoVaClipperLib::ptSubject, false);
		}
		else
		{
			if (angle < 50)
				fullInfillClipper.AddPaths(sources[2], RoVaClipperLib::ptSubject, false);
			else
				fullInfillClipper.AddPaths(sources[3], RoVaClipperLib::ptSubject, false);
		}
		fullInfillClipper.Execute(RoVaClipperLib::ctDifference, out);
		RoVaClipperLib::Paths outPaths;
		RoVaClipperLib::OpenPathsFromPolyTree(out, outPaths);
		for (auto it : outPaths)
		{
			m_fullInfillLines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
		}
	}
}
//Called complete as the original function cut perim followed by holes
void Infill::completeCut(const std::vector<Vertex2D> *perim, const std::vector<std::vector<Vertex2D>> *holes)
{
	RoVaClipperLib::Paths infillCutterPath;
	if (perim)
	{
		//Create the paths for the perimeter, offset by infilll overlap blah blah
		//oops didnt do that yet TODO: fix
		RoVaClipperLib::Path ClipPerim = RoVaClipperLib::BodySplitterToClipperPath(*perim);
		infillCutterPath.push_back(ClipPerim);

	}
	if (holes)
	{
		//We have holes, offset them too
		for (auto it : *holes)
		{
			auto tempPath = RoVaClipperLib::BodySplitterToClipperPath(it);
			//if(RoVaClipperLib::Area(tempPath) < 0)
			//	RoVaClipperLib::ReversePath(tempPath);
			infillCutterPath.push_back(tempPath);
		}
	}
	RoVaClipperLib::ClipperOffset offsetter;
	offsetter.AddPaths(infillCutterPath, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	offsetter.Execute(infillCutterPath, -DEDOUBLER*((Settings::GetSingleton().getDoubleValue("extrusionWidth")*(1.0 - Settings::GetSingleton().getDoubleValue("infillOutlineOverlapPercentage") / 100.0))));
	//for (auto &it : infillCutterPath)
	//{
	//	if (RoVaClipperLib::Area(it) < 0)
	//	{
	//		RoVaClipperLib::ReversePath(it);
	//	}
	//}
	cut(infillCutterPath);
}

void Infill::cut(const RoVaClipperLib::Paths &cutter)
{
	RoVaClipperLib::Clipper partialInfillClipper;
	RoVaClipperLib::Clipper fullInfillClipper;

	partialInfillClipper.AddPaths(cutter, RoVaClipperLib::ptClip,true);
	fullInfillClipper.AddPaths(cutter, RoVaClipperLib::ptClip,true);

	RoVaClipperLib::Paths infillPoly;
	for (auto it : m_lines)
	{
		infillPoly.push_back(RoVaClipperLib::BodySplitterToClipperPath(it));
	}
	RoVaClipperLib::Paths fullInfillPoly;
	for (auto it : m_fullInfillLines)
	{
		fullInfillPoly.push_back(RoVaClipperLib::BodySplitterToClipperPath(it));
	}
	partialInfillClipper.AddPaths(infillPoly, RoVaClipperLib::ptSubject, false);
	fullInfillClipper.AddPaths(fullInfillPoly, RoVaClipperLib::ptSubject, false);

	m_lines.clear(); m_fullInfillLines.clear();

	RoVaClipperLib::PolyTree partialSolution;
	RoVaClipperLib::PolyTree fullInfillSolution;
	partialInfillClipper.Execute(RoVaClipperLib::ctIntersection, partialSolution);// , RoVaClipperLib::pftEvenOdd, RoVaClipperLib::pftEvenOdd);
	fullInfillClipper.Execute(RoVaClipperLib::ctIntersection, fullInfillSolution);// , RoVaClipperLib::pftEvenOdd, RoVaClipperLib::pftEvenOdd);
																				  //Transfer these paths back to BodySplitter
	RoVaClipperLib::Paths partialSolutionPath;
	RoVaClipperLib::Paths fullInfillSolutionPath;
	RoVaClipperLib::OpenPathsFromPolyTree(partialSolution, partialSolutionPath);
	RoVaClipperLib::OpenPathsFromPolyTree(fullInfillSolution, fullInfillSolutionPath);

	for (auto it : partialSolutionPath)
	{
		m_lines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
	}
	for (auto it : fullInfillSolutionPath)
	{
		m_fullInfillLines.push_back(RoVaClipperLib::ClipperPathToBodySplitter(it, 0));
	}
}

void Infill::orderLines(const RoVaClipperLib::Paths &perims, bool straightJoin)
{
	//Convert the clipper poly to rs perim and holes to call the other order lines function - This is kinda clunky
	std::vector<Vertex2D> tempPerim = RoVaClipperLib::ClipperPathToBodySplitter(perims.front());
	std::vector<std::vector<Vertex2D>> tempHoles;
	if (perims.size() > 1)
	{
		for(int i = 1; i < perims.size(); i++)
			tempHoles.push_back(RoVaClipperLib::ClipperPathToBodySplitter(perims[i]));
	}
	orderLines(tempPerim, &tempHoles, straightJoin);
}

void Infill::orderLines(const std::vector<Vertex2D> &perim, const std::vector<std::vector<Vertex2D>>* holes, bool straightJoin)
{
	//This function bad, needs to be called automatically somewhere... the perim isnt stored here though...
	if (m_lines.empty() == false)
	{
		std::vector<Lines> perims;
		perims.push_back(BodySplitter::vertPathToLinePath(perim));
		if (holes)
		{
			//Holes are used here to prevent infill join lines from crossing them
			for (auto it : *holes)
				perims.push_back(BodySplitter::vertPathToLinePath(it));
		}
		sortedLines = MotionPlanner::arrangeInfill(m_lines, perims, GET_DOUBLE("infillDensity"), straightJoin);
		for (auto it : sortedLines)
			Line::addParamToSet(it, Line::LineType::INFILL);
	}
	if (m_fullInfillLines.empty() == false)
	{
		std::vector<Lines> perims;
		perims.push_back(BodySplitter::vertPathToLinePath(perim));
		if (holes)
		{
			for (auto it : *holes)
				perims.push_back(BodySplitter::vertPathToLinePath(it));
		}
		fullInfillLinesSorted = MotionPlanner::arrangeInfill(m_fullInfillLines, perims, GET_DOUBLE("topSurfaceInfillDensity"), straightJoin);
		//MotionPlanner::findShortestPath(fullInfillLines, fullInfillLinesSorted, perims);
		for (auto it : fullInfillLinesSorted)
			Line::addParamToSet(it, Line::LineType::INFILL | Line::LineType::TOP_SOLID);
	}
	m_fullInfillLines.clear();
	m_fullInfillLines.shrink_to_fit();
	fullInfillLinesSorted.shrink_to_fit();
	m_lines.clear();
	m_lines.shrink_to_fit();
	sortedLines.shrink_to_fit();
}

void Infill::colourLines()
{
	for (int i = 0; i < m_lines.size(); i++)
	{
		for (int j = 0; j < m_lines[i].size(); j++)
		{
			m_lines[i][j].setColour(
				Settings::getSingleton().getColourValue("infillColour"));
		}
	}
}

void Infill::setColour(const Colour &in)
{

	for (uint i = 0; i < m_fullInfillLines.size(); i++)
		Line::setLineSetToColour(m_fullInfillLines[i], in);
	for (int i = 0; i < m_lines.size(); i++)
	{
		for (int j = 0; j < m_lines[i].size(); j++)
		{
			m_lines[i][j].setColour(in);
		}
	}

}

Colour Infill::getColour() const
{
	return Colour(r, g, b);
}