#pragma once

#include "Line.h"

/* Currently only plans an infill path */

class MotionPlanner
{
private:
	std::vector<std::shared_ptr<Lines>> perimetersOnLayer;
public:
	MotionPlanner();
	~MotionPlanner();

	std::vector<Line> getTravelPath(const Vertex &in, const Vertex &out);
	static std::vector<Lines> arrangeInfill(const std::vector<Lines> &inLines, const std::vector<Lines> &perims, double density, bool straightJoin = false);

	static void findShortestPath(const std::vector<Lines> &inLineSet, std::vector<Lines> &output, const std::vector<Lines> &perimeters);
};

