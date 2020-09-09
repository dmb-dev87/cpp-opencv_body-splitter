#pragma once
#include "Triangle.h"
#include "RSPolygon.h"
/* TODO: doesnt handle textures on surfaces*/
class Surface
{
private:
	std::vector<Triangle > m_Triangles;
	unsigned int layerID;
	
public:
	std::shared_ptr<ExRSPolygon> m_ptrSurfacePoly;
	void addTriangle(Triangle &inTri);
	Surface(unsigned int layer);
	Surface(unsigned int layer, const std::vector<Line> &contour);
	void infillSurface();
	void generatePerimeters();
	void setColour(const Colour &in);
};

