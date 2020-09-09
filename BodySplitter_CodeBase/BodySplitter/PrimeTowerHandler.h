#pragma once
/* Deprecated: as we dropped support for anything other than the R4 printer */
#include <memory>
#include <vector>
#include <list>
#include "Singleton.h"
#include "colour.h"
#include "Line.h"

class iSlicable;

class PrimeTower
{
public:
	void setXPos(double);
	void setYPos(double);
	std::vector<std::vector<Line>> m_LinesPerLayer;
private:
	friend class PrimeTowerHandler;
	double xPos, yPos;
	unsigned int layerCount;
	std::vector < std::pair<unsigned int, Colour>> colourLayerIndex;
	void stopAtLayer(unsigned int layer);
	void setLayerColor(unsigned int layer, const Colour &col);
	void generateGCodeLines();
};

class PrimeTowerHandler : public Singleton<PrimeTowerHandler>
{
private:
	const int OBJ_BOTTOM = 0;
	const int OBJ_LEFT = 1;
	const int OBJ_RIGHT = 2;
	const int OBJ_TOP = 3;
	uint16_t towerableSides[4] = { 0,0,0,0 };
	uint16_t xTowers;
	uint16_t yTowers;
	void addTower();
	std::vector<PrimeTower*> TowersPerSide[4];
	
public:
	std::list<PrimeTower> m_Towers;
	void generateTowers(std::shared_ptr<iSlicable> &target);
	void reset();
	PrimeTowerHandler();
	~PrimeTowerHandler();
};

