#pragma once
#include <unordered_set>
#include "colour.h"
#include "iSlicable.h"
#include "PrintBed.h"

/*Interface for objects that can be drawn on the bed, this and iSlicable should be merged*/

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;

class iDrawable : public iSlicable// ATM this interface exists for mesh and coasters
{

protected:
	std::shared_ptr<BodySplitterTexture> m_TexturePtr;
	bool m_bUseTexture = false;
	bool m_hideDrawing = false;
	Colour m_Colour;
	double m_lastAppliedOffset[3] = { 0.0,0.0,0.0 };
	double m_lastAppliedRotation[3] = { 0.0,0.0,0.0 };
	double m_lastAppliedScale[3] = { 1.0,1.0,1.0 };
	double m_Offset[3];
	double m_Rotation[3];
	double m_Scale[3];
	std::string drawableName;
public:
	iDrawable();
	virtual ~iDrawable() {};

	virtual void draw(unsigned int opts = 0,const Colour *colourToDraw = nullptr) noexcept = 0;
	
	// Result of a mouse release after drag
	virtual void moveTo(double x, double y, double z = 0) = 0;
	virtual void applyRotation(double zRotation) = 0;
	virtual void applyScale(double amt) = 0;

	//Needed to find the middle position for rotation/move/scale
	virtual double getXExtent() const = 0;
	virtual double getYExtent()  const = 0;

	virtual void setMaxSize() = 0;

	virtual double getOffsetX() const = 0;
	virtual double getOffsetY() const = 0;
	virtual double getOffsetZ() const = 0;
	virtual void setOFfsetX(double newPos) = 0;
	virtual void setOFfsetY(double newPos) = 0;
	virtual void setOFfsetZ(double newPos) = 0;
	virtual void setToBed() = 0;

	virtual double getXRotation() const {
		return m_Rotation[X_AXIS];
	}
	virtual void setXRotation(double x) {
		PrintBed::getSingleton().changesNeedSaving();
		//undoTransformations();
		m_Rotation[X_AXIS] = x;
		//applyTransformations();
	}
	virtual double getYRotation() const {
		return m_Rotation[Y_AXIS];
	}
	virtual void setYRotation(double y) {
		PrintBed::getSingleton().changesNeedSaving();
	//	undoTransformations();
		m_Rotation[Y_AXIS] = y;
		//applyTransformations();
	}
	virtual double getZRotation() const {
		return m_Rotation[Z_AXIS];
	}
	virtual void setZRotation(double z) {
		//undoTransformations();
		PrintBed::getSingleton().changesNeedSaving();
		m_Rotation[Z_AXIS] = z;
		//applyTransformations();
	}

	virtual double getXScale() const {
		return m_Scale[X_AXIS];
	}
	virtual double getYScale() const {
		return m_Scale[Y_AXIS];
	}
	virtual double getZScale() const {
		return m_Scale[Z_AXIS];
	}
	virtual void setXScale(double x) {
		PrintBed::getSingleton().changesNeedSaving();
		//undoTransformations();
		m_Scale[X_AXIS] = x;
		//applyTransformations();
	}
	virtual void setYScale(double y) {
		PrintBed::getSingleton().changesNeedSaving();
		//undoTransformations();
		m_Scale[Y_AXIS] = y;
		//applyTransformations();
	}
	virtual void setZScale(double z) {
		PrintBed::getSingleton().changesNeedSaving();
		//undoTransformations();
		m_Scale[Z_AXIS] = z;
		//applyTransformations();
	}

	struct DrawingOptions {
		static const unsigned int NONE = 0;
		static const unsigned int OBJECT_PICK = 1;
	};
	virtual bool hasTexture() const = 0;
	virtual bool isUsingTexture() const
	{
		return m_bUseTexture;
	}
	virtual Colour getColour() const
	{
		return m_Colour;
	}
	virtual void setUsingTexture(bool isTexture = true)
	{
		PrintBed::getSingleton().changesNeedSaving();
		m_bUseTexture = isTexture;
	}
	virtual void setColour(const Colour &newColour)
	{
		PrintBed::getSingleton().changesNeedSaving();
		m_Colour = newColour;
	}
	virtual const std::string getName() const
	{
		return drawableName;
	}
	virtual void setName(const std::string &name)
	{
		PrintBed::getSingleton().changesNeedSaving();
		drawableName = name;
	}

	std::shared_ptr<BodySplitterTexture> getBodySplitterTexture()
	{
		return m_TexturePtr;
	}

	virtual bool isHidden() const {
		return m_hideDrawing;
	}

	virtual void SetHidden(bool isHide = true) {
		m_hideDrawing = isHide;
	}

	virtual void updateTextureColour(uint16_t colours) = 0;

	virtual std::unordered_set<uint32_t> getColours() = 0;

	virtual void savePropertiesToStream(std::ostream &outStream) const;

	virtual void readPropertiesFromStream(std::istream &inStream);
};

