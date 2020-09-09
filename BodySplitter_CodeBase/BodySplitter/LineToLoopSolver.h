#pragma once
/* Since the triangles in an obj are in no order, perimeters must be recontructed
djikstra's algorithm generates the smallest loop, so should be corrent
HOWEVER, floating point errors, modelling errors often result in loops that
dont close and a different loop method must be used- slic3r and s3d both use different
methods than RS and each other*/

#include <vector>
#include "Line.h"
#include "Vertex.h"

class LineToLoopSolver
{
public:
	LineToLoopSolver();
	~LineToLoopSolver();

	static bool findLoop(std::vector< Line >& output, const std::vector< Line >& insets, const Vertex& pt1, const Vertex& pt2);
};

