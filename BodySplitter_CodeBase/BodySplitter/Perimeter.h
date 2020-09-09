#pragma once
/* Generates perimeters from a set of lines in the layer */

#include <opencv2/core/core.hpp>
#include <vector>
#include <string>
#include <list>

#include "Line.h"
#include "ClipperLib/RoVaclipper.hpp"
#include "iSlicable.h"
#include "colour.h"
#include <memory>


class BodySplitterTexture;
class GCodeVisualizer;
class ExRSPolygon;

class Perimeter
{
private:
	friend class GCodeVisualizer;
	// Should work everytime and avoid intersecting loops
	bool createDijikstraPerimeters(Lines &lines);
	//Uses a method more prone to failure atm but produces more correct perimeters
	bool createStrongPerimeter(Lines &lines);
	// If the better method fails, fallback to this
	void createWeakPerimeter(Lines &lines, bool hasTextureMap = false, bool forceClose = false);
	std::vector<std::vector<Line>> m_insetLines;
	
public:
	Colour perimColour;
	RoVaClipperLib::Path rovaClipPath;
	std::list<std::vector<Line>> m_lineLoops;
	std::vector<Line> m_lines;
	std::weak_ptr<Perimeter> parent;
	std::vector<std::shared_ptr<Perimeter>> children;
	bool isHole;
	bool holeCalculated;
	double Area() const;
#if USE_INT_COORDS
	micrometers width() const;
	micrometers height() const;
	micrometers minX() const;
	micrometers minY() const;
#else
	double width() const;
	double height() const;
	double minX() const;
	double minY() const;
#endif
	Perimeter() = default;
	Perimeter(Lines &lines,bool hasTextureMap = false, bool forceClose = false);
	Perimeter(std::vector<cv::Point_<int>> &vcPoints);
	Perimeter(const std::vector<cv::Point_<int>> &cvPoints, unsigned int xSize, unsigned int ySize,double maxX, double maxY);
	explicit Perimeter(const BodySplitter::vert2DPoly &inPoly);
	~Perimeter() = default;

	void Reverse();
	bool pointIn(const Vertex &inVert) const;
	bool isWithin(const Perimeter &bigger) const;
	bool isBigger(const Perimeter &test) const;
#if USE_INT_COORDS
	std::vector<Lines> offset(micrometers amt) const;
#else
	std::vector<Lines> offset(double amt) const;
#endif
	void pushColourInside(std::vector<Lines> &outputLoops);
	void setColour(double r, double g, double b);
	void setColour(const Colour &in);
	Colour getColour() const;
	static void generateExRSPolygons(std::list<std::shared_ptr<ExRSPolygon>> &out, std::vector<Perimeter> &lines, iSlicable::ToolType toolIn);

	Line getInsetLineNearest(const Line &in) const;
	Vertex getInsetNearest(const Vertex &in) const;
	void simplify();

	std::vector<Lines> cut(const std::vector<Line> &cutlines);

	void writeContour(std::fstream &file);

	void breakLineIntoPixelColours(const BodySplitterTexture *ptrText);
	void travisSimplification();
};

class InnerPerims {
private:
	InnerPerims *m_ptrParent = nullptr;
	std::vector<InnerPerims*> m_children;
	
	std::vector<Vertex2D> *m_path = nullptr;
	std::vector<std::vector<Vertex2D>> *m_holePaths = nullptr;
	std::vector<std::vector<Vertex2D>> infillPath;
	std::vector<std::vector<Vertex2D>> fullInfillPath;
public:
	Colour colour;
	InnerPerims(InnerPerims *parent);
	InnerPerims(InnerPerims *parent, int perimDepth);
	explicit InnerPerims(InnerPerims *parent, const std::vector<Vertex2D> &path,const std::vector<std::vector<Vertex2D>> &holePath, int perimDepth, double offset);
	~InnerPerims();

	void setPerimeterLoop(const Perimeter &perim);
	void addHole(const std::vector<Vertex2D> &hole);
	void addHole(const std::shared_ptr<Perimeter> &hole);
	void generateInnerPerims(double offsetAmt, int perimDepth);

	//This is an output function very unfortunatly named and written
	void fillTheInnerestOfPerims(std::vector<InnerPerims*> &perims);

	const std::vector<Vertex2D> *getContour() const
	{
		return m_path;
	}
	const std::vector<std::vector<Vertex2D>>* getHoles() const {
		return m_holePaths;
	}
	const std::vector<InnerPerims*> &getChildren() const {
		return m_children;
	}
	const std::vector<std::vector<Vertex2D>> &getInfillLines() const {
		return infillPath;
	}
	const std::vector<std::vector<Vertex2D>> &getFullInfillLines() const {
		return fullInfillPath;
	}
	bool hasHole() const noexcept {
		return m_holePaths != nullptr;
	}
	void addInfillLine(const std::vector<Line> &lineSet, bool isFull);

	std::vector<std::vector<Vertex2D>> getAllInfillLines() const;
};