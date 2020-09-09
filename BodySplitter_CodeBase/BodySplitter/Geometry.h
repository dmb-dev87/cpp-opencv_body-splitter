#pragma once
#include <opencv2/core/core.hpp>
#include <stdint.h>
#include "Vertex.h"
#include "colour.h"

class Triangle;

/* Several geomery helper functions */

#define USE_INT_COORDS 0

//SEL March 2020 add some helper functions
#ifndef SQR
#define SQR(a) (a)*(a)
#endif
#ifndef DIST2D
#define DIST2D(a,b) sqrt(SQR((a.x)-(b.x)) + SQR((a.y)-(b.y)))
#endif
#ifndef DOT2D
#define DOT2D(a,b) (a.x * b.x + a.y + b.y)
#endif
//SEL end

namespace BodySplitter {
	using Vertex = vertex_t<double>;
	//using vert2DPoly = std::vector<Vertex2D>;
	//typedef vertex_t<double> Vertex;
	struct vert2DPoly {
	  Colour colour;
	  std::vector<Vertex2D> poly;
	};
	struct vert2DPath {	//is unclosed
	  Colour colour;
	  std::vector<Vertex2D> poly;
	};

	enum class AXIS : std::int8_t { X_AXIS = 0, Y_AXIS = 1, Z_AXIS = 2, E_AXIS = 3 };
};

class Line;
typedef std::vector<Line> Lines;

namespace BodySplitter
{
	//SEL March 2020 add a function to find the minimum distance between a point and a line segment
	double dMinimumDistancePtToLineSegment(const Vertex &vert, std::shared_ptr<Line> line) noexcept;
	//SEL March 2020 add a function to find the minimum distance between a point and a polygon
	double dMinimumDistancePtToPolygon(const Vertex &vert, const std::vector<std::shared_ptr<Line>> &poly) noexcept;

	bool isPointInPoly(const Vertex &vert, const Lines &poly) noexcept;
	bool isPointInPoly(const Vertex &vert, const std::vector<std::shared_ptr<Line>> &poly) noexcept;
	bool pointTouchesPolyCorner(const Vertex &vert1, const Vertex &vert2, const Lines &poly) noexcept;
	bool matchesR2Location(const Vertex &pt1, const Vertex &pt2, double threshold = THRESHOLD) noexcept;
	
	vert2DPath linePathToVertPath(const std::vector<Line> &soureLines);
	Lines vertPathToLinePath(const std::vector<Vertex2D> &path);
	
	std::vector< std::vector< Vertex2D > > brokenLinesToVert2DPath(const std::vector< Line >& lines);
	vert2DPoly brokenLinesToVert2DPoly(std::vector<Line> &lines, uint8_t r, uint8_t g, uint8_t b);
	vert2DPath brokenLinesToVert2DPath(std::vector< Line >& inlines, uint8_t r, uint8_t g, uint8_t b);
	
	std::vector<Vertex2D> simplifyPolygon(const std::vector< Vertex2D >& inPoly, double ep);
	
	void convertPixelToWorld(std::vector<Vertex2D> &inPath, double minX, double minY, double gridSize);
	void convertWorldTOPixel(std::vector<Vertex2D> &inPath, double minX, double minY, double gridSize);
	Vertex convertPixelToWorld(const Vertex &in, double minX, double minY, double gridSize);
	Vertex convertWorldtoPixel(const Vertex &in, double minX, double minY, double gridSize);
	std::pair<double,double> convertPixelToWorld(const std::pair<double,double> &pixel,double minX, double minY, double gridSize);
	
	void writePolygonToIPE(const vert2DPoly &poly, const std::string &filename);
	void writePolygonsToIPE(const std::vector<vert2DPoly> &poly, const std::string &filename);
	
	void writePathToIPE(const std::vector<Vertex2D> &poly, const std::string &filename, bool close = false);
	void writePathsToIPE(const std::vector<std::vector<Vertex2D>> &poly, const std::string &filename, bool close = false);
	void writePathToIPE(const vert2DPath &poly, const std::string &filename, bool close = false);
	void writePathsToIPE(const std::vector<vert2DPath> &poly, const std::string &filename,bool close = false);
	
	void writeLinesToIPE(const Lines &lines, const std::string &fileName);
	
	vert2DPoly mergeLineWithPath(const std::vector<std::shared_ptr<Line>> &lines, const vert2DPath &path);
	std::vector< vert2DPoly > mergeLinesWtihPaths(const std::vector< Line >& lines, std::vector< BodySplitter::vert2DPath >& poly, double tolerance);
	std::vector< vert2DPoly > mergeLinesWtihPaths(const std::vector< std::shared_ptr<Line> >& lines, std::vector< BodySplitter::vert2DPath >& poly);
	
	//SEL added
	std::vector< vert2DPoly > mergeLinesWithPaths(std::vector< vert2DPath >& outer, std::vector< vert2DPath >& inset, double tolerance);
	//SEL End

	vert2DPoly makePath(const std::vector< std::shared_ptr< Line > >& line, const std::vector< BodySplitter::vert2DPath >& paths, double tolerance);
	
	std::vector<std::vector<std::shared_ptr<Line>>> createIslandsByColour(const std::vector<std::shared_ptr<Line>> &lines);
	
	std::vector<BodySplitter::vert2DPoly> merge(std::vector<std::vector<std::shared_ptr<Line>>> &source, std::vector<vert2DPath> &insets);
	
	//45 degree smooth
	vert2DPath smoothLines(const vert2DPath &inpath);

	bool rayIntersectsTriangle(const Triangle &tri, const glm::vec3 &origin, const glm::vec3 &dir, glm::vec3 &intPt);
};
