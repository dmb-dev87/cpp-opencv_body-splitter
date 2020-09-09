#pragma once
#include "Mesh.h"

class SupportTower;
class Mesh;
class csgjs_model;
class SupportMesh :
	public Mesh
{
private:
	std::shared_ptr<csgjs_model> m_ptrCSGModel;
	std::vector<std::shared_ptr<SupportTower>> m_supportTowers;
	void convertTowersToCSGMesh();
	static csgjs_model towerToCSGMesh(std::shared_ptr<SupportTower> sTower);
	static csgjs_model meshToCSGMesh(const Mesh* mesh);
	void regenerateNormals();
	virtual void undoTransformations() override
	{

	}
	virtual void applyTransformations() override
	{

	}
	void createVBOS() {

	}
public:
	void fillInfill(unsigned int layerID, const RoVaClipperLib::Paths &fullInfill, const RoVaClipperLib::Paths *sources, bool skipTopLogic);
	void addSupportTower(std::shared_ptr<SupportTower> sTower);
	bool isSupportCut() const;
	//void clearSupportData();
	void removeSupport(std::shared_ptr<SupportTower> sTower);
	void cutOutMesh(Mesh* mesh);
	SupportMesh();
	~SupportMesh();

	virtual void createArrayData(bool moveOrigin)
	{

	}

	void draw(unsigned int opts, const Colour *colourToDraw) noexcept override;

	virtual double getMinZ() const override;
	virtual double getMaxZ() const override;
	virtual double getMinX() const override;
	virtual double getMaxX() const override;
	virtual double getMinY() const override;
	virtual double getMaxY() const override;

	//virtual double getOffsetX() const override;
	//virtual double getOffsetY() const override;
	//virtual double getOffsetZ() const override;

	virtual double getWidth() const override;	//X
	virtual double getHeight() const override;	//Y
											// Builds and returns the 2d projection to the xy plane - used for slicing infill/skirts/brim
	virtual void generateSkirt() override;
	virtual bool isColour() const override {
		return false;
	}
	bool hasTexture() const override {
		return false;
	}
	void updateTextureColour(uint16_t colours) override {

	}
	virtual std::unordered_set<uint32_t> getColours() override {
		return std::unordered_set<uint32_t>();
	}
	

	// Result of a mouse release after drag
	virtual void moveTo(double x, double y, double z = 0) override {};
	virtual void applyRotation(double zRotation) override {};
	virtual void applyScale(double amt) override {};

	//Needed to find the middle position for rotation/move/scale
	virtual double getXExtent() const override { return 0; };
	virtual double getYExtent()  const override { return 0; };

	virtual void setMaxSize() override {};


	//virtual void setOFfsetX(double newPos) override{};
	//virtual void setOFfsetY(double newPos) override{};
	//virtual void setOFfsetZ(double newPos) override{};
	virtual void setToBed() override {};
};

