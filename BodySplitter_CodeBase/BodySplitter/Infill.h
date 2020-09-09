#pragma once
#include "Line.h"
#include "ClipperLib/RoVaclipper.hpp"

/* Generates and stores infill lines */

class RSPolygon;
class Perimeter;
#if USE_INT_COORDS
using namespace BodySplitter;
#endif

class Infill
{
private:
	
	void colourLines();
	double m_density;
	double m_fLineAngle;
	std::vector<Lines> m_lines;
	std::vector<Lines> m_fullInfillLines;
public:
	void orderLines(const std::vector<Line> &perim, const std::vector<std::vector<Vertex2D>>* holes, bool straightJoin = false);
	void orderLines(const std::vector<Vertex2D> &perim, const std::vector<std::vector<Vertex2D>>* holes, bool straightJoin = false);
	void orderLines(const RoVaClipperLib::Paths &perims, bool straightJoin = false);
	double r = -1, g = -1, b = -1;
	
	std::vector<Lines> sortedLines;
	std::vector<Lines> fullInfillLinesSorted;
	Infill(float density, double width, double height, double angle, double xoff, double yoff, bool lastDir,const RoVaClipperLib::Paths &notFullInfill, bool skipTopLogic);
	Infill(double angle, const RoVaClipperLib::Paths *sources, const RoVaClipperLib::Paths &notFullInfill, bool skipTopLogic, bool isTop);

	void completeCut(const std::vector<Vertex2D> *perim, const std::vector<std::vector<Vertex2D>> *holes);
	void cut(const RoVaClipperLib::Paths &cutter);

	void setColour(const Colour &in);
	Colour getColour() const;

};

