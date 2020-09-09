#include "SupportTower.h"
#include <spdlog/spdlog.h>
#ifdef _WIN32
#include <Windows.h>
#endif
// include OpenGL
#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include "PrintBed.h"
#include "UserMesh.h"
#include "ClipperLib/RoVaclipper.hpp"
#include "Settings.h"

SupportTower::SupportTower(const glm::vec3 &centrePoint, double height, double width) : m_width(width), m_height(height)
{
	x = centrePoint.x; y = centrePoint.y;
	m_centrePt = Vertex(centrePoint);
	m_bottom = centrePoint.z;
}


SupportTower::~SupportTower()
{
}

void SupportTower::draw(unsigned int opts, const Colour *colourToDraw) noexcept {
#ifndef CONSOLE
	auto logger = spdlog::get("DEBUG_FILE");
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 0);
	const double SPACING = 0.4;
	double minX = m_centrePt.x - m_width / 2.0 + SPACING;
	double minY = m_centrePt.y - m_width / 2.0 +SPACING;
	double maxX = minX + m_width - 2* SPACING;
	double maxY = minY + m_width - 2* SPACING;
	glDisable(GL_TEXTURE_2D);
	if(opts != 0)
		glDisable(GL_LIGHTING);
	glBegin(GL_QUADS);
	// Front Face
	if (opts == 0)
	{
		if (isHighlighted)
		{
			glColor3f(1.0f, 1.0f, 0);
		}
		else
		{
			if (state == stateOfSupport::NORMAL)
				glColor3f(0.8f, 0.8f, 1.0f);
			else if (state == stateOfSupport::INVALIDMOVING)
				glColor3f(0.8f, 0.4f, 0.4f);
			else if (state == stateOfSupport::VALIDMOVING)
				glColor3f(0.4f, 0.8f, 0.4f);
		}
	}
	else
	{

		glColor3ub(colourToDraw->getR<uint8_t>(), colourToDraw->getG<uint8_t>(), colourToDraw->getB<uint8_t>());
		
		//logger->info("Support Colour (" + std::to_string((int)colourToDraw->getR<uint8_t>()) + "," +
		//	std::to_string((int)colourToDraw->getG<uint8_t>()) + "," +
		//		std::to_string((int)colourToDraw->getB<uint8_t>()) + ")");
	}
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(minX, minY, m_height);  // Bottom Left Of The Texture and Quad
	glVertex3f(maxX, minY, m_height);  // Bottom Right Of The Texture and Quad
	glVertex3f(maxX, maxY, m_height);  // Top Right Of The Texture and Quad
	glVertex3f(minX, maxY, m_height);  // Top Left Of The Texture and Quad
	
	// Back Face
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(minX, minY, m_bottom);  // Bottom Right Of The Texture and Quad
	glVertex3f(minX, maxY, m_bottom);  // Top Right Of The Texture and Quad
	glVertex3f(maxX, maxY, m_bottom);  // Top Left Of The Texture and Quad
	glVertex3f(maxX, minY, m_bottom);  // Bottom Left Of The Texture and Quad

															 // Top Face
	glNormal3f(0.0f, 1.0f, 0.0);
	glVertex3f(minX, maxY, m_bottom);  // Top Left Of The Texture and Quad
	glVertex3f(minX, maxY, m_height);  // Bottom Left Of The Texture and Quad
	glVertex3f(maxX, maxY, m_height);  // Bottom Right Of The Texture and Quad
	glVertex3f(maxX, maxY, m_bottom);  // Top Right Of The Texture and Quad

															  // Bottom Face
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(minX, minY, m_bottom);  // Top Right Of The Texture and Quad
	glVertex3f(maxX, minY, m_bottom);  // Top Left Of The Texture and Quad
	glVertex3f(maxX, minY, m_height);  // Bottom Left Of The Texture and Quad
	glVertex3f(minX, minY, m_height);  // Bottom Right Of The Texture and Quad

														  // Right face
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(maxX, minY, m_bottom);  // Bottom Right Of The Texture and Quad
	glVertex3f(maxX, maxY, m_bottom);  // Top Right Of The Texture and Quad
	glVertex3f(maxX, maxY, m_height);  // Top Left Of The Texture and Quad
	glVertex3f(maxX, minY, m_height);  // Bottom Left Of The Texture and Quad
														   // Left Face
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(minX, minY, m_bottom);  // Bottom Left Of The Texture and Quad
	glVertex3f(minX, minY, m_height);  // Bottom Right Of The Texture and Quad
	glVertex3f(minX, maxY, m_height);  // Top Right Of The Texture and Quad
	glVertex3f(minX, maxY, m_bottom);  // Top Left Of The Texture and Quad
	glEnd();
	glEnable(GL_TEXTURE_2D);
	if (opts != 0)
		glEnable(GL_LIGHTING);
#endif
}

void SupportTower::highLight(bool state) {
	isHighlighted = state;
}

void SupportTower::updatePosition(const glm::vec3 &bottom)
{
	using MeshIntPair = std::pair < std::shared_ptr<UserMesh>, glm::vec3>;
	//find height..
	m_centrePt = bottom;
	m_bottom = bottom.z;
	std::vector<MeshIntPair> intersectionPoints;
	for (auto &it : PrintBed::GetSingleton().getDrawables())
	{
		auto meshPtr = std::dynamic_pointer_cast<UserMesh>(it);
		if (meshPtr)
		{
			for (auto triangleIT : meshPtr->getTriangles())
			{
				glm::vec3 result;
				for (int i = 0; i < 3; i++)
				{
					triangleIT[i].x += meshPtr->getOffsetX();
					triangleIT[i].y += meshPtr->getOffsetY();
					//triangleIT[i].z += meshPtr->getOffsetZ();
				}
				if (BodySplitter::rayIntersectsTriangle(triangleIT, bottom, glm::vec3(0,0,1), result))
				{
					intersectionPoints.push_back(MeshIntPair(meshPtr, result));
				}
			}
		}
	}
	if (intersectionPoints.empty())
	{
		touchingMeshPtr.reset();
		state = stateOfSupport::INVALIDMOVING;
		m_height = bottom.z + 100;
	}
	else
	{
		//Get lowest height value
		double lowestZ = intersectionPoints.front().second.z;
		touchingMeshPtr = intersectionPoints.front().first;
		
		for (auto it : intersectionPoints)
		{
			if (it.second.z < lowestZ)
			{
				lowestZ = it.second.z;
				touchingMeshPtr = it.first;
			}
		}
	
		m_height = lowestZ;
		state = stateOfSupport::VALIDMOVING;
	}
}

void SupportTower::addToMesh(Mesh* meshPtr)
{
	this->state = stateOfSupport::NORMAL;
	m_centrePt.x -= meshPtr->getOffsetX();
	m_centrePt.y -= meshPtr->getOffsetY();
	m_centrePt.z -= meshPtr->getOffsetZ();
}

RoVaClipperLib::Path SupportTower::getPerimetersAsPath() const
{
	const double PADDING = 0.0;
	double minX = m_centrePt.x - m_width / 2.0- PADDING;
	double minY = m_centrePt.y - m_width / 2.0- PADDING;
	double maxX = minX + m_width+ PADDING;
	double maxY = minY + m_width+ PADDING;

	RoVaClipperLib::Path output;
	output.push_back(RoVaClipperLib::IntPoint(minX*DEDOUBLER, minY*DEDOUBLER));
	output.push_back(RoVaClipperLib::IntPoint(maxX*DEDOUBLER, minY*DEDOUBLER));
	output.push_back(RoVaClipperLib::IntPoint(maxX*DEDOUBLER, maxY*DEDOUBLER));
	output.push_back(RoVaClipperLib::IntPoint(minX*DEDOUBLER, maxY*DEDOUBLER));

	return output;
}