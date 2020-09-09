#include <opencv2/core/core.hpp>
#include "Layer.h"
#include <spdlog/spdlog.h>
#ifdef _DEBUG
#include <wx/log.h>
#endif
#include "Line.h"
#include "Settings.h"
#include "Surface.h"
#include "Perimeter.h"
#include "Triangle.h"
#include "colour.h"
#include "Mesh.h"
#include "BodySplitterTexture.h"
#include <memory>
#include <map>
#include <stdio.h>
#include <set>
#include <fstream>
//
//Test files to C# program
#ifdef DUMP_TO_TEST_FILES
FILE *layerOUTPUT = nullptr;
FILE *trianglesOUTPUT = nullptr;
#endif


Layer::Layer(double z, unsigned int layerN, iSlicable::ToolType toolIn) : minZ(z), layerNumber(layerN), thisTool(toolIn)
{
#ifdef DUMP_TO_TEST_FILES
	if (layerOUTPUT == nullptr)
	{
		layerOUTPUT = fopen("layer.loops", "w");
	}
	if (trianglesOUTPUT == nullptr)
	{
		trianglesOUTPUT = fopen("layer.tris", "w");
	}
#endif
	if (layerN == 0)
		layerThickness = Settings::getSingleton().getDoubleValue("firstLayerThickness");
	else
		layerThickness = Settings::getSingleton().getDoubleValue("layerThickness");
}


Layer::~Layer()
{
	//	for (uint i = 0; i < m_Surfaces.size(); i++)
	//		delete m_Surfaces[i];
}

extern int gl_nSEL; // SEL debug

void Layer::AddPerimeters(const std::vector<Triangle> &triangles)
{
	auto logger = spdlog::get("DEBUG_FILE");
	std::vector<Perimeter> m_Perimeters;
	Lines outputLines;
	int intersectionCounter;
	for (Triangles::const_iterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		Vertex intersectionPts[3];
		intersectionCounter = 0;
		int linesEntirelyOnSlice = 0;
		//Check if a line is entirely on the plane
		bool isLineOnPlane = false;
		int linesOnPlane = 0;
		for (uint i = 0; i < 3; i++)  // for all 3 lines in the triangle
		{
#if 0
			// SEL - I believe Robert was testing to see if both points defining the line of the triangle have Z values
			// that will be contained within the layer thickness.  I think this should be between minZ and minZ+layerThicknes
			// Further, this test is redundant because it was done in Mesh::CreateSlice and the triangle will be marked with the .type of ALL_IN
			// I will leave it in after correcting it but flag it with ??? 
			if ((it->getLine(i).pt1.z >= (minZ )) && 
				(it->getLine(i).pt1.z < (minZ + layerThickness)) &&
				(it->getLine(i).pt2.z >= (minZ )) &&
				(it->getLine(i).pt2.z < (minZ + layerThickness)) )
#else
			if (it->getLine(i).pt1.z == minZ + layerThickness / 2.0 && it->getLine(i).pt2.z == minZ + layerThickness / 2.0) //SEL this will not work
			// it uses an equality check on a float and doesn't test for the thickness of the layer.
#endif
				{
				intersectionPts[intersectionCounter++] = it->getLine(i).pt1;
				intersectionPts[intersectionCounter++] = it->getLine(i).pt2;
				isLineOnPlane = true;
				linesOnPlane++;
			}
		}
		if (linesOnPlane > 1)	//If more than one line are on the plane, the whole triangle is a surface(or the geometry is broken)
			intersectionCounter = 0;
		for (uint i = 0; i < 3 && !isLineOnPlane; i++)
		{
			if (it->getLine(i).CrossesPlane(minZ + layerThickness / 2.0))
			{
				Vertex tmpPoint = it->getLine(i).pointAt(minZ + (layerThickness) / 2.0); // Move this back into the else clause after debugging
				if (intersectionCounter >= 2)
				{
					logger->warn("Too many intersections already");
				}
				else
				{
					//Check if the point has already been counted
					//Vertex tmpPoint = it->getLine(i).pointAt(minZ + (layerThickness) / 2.0);
					if (intersectionCounter > 0 &&
						intersectionPts[0].x == tmpPoint.x &&
						intersectionPts[0].y == tmpPoint.y)
					{

					}
					else
						intersectionPts[intersectionCounter++] = tmpPoint;
				}
			}
		}
		if (intersectionCounter == 2)
		{
			if (intersectionPts[0].MatchesLocation(intersectionPts[1]))
			{

			}
			else
				outputLines.push_back(Line(intersectionPts[0], intersectionPts[1], 0));
		}
		else// if (intersectionCounter != 2) //This function doesnt seem to fail....
		{
			//if (intersectionCounter > 0)
			//{
			//	logger->warn("Not 2 edges crossing layer:" + std::to_string(layerNumber));
			//	logger->warn("Intersectoin Count:" + std::to_string(intersectionCounter));
			//}
		}
	}


	//SEL Test  to add logging
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logAddPerimeters"))
	{
		std::cout << "Add perimeter\n";
		std::cout << "This many raw output lines " << outputLines.size() << "\n";
	}
	//wxLogDebug(std::string("Start Lines: " + std::to_string(outputLines.size())).c_str());
	Line::removeEqualOpposites(outputLines);
	Line::removeLineWithMultipleConnections(outputLines);

	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logAddPerimeters"))
	{
		std::cout << "This many simplified output lines " << outputLines.size() << "\n";
	}
	//wxLogDebug(std::string("End Lines: " + std::to_string(outputLines.size())).c_str());
	//BodySplitter::writeLinesToIPE(outputLines, "TestPerim" + std::to_string((int)layerNumber) + ".ipe");
	while (outputLines.size() > 2)
	{
		m_Perimeters.push_back(Perimeter(outputLines));
	}
	Perimeter::generateExRSPolygons(m_layerPolys, m_Perimeters, thisTool);

	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logAddPerimeters"))
	{
		std::cout << "This many polygons " << m_layerPolys.size() << "\n";
		std::cout << "MinZ" << minZ << "\n";
	}

	for (auto it : m_layerPolys)
	{
		clipperlib_perimeters.push_back(RoVaClipperLib::BodySplitterToClipperPath(it->m_contour->m_lines));
		if (gl_nSEL  && Settings::getSingleton().getBoolValue("logAddPerimeters"))
		{
			std::cout << "This many holes " << it->m_holes.size() << "\n";
			std::cout << it->m_contour->m_lines.size() << "\n";
		}
		for (auto holeIT : it->m_holes)
		{
			clipperlib_perimeters.push_back(RoVaClipperLib::BodySplitterToClipperPath(holeIT->m_lines));
		}
	}
}

void Layer::AddSurfaces(const std::vector<Triangle> &triangles)
{
	if (minZ <= layerThickness)
	{
		return;
	}
	int allInTriangles = 0;
	int partialTriangles = 0;
	std::vector<const Triangle*> trianglesToConsider;
	for (uint i = 0; i < triangles.size(); i++)
	{
		if (triangles[i].type == Triangle::ALL_IN)
		{
			if (triangles[i].isVertical())
			{

			}
			else
			{
				allInTriangles++;
				trianglesToConsider.push_back(&triangles[i]);
			}
		}
		else if (triangles[i].type == Triangle::MIXED)
		{
			//Find all non vertical triangles
			if (triangles[i].isVertical())
			{

			}
			else
			{
				//Disabled for now, too aggresive
				//partialTriangles++;
			//trianglesToConsider.push_back(&triangles[i]);
			}
		}
	}
#ifdef _DEBUG
	wxLogDebug("All in: %d Partial: %d", allInTriangles, partialTriangles);
#endif
	Triangles trisToAdd;
	for (uint i = 0; i < trianglesToConsider.size(); i++)
	{
		if (trianglesToConsider[i]->type == Triangle::ALL_IN)
		{
			trisToAdd.push_back(*trianglesToConsider[i]);
		}
		else
		{
#ifdef _DEBUG
			wxLogDebug("Adding cut triangle");
#endif
			//	Triangle::cutTriangle(*trianglesToConsider[i], minZ, minZ + LAYER_THICKNESS, trisToAdd);
				//newSurface.addTriangle(*trianglesToConsider[i], minZ, minZ+LAYER_THICKNESS);
		}
	}
	if (trisToAdd.empty())
		return;

	for (uint i = 0; i < trisToAdd.size(); i++)
		trisToAdd[i].FlattenToLayer(minZ + layerThickness / 2.0);
	std::unordered_map<std::size_t, Triangle*> triangleEdges;
	for (uint i = 0; i < trisToAdd.size(); i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			std::size_t edgeHash = trisToAdd[i].getLine(j).hash();
			if (triangleEdges.count(edgeHash))
			{
				trisToAdd[i].touchingEdges[trisToAdd[i].getLineNumberFromHash(edgeHash)] = triangleEdges[edgeHash];
				triangleEdges[edgeHash]->touchingEdges[triangleEdges[edgeHash]->getLineNumberFromHash(edgeHash)] = &trisToAdd[i];
				triangleEdges[edgeHash]->edgeTouchCount++;
				trisToAdd[i].edgeTouchCount++;
			}
			triangleEdges[trisToAdd[i].getLine(j).hash()] = &trisToAdd[i];
		}
	}
	bool foundOrphan = true;
	while (foundOrphan)
	{
		foundOrphan = false;
		for (uint i = 0; i < trisToAdd.size(); i++)
		{
			if (trisToAdd[i].parentSurface == nullptr)
			{
				std::shared_ptr<Surface> newSurface = std::make_shared<Surface>(layerNumber);
				newSurface->addTriangle(trisToAdd[i]);
				foundOrphan = true;
				m_Surfaces.push_back(newSurface);
			}
		}
	}

	//wxLogDebug("Total surfaces: %d", (int)m_Surfaces.size());

	//Infil triangles

	for (uint i = 0; i < m_Surfaces.size(); i++)
	{
		m_Surfaces[i]->generatePerimeters();
	}

	Colour tempColour = m_Surfaces.front()->m_ptrSurfacePoly->m_contour->getColour();

	RoVaClipperLib::Clipper joiner;
	joiner.AddPath(RoVaClipperLib::BodySplitterToClipperPath(m_Surfaces.front()->m_ptrSurfacePoly->m_contour->m_lines), RoVaClipperLib::ptSubject, true);
	for (int i = 1; i < m_Surfaces.size(); i++)
	{
		joiner.AddPath(RoVaClipperLib::BodySplitterToClipperPath(m_Surfaces[i]->m_ptrSurfacePoly->m_contour->m_lines), RoVaClipperLib::ptClip, true);
	}
	RoVaClipperLib::Paths output;
	joiner.Execute(RoVaClipperLib::ctUnion, output);

	RoVaClipperLib::Paths offsetPaths;
	RoVaClipperLib::ClipperOffset offsetter;
	offsetter.AddPaths(output, RoVaClipperLib::jtSquare, RoVaClipperLib::etClosedPolygon);

	offsetter.Execute(offsetPaths, DEDOUBLER*(-(Settings::GetSingleton().getDoubleValue("T"+std::to_string((int)thisTool)+"ExtWidth") / 2.0)));

	m_Surfaces.clear();

	for (int i = 0; i < offsetPaths.size(); i++)
	{
		m_Surfaces.push_back(std::make_shared<Surface>(layerNumber, RoVaClipperLib::ClipperPathToBodySplitter(offsetPaths[i], Line::LineType::SURFACE)));
	}
	/*
	for (int i = 0; i < m_Surfaces.size(); i++)
	{
		m_Surfaces[i]->infillSurface();
		m_Surfaces[i]->setColour(tempColour);
	}
	*/
}

void Layer::printColours()
{
	for (auto it : coloursOnLayer)
	{
		std::cerr << "R:" << (int)it.getR<uint8_t>()
			<< " G:" << (int)it.getG<uint8_t>()
			<< " B:" << (int)it.getB<uint8_t>() << "\n";
	}
}

void Layer::generateInnerPerimeters()
{
	for (std::list<std::shared_ptr<ExRSPolygon>>::iterator exPoly = m_layerPolys.begin(); exPoly != m_layerPolys.end(); ++exPoly)
	{
		(*exPoly)->generateInnerPerimeters();
	}
}

void Layer::InfillPerimeters(double density, const RoVaClipperLib::Paths& notFullInfill, bool skipTopLogic, const RoVaClipperLib::Paths *sources)
{
	//static bool lastdir = false;

	if (m_layerPolys.empty())
		return;
	double angle = 45;
	//if (fmod(minZ, Settings::GetSingleton().getDoubleValue("layerThickness")*2.0) <= Settings::GetSingleton().getDoubleValue("layerThickness"))
	if (layerNumber % 2)
		angle = 360.0 - 45.0;
	//SetPerimsToHoles();
	bool isTop = layerNumber > 0; //This should be improved...
	for (std::list<std::shared_ptr<ExRSPolygon>>::iterator exPoly = m_layerPolys.begin(); exPoly != m_layerPolys.end(); ++exPoly)
	{
		(*exPoly)->generateInfill(angle, layerNumber % 2, Settings::GetSingleton().getDoubleValue("infillDensity"), notFullInfill, skipTopLogic, sources, isTop);
	}
}

/* Figure out details about layer: colours, time etc*/
void Layer::completeLayer()
{
	for (std::list<std::shared_ptr<ExRSPolygon>>::const_iterator exPolyIT = m_layerPolys.begin(); exPolyIT != m_layerPolys.end(); ++exPolyIT)
	{
		(*exPolyIT)->m_contour = nullptr;
	}
	for (auto it : m_layerPolys)
	{
		coloursOnLayer.insert(it->getColour());
	}
	std::cerr << "Colours on layer: " << coloursOnLayer.size() << "\n";
}

bool Layer::isGood() const
{
	return !m_layerPolys.empty() || !m_Surfaces.empty();
}

void Layer::colourSurfaces(Mesh::MeshType type, BodySplitterTexture* pTexture)
{

}

void Layer::cutOutSurfaces()
{

}

void Layer::finalizeSurfaces()
{
	for (auto it : m_Surfaces)
	{
		it->infillSurface();
		it->setColour(Colour::InfillColour());
	}
}

double Layer::getLayerZ() const
{
	if (layerNumber == 0)
		return 0;
	return Settings::GetSingleton().getDoubleValue("firstLayerThickness") +
		(layerNumber - 1)*(Settings::GetSingleton().getDoubleValue("layerThickness"));
}

std::size_t Layer::getColourCount() const
{
	return coloursOnLayer.size();
}



void Layer::addRSPoly(std::shared_ptr<ExRSPolygon> inPoly)
{
	m_layerPolys.push_back(inPoly);
	coloursOnLayer.insert(inPoly->getColour());
}

void Layer::applyCrazyInfill(bool isUVMap) {
	std::list<std::shared_ptr<ExRSPolygon>> polys = m_layerPolys;
	m_layerPolys.clear();
	for (auto it = polys.begin(); it != polys.end(); ++it)
	{
		if ((*it)->m_contour->m_lines.empty())
			continue;
		auto thisSet = (*it)->straightSkeletonColours(isUVMap);
		//if (thisSet.empty())
		//{
		//	m_layerPolys.push_back(*it);
		//}
		//else
		m_layerPolys.splice(m_layerPolys.end(), thisSet);
	}
}

//SEL I'm changing this method to my liking for human readable form and also able to open with excel as a CSV file
//    Just create a scatter plot with lines and the perimeter can be plotted
void Layer::writeDebugPerimeters(const std::string& meshName, unsigned int layerID, unsigned int nOp)
{
#if 0
	int perimCounter = 0;
	for (auto &it : m_layerPolys)
	{
		std::ofstream outfile;
		outfile.open("mesh" + std::to_string(layerID) + ".rova");
		outfile << it->m_contour->m_lines.size() << "\n";
		for (auto &lineIT : it->m_contour->m_lines)
		{
			// Write colour
			outfile << (int)lineIT.colour.getR<uint8_t>() << " " << (int)lineIT.colour.getG<uint8_t>() << " " << (int)lineIT.colour.getB<uint8_t>() << "\n";
			outfile << lineIT.pt1.x << " " << lineIT.pt1.y << " " << lineIT.pt1.z << "\n";
			outfile << lineIT.pt2.x << " " << lineIT.pt2.y << " " << lineIT.pt2.z << "\n";
		}
		outfile.close();
	}
#endif
	int i = 0, j = 0;
	std::ofstream outfile;
	outfile.open("PerimetersOnLayer_" + std::to_string(layerID) + "_Stage_" + std::to_string(nOp) + ".txt");
	for (auto &it : m_layerPolys)
	{
		outfile << "Layer ID " << std::to_string(layerID) << " Perimeter " << i++ << " has " << it->m_contour->m_lines.size() << " lines.\n";
		j = 0;
		outfile << "Layer, Red, Green, Blue, Pt1X, Pt1Y, Pt1Z, Pt2X, Pt2Y, Pt2Z, Length\n"; // Header Line
		for (auto &lineIT : it->m_contour->m_lines)
		{
			outfile << j++ << ", ";
			// Write colour
			outfile << (int)lineIT.colour.getR<uint8_t>() << ", " << (int)lineIT.colour.getG<uint8_t>() << ", " << (int)lineIT.colour.getB<uint8_t>() << ", ";
			outfile << lineIT.pt1.x << ", " << lineIT.pt1.y << ", " << lineIT.pt1.z << ", ";
			outfile << lineIT.pt2.x << ", " << lineIT.pt2.y << ", " << lineIT.pt2.z << ", ";
			outfile << lineIT.length() << "\n";
		}
	}
	outfile.close();
}

//SEL
void Layer::RemoveShortLineSegments()
{
	double dLimit = Settings::GetSingleton().getDoubleValue("removeShortLines");
	double dLimitSquared = dLimit * dLimit;
	
	for (auto &it : m_layerPolys)
	{
		Line::removeZeroLengthLines(it->m_contour->m_lines);
		if(dLimit > 0.001)
		{
			Line::removeShortLines(it->m_contour->m_lines, dLimitSquared);
		}

		//SEL I had problems reducing holes.   I should check for area of the polygon before deciding on reducing lines
		//The reduced polygon only had one line which caused an assertion 
#if 0 
		for (auto &itHoles : it->m_holes)
		{
			Line::removeZeroLengthLines(itHoles->m_lines);
			if (dLimit > 0.0001)
			{
				Line::removeShortLines(itHoles->m_lines, dLimitSquared);
			}
		}
#endif
	}

}
