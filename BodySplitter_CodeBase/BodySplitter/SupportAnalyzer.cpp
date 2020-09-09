#include "SupportAnalyzer.h"
#include "Layer.h"
#include "RSPolygon.h"
#include "Settings.h"
#include "SupportTower.h"
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include "UserMesh.h"

#define GRID_SIZE GET_DOUBLE("supportRes")

SupportAnalyzer::SupportAnalyzer(const UserMesh* mesh) : m_ptrMesh(mesh)
{
	findUnsupportTriangles();
}

void SupportAnalyzer::findUnsupportTriangles()
{
	//Consider bottom layer trianges unsupport if >= 1 pt is >= firstLayerHeight
	int unsupported = 0;
	const glm::vec3 bedNormal(0, 0, 1);
	for (auto it : m_ptrMesh->m_triangles)
	{
		bool isBaseLayer = true;
		for (int i = 0; i < 3; i++)
		{
			//Stop early if any point z is not on a the base layser
			if (it.getPt(i).z > Settings::GetSingleton().getDoubleValue("firstLayerThickness"))
			{
				isBaseLayer = false;
				break;
			}
		}
		if (isBaseLayer)
			continue;
		glm::vec3 normal = it.getNormal();
		//Check if the angle is less than some critical angle
		double angle = acos(glm::dot(bedNormal, normal))*360.0/(2*3.141592653);
		if (angle > GET_DOUBLE("maxUnsupportedAngle")+90)
		{
			//Needs support
			unsupportedTriangles.push_back(it);
			unsupported++;
		}
	}
	hasFoundUnsupportedTriangles = true;
	generateSupportTowers(m_ptrMesh->m_triangles);
}

void SupportAnalyzer::generateSupportTowers(const std::deque<Triangle> &triangles)
{
	assert(hasFoundUnsupportedTriangles);
	if (unsupportedTriangles.empty())
		return;

	
	std::vector<std::shared_ptr<SupportTower>> occupiedSpaces;
	double width = m_ptrMesh->getMaxX() - m_ptrMesh->getMinX();
	double height = m_ptrMesh->getMaxY() - m_ptrMesh->getMinY();
	occupiedSpaces.resize(((width/GRID_SIZE+4)*(height/GRID_SIZE+4)),nullptr);

	for (auto it : unsupportedTriangles)
	{
		//Find middle grid points that align with the triangle
		double minX = std::min(it.pt1.x, std::min(it.pt2.x, it.pt3.x));
		double minY = std::min(it.pt1.y, std::min(it.pt2.y, it.pt3.y));

		double maxX = std::max(it.pt1.x, std::max(it.pt2.x, it.pt3.x));
		double maxY = std::max(it.pt1.y, std::max(it.pt2.y, it.pt3.y));
		//Bin it to win it
		minX += -fmod(minX, GRID_SIZE) -GRID_SIZE;// / 2.0;
		minY += -fmod(minY, GRID_SIZE) -GRID_SIZE;// / 2.0;

		maxX += GRID_SIZE -fmod(maxX, GRID_SIZE);// +GRID_SIZE;// / 2.0;
		maxY += GRID_SIZE -fmod(maxY, GRID_SIZE);// +GRID_SIZE;// / 2.0;

		for (double xPos = minX; xPos <= maxX; xPos += GRID_SIZE)
		{
			for (double yPos = minY; yPos <= maxY; yPos += GRID_SIZE)
			{
				//These should all intersect the triangle, but if not skip them
				glm::vec3 intPos;
				glm::vec3 maxPt;
				maxPt.z = -1;
				//Check all 4 points, find highest of intersections, need to then check the down vector at all 4 points...
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos - GRID_SIZE / 2.0, yPos - GRID_SIZE / 2.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos + GRID_SIZE / 2.0, yPos - GRID_SIZE / 2.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos + GRID_SIZE / 2.0, yPos + GRID_SIZE / 2.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos - GRID_SIZE / 2.0, yPos + GRID_SIZE / 2.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}


				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos - GRID_SIZE / 4.0, yPos - GRID_SIZE / 4.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos + GRID_SIZE / 4.0, yPos - GRID_SIZE / 4.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos + GRID_SIZE / 4.0, yPos + GRID_SIZE / 4.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos - GRID_SIZE / 4.0, yPos + GRID_SIZE / 4.0, 0), glm::vec3(0, 0, 1), intPos))
				{
					if (intPos.z > maxPt.z)
						maxPt = intPos;
				}
				//intPos.z = maxIntersection;


				if(maxPt.z>0)
				//if (BodySplitter::rayIntersectsTriangle(it, glm::vec3(xPos, yPos,0), glm::vec3(0, 0, 1), intPos))
				{
					//if (occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE)*(width / GRID_SIZE + 2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE)] && 
					//	maxPt.z < occupiedSpaces[((yPos- m_ptrMesh->getMinY()) / GRID_SIZE)*(width / GRID_SIZE + 2) + ((xPos- m_ptrMesh->getMinX()) / GRID_SIZE)]->getTop())
					//{

					//}
					//else
					{
						//Now check for intersection with any lower triankges
						float bottom = 0;
						intPos.z = maxPt.z;
						float top = intPos.z;
						glm::vec3 bottomInt;
						for (auto AllIT : triangles)
						{
							if (BodySplitter::rayIntersectsTriangle(AllIT, maxPt, glm::vec3(0, 0, -1), bottomInt))
							{
								if (bottomInt.z > bottom)
								{
									bottom = bottomInt.z;
								}
							}
						}
						//intPos.z = bottom;

						if (occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)])
						{
							occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)]->m_bottom =
								std::min((float)occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)]->m_bottom,
									bottomInt.z);
							occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)]->m_height =
								std::max((float)occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)]->m_height,
									intPos.z);
							//supportTowers.erase(std::find(supportTowers.begin(), supportTowers.end(),
								//occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE)*(width / GRID_SIZE + 2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE)]));
						}
						else
						{
							if (top - bottom > Settings::GetSingleton().getDoubleValue("layerThickness"))
							{
								intPos.z = bottomInt.z;
								intPos.x = xPos;
								intPos.y = yPos;
								supportTowers.push_back(std::make_shared<SupportTower>(intPos, top, GRID_SIZE));
								occupiedSpaces[((yPos - m_ptrMesh->getMinY()) / GRID_SIZE+1)*(width / GRID_SIZE+2) + ((xPos - m_ptrMesh->getMinX()) / GRID_SIZE+1)] = supportTowers.back();
							}
						}
					}
				}
			}
		}

	}
}

SupportAnalyzer::~SupportAnalyzer()
{
}

