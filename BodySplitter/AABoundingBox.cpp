#include "AABoundingBox.h"
#include <algorithm>
#include <numeric>

AABoundingBox::AABoundingBox(const std::vector<Vertex> &verts)
{
	double minX(std::numeric_limits<double>::max());	double minY(minX);	double minZ(minX);
	double maxX(-std::numeric_limits<double>::max());	double maxY(maxX);	double maxZ(maxX);
	for (auto it = verts.begin(); it != verts.end(); ++it)
	{
		maxX = std::max(maxX, it->x);
		minX = std::min(minX, it->x);

		maxY = std::max(maxY, it->y);
		minY = std::min(minY, it->y);

		maxZ = std::max(maxZ, it->z);
		minZ = std::min(minZ, it->z);
	}
	max.x = maxX; max.y = maxY; max.z = maxZ;
	min.x = minX; min.y = minY; min.z = minZ;
}

AABoundingBox::AABoundingBox(const std::deque<Vertex> &verts)
{
	double minX(std::numeric_limits<double>::max());	double minY(minX);	double minZ(minX);
	double maxX(-std::numeric_limits<double>::max());	double maxY(maxX);	double maxZ(maxX);

	for (auto it = verts.begin(); it != verts.end(); ++it)
	{
		maxX = std::max(maxX, it->x);
		minX = std::min(minX, it->x);

		maxY = std::max(maxY, it->y);
		minY = std::min(minY, it->y);

		maxZ = std::max(maxZ, it->z);
		minZ = std::min(minZ, it->z);
	}
	max.x = maxX; max.y = maxY; max.z = maxZ;
	min.x = minX; min.y = minY; min.z = minZ;
}

AABoundingBox::~AABoundingBox()
{
}
