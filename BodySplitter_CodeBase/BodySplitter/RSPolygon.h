#pragma once
/* RS Polygons hold perims, inner perims and infill info*/
#include "Line.h"
#include "Infill.h"
#include "iSlicable.h"
#include <vector>
#include <memory>


#include "Perimeter.h"

class RSPolygon
{
private:
	friend class Surface;
	
	double m_area;
	
	RSPolygon* parent;
	
	void addChild(RSPolygon* child);
	double minX, maxX;
	double minY, maxY;
public:
	double r, g, b;
	void reverseWinding();
	void resisterChild();
	std::vector<Line> m_lines;
	std::vector<RSPolygon*> children;
	bool isHole = false;
	RSPolygon(const std::vector<cv::Point_<int> > &pointSource, RSPolygon *inParent);
	void setColour(float inR, float inG, float inB);
	RSPolygon(RSPolygon *inParent);
	~RSPolygon();
	double getWidth() const
	{
		return maxX - minX;
	}
	double getHeight() const
	{
		return maxY - minY;
	}
	double getLeft() const
	{
		return minX;
	}
	double getBottom() const
	{
		return minY;
	}
	double area() const
	{
		return m_area;
	}
};

class ExRSPolygon
{
private:
	friend class GCodeVisualizer;
	double r, g, b;
	double extWidth;
	double infillExtWidth; //Dial this down when bridging or printing top/bottom solid layers not on the print bed
	unsigned int innerPerimCount;
	std::vector<std::shared_ptr<Perimeter>> getInnerPerims(std::shared_ptr<Perimeter> perim) const;
	ExRSPolygon* parent = nullptr;
	InnerPerims *perimeterTree = nullptr;
	iSlicable::ToolType thisTool;
	double getPolytreeHolelessArea(const RoVaClipperLib::PolyNode *polyOut);
	std::list<std::shared_ptr<ExRSPolygon>> offsetPolygon(bool hasUVMAp = true);
public:
	std::vector<std::shared_ptr<Perimeter>> m_holes;
	std::vector<Lines> basicLinePaths;
	bool infillPiece = true;
	//std::vector<std::shared_ptr<Infill>> m_infillPieces;
	std::shared_ptr<Perimeter> m_contour;
//	std::vector<std::shared_ptr<Perimeter>> *innerPerims = nullptr;	//List to preserve memory locations
//	std::vector<std::shared_ptr<Perimeter>> innerestPerims;	//Store references so no one tries to delete these...

	void addHole(const RSPolygon &hole);
	//void addHoles(const std::vector<Perimeter*> &perims);
	void addHole(const Perimeter &perim);
	void addHole(const std::shared_ptr<Perimeter>& perim);
	void setContour(const Perimeter &perim);
	void setContour(const std::shared_ptr<Perimeter> &perim);

	// Optimized infll source path is created by slicer.cpp and passed on
	void generateInfill(double angle,bool dir,double density, const RoVaClipperLib::Paths &notFullInfill, bool skipTopLogic, const RoVaClipperLib::Paths *sources, bool isTop);
	// Unoptimzed, exists to satisfy current coasterprint.cpp method
	void generateInfill(double angle, bool dir, double density);

	void cutInfill(const std::vector<Line> &lines);
	void setInfillColour(const Colour& in);

	std::vector<std::shared_ptr<ExRSPolygon>> cutOut(const std::vector<Line> &lines);

	//explicit ExRSPolygon(const std::vector<RSPolygon> &contoursAndHoles);
	ExRSPolygon(const std::shared_ptr<Perimeter> &contour, ExRSPolygon *par);
	ExRSPolygon(iSlicable::ToolType inType);
	//ExRSPolygon(const std::vector<std::vector<cv::Point> > &input);
	~ExRSPolygon();
	//void setAsInset();
	// Default value gets inner perim count from settings
	void generateInnerPerimeters(int innerPerimCount = -1);

	void setExtWidth(double wid);
	double getExtWidth() const {
		return extWidth;
	}
	double getFullInfillExtWidth() const {
		return infillExtWidth;
	}
	double getSquaredDistanceNearest(const Vertex &in) const;
	Colour getColour() const;
	void addParamToLines(unsigned int param);

	void addParamToLines(unsigned int param,const RoVaClipperLib::Paths &linesInside);

	void registerChild();

	std::list<std::shared_ptr<ExRSPolygon>> straightSkeletonColours(bool isUVMap);

	const InnerPerims *getPerimTree() const
	{
		return (const InnerPerims*)perimeterTree;
	}

	static void ClipperLibPolytreeToRSPolygon(const RoVaClipperLib::PolyNode *polyOut,std::list<std::shared_ptr<ExRSPolygon>> &output, const Colour &colour);
};