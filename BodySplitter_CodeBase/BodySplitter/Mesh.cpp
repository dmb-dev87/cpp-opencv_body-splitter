#include "Mesh.h"
#include <spdlog/spdlog.h>
#ifdef _DEBUG
#include <wx/log.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <assimp/Exporter.hpp>

#include "OpenglExt.h"
#include "PrintBed.h"

#include "SupportTower.h"
#include "BodySplitterTexture.h"
#include "Settings.h"
#include "ObjImporter.h"

#include "Triangle.h"
#include "AABoundingBox.h"
#include "Vertex.h"
#include "Layer.h"
#include "colour.h"
#include "SupportAnalyzer.h"
#include <thread>

#include <vector>

#include "SupportMesh.h"

#define MAX_BUFFER_SIZE 32*1024*1024 //32 mb

extern int gl_nSEL; //SEL Debug - verbose logging if true


Mesh::Mesh() : iDrawable(), m_bounds(nullptr),
	m_LayerArray(nullptr)
{

}


Mesh::~Mesh()
{
	if (m_LayerArray)
		delete[] m_LayerArray;
}

void Mesh::setToBed()
{
	double xMove = 0;
	double yMove = 0;
	m_Offset[Z_AXIS] = 0;
	applyTransformations();
	double minZValue = std::numeric_limits<double>::max();
	for (auto &it : m_triangles)
	{
		for (int i = 0; i < 3; i++)
			minZValue = std::min(minZValue, it.getPt(i).z);
	}

	centreBedMove[0] = xMove;
	centreBedMove[1] = yMove;
	undoTransformations();
	m_Offset[Z_AXIS] -= minZValue;
}

std::shared_ptr<Layer> Mesh::getLayer(unsigned int id)const
{
	assert(m_LayerArray);
	return m_LayerArray[id];
}

unsigned int Mesh::getLayerCount() const
{
	return layerCount;
}



void Mesh::startSlice(wxFrame *baseFrame)
{
	m_ptrVisualizer = std::make_shared<GCodeVisualizer>(this);
	if (thisType == MeshType::UVMap && m_bUseTexture && m_TexturePtr->isSimplified() == false && thisTool == ToolType::FULL_COLOUR)
	{
		wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, 10000);
		event.SetString("Analyzing texture, this may take awhile");
#ifndef _CONSOLE	// freelancer latino
		if (baseFrame)
			baseFrame->GetEventHandler()->AddPendingEvent(event);
#endif
		if (!GET_BOOL("useKMeans"))
		{
			m_TexturePtr->makeSimplified(0,0);
		}
		else
		{
			m_TexturePtr->makeSimplified(Settings::GetSingleton().getDoubleValue("kMeansColourCount"),
				Settings::GetSingleton().getDoubleValue("texturePrintRes"));
		}
	}
	undoTransformations();
	applyTransformations();
	if (m_LayerArray)
		delete[] m_LayerArray;
	layerCount = 0;// (getMaxZ() - getMinZ()) / (Settings::getSingleton().getDoubleValue("layerThickness"));
	double zDistance = (getMaxZ() - 0);// getMinZ()); ignore mesh below build plate
	//First layer
	layerCount++;
	zDistance -= Settings::GetSingleton().getDoubleValue("firstLayerThickness");
	layerCount += zDistance / Settings::GetSingleton().getDoubleValue("layerThickness");
	if (Settings::GetSingleton().getBoolValue("considerSurfaces"))
		layerCount++;//Avoid skipping the top surface layer
	m_LayerArray = new std::shared_ptr<Layer>[layerCount];
}

void Mesh::generateSkirt()
{
	if (getLayerCount() == 0)
		return;

	m_skirts.clear();
	auto polygons = m_LayerArray[0]->getRSPolies();

	RoVaClipperLib::Paths skirtOutline;
	for (auto &it : polygons)
	{
		RoVaClipperLib::Clipper joinUs;
		joinUs.AddPaths(skirtOutline, RoVaClipperLib::ptSubject, true);
		joinUs.AddPath(RoVaClipperLib::BodySplitterToClipperPath(it->m_contour->m_lines), RoVaClipperLib::ptClip, true);
		joinUs.Execute(RoVaClipperLib::ctUnion, skirtOutline, RoVaClipperLib::pftNonZero);
	}
	RoVaClipperLib::ClipperOffset offsetter;
	offsetter.AddPaths(skirtOutline, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	double offsetAmount = (Settings::GetSingleton().getDoubleValue("skirtOffset") + Settings::GetSingleton().getDoubleValue("skirtLoops")*
		Settings::GetSingleton().getDoubleValue("extrusionWidth"));
	offsetter.Execute(skirtOutline, offsetAmount*DEDOUBLER);
	for (auto pathIT : skirtOutline)
	{
		m_skirts.push_back(std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR));
		auto clipPath = RoVaClipperLib::ClipperPathToBodySplitter(pathIT,0);
		m_skirts.back()->setContour(Perimeter(clipPath));
		m_skirts.back()->generateInnerPerimeters(Settings::GetSingleton().getDoubleValue("skirtLoops"));
	}
}

RoVaClipperLib::Paths Mesh::getPerimeters(unsigned int layerID) {
	return m_LayerArray[layerID]->clipperlib_perimeters;
}

void Mesh::createSlice(unsigned int layerID)
{
	double minZ;
	if (layerID == 0)
	{
		minZ = 0;
	}           
	else
		//minZ = Settings::getSingleton().getDoubleValue("layerThickness") + (layerID - 1)*(Settings::getSingleton().getDoubleValue("layerThickness")); //SEL change layerThickness to firstLayerThickness
		minZ = Settings::getSingleton().getDoubleValue("firstLayerThickness") + (layerID - 1)*(Settings::getSingleton().getDoubleValue("layerThickness"));

	//Find relevent triangles
	const double OVERLAP = 0.001;
	m_LayerArray[layerID] = std::make_shared<Layer>(minZ,layerID,thisTool);
	Triangle::TriangleResult triResult;
	//Dont use the vertex list, verticies are not sorted...
	double thickness;
	if(layerID == 0)                                                                                                                    
		thickness = (Settings::GetSingleton().getDoubleValue("firstLayerThickness"));
	else
		thickness = (Settings::GetSingleton().getDoubleValue("layerThickness"));
	for (auto it = m_triangles.begin(); it != m_triangles.end(); ++it)
	{
#if 1
		//Check 3 verts  
		if (it->pt1.z <= minZ && it->pt2.z <= minZ && it->pt3.z <= minZ)
			triResult = Triangle::ALL_BELOW;
		else if (it->pt1.z >= minZ + thickness && it->pt2.z >= minZ + thickness && it->pt3.z >= minZ + thickness)
			triResult = Triangle::ALL_ABOVE;
		else if (it->pt1.z >= minZ && it->pt1.z <= minZ + thickness &&
			it->pt2.z >= minZ && it->pt2.z <= minZ + thickness &&
			it->pt3.z >= minZ && it->pt3.z <= minZ + thickness)
#else
		//Check 3 verts  SEL I think we should take better care of >= on the checks.  It is possible that the same
		// trianle can appear on more than one layer which would not be correct. When testing if in the layer  I think we should use >= on
		// the minimum test case and < on the maximum.   ALL_BELOW should be < only and ALL_ABOVE shoud be > only
		if (it->pt1.z < minZ && it->pt2.z < minZ && it->pt3.z < minZ)
			triResult = Triangle::ALL_BELOW;
		else if (it->pt1.z > minZ + thickness && it->pt2.z > minZ + thickness && it->pt3.z > minZ + thickness)
			triResult = Triangle::ALL_ABOVE;
		else if (it->pt1.z >= minZ && it->pt1.z < minZ + thickness &&
			it->pt2.z >= minZ && it->pt2.z < minZ + thickness &&
			it->pt3.z >= minZ && it->pt3.z < minZ + thickness)
#endif
		{
			triResult = Triangle::ALL_IN;
			m_LayerArray[layerID]->hasSurface = true;
			m_LayerArray[layerID]->hasPerimeter = true;
		}
		else
		{
			triResult = Triangle::MIXED;
			m_LayerArray[layerID]->hasSurface = true;  //SEL should this be false ???
			//m_LayerArray[layerID]->hasSurface = false;  //SEL I'm going to set this the way I think and test it later
			m_LayerArray[layerID]->hasPerimeter = true;
		}
		if (triResult == Triangle::ALL_ABOVE || triResult == Triangle::ALL_BELOW)
		{
			//Not on slice
		}
		else
		{
			Triangle newTriangle(*it);
			newTriangle.type = triResult;
			m_LayerArray[layerID]->m_triangles.push_back(newTriangle);
		}
	}
}

extern int gl_nSEL; //SEL debug I could put a debug logging catch in slicePerimeters if I want to trigger off of the slice layerID

void Mesh::slicePerimeters(unsigned int layerID)
{
	if (m_LayerArray[layerID]->hasPerimeter)
	{  
		if(((int)(Settings::getSingleton().getDoubleValue("debugLogSliceID")) == layerID))
		{
			gl_nSEL = layerID;  //SEL March 2020 I think it would be handy to have the layer ID for logging
		}

		m_LayerArray[layerID]->AddPerimeters(m_LayerArray[layerID]->m_triangles);
		
		if (gl_nSEL && Settings::getSingleton().getBoolValue("savePerimetersToTestFile"))
		{
			writePerimeterDebug(layerID,0);
		}

		//SEL added:  This would be the best time to combine lines that are too small
		m_LayerArray[layerID]->RemoveShortLineSegments();

		if (gl_nSEL && Settings::getSingleton().getBoolValue("savePerimetersToTestFile"))
		{
			writePerimeterDebug(layerID, 10);
		}


		//add loging of the perimeter before each of these methods
		if (thisType == MeshType::UVMap && m_bUseTexture && thisTool == ToolType::FULL_COLOUR)
		{
			splitPerimeterLinesOnContours(*m_LayerArray[layerID].get());
		}
		else if (thisType == MeshType::VertexColor)
		{
			spitPerimeterLinesForVertexColor(*m_LayerArray[layerID].get());
		}
		else if (m_Colour.isSet() || thisTool != ToolType::FULL_COLOUR)
			colourLayer(m_LayerArray[layerID]);
		
		if(gl_nSEL && Settings::getSingleton().getBoolValue("savePerimetersToTestFile"))
		{
			writePerimeterDebug(layerID,1);
		}
		
	//	if (Settings::getSingleton().getBoolValue("useCrazyInfill"))
		{
			//Crazy infill also applies the offset to accomodate the extrusion width
			m_LayerArray[layerID]->applyCrazyInfill(thisType == MeshType::UVMap);
		}
		m_LayerArray[layerID]->generateInnerPerimeters();

		if (gl_nSEL && Settings::getSingleton().getBoolValue("savePerimetersToTestFile"))
		{
			writePerimeterDebug(layerID,2);
		}

		gl_nSEL = 0;  //SEL debug  turn off verbose logging for next slice

	}
}

void Mesh::writePerimeterDebug(unsigned int layerID, unsigned int nOp)
{
  m_LayerArray[layerID]->writeDebugPerimeters(this->drawableName,layerID, nOp);
}

void Mesh::colourLayer(std::shared_ptr<Layer> layer)
{
	for (auto &it : layer->getRSPolies())
	{
		for (auto &lineIT : it->m_contour->m_lines)
		{
			if (thisTool == ToolType::FULL_COLOUR)
				lineIT.colour = m_Colour;
			else if (thisTool == ToolType::FLEXIBLE)
				lineIT.colour = GET_COLOUR("t1Colour");
			else if (thisTool == ToolType::SUPPORT)
				lineIT.colour = GET_COLOUR("t2Colour");
		}
	}
}

void Mesh::fillInfill(unsigned int layerID ,const RoVaClipperLib::Paths &fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic)
{
	if (Settings::getSingleton().getBoolValue("useInfill"))
	{
		if (m_LayerArray[layerID]->hasPerimeter)
			m_LayerArray[layerID]->InfillPerimeters(Settings::GetSingleton().getDoubleValue("infillDensity"),fullInfill,skipTopLogic, sources);
	}
}

void Mesh::createSurfaces(unsigned int layerID)
{
	//Create surfaces
	if (m_LayerArray[layerID]->hasSurface)
	{
		if (Settings::getSingleton().getBoolValue("considerSurfaces"))
		{
			bool allIn = false;
			for (auto it : m_LayerArray[layerID]->m_triangles)
			{
				allIn = (it.type == Triangle::ALL_IN);
				if (allIn)
					break;
			}
			if(allIn)
				m_LayerArray[layerID]->AddSurfaces(m_LayerArray[layerID]->m_triangles);
		}
	}
	if (m_LayerArray[layerID]->hasPerimeter && m_LayerArray[layerID]->hasSurface)
	{
		if (Settings::getSingleton().getBoolValue("considerSurfaces"))
		{
			//Cut out surfaces
			m_LayerArray[layerID]->cutOutSurfaces();
		}
	}
	m_LayerArray[layerID]->finalizeSurfaces();
}

void Mesh::completeLayer(unsigned int layerID)
{
  m_LayerArray[layerID]->completeLayer();
  isSliced = true;
}

void Mesh::MoveMeshZero()
{
	PrintBed::getSingleton().changesNeedSaving();
	double xMove = -(m_bounds->getMinX());
	double yMove = -(m_bounds->getMinY());
	double zMove = -(m_bounds->getMinZ());

	m_Offset[X_AXIS] = xMove;
	m_Offset[Y_AXIS] = yMove;
	m_Offset[Z_AXIS] = zMove;
	return;
}

void Mesh::scaleMesh(double x, double y, double z)
{
	for (uint i = 0; i < m_verts.size(); ++i)	//drop iterators for performance especially in debug mode
	{
		m_verts[i].x *= x;
		m_verts[i].y *= y;
		m_verts[i].z *= z;
	}
	for (uint i = 0; i < m_triangles.size(); ++i)
	{
		m_triangles[i].pt1.x *= x; m_triangles[i].pt2.x *= x; m_triangles[i].pt3.x *= x;
		m_triangles[i].pt1.y *= y; m_triangles[i].pt2.y *= y; m_triangles[i].pt3.y *= y;
		m_triangles[i].pt1.z *= z; m_triangles[i].pt2.z *= z; m_triangles[i].pt3.z *= z;
	}

	if (m_bounds)
		delete m_bounds;
	m_bounds = new AABoundingBox(m_verts);
}

void Mesh::moveTo(double x, double y, double z)
{
	PrintBed::getSingleton().changesNeedSaving();
	m_Offset[X_AXIS] = x;
	m_Offset[Y_AXIS] = y;
}

/* moves the mesh for printing purposes*/
void Mesh::moveMesh(double x, double y, double z)
{
	for (uint i = 0; i < m_verts.size(); ++i)	//drop iterators for performance especially in debug mode
	{
		m_verts[i].x += x;
		m_verts[i].y += y;
		m_verts[i].z += z;
	}
	for (uint i = 0; i < m_triangles.size(); ++i)
	{
		m_triangles[i].pt1.x += x; m_triangles[i].pt2.x += x; m_triangles[i].pt3.x += x;
		m_triangles[i].pt1.y += y; m_triangles[i].pt2.y += y; m_triangles[i].pt3.y += y;
		m_triangles[i].pt1.z += z; m_triangles[i].pt2.z += z; m_triangles[i].pt3.z += z;
	}
	
	if (m_bounds)
		delete m_bounds;
	m_bounds = new AABoundingBox(m_verts);
}

double Mesh::getMaxZ() const
{
	if (Settings::GetSingleton().getDoubleValue("endZ") >= 0)
	{
		return std::min((Settings::GetSingleton().getDoubleValue("endZ")), m_bounds->getMaxZ());
	}
	return m_bounds->getMaxZ();
}

double Mesh::getMinZ() const
{
	return m_bounds->getMinZ();
}

void Mesh::rotateMesh(double angle, int axis)
{
	std::cerr << "Apply rotation: "<<angle<<" Axis: "<<axis<<"\n";
	aiMatrix3x3 rotationMat;
	if (axis == 0)
		aiMatrix3x3::Rotation(angle, aiVector3D(1, 0, 0), rotationMat);
	else if (axis == 1)
		aiMatrix3x3::Rotation(angle, aiVector3D(0, 1, 0), rotationMat);
	else
		aiMatrix3x3::Rotation(angle, aiVector3D(0, 0, 1), rotationMat);



	for (uint i = 0; i < m_verts.size(); ++i)	//drop iterators for performance especially in debug mode
	{
		aiVector3D temp(m_verts[i].x, m_verts[i].y, m_verts[i].z);
		temp *= rotationMat;
		m_verts[i].x = temp.x;
		m_verts[i].y = temp.y;
		m_verts[i].z = temp.z;
	}
	for (uint i = 0; i < m_triangles.size(); ++i)
	{
		aiVector3D temp;
		temp.x = m_triangles[i].pt1.x;
		temp.y = m_triangles[i].pt1.y;
		temp.z = m_triangles[i].pt1.z;
		temp *= rotationMat;
		m_triangles[i].pt1.x = temp.x;
		m_triangles[i].pt1.y = temp.y;
		m_triangles[i].pt1.z = temp.z;

		temp.x = m_triangles[i].pt2.x;
		temp.y = m_triangles[i].pt2.y;
		temp.z = m_triangles[i].pt2.z;
		temp *= rotationMat;
		m_triangles[i].pt2.x = temp.x;
		m_triangles[i].pt2.y = temp.y;
		m_triangles[i].pt2.z = temp.z;

		temp.x = m_triangles[i].pt3.x;
		temp.y = m_triangles[i].pt3.y;
		temp.z = m_triangles[i].pt3.z;
		temp *= rotationMat;
		m_triangles[i].pt3.x = temp.x;
		m_triangles[i].pt3.y = temp.y;
		m_triangles[i].pt3.z = temp.z;
	}
	if (m_bounds)
		delete m_bounds;
	m_bounds = new AABoundingBox(m_verts);
}

void Mesh::splitPerimeterLinesOnContours(Layer &layer)
{
	//Do contour first
	for (auto layerIT = layer.getRSPolies().begin(); layerIT != layer.getRSPolies().end(); ++layerIT) {
		Perimeter *activePerim = (*layerIT)->m_contour.get();
//		activePerim->simplify();

		if (GET_BOOL("simplifyColourPerLine"))
		{
			activePerim->breakLineIntoPixelColours(m_TexturePtr.get());
		}
		else
			splitByPerim(activePerim);

		if (GET_BOOL("travisSimplification"))
		{
			activePerim->travisSimplification();
		}

		for (auto holeIT = (*layerIT)->m_holes.begin(); holeIT != (*layerIT)->m_holes.end(); ++holeIT)
		{
			splitByPerim((*holeIT).get());
		}
	}
}

void Mesh::splitByPerim(Perimeter* perim)
{
	//Perimeter* activePerim = &layer.m_Perimeters[j];
	std::vector<Line> linesToProcess;
	//bool moreToGo = true;
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
	{
		std::cout << "************* Split by perimeter ***************\n";
	}

	//while (moreToGo)//commented out by CG on March 28th 2020 because it can only execute once
	//{
		//moreToGo = false;
	for (uint k = 0; k < perim->m_lines.size(); k++)
	{
		std::vector<Line> newLines;

		if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
		{
			std::cout << "Line " << k << " of " << perim->m_lines.size() << "\n";
		}

		//wxLogDebug("splitByPerim: Line (%2.2f,%2.2f) to (%2.2f, %2.2f)", linesToProcess[k].pt1.x, linesToProcess[k].pt1.y, linesToProcess[k].pt2.x, linesToProcess[k].pt2.y);
		if (m_TexturePtr->LineCrossesContour(perim->m_lines[k], newLines))
		{
			//linesToProcess.erase(linesToProcess.begin() + k);
			//wxLogDebug("Line %u on layer %2.2f interects contour", k, linesToProcess[k].pt1.z);
			//	moreToGo = true;
			//////Must split the line

			if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
			{
				std::cout << "We Must split this line into " << newLines.size() << " Lines \n";
			}

			for (uint m = 0; m < newLines.size(); m++)
			{
				// Get the colour in approx the centre of the line
				double u = (newLines[m].pt1.u + newLines[m].pt2.u) / 2.0;
				double v = (newLines[m].pt1.v + newLines[m].pt2.v) / 2.0;

				newLines[m].setColour(m_TexturePtr->getColour(
					u, v));

				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
				{
					std::cout << "Line " << m << " Colour at center (" << u << ", " << v << ") = " << m_TexturePtr->getColour(u, v) << "\n";
					std::cout << "Pt1 = (" << newLines[m].pt1.x << ", " << newLines[m].pt1.y << ", " << newLines[m].pt1.z << ")  Pt2 = (";
					std::cout << newLines[m].pt2.x << ", " << newLines[m].pt2.y << ", " << newLines[m].pt2.z << ") ";
					std::cout << ")  Length = " << newLines[m].length() << "\n";
				}
			}
			linesToProcess.insert(linesToProcess.end(), newLines.begin(), newLines.end());
		}
		else
		{
			Line newLine = perim->m_lines[k];
			// Get the colour in approx the centre of the line
			double u = (perim->m_lines[k].pt1.u + perim->m_lines[k].pt2.u) / 2.0;
			double v = (perim->m_lines[k].pt1.v + perim->m_lines[k].pt2.v) / 2.0;
			newLine.setColour(m_TexturePtr->getColour(
				u, v));
			linesToProcess.push_back(newLine);

			if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
			{
				std::cout << "Don't split this line. Colour at center (" << u << ", " << v << ") = " << m_TexturePtr->getColour(u, v) << "\n";
				std::cout << "Pt1 = (" << newLine.pt1.x << ", " << newLine.pt1.y << ", " << newLine.pt1.z << ")  Pt2 = (";
				std::cout << newLine.pt2.x << ", " << newLine.pt2.y << ", " << newLine.pt2.z;
				std::cout << ")  Length = " << newLine.length() << "\n";
			}
		}
		//}
	}




#if 0  // the old way - I'm not going to use this either
		perim->m_lines.clear();

		uint8_t nonMatch = 0;
		if (i == 0)
		{
			if (linesToProcess.back().getColour() != linesToProcess[0].getColour())
				nonMatch++;
		}
		else
		{
			if (linesToProcess[i - 1].getColour() != linesToProcess[i].getColour())
				nonMatch++;
		}
		if (i + 1 == linesToProcess.size())
		{
			if (linesToProcess.front().getColour() != linesToProcess[i].getColour())
				nonMatch++;
		}
		else
		{
			if (linesToProcess[i].getColour() != linesToProcess[i + 1].getColour())
				nonMatch++;
		}

		if (nonMatch == 2 && linesToProcess[i].sqrdLength() < pow(Settings::getSingleton().getDoubleValue("minimumLineLength"), 2))
		{
			if (i == 0)
				linesToProcess[i].setColour(linesToProcess[i + 1].getColour());
			else
				linesToProcess[i].setColour(linesToProcess[i - 1].getColour());
		}

		perim->m_lines.push_back(linesToProcess[i]);
	}
#endif

	perim->m_lines.clear();
	perim->m_lines = linesToProcess;


}

void Mesh::spitPerimeterLinesForVertexColor(Layer &layer)
{
// 	for (auto it = layer.m_layerPolys.begin(); it != layer.m_layerPolys.end(); ++it)
// 	{
// 		splitLinesByVertex((*it)->m_contour->m_lines);
// 
// 		for (auto holeIT = (*it)->m_holes.begin(); holeIT != (*it)->m_holes.end(); ++holeIT)
// 		{
// 			splitLinesByVertex((*holeIT)->m_lines);
// 		}
// 	}
}

void Mesh::sloppyColourTexture(Layer &layer)
{
	//for (auto it = layer.m_layerPolys.begin(); it != layer.m_layerPolys.end(); ++it)
	//{
	//	/*
	//	if ((*it)->m_infillPiece)
	//	{
	//		for (int i = 0; i < (*it)->m_infillPiece->m_lines.size(); i++)
	//		{
	//			for (int m = 0; m < (*it)->m_infillPiece->m_lines[i].size(); m++)
	//			{
	//				(*it)->m_infillPiece->m_lines[i][m].setColour(m_ptrTexture->getColour((*it)->m_infillPiece->m_lines[i][m].pt1.u, (*it)->m_infillPiece->m_lines[i][m].pt1.v));
	//			}
	//		}
	//	}
	//	*/
	//}
	/*
	for (int i = 0; i < layer.m_Surfaces.size(); i++)
	{
		for (int j = 0; j < layer.m_Surfaces[i]->m_lines.size(); j++)
		{
			for (int m = 0; m < layer.m_Surfaces[i]->m_lines[j].m_lines.size(); m++)
			{
				layer.m_Surfaces[i]->m_lines[j].m_lines[m].setColour(m_ptrTexture->getColour(
					layer.m_Surfaces[i]->m_lines[j].m_lines[m].pt1.u, layer.m_Surfaces[i]->m_lines[j].m_lines[m].pt1.v));
			}
		}
		//for (int j = 0; j < layer.m_Surfaces[i]->m_infillPieces.size(); j++)
		//{
		//	//layer.m_Surfaces[i]->m_infillPieces
		//	for (int k = 0; k < layer.m_Surfaces[i]->m_infillPieces[j].m_lines.size(); k++)
		//	{
		//		for (int m = 0; m < layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k].size(); m++)
		//		{
		//			layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][m].setColour(m_ptrTexture->getColour(
		//				layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][m].pt1.u, layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][m].pt1.v
		//				));
		//		}
		//	}
		//}
			//splitLinesByVertex(layer.m_Surfaces[i]->m_lines[j].m_lines);
			/*
			for (int k = 0; k < layer.m_Surfaces[i]->m_infillPieces[j].m_lines.size(); k++)
			{
				for (int n = 0; n < layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k].size(); n++)
				{
					layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][n].setColour(m_ptrTexture->getColour(
						layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][n].pt1.u, layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k][n].pt1.v));
				}
				//splitLinesByVertex(layer.m_Surfaces[i]->m_infillPieces[j].m_lines[k]);
			}
			*/
		
	//}
}

void Mesh::updateTextureColour(uint16_t newColours)
{
	m_TexturePtr->adjustColour(newColours);
}

void Mesh::makeSliced()
{
	m_bisSliced = true;
}

unsigned int Mesh::getLineCount() const
{
	unsigned int count = 0;
	return count;
}



RoVaClipperLib::Paths Mesh::getEnclosingPerimeters() const
{
	if (!m_enclosingPerimeter.empty())
		return m_enclosingPerimeter;

	RoVaClipperLib::Paths m_enclosingPerimeter;
	RoVaClipperLib::Clipper triangleXYSum;
	bool isFirst = true;
	for (auto &it : m_triangles)
	{
		RoVaClipperLib::Path trianglePath;
		RoVaClipperLib::Clipper tempWorker;
		tempWorker.AddPaths(m_enclosingPerimeter, RoVaClipperLib::ptSubject, true);
		for(int i = 0;i < 3; ++i)
			trianglePath.push_back(it.getPt(i));
		tempWorker.AddPath(trianglePath, RoVaClipperLib::ptClip, true);
		tempWorker.Execute(RoVaClipperLib::ctUnion, m_enclosingPerimeter, RoVaClipperLib::pftPositive, RoVaClipperLib::pftPositive);
	}
//	triangleXYSum.Execute(RoVaClipperLib::ctUnion, output,RoVaClipperLib::pftNonZero,RoVaClipperLib::pftNonZero);
	return m_enclosingPerimeter;
}

void Mesh::setOFfsetX(double newPos)
{
	m_Offset[X_AXIS] = newPos;
}

void Mesh::setOFfsetY(double newPos)
{
	m_Offset[Y_AXIS] = newPos;
}

void Mesh::setOFfsetZ(double newPos)
{
	//undoTransformations();
	m_Offset[Z_AXIS] = newPos;
	//applyTransformations();
}

double Mesh::getOffsetX() const
{
  return m_Offset[X_AXIS];
}
double Mesh::getOffsetY() const
{
  return m_Offset[Y_AXIS];
}
double Mesh::getOffsetZ() const
{
  return m_Offset[Z_AXIS];
}


//SEL Jan 2020
//
//  This member function is called to export all the created polygons for every layer into a text file
//  for use by BodySplitter.   I think I will also log a Ruby script that can be run on SketchUp to visualize all the polygons
//  in their own colour separated by layer.
//
// I added booleans to the ini file to enable the export feature.   If polygons are exported then we will not have the offset of the extrusion width and the program will terminate
// after this function is completed
//      exportPolygons 
//      exportPolygonsToSketchup
//
void Mesh::LogPerimeters()
{
	int bExportToSketchup;
	int nLayer, nPoly, nHole, nHoles, nSegments, nLine;
	double dLayerZ;
	Colour itColour;
	std::fstream fSU;

	if (!Settings::getSingleton().getBoolValue("exportPolygons"))
		return;  

	bExportToSketchup = Settings::getSingleton().getBoolValue("exportPolygonsToSketchup");

	if (bExportToSketchup)
	{
		fSU.open("exportedPolygons.rb", std::fstream::out);
		if (!fSU.is_open())
		{
			std::cerr << "Unable to open exportedPolygons.rb file \n";
			bExportToSketchup = 0;
		}
	}

	// I'm going to hardcode a file name
	std::fstream fs("exportedPolygons.txt",std::fstream::out);

	// test that file is open
	if (!fs.is_open())
	{
		std::cerr << "Unable to open exportedPolygons.txt file \n";
		return;
	}

	//log number of layers
	fs << "[Object Attributes]\n";
	fs << "NumberOfLayers= " << layerCount << "\n";

	if (bExportToSketchup)
	{
		fSU << "model = Sketchup.active_model\n";
		fSU << "ents = model.active_entities\n";
		fSU << "Pt = []\n";

	}

	//for each layer
	for (nLayer = 0; nLayer < layerCount; nLayer++)
	{
		fs << "\n[Layer " << nLayer << "]\n";

		//get z height because this is the same number for the entire layer
		dLayerZ = m_LayerArray[nLayer]->getLayerZ();
		fs << "Z= " << dLayerZ << "\n";

		//log number of polygons
		fs << "NumPolys= " << m_LayerArray[nLayer]->m_layerPolys.size() << "\n\n";

		//for each polygon std::list<std::shared_ptr<ExRSPolygon>>
		nPoly = 0;
		for ( auto itPoly = m_LayerArray[nLayer]->m_layerPolys.begin(); itPoly != m_LayerArray[nLayer]->m_layerPolys.end(); ++itPoly, nPoly++)
		{

			if ((*itPoly)->m_contour->Area() < 2)
			{
				nSegments = 0;
			}
			else
			{
				nSegments = (*itPoly)->m_contour->m_lines.size();
			}
			
			//log number of segments
			fs << "[Layer" << nLayer << "Poly" << nPoly << "]\n";
			fs << "Segs=" << nSegments << "\n";

			//log colour
			itColour = (*itPoly)->getColour();
			fs << "R= " << (int)(itColour.getR<uint8_t>()) << "\n";
			fs << "G= " << (int)(itColour.getG<uint8_t>()) << "\n";
			fs << "B= " << (int)(itColour.getB<uint8_t>()) << "\n";

			if (nSegments)
			{
				if (bExportToSketchup)
				{
					fSU << "Pt.clear\n";
				}

				//for each line
				nLine = 0;
				for (auto itLine = (*itPoly)->m_contour->m_lines.begin(); itLine != (*itPoly)->m_contour->m_lines.end(); itLine++, nLine++)
				{

					//log coordinates of first point only
					fs << "Pt" << nLine << "= " << (*itLine).pt1.x << ", " << (*itLine).pt1.y << "\n";
					if (bExportToSketchup)
					{
						fSU << "Pt[" << nLine << "] = [";
						fSU << (*itLine).pt1.x << ", " << (*itLine).pt1.y << ", " << dLayerZ << "]\n";
					}

				}// for each line segment
				// close the loop back to the first point, which should be the same as pt2 on the last line
				fs << "Pt" << nLine << "= " << ((*itPoly)->m_contour->m_lines.begin()->pt1.x) << ", " << ((*itPoly)->m_contour->m_lines.begin()->pt1.y) << "\n\n";
				if (bExportToSketchup)
				{
					// close the curve and create a face entity
					fSU << "Pt[" << nLine << "] = [";
					fSU << ((*itPoly)->m_contour->m_lines.begin()->pt1.x) << ", " << ((*itPoly)->m_contour->m_lines.begin()->pt1.y) << ", " << dLayerZ << "]\n";
					fSU << "faceL" << nLayer << "P" << nPoly << " = ents.add_face ents.add_curve Pt\n";

					// set the material colour to this face (front only)
					fSU << "faceL" << nLayer << "P" << nPoly << ".material = [";
					fSU << (int)(itColour.getR<uint8_t>()) << ", ";
					fSU << (int)(itColour.getG<uint8_t>()) << ", ";
					fSU << (int)(itColour.getB<uint8_t>()) << "]\n";
					// set the back material colour to this face (front only)
					fSU << "faceL" << nLayer << "P" << nPoly << ".back_material = [";
					fSU << (int)(itColour.getR<uint8_t>()) << ", ";
					fSU << (int)(itColour.getG<uint8_t>()) << ", ";
					fSU << (int)(itColour.getB<uint8_t>()) << "]\n\n\n";
				}

				//log number of holes
				nHoles = (*itPoly)->m_holes.size();
				fs << "NumberOfHoles= " << nHoles << "\n";

				nHole = 0;
				//for each hole
				for (auto itHole = (*itPoly)->m_holes.begin(); itHole != (*itPoly)->m_holes.end(); itHole++, nHole++)
				{

					//log number of segments
					nSegments = (*itHole)->m_lines.size();
					fs << "[Layer" << nLayer << "Poly" << nPoly << "Hole" << nHole << "]\n";
					fs << "Segs=" << nSegments << "\n";

					//log colour
					itColour = (*itHole)->getColour();
					fs << "R= " << (int)(itColour.getR<uint8_t>()) << "\n";
					fs << "G= " << (int)(itColour.getR<uint8_t>()) << "\n";
					fs << "B= " << (int)(itColour.getR<uint8_t>()) << "\n";


					//for each line
					nLine = 0;
					for (auto itLine = (*itHole)->m_lines.begin(); itLine != (*itHole)->m_lines.end(); itLine++, nLine++)
					{

						//log coordinates of first point only
						fs << "Pt" << nLine << "= " << (*itLine).pt1.x << ", " << (*itLine).pt1.y << "\n";

					}// for each line segment
					 // close the loop back to the first point, which should be the same as pt2 on the last line
					fs << "Pt" << nLine << "= " << ((*itHole)->m_lines.begin()->pt1.x) << ", " << ((*itHole)->m_lines.begin()->pt1.y) << "\n\n";


				}// for all holes in this poly
			}//if poly is large enough to keep
		}// for each Poly
	
	}//for each layer

	fs.close();
	if (bExportToSketchup)
	{
		fSU.close();
	}
}

// iDrawble
void Mesh::applyRotation(double zRotation)
{
	//undoTransformations();
	this->rotateMesh(AI_DEG_TO_RAD(zRotation), 2);
	//applyTransformations();
}
void Mesh::applyScale(double amt)
{
	//undoTransformations();
	scaleMesh(amt, amt, amt);
	//applyTransformations();
}
double Mesh::getXExtent() const
{
	return m_bounds->getMaxX()-m_bounds->getMinX();
}
double Mesh::getYExtent()  const
{
	return m_bounds->getMaxY() - m_bounds->getMinY();
}

double Mesh::getZExtent() const
{
	return m_bounds->getMaxZ() - m_bounds->getMinZ();
}

void Mesh::setMaxSize()
{
	double xRatio = Settings::getSingleton().getDoubleValue("maxX")/getXExtent();
	double yRatio = Settings::getSingleton().getDoubleValue("maxY") / getYExtent();
	double zRatio = Settings::getSingleton().getDoubleValue("maxZ") / getZExtent();
	double minRatio = std::min(xRatio, std::min(yRatio, zRatio));
	this->setXScale(minRatio);
	this->setYScale(minRatio);
	this->setZScale(minRatio);
}



bool Mesh::hasTexture() const
{
	return thisType == MeshType::UVMap;
}

void Mesh::applyTransformations()
{
	if (m_bIsTransformed)
		undoTransformations();
	std::copy(m_Rotation, m_Rotation + 3, m_lastAppliedRotation);
	this->rotateMesh(AI_DEG_TO_RAD(m_Rotation[X_AXIS]), 0);
	this->rotateMesh(AI_DEG_TO_RAD(m_Rotation[Y_AXIS]), 1);
	this->rotateMesh(AI_DEG_TO_RAD(m_Rotation[Z_AXIS]), 2); 
	std::copy(m_Scale, m_Scale + 3, m_lastAppliedScale);
	this->scaleMesh(m_Scale[X_AXIS], m_Scale[Y_AXIS], m_Scale[Z_AXIS]);
	m_lastAppliedOffset[Z_AXIS] = getOffsetZ();
	this->moveMesh(0, 0, getOffsetZ());
	m_bIsTransformed = true;
}

void Mesh::undoTransformations()
{
	if (m_bIsTransformed)
	{
		this->moveMesh(0, 0, -m_lastAppliedOffset[Z_AXIS]);
		//m_Offset[Z_AXIS] = m_lastAppliedOffset[Z_AXIS];
		this->scaleMesh(1.0 / m_lastAppliedScale[X_AXIS], 1.0 / m_lastAppliedScale[Y_AXIS], 1.0 / m_lastAppliedScale[Z_AXIS]);
		//std::copy(m_lastAppliedScale, m_lastAppliedScale + 3, m_Scale);
		this->rotateMesh(AI_DEG_TO_RAD(-m_lastAppliedRotation[Z_AXIS]), 2);
		this->rotateMesh(AI_DEG_TO_RAD(-m_lastAppliedRotation[Y_AXIS]), 1);
		this->rotateMesh(AI_DEG_TO_RAD(-m_lastAppliedRotation[X_AXIS]), 0);
		//std::copy(m_lastAppliedRotation, m_lastAppliedRotation + 3, m_Rotation);
	}
	m_bIsTransformed = false;
}

std::unordered_set<uint32_t> Mesh::getColours()
{
	if (!m_bUseTexture && m_Colour.isSet())
	{

		return std::unordered_set<uint32_t>{m_Colour};
	}
	//else if (m_Colour.isSet())
	//{
	//	return std::unordered_set<uint32_t>{m_Colour};
	//}
	//m_TexturePtr->makeSimplified(Settings::GetSingleton().getDoubleValue("colourRes"),
	//	Settings::GetSingleton().getDoubleValue("texturePrintRes"));
	if (m_TexturePtr)
		return m_TexturePtr->getColours();
	else
		return std::unordered_set<uint32_t>();
}

std::set<uint16_t> Mesh::getToolsUsed() {
	std::set<uint16_t> temp;
	temp.insert((uint16_t)this->getTool());
	return temp;
}

bool Mesh::isColour() const
{
	return (m_bUseTexture || m_Colour.isSet());
}