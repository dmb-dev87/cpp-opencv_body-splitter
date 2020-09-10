#include "PrimeTowerHandler.h"
#include "iSlicable.h"
#include "Layer.h"
#include <algorithm>
#include "Settings.h"

#define TOWER_RAD()  Settings::GetSingleton().getDoubleValue("primeTowerRadius")

PrimeTowerHandler::PrimeTowerHandler()
{

}


PrimeTowerHandler::~PrimeTowerHandler()
{
}

void PrimeTowerHandler::generateTowers(std::shared_ptr<iSlicable> &target)
{
	uint16_t maxColoursOnLayer = 0;
	if (target->getMinX() > 2 * TOWER_RAD())
		towerableSides[OBJ_LEFT] = (target->getMaxY() - target->getMinY()) / ((TOWER_RAD() + 2) * 2);
	if (target->getMinY() > 2 * TOWER_RAD())
		towerableSides[OBJ_BOTTOM] = 2 + (target->getMaxX() - target->getMinX()) / ((TOWER_RAD() + 2) * 2);
	if (target->getMaxX() < Settings::GetSingleton().getDoubleValue("maxX") - 2 * (TOWER_RAD()+2))
		towerableSides[OBJ_RIGHT] = (target->getMaxY() - target->getMinY()) / ((TOWER_RAD() + 2) * 2);
	if (target->getMaxY() < Settings::getSingleton().getDoubleValue("maxY") - 2 * (TOWER_RAD() + 2))
		towerableSides[OBJ_TOP] = 2 + (target->getMaxX() - target->getMinX()) / ((TOWER_RAD() + 2) * 2);

	if (!towerableSides[0] && !towerableSides[1] && !towerableSides[2] && !towerableSides[3])
	{
		//Cant find any prime towers...
#ifndef CONSOLE
		wxMessageBox("Cant fit any prime towers in the space available", "Error");
#endif
	}

	xTowers = 2 + (target->getMaxX() - target->getMinX()) / ((TOWER_RAD()+2)*2);
	yTowers = (target->getMaxY() - target->getMinY()) / ((TOWER_RAD()+2)*2);
	for (unsigned int i = 0; i < target->getLayerCount(); i++)
	{
		maxColoursOnLayer = std::max(maxColoursOnLayer, (uint16_t)target->getLayer(i)->getColourCount());
	}
	//Now we know how many prime towers we need in total
	//Create the prime towers
	std::string OutputMessage = "Prime Towers needed:" + std::to_string(maxColoursOnLayer);
#ifndef CONSOLE
	wxMessageBox(OutputMessage, "Info");
#endif

	for (int i = 0; i < maxColoursOnLayer; i++)
	{
		int side = 0;
		for (; side < 4; side++)
		{
			if (TowersPerSide[side].size() < towerableSides[side])
				break;
		}
		if (side == 4)
		{
#ifndef CONSOLE
			wxMessageBox("No room for the priming towers", "Erorr");
#endif
			return;
		}
		m_Towers.push_back(PrimeTower());
		PrimeTower *newTower = &m_Towers.back();
		TowersPerSide[side].push_back(newTower);
		if (side == OBJ_LEFT || side == OBJ_RIGHT)
		{
			if(side == OBJ_LEFT)
				newTower->setXPos(target->getMinX()-(2+(2*TOWER_RAD())));
			else
				newTower->setXPos(target->getMinX() + 2);
			newTower->setYPos(target->getMinY() + TowersPerSide[side].size() * (2 + TOWER_RAD() * 2));
		}
		else
		{
			if (side == OBJ_TOP)
				newTower->setYPos(target->getMaxY() + 2);
			else
				newTower->setYPos(target->getMinY() - 2 - 2 * TOWER_RAD());
			newTower->setXPos(target->getMinX() + TowersPerSide[side].size()*(2 + TOWER_RAD() * 2));
		}
	}
	//for (int i = 0; i < target->getLayerCount(); i++)
	//{
	//	std::list<PrimeTower>::iterator lastUsed = m_Towers.begin();
	//	for (auto it = target->getLayer(i)->colourList.begin(); it != target->getLayer(i)->colourList.end(); ++it)
	//	{
	//		lastUsed->setLayerColor(i, it->second);
	//		++lastUsed;
	//	}
	//}
	for (auto it = m_Towers.begin(); it != m_Towers.end(); ++it)
	{
		it->generateGCodeLines();
	}
}

void PrimeTowerHandler::addTower()
{
	//Find next open space
	int towerIndex = m_Towers.size() + 1;

	/* Index map
	E D C B A 9
	F         8
	G         7
	H         6  - YTowers
	0 1 2 3 4 5  - XTowers
	*/

}

void PrimeTowerHandler::reset()
{
	m_Towers.clear();
	for (int i = 0; i < 4; i++)
		TowersPerSide[i].clear();
}

void PrimeTower::setXPos(double x)
{
	xPos = x;
}

void PrimeTower::setYPos(double y)
{
	yPos = y;
}

void PrimeTower::setLayerColor(unsigned int layer, const Colour &col)
{
	colourLayerIndex.push_back(std::pair<unsigned int, Colour>(layer, col));
}

void PrimeTower::generateGCodeLines()
{
	for (unsigned int i = 0; i < colourLayerIndex.back().first; i++)
	{
		m_LinesPerLayer.push_back(std::vector<Line>());
		for (double r = TOWER_RAD(); r > 0; r -= 1)
		{
			Vertex lastPoint(xPos + 1 + TOWER_RAD() + sin(0)*r, yPos + 1 + TOWER_RAD() + cos(0)*r);
			for (double j = 0; j < 2.0*3.14159; j += 0.1*3.14159)
			{
				Vertex nextPoint(xPos + 1 + TOWER_RAD() + sin(j)*r,
					yPos + 1.0 + TOWER_RAD() + cos(j)*r);
				m_LinesPerLayer[i].push_back(Line(lastPoint, nextPoint,Line::LineType::PRIME_TOWER));
				m_LinesPerLayer[i].back().setColour(colourLayerIndex[i].second);
				lastPoint = nextPoint;
			}
			Vertex finalPoint(xPos + 1 + TOWER_RAD() + sin(0)*r, yPos + 1 + TOWER_RAD() + cos(0)*r);
			m_LinesPerLayer[i].push_back(Line(lastPoint, finalPoint, Line::LineType::PRIME_TOWER));
			m_LinesPerLayer[i].back().setColour(colourLayerIndex[i].second);
		}
	}
}