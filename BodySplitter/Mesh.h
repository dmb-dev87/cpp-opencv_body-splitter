#pragma once
/* Should be able to load all mesh formats assimp supports, generates
perimeters for each layer */

#include <opencv2/core/core.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <vector>
#include <list>
#include <memory>
#include <fstream>
#include "Triangle.h"
#include <deque>

#include "AABoundingBox.h"
#include "iSlicable.h"
#include "iDrawable.h"

#define USE_ARB_VBO

typedef unsigned int GLuint;
typedef float GLfloat;

class Slice;
class Layer;
class ExRSPolygon;
class wxFrame;
class SupportAnalyzer;
class GCodeVisualizer;
class BodySplitterTexture;
class Perimeter;
class Line;
class SupportTower;
class SupportMesh;
class Mesh : public iDrawable
{
private:
	friend class GCodeVisualizer;

protected:
	RoVaClipperLib::Paths m_enclosingPerimeter;

	AABoundingBox *m_bounds;
	std::deque<Triangle> m_triangles;
	std::deque<Vertex> m_verts;
	std::shared_ptr<Layer> *m_LayerArray = nullptr;
	double centreBedMove[3];
	unsigned int layerCount;

	void writePerimeterDebug(unsigned int layerID, unsigned int nOp=0);
	virtual void createVBOS() = 0;


	void splitPerimeterLinesOnContours(Layer &layer);
	void splitByPerim(Perimeter* perim);
	void spitPerimeterLinesForVertexColor(Layer &layer);
	void sloppyColourTexture(Layer &layer);
	void colourLayer(std::shared_ptr<Layer> layer);

	bool isSliced;
public:
	void applyTransformations() override;
	void undoTransformations() override;

	std::deque<Triangle> getTriangles() const {
		return m_triangles;
	}

	virtual void createArrayData(bool moveOrigin) =0;
	enum class MeshType{noColor,VertexColor,UVMap,RegionColour,SupportMesh};
	MeshType thisType = MeshType::noColor;
	Mesh();

	//
	//SEL Jan 2020 add support to export all the polygons for use by the program BodySplitter
	//
	void LogPerimeters();

	void createSlice(unsigned int sliceID);
	double getMaxZ() const;
	double getMinZ() const;
	virtual void generateSkirt();
	std::shared_ptr<Layer> getLayer(unsigned int id)const;
	unsigned int getLayerCount()const;
	virtual void scaleMesh(double x, double y, double z);
	void rotateMesh(double angle, int axis);
	RoVaClipperLib::Paths getEnclosingPerimeters() const;
	void fillInfill(unsigned int layerID, const RoVaClipperLib::Paths &fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic);
	void updateTextureColour(uint16_t newcolours) override;
	virtual ~Mesh();
	virtual void startSlice(wxFrame *baseFrame);
	void makeSliced();
// ------------------- iDrawable ------------------------------------------- //
	virtual void draw(unsigned int opts, const Colour *colourToDraw) noexcept = 0;
	void moveTo(double x, double y, double z) override;

	void setToBed();

	void MoveMeshZero();	//Moves the mesh so the minimum point of the bounding box is at 0
	unsigned int getLineCount()const;
	void moveMesh(double x, double y, double z);
	virtual double getMinX() const {
		return m_bounds->getMinX();
	}
	virtual double getMaxX() const {
		return m_bounds->getMaxX();
	}
	virtual double getMinY() const {
		return m_bounds->getMinY();
	}
	virtual double getMaxY() const {
		return m_bounds->getMaxY();
	}

	void slicePerimeters(unsigned int layerID);
	void createSurfaces(unsigned int layerID);
	void completeLayer(unsigned int layerID);

	double getZExtent() const;

	//
	//SEL Jan 2020 add support to export all the polygons for use by the program BodySplitter
	//
	//void LogPerimeters();
	
	
// --------------------- iDrawable --------------------------//
	virtual void applyRotation(double zRotation) override;
	virtual void applyScale(double amt) override;
	virtual double getXExtent() const override;
	virtual double getYExtent()  const override;
	virtual void setMaxSize() override;

	bool hasTexture() const override;

	virtual void setOFfsetX(double newPos) override;
	virtual void setOFfsetY(double newPos) override;
	virtual void setOFfsetZ(double newPos) override;

	std::unordered_set<uint32_t> getColours() override;

	

// --------------------- iSlicable ------------------------- //
	virtual double getOffsetX() const override;
	virtual double getOffsetY() const override;
	virtual double getOffsetZ() const override;

	virtual double getWidth() const override {
		return m_bounds->getMaxX() - m_bounds->getMinX();
	}
	virtual double getHeight() const override {
		return m_bounds->getMaxY() - m_bounds->getMinY();
	}
	RoVaClipperLib::Paths getPerimeters(unsigned int layerID) override;

	std::set<uint16_t> getToolsUsed();
	bool isColour() const;
};

