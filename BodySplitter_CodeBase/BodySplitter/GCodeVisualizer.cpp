#include <spdlog/spdlog.h>
#include "GCodeVisualizer.h"
#include "Mesh.h"
#include "Layer.h"
#include "Settings.h"
#include "MotionPlanner.h"
#include "Settings.h"

void setColourByType(const Line const* currentLine, double &r, double &g, double &b)
{
	if (currentLine->isLineOfType(Line::LineType::CREATED_NO_PARAM))
	{
		r = 0.1*255.0;
		g = 0.1*255.0;
		b = 0.1*255.0;
	}
	if ((currentLine->isLineOfType(Line::LineType::OUTERPERIM)))
	{
		r = 0; g = 0; b = 255;
	}
	if ((currentLine->isLineOfType(Line::LineType::INFILL)))
	{
		r = 255;
		g = 255;
		b = 0.25 * 255;
	}
	if (currentLine->isLineOfType(Line::LineType::SURFACE))
	{
		r = 0;
		g = 255;
		b = 0;
	}
	if (currentLine->isLineOfType(Line::LineType::BOTTOM_LAYER | Line::LineType::TOP_SOLID))
	{
		r = 0;
		g = 0;
		b = 255;
	}
	if (currentLine->isLineOfType(Line::LineType::TO_INSET))
	{
		r = 255;
		g = 255;
		b = 0;
		//return;

	}
	if (currentLine->isLineOfType(Line::LineType::UNSUPPORTED | Line::LineType::HOLE_PERIM))
	{
		r = 255;
		g = 0;
		b = 0;
	}
	if (currentLine->isLineOfType(Line::LineType::SUPPORT_INTERFACE))
	{
		r = 220;
		g = 230;
		b = 110;
	}
	if (currentLine->isLineOfType(Line::LineType::SUPPORT_BASE))
	{
		r = 120; g = 147; b = 240;
	}
}

void GCodeVisualizer::createVBOs()
{
#ifndef CONSOLE
	lineCount = 0;
	if (!m_ptrSlicable->isSliced())
	{
		readyToDraw = false;
		return;
	}
	LayerStartLayerEnd.clear();
	//Perimeters
	layerCount = m_ptrSlicable->getLayerCount();
	for (unsigned int i = 0; i < blocksRequired; i++)
	{
		if (m_pColours)
			delete[] m_pColours[i];
		if (m_pNormals)
			delete[] m_pNormals[i];
		if (m_pVerts)
			delete[] m_pVerts[i];
	}
	if (m_pColours)
		delete[] m_pColours;
	if (m_pNormals)
		delete[] m_pNormals;
	if (m_pVerts)
		delete[] m_pVerts;

	unsigned int *VertsPerLayer = new unsigned int[layerCount];
	float **total_points = new float*[layerCount];
	float **total_colours = new float*[layerCount];
	float **total_normals = new float*[layerCount];
	//m_pLayers = new gCodeLayer[layerCount];
	startVertNumber = 0;

	uint vertexCountTotal = 0;

	for (unsigned int i = 0; i < layerCount; i++)
	{
		assert(m_ptrSlicable->getLayer(i));

		unsigned int vertexCount = 0;
		std::vector<float> points;
		std::vector<GLubyte> colours;
		std::vector<float> normals;
#if USE_INT_COORDS
		micrometers z = m_ptrSlicable->getLayer(i)->minZ;
		z += to_micrometers(Settings::getSingleton().getDoubleValue("layerThickness"));
#else
		double z = m_ptrSlicable->getLayer(i)->getLayerZ();// +m_ptrSlicable->getLayer(i)->getThickness();
		double thickness = m_ptrSlicable->getLayer(i)->getThickness();
		/*z += (Settings::getSingleton().getDoubleValue("layerThickness"));*/
#endif
		//Priming Towers
		if (Settings::GetSingleton().getBoolValue("generatePrimingTowers"))
		{
			for (auto it = PrimeTowerHandler::GetSingleton().m_Towers.begin(); it != PrimeTowerHandler::GetSingleton().m_Towers.end(); ++it)
			{
				if (i < it->m_LinesPerLayer.size())
				{
					for (std::size_t j = 0; j < it->m_LinesPerLayer[i].size(); j++)
					{
						const Line* currentLine = &(it->m_LinesPerLayer[i][j]);
						lineToGLPoints(z, thickness,GET_DOUBLE("extrusionWidth") ,points, normals, colours, currentLine);
					}
				}
			}
		}
		//Each layer can have multiple perimeters


		for (auto ExrsIT = m_ptrSlicable->getLayer(i)->m_layerPolys.begin(); ExrsIT != m_ptrSlicable->getLayer(i)->m_layerPolys.end(); ++ExrsIT)
		{
			//Draw perimeter
			const InnerPerims* perimTree = (*ExrsIT)->getPerimTree();
			if(perimTree)
				drawPerimTree(z, thickness,(*ExrsIT)->getExtWidth(),(*ExrsIT)->infillExtWidth, points, normals, colours, perimTree);
			//Draw Basic line path
			if (((*ExrsIT)->basicLinePaths.empty() == false))
			{
				for (auto linePathIT : (*ExrsIT)->basicLinePaths)
				{
					for (uint j = 0; j < linePathIT.size(); j++)
					{
						const Line* currentLine = &linePathIT[j];
						lineToGLPoints(z, thickness, (*ExrsIT)->getExtWidth(), points, normals, colours, currentLine);// , currentLine->pt1.r, currentLine->pt1.g, currentLine->pt1.b);
					}
				}
			}
		}
		for (uint j = 0; j < m_ptrSlicable->getLayer(i)->m_Surfaces.size(); j++)
		{
			//Surface Contour
			for (uint k = 0; k < m_ptrSlicable->getLayer(i)->m_Surfaces[j]->m_ptrSurfacePoly->m_contour->m_lines.size(); k++)
			{
				const Line* currentLine = &m_ptrSlicable->getLayer(i)->m_Surfaces[j]->m_ptrSurfacePoly->m_contour->m_lines[k];
				lineToGLPoints(z, thickness, GET_DOUBLE("extrusionWidth"), points, normals, colours, currentLine);
			}
		}
		
		//Draw skirt
		if (i == 0)
		{
			for (auto skirtIT : m_ptrSlicable->getSkirts())
			{
				drawPerimTree(z, thickness, GET_DOUBLE("extrusionWidth"), GET_DOUBLE("extrusionWidth"), points, normals, colours, skirtIT->getPerimTree());
			}
		}

		//Draw support
		/*
		if (m_ptrSlicable->hasSupport())
		{
			
			auto supportLines = m_ptrSlicable->getLayer(i)->getSupportLines();
			for (auto lineSetIT : supportLines)
			{
				for (auto lineIT : lineSetIT)
				{
					//Get colour for this line...
					//lineIT.setColour(Colour((uint8_t)120, (uint8_t)100, (uint8_t)60));
					lineIT.addParameter(Line::LineType::SUPPORT_BASE);
					const Line* currentLine = &lineIT;
					lineToGLPoints(z, thickness, GET_DOUBLE("extrusionWidth"), points, normals, colours, currentLine);
				}
			}

			auto supportInterfaceLines = m_ptrSlicable->getLayer(i)->getSupportInterfaceLines();
			for (auto lineSetIT : supportInterfaceLines)
			{
				for (auto lineIT : lineSetIT)
				{
					//lineIT.setColour(Colour((uint8_t)120, (uint8_t)200, (uint8_t)60));
					lineIT.addParameter(Line::LineType::SUPPORT_INTERFACE);
					const Line* currentLine = &lineIT;
					lineToGLPoints(z, thickness, GET_DOUBLE("extrusionWidth"), points, normals, colours, currentLine);
				}
			}

			
			//This is for visualizer only, the perimeter wont actually be printed
			/*
			for (auto it : m_ptrSlicable->getLayer(i)->getSupportPoly())
			{
				auto tempPath = RoVaClipperLib::ClipperPathToBodySplitter(it, Line::LineType::WEAK_SUPPORT);
				for (uint k = 0; k < tempPath.size(); k++)
				{
					tempPath[k].colour = Colour((uint8_t)128, (uint8_t)128, (uint8_t)25);
					const Line* currentLine = &tempPath[k];
					lineToGLPoints(z, thickness, GET_DOUBLE("extrusionWidth"), points, normals, colours, currentLine);
				}
			}

			for (auto it : m_ptrSlicable->getLayer(i)->getSupportInterfacePoly())
			{
				auto tempPath = RoVaClipperLib::ClipperPathToBodySplitter(it, Line::LineType::WEAK_SUPPORT);
				for (uint k = 0; k < tempPath.size(); k++)
				{
					tempPath[k].colour = Colour((uint8_t)128, (uint8_t)128, (uint8_t)75);
					const Line* currentLine = &tempPath[k];
					lineToGLPoints(z, thickness, GET_DOUBLE("extrusionWidth"), points, normals, colours, currentLine);
				}
			}
			
		}
		*/
		
		vertexCount = points.size() / 3;
		LayerStartLayerEnd.push_back(std::pair<uint, uint>(vertexCountTotal, vertexCount + vertexCountTotal));
		vertexCountTotal += vertexCount;

		VertsPerLayer[i] = points.size() / 3;
		try {
			total_points[i] = new float[points.size()];
			total_colours[i] = new float[colours.size()];
			total_normals[i] = new float[normals.size()];
			std::copy(points.begin(), points.end(), total_points[i]);
			std::copy(colours.begin(), colours.end(), total_colours[i]);
			std::copy(normals.begin(), normals.end(), total_normals[i]);
			points.clear();
			colours.clear();
			normals.clear();
		}
		catch (...)
		{
			needsRebuilding = false;
			readyToDraw = false;
			std::string message = "Failed to allocate memory! Preview not available.";
			spdlog::get("DEBUG_FILE")->error(message);
			wxMessageBox(message, "Error");
			for (int j = 0; j < i; j++)
			{
				delete[] total_points[j];
				delete[] total_colours[j];
				delete[] total_normals[j];
			}
			delete[] total_points;
			delete[] total_colours;
			delete[] total_normals;
			return;
		}
	}
	unsigned int vertsUsed = 0;

	try
	{
		unsigned int memorySize = LayerStartLayerEnd.back().second * 3 * sizeof(GLfloat);
		//std::string message = "Memory required: " + std::to_string((memorySize/1024)/1024)+"mb";
	//	
		blocksRequired = ceill(((long double)LayerStartLayerEnd.back().second) / MAX_VERTS_PER_SECTION);
		bufferIDs = new GLuint[blocksRequired * 3];
		glGenBuffers(blocksRequired*3, bufferIDs);
		//message += "\nBlocks Required: " + std::to_string(blocksRequired);
		m_pVerts = new GLfloat*[blocksRequired];
		m_pNormals = new GLshort*[blocksRequired];
		m_pColours = new GLubyte*[blocksRequired];
		if (blocksRequired > 0)
		{
			for (int i = 0; i < blocksRequired - 1; i++)
			{
				m_pVerts[i] = new GLfloat[MAX_VERTS_PER_SECTION * 3];
				m_pNormals[i] = new GLshort[MAX_VERTS_PER_SECTION * 3];
				m_pColours[i] = new GLubyte[MAX_VERTS_PER_SECTION * 3];
			}
			lastBlockSize = LayerStartLayerEnd.back().second - (MAX_VERTS_PER_SECTION*(blocksRequired - 1));
			//std::string message = "Last Block size: " + std::to_string(lastBlockSize);
			//spdlog::get("DEBUG_FILE")->error(message);
			//message += "\nLastBlock size:" + std::to_string(lastBlockSize);
			m_pColours[blocksRequired - 1] = new GLubyte[lastBlockSize * 3];
			m_pNormals[blocksRequired - 1] = new GLshort[lastBlockSize * 3];
			m_pVerts[blocksRequired - 1] = new GLfloat[lastBlockSize * 3];
		}
		//wxMessageBox(message, "Info");
	//	wxMessageBox("Memory allocated", "Info");
	}
	catch (...)
	{
	  needsRebuilding = false;
	  readyToDraw = false;
		unsigned int memorySize = LayerStartLayerEnd.back().second * 3 * (sizeof(GLfloat)*2+sizeof(GLubyte));
		std::string message = "Failed to allocate memory! Preview not available. \nMemory required: " + std::to_string((memorySize / 1024) / 1024) + "mb";
		spdlog::get("DEBUG_FILE")->error(message);
		wxMessageBox(message, "Error");
		endVertNumber = 0;
		for (unsigned int j = 0; j < layerCount; j++)
		{
			delete[] total_colours[j];
			delete[] total_normals[j];
			delete[] total_points[j];
		}
		delete[] total_colours;
		delete[] total_normals;
		delete[] total_points;
		throw;
	}
	unsigned int i = 0;
	for (int k = 0; k < layerCount; k++)
	{
		for (int j = 0; j < VertsPerLayer[k]; j++)
		{
				unsigned int block = floorl(i / MAX_VERTS_PER_SECTION);
				unsigned int count = fmodl(i, MAX_VERTS_PER_SECTION);
				for (int m = 0; m < 3; m++)
				{
					m_pColours[block][count * 3 + m] = total_colours[k][j*3 + m];
					m_pNormals[block][count * 3 + m] = total_normals[k][j * 3 + m]* 32767;
					m_pVerts[block][count * 3 + m] = total_points[k][j * 3 + m];
				}
				i++;
		}
	}
	auto glErr = glGetError();
	endVertNumber = LayerStartLayerEnd.back().second;
	if (blocksRequired > 0)
	{
		for (int i = 0; i < blocksRequired - 1; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*MAX_VERTS_PER_SECTION * 3, m_pVerts[i], GL_STATIC_DRAW_ARB);

			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3 + 1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLshort)*MAX_VERTS_PER_SECTION * 3, m_pNormals[i], GL_STATIC_DRAW_ARB);

			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3 + 2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)*MAX_VERTS_PER_SECTION * 3, m_pColours[i], GL_STATIC_DRAW_ARB);
		}

		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksRequired - 1) * 3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*(lastBlockSize * 3), m_pVerts[blocksRequired - 1], GL_STATIC_DRAW_ARB);

			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksRequired - 1) * 3 + 1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLshort)*(lastBlockSize * 3), m_pNormals[blocksRequired - 1], GL_STATIC_DRAW_ARB);

			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksRequired - 1) * 3 + 2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte)*(lastBlockSize * 3), m_pColours[blocksRequired - 1], GL_STATIC_DRAW_ARB);
		}
	}
	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		spdlog::get("DEBUG_FILE")->error("GL ERROR: "+std::to_string((int)glErr)+(char*)glewGetErrorString(glErr));
		glErr = glGetError();
	}
	for (unsigned int j = 0; j < layerCount; j++)
	{
		delete[] total_colours[j];
		delete[] total_normals[j];
		delete[] total_points[j];
	}
	delete[] total_colours;
	delete[] total_normals;
	delete[] total_points;
	delete[] VertsPerLayer;
	//for (unsigned int i = 0; i < LayerStartLayerEnd.back().second; i++)
	//{
	//	unsigned int block = floorl(i / MAX_VERTS_PER_SECTION);
	//	unsigned int count = fmodl(i, MAX_VERTS_PER_SECTION);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		m_pColours[block][count*3+j] = 
	//	}
	//}
	
	//wxMessageBox("VBOs created", "Info");
	readyToDraw = true;
	needsRebuilding = false;
#endif
}

void GCodeVisualizer::drawPerimTree(double z,double thickness, double extWidth, double fullInfillExtWidth, std::vector<float> &pts, 
	std::vector<float> &norms, std::vector<GLubyte> &colours, const InnerPerims *set)
{
	Vertex pt1 = Vertex(set->getContour()->front().x, set->getContour()->front().y);
	Line lastLine;
	for (int i = 1; i < set->getContour()->size(); ++i)
	{
		Vertex pt2 = Vertex(set->getContour()->at(i).x, set->getContour()->at(i).y);
		Line newLine(pt1, pt2,0);
		newLine.setColour(set->colour);
		newLine.addParameter(Line::LineType::PERIM);
		if(i == 1)
			lineToGLPoints(z, thickness, extWidth, pts, norms, colours, &newLine);
		else
			lineToGLPoints(z, thickness, extWidth, pts, norms, colours, &newLine,-1,-1,-1,&lastLine);
		lastLine = newLine;
		pt1 = pt2;
	}
	if (set->hasHole())
	{
		auto holePaths = set->getHoles();
		for (auto &holeSet : *holePaths)
		{
			Vertex pt1 = Vertex(holeSet.front().x, holeSet.front().y);
			for (int i = 1; i < holeSet.size(); i++)
			{
				Vertex pt2(holeSet[i].x, holeSet[i].y);
				Line newLine(pt1, pt2, 0);
				newLine.colour = set->colour;
				newLine.addParameter(Line::LineType::HOLE_PERIM);
				lineToGLPoints(z, thickness, extWidth, pts, norms, colours, &newLine);
				pt1 = pt2;
			}
		}
	}
	//Partial infill
	for (auto lineSetInfillIT : set->getInfillLines())
	{
		pt1 = (Vertex(lineSetInfillIT.front().x, lineSetInfillIT.front().y));
		for (int i = 1; i < lineSetInfillIT.size(); ++i)
		{
			Vertex pt2 = Vertex(lineSetInfillIT[i].x, lineSetInfillIT[i].y);
			Line newLine(pt1, pt2, 0);
			newLine.colour = set->colour;
			newLine.addParameter(Line::LineType::INFILL);
			lineToGLPoints(z, thickness, extWidth, pts, norms, colours, &newLine);
			pt1 = pt2;
		}
	}
	//Full infill
	for (auto lineSetInfillIT : set->getFullInfillLines())
	{
		pt1 = (Vertex(lineSetInfillIT.front().x, lineSetInfillIT.front().y));
		for (int i = 1; i < lineSetInfillIT.size(); ++i)
		{
			Vertex pt2 = Vertex(lineSetInfillIT[i].x, lineSetInfillIT[i].y);
			Line newLine(pt1, pt2, 0);
			newLine.colour = set->colour;
			newLine.addParameter(Line::LineType::INFILL | Line::LineType::TOP_SOLID);
			lineToGLPoints(z, thickness, fullInfillExtWidth, pts, norms, colours, &newLine);
			pt1 = pt2;
		}
	}
	for (int i = 0; i < set->getChildren().size(); i++)
	{
		drawPerimTree(z, thickness, extWidth, fullInfillExtWidth, pts, norms, colours, set->getChildren()[i]);
	}
}
void GCodeVisualizer::lineToGLPoints(double z, double thickness, double extWidth, std::vector<float> &points, std::vector<float> &normals, std::vector<GLubyte> &colours, const Line *currentLine, double r, double g, double b, const Line* lastLine)
{
	lineCount++;
	double extrusionWidth = extWidth*currentLine->extrusionThickness;
	aiVector3D pt1((currentLine->pt1.x),
		(currentLine->pt1.y),
		(currentLine->pt1.z));
	aiVector3D pt2((currentLine->pt2.x),
		(currentLine->pt2.y),
		(currentLine->pt2.z));

	//Ignore end triangles for now
	//Top points, rest are based on them
	if (r < 0)
	{
		if (currentLine->getColour().isSet() == false)
		{
			r = 255;
			g = 0;
			b = 255;
			//a = 0.5;
		}
	}
	if (Settings::getSingleton().getBoolValue("colourPreviewByType"))
	{
		r = 255; g = 255; b = 255;
		setColourByType(currentLine, r, g, b);
	}
	aiVector3D vpoints[4];
	aiVector3D lineA(pt2.x - pt1.x, pt2.y - pt1.y, 0);
	aiVector3D normalLine = lineA^aiVector3D(0, 0, 1);
	normalLine.Normalize();
	normalLine *= extrusionWidth / 2.0;
	double zHeight = thickness;

	aiVector3D faceEdge1, faceEdge2;
	aiVector3D faceNormal;
	
	std::vector<uint8_t> colourToInsert;
	if (r < 0)
	{
		if (Settings::getSingleton().getStringValue("headChoice") == "Diamond Hotend - CMY")
		{
			auto cmy = currentLine->colour.getCMY();
			// normalize
			double length = sqrt(cmy.c*cmy.c + cmy.m*cmy.m + cmy.y*cmy.y);
			colourToInsert.push_back((1 - cmy.c / length) * 255.0);
			colourToInsert.push_back((1 - cmy.m / length) * 255.0);
			colourToInsert.push_back((1 - cmy.y / length) * 255.0);
		}
		else
		{
			colourToInsert.push_back(currentLine->colour.getR<uint8_t>());
			colourToInsert.push_back(currentLine->colour.getG<uint8_t>());
			colourToInsert.push_back(currentLine->colour.getB<uint8_t>());
		}
	}
	else
	{
		colourToInsert.push_back(r); colourToInsert.push_back(g); colourToInsert.push_back(b);
	}
	
	double xoff = 0;// m_ptrSlicable->getMinX() + (m_ptrSlicable->getMaxX() - m_ptrSlicable->getMinX()) / 2.0;// m_ptrSlicable->getOffsetX();// Settings::GetSingleton().getDoubleValue("printXOffset");
	double yoff = 0;// m_ptrSlicable->getMinY() + (m_ptrSlicable->getMaxY() - m_ptrSlicable->getMinY()) / 2.0;// m_ptrSlicable->getOffsetY();// Settings::GetSingleton().getDoubleValue("printYOffset");
	//Top Left - Works
	{

		vpoints[0] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight);	//Top line
		vpoints[1] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight);
		vpoints[2] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight / 2.0) - normalLine;	//bottom Line
		vpoints[3] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight / 2.0) - normalLine;
		//0

		faceEdge1 = vpoints[0] - vpoints[1]; faceEdge2 = vpoints[0] - vpoints[2];
		faceNormal = aiVector3D(0, 0, 1);//faceEdge1^faceEdge2;
		faceNormal.Normalize();
		points.push_back(vpoints[0].x); points.push_back(vpoints[0].y); points.push_back(vpoints[0].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//1
		points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//2
		points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
		normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());


		//Triangle 2
		points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
		normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//1
		points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//2
		points.push_back(vpoints[3].x); points.push_back(vpoints[3].y); points.push_back(vpoints[3].z);
		normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
	}
	//Top right - Works
	{


		vpoints[0] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight);	//Top line
		vpoints[1] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight);
		vpoints[2] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight / 2.0) + normalLine;	//bottom Line
		vpoints[3] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight / 2.0) + normalLine;
		//0

		faceEdge1 = vpoints[0] - vpoints[1]; faceEdge2 = vpoints[0] - vpoints[2];
		faceNormal = aiVector3D(0, 0, 1);// faceEdge1^faceEdge2;
		faceNormal.Normalize();
		points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//1
		points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//2
		points.push_back(vpoints[0].x); points.push_back(vpoints[0].y); points.push_back(vpoints[0].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());


		//Triangle 2
		points.push_back(vpoints[3].x); points.push_back(vpoints[3].y); points.push_back(vpoints[3].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//1
		points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		//2
		points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
		normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
		colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
	}

	if (!Settings::GetSingleton().getBoolValue("simpleGCodePreview"))
	{
		//Bottom Right
		{


			vpoints[0] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight / 2.0) + normalLine;	//Top line
			vpoints[1] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight / 2.0) + normalLine;
			vpoints[2] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z);	//bottom Line
			vpoints[3] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z);
			//0

			faceEdge1 = vpoints[0] - vpoints[1]; faceEdge2 = vpoints[0] - vpoints[2];
			faceNormal = aiVector3D(0, 0, -1);// faceEdge1^faceEdge2;
			faceNormal.Normalize();
			points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//1
			points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//2
			points.push_back(vpoints[0].x); points.push_back(vpoints[0].y); points.push_back(vpoints[0].z);
			normals.push_back(normalLine.x); normals.push_back(normalLine.y); normals.push_back(normalLine.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());


			//Triangle 2
			points.push_back(vpoints[3].x); points.push_back(vpoints[3].y); points.push_back(vpoints[3].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//1
			points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
			normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//2
			points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		}

		{
			//Bottom left
			vpoints[0] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight / 2.0) - normalLine;	//Top line
			vpoints[1] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z + zHeight / 2.0) - normalLine;
			vpoints[2] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z);	//bottom Line
			vpoints[3] = aiVector3D(pt2.x + xoff, pt2.y + yoff, z);
			//0

			faceEdge1 = vpoints[0] - vpoints[1]; faceEdge2 = vpoints[0] - vpoints[2];
			faceNormal = aiVector3D(0, 0, -1);//faceEdge1^faceEdge2;
			faceNormal.Normalize();
			points.push_back(vpoints[0].x); points.push_back(vpoints[0].y); points.push_back(vpoints[0].z);
			normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//1
			points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
			normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//2
			points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());


			//Triangle 2
			points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//1
			points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
			normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
			//2
			points.push_back(vpoints[3].x); points.push_back(vpoints[3].y); points.push_back(vpoints[3].z);
			normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
			colours.insert(colours.end(),colourToInsert.begin(),colourToInsert.end());
		}

		// Create the connector triangle
		if (lastLine)
		{
			if (Line::getAngleBetween(*lastLine, *currentLine) < 180)
			{
				
				// right turn
				vpoints[0] = aiVector3D(lastLine->pt2.x + xoff, lastLine->pt2.y + yoff, z + zHeight);	//Top line
				vpoints[1] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight);
				vpoints[2] = aiVector3D(lastLine->pt2.x + xoff, lastLine->pt2.y + yoff, z + zHeight / 2.0);	//bottom Line
				vpoints[3] = aiVector3D(pt1.x + xoff, pt1.y + yoff, z + zHeight / 2.0);
				//0

				faceEdge1 = vpoints[0] - vpoints[1]; faceEdge2 = vpoints[0] - vpoints[2];
				faceNormal = faceEdge1^faceEdge2;
				faceNormal.Normalize();
				points.push_back(vpoints[0].x); points.push_back(vpoints[0].y); points.push_back(vpoints[0].z);
				normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
				colours.insert(colours.end(), colourToInsert.begin(), colourToInsert.end());
				//1
				points.push_back(vpoints[1].x); points.push_back(vpoints[1].y); points.push_back(vpoints[1].z);
				normals.push_back(faceNormal.x); normals.push_back(faceNormal.y); normals.push_back(faceNormal.z);
				colours.insert(colours.end(), colourToInsert.begin(), colourToInsert.end());
				//2
				points.push_back(vpoints[2].x); points.push_back(vpoints[2].y); points.push_back(vpoints[2].z);
				normals.push_back(-normalLine.x); normals.push_back(-normalLine.y); normals.push_back(-normalLine.z);
				colours.insert(colours.end(), colourToInsert.begin(), colourToInsert.end());
			}
			else
			{

			}
		}
	}
}

void GCodeVisualizer::draw() noexcept
{
#ifndef CONSOLE
	if (needsRebuilding)
		createVBOs();
	if (readyToDraw == false)	//dont draw if nothing to draw...
		return;
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR);
	//glShadeModel(GL_FLAT);
// 	glMateriali(GL_FRONT_AND_BACK, GL_SPECULAR, 50);
// 	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 60);
	glFlush();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	if (m_ptrSlicable)
		glTranslated(m_ptrSlicable->getOffsetX(),
			m_ptrSlicable->getOffsetY(),
			0);
	if (endVertNumber > 2)
	{
		int blocksNeed = floorl((long double)endVertNumber / ((long double)MAX_VERTS_PER_SECTION));
		for (int i = 0; i < blocksNeed; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3]);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3 + 1]);
			glNormalPointer(GL_SHORT, 0,0);
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[i * 3 + 2]);
			glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, MAX_VERTS_PER_SECTION);
		}
		long int remainderVerts;
		remainderVerts = endVertNumber - blocksNeed*MAX_VERTS_PER_SECTION;
		{
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksNeed) * 3]);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksNeed) * 3 + 1]);
			glNormalPointer(GL_SHORT, 0,0);
			glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[(blocksNeed) * 3 + 2]);
			glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, remainderVerts);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (m_ptrSlicable)
		glTranslated(-m_ptrSlicable->getOffsetX(),
			-m_ptrSlicable->getOffsetY(),
			0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

GCodeVisualizer::GCodeVisualizer(iSlicable *parent) : m_ptrSlicable(parent)
{

}

void GCodeVisualizer::setEndLayer(unsigned int i)
{
	if (i >= LayerStartLayerEnd.size())
	{
		if (LayerStartLayerEnd.empty())
			endVertNumber = 0;
		else
			endVertNumber = LayerStartLayerEnd.back().second;
	}
	else
		endVertNumber = LayerStartLayerEnd[i].second;
}

void GCodeVisualizer::setStartLayer(unsigned int i)
{
	if (i >= LayerStartLayerEnd.size())
		startVertNumber = LayerStartLayerEnd.back().first;
	else
		startVertNumber = LayerStartLayerEnd[i].first;
}

void GCodeVisualizer::setEndLine(unsigned int line)
{
	endVertNumber = line * 6*4;
}


GCodeVisualizer::~GCodeVisualizer()
{
	for (unsigned int i = 0; i < blocksRequired; i++)
	{
		if (m_pColours)
			delete[] m_pColours[i];
		if (m_pNormals)
			delete[] m_pNormals[i];
		if (m_pVerts)
			delete[] m_pVerts[i];
	}
#ifndef CONSOLE
	glDeleteBuffers(blocksRequired * 3, bufferIDs);
#endif
	if(bufferIDs)
		delete[] bufferIDs;
	if (m_pColours)
		delete[] m_pColours;
	if (m_pNormals)
		delete[] m_pNormals;
	if (m_pVerts)
		delete[] m_pVerts;


}

std::size_t GCodeVisualizer::getLineCount() const
{
	return lineCount;
}