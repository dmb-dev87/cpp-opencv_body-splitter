#pragma once
//#include "RoVaclipper.hpp"
#include <memory>
#include "Geometry.h"
#include "GCodeVisualizer.h"
#include "ClipperLib/RoVaclipper.hpp"
#include "PrintBed.h"

/* Handles an interface to slicable items, should be merged with iDrawable*/

class Layer;
class wxFrame;
class GCodeVisualizer;

#if USE_INT_COORDS
using namespace BodySplitter;
#endif
namespace RoVaClipperLib
{
	struct IntPoint;
};
#include <vector>

class ExRSPolygon;
class Colour;
class BodySplitterTexture;

class iSlicable
{
public:
	// who needs more than 5 tools anyways
	enum class ToolType : uint8_t { FULL_COLOUR = 0, FLEXIBLE, SUPPORT, GENERIC1, GENERIC2, GENERIC3 };
private:
	
protected:
	bool m_simplifiedTexture = false;
	bool m_bisSliced = false;
	bool m_bIsTransformed = false;
	std::shared_ptr<GCodeVisualizer> m_ptrVisualizer;
	std::vector<std::shared_ptr<ExRSPolygon>> m_skirts;
	

	virtual void undoTransformations() = 0;
	virtual void applyTransformations() = 0;

	ToolType thisTool = ToolType::FULL_COLOUR; //This is BodySplitter, full colour for life
public:
	virtual ~iSlicable() {
	};
	iSlicable()
	{
		
	}

	virtual double getMinZ() const = 0;
	virtual double getMaxZ() const = 0;
	virtual double getMinX() const = 0;
	virtual double getMaxX() const = 0;
	virtual double getMinY() const = 0;
	virtual double getMaxY() const = 0;

	//SEL Jan 2020 add logging function
	virtual void LogPerimeters();

	virtual void createSlice(unsigned int) = 0;
	virtual std::shared_ptr<Layer> getLayer(unsigned int id)const =0;
	virtual RoVaClipperLib::Paths getPerimeters(unsigned int layerID) = 0;
	virtual unsigned int getLayerCount()const = 0;
	virtual unsigned int getLineCount() const = 0;
	virtual void startSlice(wxFrame* baseframe) = 0;
	virtual void slicePerimeters(unsigned int layerID) = 0;
	virtual void fillInfill(unsigned int layerID ,const std::vector<std::vector<RoVaClipperLib::IntPoint>> &fullInfill,const RoVaClipperLib::Paths *sources, bool skipTopLogic = false) = 0;
	virtual void createSurfaces(unsigned int layerID) = 0;
	virtual void completeLayer(unsigned int layerID) = 0;
	
	virtual double getOffsetX() const = 0;
	virtual double getOffsetY() const = 0;
	virtual double getOffsetZ() const = 0;

	virtual double getWidth() const = 0;	//X
	virtual double getHeight() const = 0;	//Y
	// Builds and returns the 2d projection to the xy plane - used for slicing infill/skirts/brim
	virtual RoVaClipperLib::Paths getEnclosingPerimeters() const = 0;
	virtual void generateSkirt() = 0;
	virtual bool isColour() const = 0;
	virtual void invalidateSlice()
	{
		m_bisSliced = false;
		m_ptrVisualizer.reset();
		//undoTransformations();
	}

	virtual std::vector<std::shared_ptr<ExRSPolygon>>& getSkirts() {
		return m_skirts;
	}
	
	bool isSliced() const
	{
		return m_bisSliced;
	}
	virtual void makeSliced()
	{
		m_bisSliced = true;
	}
	void setTextureSimplified(bool simply)
	{
		m_simplifiedTexture = simply;
	}
	bool getSimpleTextureOn() const
	{
		return m_simplifiedTexture;
	}

	std::shared_ptr<GCodeVisualizer> getVisualizer() const
	{
		return m_ptrVisualizer;
	}
	virtual bool hasSupport() const {
		return false;
	}


	virtual std::set<uint16_t> getToolsUsed() {
		return std::set<uint16_t>();
	}
	

	//is enum, pass by value
	virtual void setTool(ToolType newTool) {
		PrintBed::getSingleton().changesNeedSaving();
		thisTool = newTool;
	}
	virtual ToolType getTool() const
	{
		return thisTool;
	}
};

