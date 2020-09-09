#pragma once
#include <assert.h>
#include <vector>
#include <glm/vec3.hpp>
#include "Vertex.h"
#include "Line.h"

class Surface;

typedef unsigned int uint;

class Triangle
{
public:
	enum TriangleResult { ALL_BELOW, ALL_ABOVE, ALL_IN, MIXED, MESH };
	Vertex pt1, pt2, pt3;
	TriangleResult type;
	Triangle();
	Triangle(TriangleResult inType, const Vertex &inPt1, const Vertex &inPt2, const Vertex &inPt3) :
		pt1(inPt1), pt2(inPt2), pt3(inPt3), type(inType),parentSurface(nullptr) {}
	~Triangle();
	bool isVertical()const;
	bool touchesTriangle(const Triangle &in) const;
	double getXYArea()const;
	glm::vec3 getNormal() const;
	void FlattenToLayer(double z)
	{
#if USE_INT_COORDS
		pt1.z = to_micrometers(z); pt2.z = to_micrometers(z); pt3.z = to_micrometers(z);
#else
		pt1.z = (z); pt2.z = (z); pt3.z = (z);
#endif
	}
	Line getIntersectionLine(double z) const;	//Gets the intersection line of the triangle with horizontal plane
	Vertex getPt(int x)const
	{
		assert(x < 3);
		if (x == 0)
			return pt1;
		if (x == 1)
			return pt2;
		return pt3;
	}
	Vertex& operator[] (int x) {
		assert(x<3);//Fail if x >3
		if (x == 0)
			return pt1;
		if (x == 1)
			return pt2;

		return pt3;
	}
	Vertex operator[] (int x) const {
		assert(x<3);//Fail if x >3
		if (x == 0)
			return pt1;
		if (x == 1)
			return pt2;

		return pt3;
	}
	static void cutTriangle(const Triangle &inTri, double minZ, double maxZ, std::vector<Triangle> &output);
	
	Triangle *touchingEdges[3] = { nullptr,nullptr,nullptr };
	Surface *parentSurface = nullptr;
	uint getLineNumberFromHash(std::size_t hash);
	int edgeTouchCount = 0;
	Line getLine(uint line) const;
};

typedef std::vector<Triangle> Triangles;
