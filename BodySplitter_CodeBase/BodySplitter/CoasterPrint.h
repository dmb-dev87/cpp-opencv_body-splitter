#pragma once
#include <opencv2/core/core.hpp>
#include <list>
#include "iSlicable.h"
#include "iDrawable.h"
#include "ClipperLib/RoVaclipper.hpp"
class CoasterPrintTexture;
class ExRSPolygon;
class wxFrame;

/* Coaster printing, aka 2.5D, aka image, A nice improvement would be to add
concentric infill as an option*/

class CoasterPrint :
	public iDrawable
{
private:
	double dpi;
	// In this case a polygon is an enclosed area of a single colour
	std::list < std::shared_ptr<ExRSPolygon> > polygons;
	double widthX, heightY;
	double zHeight;
	//4 layers
	std::shared_ptr<Layer> layerArray[4];//Solid layer, partial layer
	void generateSolidLayers();
	std::shared_ptr<Layer> *layerOutput = nullptr;
protected:
	void applyTransformations() override;
	void undoTransformations() override;
public:
	CoasterPrint();
	~CoasterPrint();

	virtual double getMinZ() const { return 0; }
	virtual double getMaxZ() const { return zHeight; }
	virtual double getMinX() const { return 0; }
	virtual double getMaxX() const { return widthX; }
	virtual double getMinY() const { return 0; }
	virtual double getMaxY() const { return heightY; }

	virtual void startSlice(wxFrame *baseFrame);
	bool loadImage(const std::string &filename);
	bool loadImage(const std::vector<unsigned char> &buffer, const std::string &name);
	// ------------------- iDrawable ------------------------------------------- //
	void draw(unsigned int opts, const Colour *colourToDraw) noexcept override;
	void moveTo(double x, double y, double z) override;

	//
	//SEL Jan 2020 add support to export all the polygons for use by the program BodySplitter
	//
	virtual void LogPerimeters();

	virtual void createSlice(unsigned int);
	virtual std::shared_ptr<Layer> getLayer(unsigned int id)const;
	virtual unsigned int getLayerCount()const;
	virtual unsigned int getLineCount() const;
	virtual void slicePerimeters(unsigned int layerID);
	virtual void fillInfill(unsigned int layerID, const std::vector<std::vector<RoVaClipperLib::IntPoint>> &fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic = false) override;
	virtual void createSurfaces(unsigned int layerID);
	virtual void completeLayer(unsigned int layerID);
	virtual void generateSupport();

	void updateTextureColour(uint16_t colour) override;

	// --------------------- iDrawable --------------------------//
	virtual void applyRotation(double zRotation) override;
	virtual void applyScale(double amt) override;
	virtual double getXExtent() const override;
	virtual double getYExtent()  const override;
	virtual void setMaxSize() override;

	bool hasTexture() const override
	{
		return true; // Coasters always have a texture
	}
	virtual void setOFfsetX(double newPos) override;
	virtual void setOFfsetY(double newPos) override;
	virtual void setOFfsetZ(double newPos) override;
	virtual void setToBed() override {}

	std::unordered_set<uint32_t> getColours() override;
	// --------------------- iSlicable ------------------------- //
	virtual double getOffsetX() const override;
	virtual double getOffsetY() const override;
	virtual double getOffsetZ() const override;
	RoVaClipperLib::Paths getPerimeters(unsigned int layerID) override;



	virtual double getWidth() const override {
		return widthX;
	}
	virtual double getHeight() const override {
		return heightY;
	}
	bool hasSupport() const override
	{
		return false;
	}
	static bool isValidExtension(const std::string &filePath);

	RoVaClipperLib::Paths getEnclosingPerimeters() const override;
	void generateSkirt() override
	{
		//TODO: thiss
	}

	std::set<uint16_t> getToolsUsed() {
		std::set<uint16_t> temp;
		temp.insert(0);//fullcolour
		return temp;
	}

	bool isColour() const {
		return true;
	}
};

