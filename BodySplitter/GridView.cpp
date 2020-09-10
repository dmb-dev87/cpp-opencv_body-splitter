#include "GridView.h"
#include <vector>
#include "Settings.h"

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __WXMAC__
#include <OpenGL/glew.h>
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

const double GRID_AXIS_SIZE = 40;
const double GRID_Z = -0.02;

GridView::GridView()
{
	sizeX = Settings::GetSingleton().getDoubleValue("maxX");
	sizeY = Settings::GetSingleton().getDoubleValue("maxY");
	sizeZ = Settings::getSingleton().getDoubleValue("maxZ");

	float gridSize = Settings::GetSingleton().getDoubleValue("gridSize");

	std::vector<float> points;
	for (int x = 0; x < sizeX / gridSize; x++)
	{
		points.push_back(x*gridSize);
		points.push_back(0); points.push_back(GRID_Z);

		points.push_back(x*gridSize);
		points.push_back(sizeY); points.push_back(GRID_Z);
	}
	for (int y = 0; y < sizeY / gridSize; y++)
	{
		points.push_back(0);
		points.push_back(y*gridSize);
		points.push_back(GRID_Z);

		points.push_back(sizeX);
		points.push_back(y*gridSize);
		points.push_back(GRID_Z);
	}
	//Draw outside edges
	points.push_back(0); points.push_back(0); points.push_back(GRID_Z);
	points.push_back(sizeX); points.push_back(0); points.push_back(GRID_Z);

	points.push_back(sizeX); points.push_back(0); points.push_back(GRID_Z);
	points.push_back(sizeX); points.push_back(sizeY); points.push_back(GRID_Z);

	points.push_back(0); points.push_back(sizeY); points.push_back(GRID_Z);
	points.push_back(sizeX); points.push_back(sizeY); points.push_back(GRID_Z);

	points.push_back(0); points.push_back(0); points.push_back(GRID_Z);
	points.push_back(0); points.push_back(sizeY); points.push_back(GRID_Z);

	//Draw the enclosing volume
		// Risers
	points.push_back(0); points.push_back(0); points.push_back(GRID_Z);
	points.push_back(0); points.push_back(0); points.push_back(sizeZ+ GRID_Z);

	points.push_back(sizeX); points.push_back(0); points.push_back(GRID_Z);
	points.push_back(sizeX); points.push_back(0); points.push_back(sizeZ+ GRID_Z);

	points.push_back(0); points.push_back(sizeY); points.push_back(GRID_Z);
	points.push_back(0); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);

	points.push_back(sizeX); points.push_back(sizeY); points.push_back(GRID_Z);
	points.push_back(sizeX); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);
		//Upper bound
	points.push_back(0); points.push_back(0); points.push_back(sizeZ+ GRID_Z);
	points.push_back(sizeX); points.push_back(0); points.push_back(sizeZ+ GRID_Z);

	points.push_back(sizeX); points.push_back(0); points.push_back(sizeZ+ GRID_Z);
	points.push_back(sizeX); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);

	points.push_back(sizeX); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);
	points.push_back(0); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);

	points.push_back(0); points.push_back(sizeY); points.push_back(sizeZ+ GRID_Z);
	points.push_back(0); points.push_back(0); points.push_back(sizeZ+ GRID_Z);


	gridVertCount = points.size() / 3;
	m_gridCoords = new float[points.size()];
	std::copy(points.begin(), points.end(), m_gridCoords);

	// Grid axis
	std::vector<float> gridAxis;
	std::vector<float> gridAxisColours; // r-g-b
	// X-Axis
	gridAxisColours.push_back(1.0); gridAxisColours.push_back(0.0); gridAxisColours.push_back(0.0);
	gridAxis.push_back(0); gridAxis.push_back(0); gridAxis.push_back(GRID_Z);
	gridAxisColours.push_back(1.0); gridAxisColours.push_back(0.0); gridAxisColours.push_back(0.0);
	gridAxis.push_back(GRID_AXIS_SIZE); gridAxis.push_back(0); gridAxis.push_back(GRID_Z);
	//Y-Axis
	gridAxisColours.push_back(0.0); gridAxisColours.push_back(1.0); gridAxisColours.push_back(0.0);
	gridAxis.push_back(0); gridAxis.push_back(0); gridAxis.push_back(GRID_Z);
	gridAxisColours.push_back(0.0); gridAxisColours.push_back(1.0); gridAxisColours.push_back(0.0);
	gridAxis.push_back(0); gridAxis.push_back(GRID_AXIS_SIZE); gridAxis.push_back(GRID_Z);
	//Z-Axis
	gridAxisColours.push_back(0.0); gridAxisColours.push_back(0.0); gridAxisColours.push_back(1.0);
	gridAxis.push_back(0); gridAxis.push_back(0); gridAxis.push_back(GRID_Z);
	gridAxisColours.push_back(0.0); gridAxisColours.push_back(0.0); gridAxisColours.push_back(1.0);
	gridAxis.push_back(00); gridAxis.push_back(0); gridAxis.push_back(GRID_AXIS_SIZE+ GRID_Z);
	m_gridAxisCoords = new float[gridAxis.size()];
	m_gridAxisColours = new float[gridAxisColours.size()];
	std::copy(gridAxisColours.begin(), gridAxisColours.end(), m_gridAxisColours);
	std::copy(gridAxis.begin(), gridAxis.end(), m_gridAxisCoords);
	gridAxisCount = gridAxis.size()/3;
}


GridView::~GridView()
{
	delete[] m_gridCoords;
	if (m_gridAxisCoords)
		delete[] m_gridAxisCoords;
	if (m_gridAxisColours)
		delete[] m_gridAxisColours;
}


void GridView::draw()
{
#ifndef CONSOLE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
		glColor4f(0.3, 0.3f, 0.3f,0.75f);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0, 0, GRID_Z);  // Bottom Left Of The Texture and Quad
		glVertex3f(sizeX, 0, GRID_Z);  // Bottom Right Of The Texture and Quad
		glVertex3f(sizeX, sizeY, GRID_Z);  // Top Right Of The Texture and Quad
		glVertex3f(0, sizeY, GRID_Z);  // Top Left Of The Texture and Quad
	glEnd();

	glEnable(GL_BLEND);
	glColor3f(0.1f, 0.1f, 0.1f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH);
	glVertexPointer(3, GL_FLOAT, 0, m_gridCoords);
	glDrawArrays(GL_LINES, 0, gridVertCount);

	if (GET_BOOL("showAxis"))
	{
		glLineWidth(2);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, m_gridAxisColours);
		glVertexPointer(3, GL_FLOAT, 0, m_gridAxisCoords);
		glDrawArrays(GL_LINES, 0, gridAxisCount);

		glDisableClientState(GL_COLOR_ARRAY);
		glLineWidth(1);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
#endif
}