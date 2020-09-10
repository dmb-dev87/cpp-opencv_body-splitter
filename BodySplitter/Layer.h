#pragma once
/* Holds information for each layer after slicing, perims, infll, islands */

#include <vector>
#include <unordered_map>
#include "Perimeter.h"
#include "Surface.h"
#include "Infill.h"
#include "Mesh.h"
#include "iSlicable.h"
#include <unordered_set>
class Triangle;
class GCodeVisualizer;

class Layer
{
private:
	friend class GCodeVisualizer;
public:
	std::unordered_set<Colour> coloursOnLayer;
	unsigned int layerNumber;
	std::list<std::shared_ptr<ExRSPolygon>> m_layerPolys;
	double layerThickness;
	RoVaClipperLib::Paths m_supportPoly;
	std::vector<Lines> m_supportInfillLines;
	RoVaClipperLib::Paths m_supportInterfacePoly;
	std::vector<Lines> m_supportInterfaceLines;
	iSlicable::ToolType thisTool;
public:
	std::vector<Triangle> m_triangles;
	
	
	std::vector<std::shared_ptr<Surface> > m_Surfaces;
	
	bool hasSurface = false;
	bool hasPerimeter = false;
	double minZ;
	Layer(double z, unsigned int layerNum, iSlicable::ToolType toolIn);
	// TODO: make private
	RoVaClipperLib::Paths clipperlib_perimeters;
	~Layer();
	void InfillPerimeters(double density,const RoVaClipperLib::Paths &full, bool skipTopLogic, const RoVaClipperLib::Paths *sources);
	void fillSupport(const RoVaClipperLib::Paths *sources);
	void AddPerimeters(const std::vector<Triangle> &triangles);	//Not const so any triangles that cant make a perimeter become surface triangles
	void AddSurfaces(const std::vector<Triangle> &triangles);
	void completeLayer();
	bool isGood() const;
	void colourSurfaces(Mesh::MeshType colourType, BodySplitterTexture* pTexture = nullptr);
	void generateInnerPerimeters();
	void cutOutSurfaces();
	std::size_t getColourCount() const;
	void addRSPoly(std::shared_ptr<ExRSPolygon> inPoly);
	void applyCrazyInfill(bool isUVMap);

	RoVaClipperLib::Paths& getSupportPoly() {
		return m_supportPoly;
	}

	void printColours();
	double getThickness()const {
		return layerThickness;
	}
	// TODO: Remove this function
	std::list<std::shared_ptr<ExRSPolygon>> &getRSPolies() {
		return m_layerPolys;
	}
	double getLayerZ() const;
	void finalizeSurfaces();

	std::unordered_set<Colour> getColoursOnLayer() const {
		return coloursOnLayer;
	}
	
	void writeDebugPerimeters(const std::string &meshName, unsigned int layerID, unsigned int nOp);
	void RemoveShortLineSegments();
};

