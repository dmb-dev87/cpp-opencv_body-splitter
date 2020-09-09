#pragma once
#include <vector>
#include <deque>
#include "Vertex.h"

/* Partially unused, holds the size of a box that contains the mesh*/
class AABoundingBox
{
private:
	Vertex min;
	Vertex max;
public:
	explicit AABoundingBox(const std::vector<Vertex> &verts);
	explicit AABoundingBox(const std::deque<Vertex> &verts);
	AABoundingBox(double x, double y, double z);
	~AABoundingBox();
	//X
	double getMinX() const {
		return min.x;
	}
	double getMaxX() const {
		return max.x;
	}
	//Y
	double getMinY() const {
		return min.y;
	}
	double getMaxY() const {
		return max.y;
	}
	//Z
	auto getMinZ() const {
		return min.z;
	}
	auto getMaxZ() const {
		return max.z;
	}
};

