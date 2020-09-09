#include <spdlog/spdlog.h>

#include "SupportGCodeVisualizer.h"
#include "PrintBed.h"
#include "SupportManager.h"
#include "Settings.h"

SupportGCodeVisualizer::SupportGCodeVisualizer() : GCodeVisualizer(nullptr)
{
	needsRebuilding = true;
}


SupportGCodeVisualizer::~SupportGCodeVisualizer()
{
}

void SupportGCodeVisualizer::createVBOs()
{
	lineCount = 0;
	LayerStartLayerEnd.clear();
	//Perimeters
	unsigned int layerCount = PrintBed::getSingleton().getSupportManager()->getLayerCount();
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

		unsigned int vertexCount = 0;
		std::vector<float> points;
		std::vector<GLubyte> colours;
		std::vector<float> normals;

		double z = i*Settings::getSingleton().getDoubleValue("layerThickness");
		double thickness = Settings::getSingleton().getDoubleValue("layerThickness");
		//Draw support
		if (i < PrintBed::GetSingleton().getSupportManager()->getNonInterfaceLayerCount())
		{
			auto supportLines = PrintBed::GetSingleton().getSupportManager()->getSortedLinesPerLayer(i);
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
		}
		if (i < PrintBed::GetSingleton().getSupportManager()->getInterfaceLayerCount())
		{
			auto supporInterfacetLines = PrintBed::GetSingleton().getSupportManager()->getSortedInterfaceLinesPerLayer(i);
			for (auto lineSetIT : supporInterfacetLines)
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
		}
		/*
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
		*/

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
#ifndef CONSOLE
			wxMessageBox(message, "Error");
#endif
			for (unsigned int j = 0; j < i; j++)
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
#ifndef CONSOLE
	try
	{
		unsigned int memorySize = LayerStartLayerEnd.back().second * 3 * sizeof(GLfloat);
		blocksRequired = ceill(((long double)LayerStartLayerEnd.back().second) / MAX_VERTS_PER_SECTION);
		bufferIDs = new GLuint[blocksRequired * 3];
		glGenBuffers(blocksRequired * 3, bufferIDs);
		//message += "\nBlocks Required: " + std::to_string(blocksRequired);
		m_pVerts = new GLfloat*[blocksRequired];
		m_pNormals = new GLshort*[blocksRequired];
		m_pColours = new GLubyte*[blocksRequired];
		if (blocksRequired) for (unsigned int i = 0; i < blocksRequired - 1; i++)
		{
			m_pVerts[i] = new GLfloat[MAX_VERTS_PER_SECTION * 3];
			m_pNormals[i] = new GLshort[MAX_VERTS_PER_SECTION * 3];
			m_pColours[i] = new GLubyte[MAX_VERTS_PER_SECTION * 3];
		}
		lastBlockSize = LayerStartLayerEnd.back().second - (MAX_VERTS_PER_SECTION*(blocksRequired - 1));
		std::string message = "Last Block size: " + std::to_string(lastBlockSize);
		spdlog::get("DEBUG_FILE")->error(message);
		//message += "\nLastBlock size:" + std::to_string(lastBlockSize);
		m_pColours[blocksRequired - 1] = new GLubyte[lastBlockSize * 3];
		m_pNormals[blocksRequired - 1] = new GLshort[lastBlockSize * 3];
		m_pVerts[blocksRequired - 1] = new GLfloat[lastBlockSize * 3];
		//wxMessageBox(message, "Info");
		//	wxMessageBox("Memory allocated", "Info");
	}
	catch (...)
	{
		needsRebuilding = false;
		readyToDraw = false;
		unsigned int memorySize = LayerStartLayerEnd.back().second * 3 * (sizeof(GLfloat) * 2 + sizeof(GLubyte));
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
	for (unsigned int k = 0; k < layerCount; k++)
	{
		for (unsigned int j = 0; j < VertsPerLayer[k]; j++)
		{
			unsigned int block = floorl(i / MAX_VERTS_PER_SECTION);
			unsigned int count = fmodl(i, MAX_VERTS_PER_SECTION);
			for (int m = 0; m < 3; m++)
			{
				m_pColours[block][count * 3 + m] = total_colours[k][j * 3 + m];
				m_pNormals[block][count * 3 + m] = total_normals[k][j * 3 + m] * 32767;
				m_pVerts[block][count * 3 + m] = total_points[k][j * 3 + m];
			}
			i++;
		}
	}
	auto glErr = glGetError();
	endVertNumber = LayerStartLayerEnd.back().second;
	if (blocksRequired) for (unsigned int i = 0; i < blocksRequired - 1; i++)
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
	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		spdlog::get("DEBUG_FILE")->error("GL ERROR: " + std::to_string((int)glErr) + (char*)glewGetErrorString(glErr));
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
#endif
	//wxMessageBox("VBOs created", "Info");
	readyToDraw = true;
	needsRebuilding = false;
}