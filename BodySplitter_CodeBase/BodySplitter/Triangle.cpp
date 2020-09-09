#include "Triangle.h"

#include <glm/glm.hpp>

#include "Line.h"

Triangle::Triangle() : touchingEdges{nullptr,nullptr,nullptr}, parentSurface(nullptr)
{
}


Triangle::~Triangle()
{
}

bool Triangle::isVertical() const
{
	glm::vec3 edge1(pt2.x - pt1.x, pt2.y - pt1.y, pt2.z - pt1.z);
	glm::vec3 edge2(pt3.x - pt1.x, pt3.y - pt1.y, pt3.z - pt1.z);
	return glm::normalize(glm::cross(edge1, edge2)).z > 0.0001;

	//aiVector3D vector1(pt2.x - pt1.x, pt2.y - pt1.y, pt2.z - pt1.z);
	//aiVector3D vector2(pt3.x - pt1.x, pt3.y - pt1.y, pt3.z - pt1.z);
	//aiVector3D normal = vector1^vector2;
	//if (fabs(normal.z) > 0.001)
	//{
	//	return false;
	//}
	//return true;
}

glm::vec3 Triangle::getNormal() const
{
	glm::vec3 edge1(pt2.x - pt1.x, pt2.y - pt1.y, pt2.z - pt1.z);
	glm::vec3 edge2(pt3.x - pt1.x, pt3.y - pt1.y, pt3.z - pt1.z);
	return glm::normalize(glm::cross(edge1, edge2));
}

//bool Triangle::touchesTriangle(const Triangle &inTri)const
//{
//	int pointMatchCount = 0;
//	for (int i = 0; i < 3; i++)
//	{
//		for (int j = 0; j < 3; j++)
//		{
//			if (this->getPt(i).MatchesLocation(inTri[j]))
//				return true;
//		}
//	}
//	return false;
//}

uint Triangle::getLineNumberFromHash(std::size_t hash)
{
	for (uint i = 0; i < 3; i++)
	{
		if (this->getLine(i).hash() == hash)
			return i;
	}
	assert(false);
	return 0; //This will uhh cause a runtime error maybe? wtf
}

Line Triangle::getIntersectionLine(double z) const
{
	Line lines[3] = { Line(pt1,pt2,0),Line(pt2,pt3,0),Line(pt3,pt1,0) };	//Love C++11 initializer lists

	Vertex intersectionPts[2];
	int intersectionCounter = 0;
	for (int j = 0; j < 3; j++)
	{
		if (lines[j].CrossesPlane(z))
		{
			intersectionPts[intersectionCounter] = lines[j].pointAt(z);
			intersectionCounter++;
		}
		if (intersectionCounter == 2)
			break;
	}
	assert(intersectionCounter >= 2);
	return Line(intersectionPts[0], intersectionPts[1],0);
}

void Triangle::cutTriangle(const Triangle &inTri, double minZ, double maxZ, std::vector<Triangle> &output)
{
	//Build new triangles with the top line
	Vertex pointsBelowCut[3];
	int pointsBelowCutCount = 0;
	for (int i = 0; i < 3; i++)
	{
		if (inTri[i].z < maxZ)
		{
			pointsBelowCut[pointsBelowCutCount] = inTri[i];
			pointsBelowCutCount++;
		}
	}
	Triangles afterTopCut;
	if (pointsBelowCutCount < 3 && pointsBelowCutCount > 0)
	{
		Line output = inTri.getIntersectionLine(maxZ);
		//Create 1-2 new triangles
		if (pointsBelowCutCount == 1)
		{
			afterTopCut.push_back(Triangle());
			afterTopCut.back().pt1 = output.pt1; afterTopCut.back().pt2 = output.pt2; afterTopCut.back().pt3 = pointsBelowCut[0];
		}
		else if (pointsBelowCutCount == 2)
		{
			afterTopCut.push_back(Triangle());
			afterTopCut.back().pt1 = output.pt1; afterTopCut.back().pt2 = output.pt2; afterTopCut.back().pt3 = pointsBelowCut[1];
			afterTopCut.push_back(Triangle());
			afterTopCut.back().pt1 = output.pt1; afterTopCut.back().pt2 = pointsBelowCut[0]; afterTopCut.back().pt3 = pointsBelowCut[1];
		}
		else if (pointsBelowCutCount == 3)
		{
			afterTopCut.push_back(inTri);
		}
	}

	//Cut with bottom edge...
	for (int i = 0; i < afterTopCut.size(); i++)
	{
		Vertex PointsAbove[3];
		int pointsAboveCount = 0;
		for (int j = 0; j < 3; j++)
		{
			if (afterTopCut[i][j].z > minZ)
			{
				PointsAbove[pointsAboveCount] = afterTopCut[i][j];
				pointsAboveCount++;
			}
		}
		if (pointsAboveCount == 3)
		{
			//Triangle needs no further cutting
			output.push_back(afterTopCut[i]);
			continue;
		}
		Line intersectionLine = afterTopCut[i].getIntersectionLine(minZ);
		if (pointsAboveCount == 1)
		{
			output.push_back(Triangle());
			output.back().pt1 = intersectionLine.pt1;
			output.back().pt2 = intersectionLine.pt2;
			output.back().pt3 = PointsAbove[0];
		}
		if (pointsAboveCount == 2)
		{
			//Two triangles out of this triangle
			output.push_back(Triangle());
			output.back().pt1 = intersectionLine.pt1;
			output.back().pt2 = intersectionLine.pt2;
			output.back().pt3 = PointsAbove[0];

			output.push_back(Triangle());
			output.back().pt1 = intersectionLine.pt1;
			output.back().pt2 = PointsAbove[0];
			output.back().pt3 = PointsAbove[1];
		}
	}
}

Line Triangle::getLine(uint line) const
{
	if (line == 0)
		return Line(pt2, pt1,0);
	if (line == 1)
		return Line(pt3, pt2,0);
	if (line == 2)
		return Line(pt1, pt3,0);

	return Line(pt1, pt1,0);
}