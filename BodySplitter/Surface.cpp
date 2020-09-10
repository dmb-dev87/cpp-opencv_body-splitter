#include "RSPolygon.h"
#include "Surface.h"
#include "Triangle.h"
#include "Perimeter.h"
#include "Settings.h"
#include "colour.h"

Surface::Surface(unsigned int layer) : layerID(layer)
{
}


void Surface::addTriangle(Triangle &inTri)
{
	if (inTri.parentSurface == nullptr)
	{
		inTri.parentSurface = this;
		m_Triangles.push_back(inTri);
		for (int i = 0; i < 3; i++)
		{
			if(inTri.touchingEdges[i])
				addTriangle(*inTri.touchingEdges[i]);
		}
	}
}

Surface::Surface(unsigned layerID, const std::vector<Line> &perim)
{
	Lines tempLines = perim;
	m_ptrSurfacePoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
	m_ptrSurfacePoly->setContour(Perimeter(tempLines));
}

void Surface::setColour(const Colour &in)
{
	m_ptrSurfacePoly->m_contour->setColour(in);
	//for (auto it : m_ptrSurfacePoly->m_infillPieces)
	//{
	//	it->setColour(in);
	//}
}

void Surface::generatePerimeters()
{
	Lines triangleSurfacePerims;
	for (uint i = 0; i < m_Triangles.size(); i++)
	{
		//Find unconnected edges
		for (uint j = 0; j < 3; j++)
		{
			if (m_Triangles[i].edgeTouchCount == 3)
				break;//no outside edges
			if (m_Triangles[i].touchingEdges[j] == nullptr)
			{
				triangleSurfacePerims.push_back(m_Triangles[i].getLine(j));
			}
		}
	}
	m_ptrSurfacePoly = std::make_shared<ExRSPolygon>(iSlicable::ToolType::FULL_COLOUR);
	m_ptrSurfacePoly->setContour(Perimeter(triangleSurfacePerims));
	for (uint i = 0; i < m_ptrSurfacePoly->m_contour->m_lines.size(); i++)
		m_ptrSurfacePoly->m_contour->m_lines[i].addParameter(Line::LineType::OUTERPERIM);

}
void Surface::infillSurface()
{
//	m_ptrSurfacePoly->generateInfill(45, true, Settings::GetSingleton().getDoubleValue("surfaceInfillDensity"), RoVaClipperLib::Paths(), false);
	/*
	for (uint i = 0; i < m_ptrSurfacePoly->m_infillPieces.size(); i++)
	{
		m_ptrSurfacePoly->m_infillPieces[i]->orderLines(m_ptrSurfacePoly->m_contour->m_lines);
	}
	colour tempColour = colour::InfillColour();
	for (uint i = 0; i < m_ptrSurfacePoly->m_infillPieces.size(); i++)
	{
		for (uint j = 0; j < m_ptrSurfacePoly->m_infillPieces[i]->fullInfillLinesSorted.size(); j++)
		{
			for (uint k = 0; k < m_ptrSurfacePoly->m_infillPieces[i]->fullInfillLinesSorted[j].size(); k++)
			{
				m_ptrSurfacePoly->m_infillPieces[i]->fullInfillLinesSorted[j][k].setColour(tempColour);
				m_ptrSurfacePoly->m_infillPieces[i]->fullInfillLinesSorted[j][k].addParameter(Line::LineType::SURFACE);
			}
		}
	}
	for (uint i = 0; i < m_ptrSurfacePoly->m_contour->m_lines.size(); i++)
	{
		m_ptrSurfacePoly->m_contour->m_lines[i].addParameter(Line::LineType::SURFACE);
	}
	*/
}

/*
void Surface::createTriangleInfill()
{
	//Get perimeters
	Lines triangleSurfacePerims;
	for (uint i = 0; i < triangles.size(); i++)
	{
		//Find unconnected edges
		for (uint j = 0; j < 3; j++)
		{
			if (triangles[i].edgeTouchCount == 3)
				break;//no outside edges
			if (triangles[i].touchingEdges[j] == nullptr)
			{
				triangleSurfacePerims.push_back(triangles[i].getLine(j));
			}
		}
	}

	if (triangleSurfacePerims.size() == 0)
		return;	//No outside edges on this surface, i dont know what this means, are there inside edges?
	while (triangleSurfacePerims.size() > 2)
	{
		m_lines.push_back(Perimeter(triangleSurfacePerims, true, false));
	}
	//Loses texture coords
	//std::vector<Perimeter> offsetPerims;
	//for (int i = 0; i < m_lines.size(); i++)
	//{
	//	std::vector<Lines> offsetOutput;
	//	offsetOutput = m_lines[i].offset(-Settings::GetSingleton().getDoubleValue("extrusionWidth") / 2);
	//	for (int j = 0; j < offsetOutput.size(); j++)
	//	{
	//		offsetPerims.push_back(Perimeter(offsetOutput[j]));
	//	}
	//}
	//m_lines = offsetPerims;
	//Cut existing infill to accomodate the surfaace
	//Existing infil is stored in the layer...
	//Find the largest
	if (m_lines.size() == 0)
		return;
	Perimeter* largest = &m_lines.front();
	bool foundLarger = true;
	while (foundLarger)
	{
		foundLarger = false;
		for (uint i = 0; i < m_lines.size(); i++)
		{
			if (m_lines[i].isBigger(*largest))
			{
				foundLarger = true;
				largest = &m_lines[i];
			}
		}
	}
	Infill *baseInfillPiece = new Infill(Settings::getSingleton().getDoubleValue("surfaceInfillDensity"), largest->width(), largest->height(), 45, largest->minX(), largest->minY(),false,RoVaClipperLib::Paths(),true);
	baseInfillPiece->cut(&largest->m_lines, false);
	//m_infillPieces.push_back(Infill(Settings::getSingleton().getDoubleValue("surfaceInfillDensity"), m_lines.back().width(), m_lines.back().height(), 45, m_lines.back().minX(), m_lines.back().minY()));
	//m_infillPieces.back().cut(&largest->m_lines, false);
	for (uint i = 0; i < m_lines.size(); i++)
	{
		if (m_lines[i].isWithin(*largest))
		{
			baseInfillPiece->cut(&m_lines[i].m_lines, true);
			//baseInfillPiece->orderLines(m_lines[i].m_lines);
		}
		else
		{
			//This is a sperate infill piece then...
			Perimeter* tempBigFill = &m_lines[i];
			Infill *smallerInfill = new Infill(Settings::getSingleton().getDoubleValue("surfaceInfillDensity"), tempBigFill->width(), tempBigFill->height(), 45, tempBigFill->minX(), tempBigFill->minY(),false,RoVaClipperLib::Paths(),true);
			smallerInfill->cut(&tempBigFill->m_lines, false);
			for (uint i = 0; i < m_lines.size(); i++)
			{
				if (m_lines[i].isWithin(*tempBigFill))
				{
					smallerInfill->cut(&m_lines[i].m_lines, true);
				}
			}
			//smallerInfill->orderLines(tempBigFill->m_lines);
			m_infillPieces.push_back(*smallerInfill);
			delete smallerInfill;
		}
	}
	m_infillPieces.push_back(*baseInfillPiece);
	delete baseInfillPiece;
}

*/