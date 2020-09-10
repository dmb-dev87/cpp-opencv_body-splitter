#include "TestPrint.h"
#include "Settings.h"
#include "Line.h"
#include "Layer.h"
#include "RSPolygon.h"
#include "Infill.h"


TestPrint::TestPrint(testType type, double xSize, double ySize) : xWidth(xSize), yHeight(ySize)
{

}


TestPrint::~TestPrint()
{
}

ClosingCircle::ClosingCircle(double x, double y, double z, unsigned int facetCount) : 
	TestPrint(testType::CLOSING_CIRCLE,x,y), zHeight(z), facets(facetCount)
{

}

ClosingCircle::~ClosingCircle()
{
	if (m_ptrLayerArray)
		delete[] m_ptrLayerArray;
}

void ClosingCircle::startSlice()
{
	layerCount = zHeight / Settings::getSingleton().getDoubleValue("layerThickness") + 1;
	if (m_ptrLayerArray)
		delete[] m_ptrLayerArray;
	m_ptrLayerArray = new std::shared_ptr<Layer>[layerCount];
}

void ClosingCircle::slicePerimeters(unsigned int layerID)
{
	//Layer 1 is just a circle with rect infill
	double circleCentre = xWidth / 2.0;
	if (layerID == 0)
	{
		
		Lines perimLines;
		Vertex pt1(cos(0)*circleCentre + circleCentre, sin(0)*circleCentre + circleCentre);
		for (double angle = (2.0*PI) / (double)facets; angle < 2.0 * PI; angle += (2.0*PI) / (double)facets)
		{
			Vertex pt2(cos(angle)*circleCentre + circleCentre, sin(angle)*circleCentre + circleCentre);
			perimLines.push_back(Line(pt1, pt2, Line::LineType::OUTERPERIM));
			pt1 = pt2;
		}
		m_ptrLayerArray[0] = std::make_shared<Layer>(Settings::getSingleton().getDoubleValue("layerThickness"),0,iSlicable::ToolType::FULL_COLOUR);
		std::shared_ptr<ExRSPolygon> rsPoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
		perimLines.push_back(Line(perimLines.back().pt2, perimLines.front().pt1, Line::LineType::OUTERPERIM));
		Line::addParamToSet(perimLines, Line::LineType::BOTTOM_LAYER);
		Perimeter newPerim(perimLines);
		rsPoly->setContour(newPerim);
		m_ptrLayerArray[0]->addRSPoly(rsPoly);
	}
	else
	{
		std::shared_ptr<ExRSPolygon> rsPoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
		//double shrinkage = Settings::GetSingleton().getDoubleValue("nozzleD");
		double shrinkage = Settings::GetSingleton().getDoubleValue("extrusionWidth");
		double rad = xWidth / 2.0;

		m_ptrLayerArray[layerID] = std::make_shared<Layer>(Settings::getSingleton().getDoubleValue("layerThickness")*(layerID + 1), layerID, iSlicable::ToolType::FULL_COLOUR);

		for (; rad > shrinkage; rad -= shrinkage)
		{
			Vertex pt1(cos(0)*rad + circleCentre, sin(0)*rad + circleCentre);

			Lines perimLines;

			for (double angle = (2.0*PI) / (double)facets; angle < 2.0 * PI; angle += (2.0*PI) / (double)facets)
			{
				Vertex pt2(cos(angle)*rad + circleCentre, sin(angle)*rad + circleCentre);
				perimLines.push_back(Line(pt1, pt2, Line::LineType::OUTERPERIM));
				pt1 = pt2;
			}

			perimLines.push_back(Line(perimLines.back().pt2, perimLines.front().pt1, Line::LineType::PERIM));
			//Line::addParamToSet(perimLines, Line::LineType::BOTTOM_LAYER);
			//Perimeter newPerim(perimLines);

			rsPoly->basicLinePaths.push_back(perimLines);
		}
		m_ptrLayerArray[layerID]->addRSPoly(rsPoly);
	}
}



void ClosingCircle::fillInfill(unsigned int layerID, std::vector<std::vector<RoVaClipperLib::IntPoint>> &fullInfill, bool skipTopLogic)
{
	if (layerID == 0)
	{
//		m_ptrLayerArray[0]->InfillPerimeters(100, fullInfill, true);

	}
	//for (auto it = m_ptrLayerArray[layerID]->m_layerPolys.begin(); it != m_ptrLayerArray[layerID]->m_layerPolys.end(); ++it)
	//{
	//	for (uint i = 0; i < (*it)->m_infillPieces.size(); i++)
	//	{
	//		(*it)->m_infillPieces[i]->orderLines((*it)->m_contour->m_lines);
	//	}
	//}
}

void ClosingCircle::completeLayer(unsigned int layerID)
{

}