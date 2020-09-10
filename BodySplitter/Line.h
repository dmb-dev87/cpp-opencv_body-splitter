#pragma once
/* Holds the information for each print line */

#include <vector>
#include <algorithm>
#include <list>
#include <memory>
#include <unordered_map>
#include <deque>

#include "colour.h"
#include "Vertex.h"

class BodySplitterTexture;
class Line
{
private:
	Line(const Vertex &in1, const Vertex &in2);

public:
	// Line type is used for gcode visualize by type, it could also
	// but isnt used for extrusion speed, multipleir etc
	struct LineType {
		static const unsigned int PERIM				= 0x00001;
		static const unsigned int TO_INSET			= 0x00002;
		static const unsigned int INSET				= 0x00004;
		static const unsigned int OUTERPERIM			= 0x00008;
		static const unsigned int INNERPERIM			= 0x00010;
		static const unsigned int INFILL			= 0x00020;
		static const unsigned int TRAVEL			= 0x00040;
		static const unsigned int RETRACT			= 0x00080;
		static const unsigned int ISSUE				= 0x00100;
		static const unsigned int COAST				= 0x00200;
		static const unsigned int Z_MOVE			= 0x00400;
		static const unsigned int SURFACE			= 0x00800;
		static const unsigned int BOTTOM_LAYER			= 0x01000;
		static const unsigned int TOP_SOLID			= 0x02000;
		static const unsigned int UNSUPPORTED			= 0x04000;
		static const unsigned int PARTIAL_SUPPORT		= 0x08000;
		static const unsigned int CREATED_NO_PARAM		= 0x10000;
		static const unsigned int PRIME_TOWER			= 0x20000;
		static const unsigned int HOLE_PERIM			= 0x40000;
		static const unsigned int WEAK_SUPPORT			= 0x80000;
		static const unsigned int SUPPORT_INTERFACE		= 0x100000;
		static const unsigned int SUPPORT_BASE			= 0x200000;
		static const unsigned int MANUAL_MOVE			= 0x400000;
		static const unsigned int RETRACT_WIPE			= 0x800000;
	};
	static const uint8_t maxColour = 128;
	Colour colour;

	Vertex pt1, pt2;
	unsigned int LineParam = 0;
	Line(const Vertex &in1, const Vertex &in2,unsigned int linetype);
	
	Line() noexcept {};
	double extrusionThickness = 1.0;
	bool IntersectsPlane(double z, double height) const;	//Thats right, planes have thickness now
	bool CrossesPlane(double planeHeight) const;
	bool InsideLayer(double z, double height) const;		//Used to determine if entire line inside a slice, means triangle is part surface
	bool IsColinear(const Line &input) const;
	bool touchesLine(const Line &input) const;
	bool touchesPoint(const Vertex &pt) const;
	bool hasXYLength() const;
// 	bool isInset() const
// 	{
// 		return inset;
// 	}
	void mergeWith(const Line &input);
	Vertex pointAt(double z) const;
	Vertex pointAtDistance(double distance) const;
	double distancetoPointR2(const Vertex &vert) const;
	Vertex pointClostestTo(const Vertex &inPt) const;
	inline void reverse() { std::swap(pt1, pt2); }
	Line getReverse() const {
		return Line(pt2, pt1);
	}
	double length()const;
	double sqrdLength()const;
	std::size_t hash() const;
	std::size_t hash_position() const noexcept;
	std::size_t hash_with_colour() const noexcept;
	bool operator==(const Line &input) const
	{
		return input.pt1 == this->pt1 && input.pt2 == this->pt2;
	}
	bool operator!=(const Line &input) const
	{
		return input.pt1 != this->pt1 || input.pt2 != this->pt2;
	}
	void setColour(const Colour &inColour);
	bool splitLine(double length, Line &outA, Line &outB) const;
	//Split line at UV coords
	bool splitLine(double u, double v, Line &outA, Line &outB) const;
	Line getColinearLine(double u1, double v1, double u2, double v2) const;
	//bool splitLine(double x, double y, Line &outA, Line &outB) const;
	//void setColourAverage();//Sets both points to a colour that is the average of the two
	static void divideIntoColourSegments(const Line &input, double colourRes, std::vector<Line> &out);
	bool interesctsLineR2(const Line &targ) const;	//Check for intersection of line segments
	Colour getColour()const;
	Vertex UVToVertexCoords(double u, double v) const;
	void addParameter(unsigned int newType);
	bool isLineOfType(unsigned int type) const;
	bool matchesLocation(const Line& in) const;
	void rotateLine(double angle);
	unsigned int getParam()const {
	    return LineParam;
	}

	std::vector<Line> breakLineToPixels(const BodySplitterTexture *ptrTexture) const;
	size_t color_hash() const;

	static bool lineCrossesContour(const Line &testLine, const std::vector<Line> &contour);
	static void removeShortLines(std::vector<Line> &inputLines, double sqrdDistance);
	static void removeDuplicateLines(std::vector<Line> &inputLines);
	static void removeEqualOpposites(std::vector<Line> &inputLines);
	static void removeExtraInsets(std::vector<Line> &inputlines);
	static void removeZeroLengthLines(std::vector<Line> &inputLines);
	static void setRandomColour(std::vector<Line> &inputLines);
	static void setLineSetToColour(std::vector<Line> &input, const Colour &col);
	static void setLineSetToColour(std::deque<Line> &input, const Colour &col);
	static void removePointlessPoints(std::vector<Line> &input);
	static void rotateLines(std::vector<Line> &input,double degrees);
	static std::vector<Line>::iterator getShortestLine(std::vector<Line> &inputSet);
	static std::vector<Line>::iterator getClosestLine(std::vector<Line> &inputSet, const Line &targetLine);
	static void generateLines(std::vector<Line> &output, double lineSpacing, double x1, double x2, double y1, double y2, unsigned int linetype = 0);
	static void generateLinesB(std::vector<Line> &output, double lineSpacing, double x1, double x2, double y1, double y2, unsigned int linetype = 0);
	static void translateLines(std::vector<Line> &output, double x, double y);
	static void translateLines(std::vector<Vertex2D>& output, double x, double y);
	static void addParamToSet(std::vector<Line> &output, unsigned int param);
	static void reverseSet(std::vector<Line> &output);
	static double getSetLength(const std::vector<Line> &lineSet);
	static void makePathContinous(std::vector<Line> &out);
	static void removeUnconnectedLines(std::vector<Line> &out);
	static void removeLineWithMultipleConnections(std::vector<Line> &out);
	static void createLargestLoop(std::vector<std::vector<Line>> &out, const std::vector<Line> &inputPerimeter, const std::vector<Line> &insetLines);
	static void findLoop(std::vector<Line> &output, const std::vector<Line> &insets, const Vertex &pt1, const Vertex &pt2);
	static bool isSelfIntersecting(const std::vector<Line> &in);
	static double getAngleBetween(const Line &lineA, const Line &lineB);
#ifndef NDEBUG
	static void writeLinesToPolyFile(const std::string &filename, const std::vector<Line> &lines);
#endif
	static void writeLinesWithColourToFile(const std::string &fileName, const std::vector<Line> &lines);
	static void writeLinesWithColourToFile(std::ofstream &fileStream, const std::vector<Line> &lines);
	//GCode Writer

};

class GCodeLine : public Line
{
  private:
	friend class GCodeWriter;
	friend class MovementWriter;
	uint16_t tool;
	Colour colour;
	double e1, e2;
	double getExtrusionLength() const {
		return e2 - e1;
	}
	bool hasELength() const
	{
		return fabs(e2 - e1) > 0.1;
	}
	double feedrate;
	std::string comment;
public:
	GCodeLine(const Line &in) : Line(in) {};
	GCodeLine(uint16_t toolIn, Colour &col) : tool(toolIn), colour(col)
	{
	}
	double fullLength() const
	{
		return sqrt(pow(pt2.x - pt1.x, 2) +
			pow(pt2.y - pt1.y, 2) +
			pow(pt2.z - pt1.z, 2) +
			pow(e2-e1,2)
		);
	}
};
/*
class IntersectionLine : public Line
{
public:
	Vertex intPt;
	IntersectionLine(const Line &edge, const Vertex &intersectionPt) : Line(edge), intPt(intersectionPt) {}
	IntersectionLine() {};
	void setFromLine(const Line &inLine,const Vertex &inPoint) {
		this->pt1 = inLine.pt1; this->pt2 = inLine.pt2;
		intPt = inPoint;
	}
	bool MatchesReverseWinding(const IntersectionLine &input) const
	{
		return input.pt1 == this->pt2 && input.pt2 == this->pt1;
	}
};
*/
//class LineTreeNode
//{
//public:
//	std::vector<std::weak_ptr<Line>> leftHandLines;//left and right dont refer to actual left and righ
//	std::vector<std::weak_ptr<Line>> rightHandLines;
//	std::shared_ptr<Line> thisLine;
//	static void populateTree(const std::list<std::shared_ptr<Line>> insetLines, std::shared_ptr<LineTreeNode> node, std::unordered_map<std::size_t, std::shared_ptr<LineTreeNode>> &existingNodes);
//};

//typedef std::vector<IntersectionLine> IntersectionLines;
typedef std::vector<Line> Lines;

class LineHasher
{
public:
	size_t operator() (Line const& key) const
	{
		return key.hash();
	}
};

class LineEqualFn
{
public:
	bool operator() (Line const& t1, Line const& t2) const
	{
		if (t1.pt1 == t2.pt1 && t1.pt2 == t2.pt2)
			return true;
		if (t1.pt2 == t2.pt1 && t1.pt1 == t2.pt1)
			return true;
		return false;
	}
};

namespace BodySplitter {
	/* Returns 0 if lines dont intersect
	returns 1 if lines intersect at one point
	returns 2 if lines are on top of each
	output point is first intersection point*/
	unsigned int lineIntersection(double Line1Pt1x, double Line1Pt1y, double Line1Pt2x, double Line1Pt2y,
		double Line2Pt1x, double Line2Pt1y, double Line2Pt2x, double Line2Pt2y,
		double &outx, double &outy);
};
