#pragma once
#include "iSlicable.h"
class wxFrame;
/*DEPRECATED*/
class TestPrint :
	public iSlicable
{
public:
	enum class testType {
		CLOSING_CIRCLE, CUBE, SNAKE_CUBE
	};
private:
	testType thisType; //Use inheritance?
protected:
#if USE_INT_COORDS
	micrometers xWidth;
	micrometers yHeight;
#else
	double xWidth;
	double yHeight;
#endif
	unsigned int layerCount;
	std::shared_ptr<Layer> *m_ptrLayerArray = nullptr;

	TestPrint(testType type, double xSize, double ySize);
public:
#if USE_INT_COORDS
	virtual micrometers getMinZ() const {
		return 0;
	}
	
	virtual micrometers getMinX() const {
		return 0;
	}
	virtual micrometers getMaxX() const {
		return xWidth;
	}
	virtual micrometers getMinY() const {
		return 0;
	}
	virtual micrometers getMaxY() const {
		return yHeight;
	}
	virtual micrometers getMaxZ() const { return 0; }
#else
	virtual double getMinZ() const {
		return 0;
	}

	virtual double getMinX() const {
		return 0;
	}
	virtual double getMaxX() const {
		return xWidth;
	}
	virtual double getMinY() const {
		return 0;
	}
	virtual double getMaxY() const {
		return yHeight;
	}
	virtual double getMaxZ() const { return 0; }
#endif

	virtual void createSlice(unsigned int) {};
	virtual std::shared_ptr<Layer> getLayer(unsigned int id)const {
		return m_ptrLayerArray[id];
	}
	virtual unsigned int getLayerCount()const {
		return layerCount;
	}
	virtual unsigned int getLineCount() const { return 10; }
	virtual void startSlice(wxFrame *baseFrame) {};
	virtual void slicePerimeters(unsigned int layerID) {};
	virtual void fillInfill(unsigned int layerID,const std::vector<std::vector<RoVaClipperLib::IntPoint>> &fullInfill, bool skipTopLogic = false) {};
	virtual void createSurfaces(unsigned int layerID) {};
	virtual void completeLayer(unsigned int layerID) {};
	virtual void generateSupport() {};
	
	virtual double getOffsetX() const{
	  return 0.0;
	}
	virtual double getOffsetY() const{
	  return 0.0;
	}
	virtual double getOffsetZ() const{
	  return 0.0;
	}
	virtual ~TestPrint();
};

class ClosingCircle : public TestPrint
{
private:
#if USE_INT_COORDS
	micrometers zHeight;
#else
	double zHeight;
#endif
	unsigned int facets;
public:
	ClosingCircle(double xSize, double ySize, double zSize, unsigned int facetCount = 16);
	virtual ~ClosingCircle();
#if USE_INT_COORDS
	micrometers getMaxZ() const {
#else
	double getMaxZ() const {
#endif
		return zHeight;
	}
	virtual void startSlice();
	virtual void slicePerimeters(unsigned int layerID);
	virtual void fillInfill(unsigned int layerID, std::vector<std::vector<RoVaClipperLib::IntPoint>> &fullInfill, bool skipTopLogic = false);
	virtual void completeLayer(unsigned int layerID);
};