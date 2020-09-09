#include "SupportManager.h"

#include "Mesh.h"
#include "iDrawable.h"
#include "Settings.h"
#include "Infill.h"
#include "SupportGCodeVisualizer.h"


SupportManager::SupportManager()
{
}


SupportManager::~SupportManager()
{
}

void SupportManager::addObjects(std::list<std::shared_ptr<iDrawable>> objs)
{
	// Add support polygons (i hope these exists)
	for (auto it : objs)
	{
		auto mesh_ptr = std::dynamic_pointer_cast<Mesh>(it);
		if (mesh_ptr && mesh_ptr->hasSupport())
		{
			for (unsigned int i = 0; i < mesh_ptr->getLayerCount(); i++)
			{
				RoVaClipperLib::Paths tempPath = mesh_ptr->getLayer(i)->getSupportPoly();
				addSupportPerimter(i, tempPath, mesh_ptr->getOffsetX(), mesh_ptr->getOffsetY());
			}
		}
	}
	for (auto it : objs)
	{
		auto mesh_ptr = std::dynamic_pointer_cast<iSlicable>(it);
		if (mesh_ptr)
		{
			for (unsigned int i = 0; i < mesh_ptr->getLayerCount(); ++i)
			{
				cutOutPerimeter(i, mesh_ptr->getPerimeters(i), mesh_ptr->getOffsetX(), mesh_ptr->getOffsetY());
			}
		}
	}
	// If we are doing a percise cut, union this perim now
	if (Settings::GetSingletonPtr()->getBoolValue("preciseSupportCut"))
	{
		for (auto it : objs)
		{
			for (int i = 0; i < it->getLayerCount(); i++)
			{
				RoVaClipperLib::Clipper precise;
				RoVaClipperLib::Paths path = it->getPerimeters(i);
				RoVaClipperLib::translatePath(path, it->getOffsetX()*DEDOUBLER, it->getOffsetY()*DEDOUBLER);
				precise.AddPaths(path, RoVaClipperLib::ptSubject, true);
				precise.AddPaths(totalExtentPath, RoVaClipperLib::ptClip, true);
				precise.Execute(RoVaClipperLib::ctUnion, totalExtentPath, RoVaClipperLib::pftNonZero);
			}
		}
	}
}

void SupportManager::addSupportPerimter(unsigned int layerID, RoVaClipperLib::Paths &path, double xOff, double yOff)
{
	if (m_supportPerimters.size() <= layerID)
		m_supportPerimters.resize(layerID + 1);
	RoVaClipperLib::translatePath(path, xOff*DEDOUBLER, yOff*DEDOUBLER);
	RoVaClipperLib::Clipper clipper;
	clipper.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::ptSubject, true);
	clipper.AddPaths(path, RoVaClipperLib::ptClip, true);
	clipper.Execute(RoVaClipperLib::ctUnion, m_supportPerimters[layerID]);
}

void SupportManager::cutOutPerimeter(unsigned int layerID,const RoVaClipperLib::Paths &inPath, double xOff, double yOff)
{
	if (layerID > m_supportPerimters.size())
		return;
	if (m_supportPerimters[layerID].empty())
		return;
	RoVaClipperLib::Paths path(inPath);
	for (RoVaClipperLib::Paths::iterator pathIterator = path.begin();;)
	{
		if (fabs(RoVaClipperLib::Area((*pathIterator))) < GET_DOUBLE("minimumHoleAreaSupport")*DEDOUBLER)
		{
			pathIterator = path.erase(pathIterator);
		}
		else
			++pathIterator;
		if (pathIterator == path.end())
			break;
	}
	RoVaClipperLib::translatePath(path, xOff*DEDOUBLER, yOff*DEDOUBLER);

	// Offset the cut lines based on "supportOffsetFromPart" mm
	RoVaClipperLib::ClipperOffset offsetter;
	offsetter.AddPaths(path, RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
	RoVaClipperLib::Paths offsetPath;
	offsetter.Execute(offsetPath, Settings::GetSingleton().getDoubleValue("supportOffsetFromPart")*DEDOUBLER);
	RoVaClipperLib::Clipper clipper;
	clipper.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::ptSubject, true);
	clipper.AddPaths(offsetPath, RoVaClipperLib::ptClip, true);
	clipper.Execute(RoVaClipperLib::ctDifference, m_supportPerimters[layerID]);
}


void SupportManager::generateSupportInterfaceLayers()
{
	//First remove all empty top layers - Start at bottom
	m_supportInterfacePolys.resize(m_supportPerimters.size());
	if (Settings::GetSingleton().getDoubleValue("emptySupportLayers") > 0 || GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers") > 0)
	{
		for (unsigned int layer = 0; layer < m_supportPerimters.size(); ++layer)
		{
			removeTopSupportLayers(layer);
		}

		for (unsigned int layer = m_supportPerimters.size() - 1; layer > 0; --layer)
		{
			removeBottomSupportLayers(layer);
		}
		for (unsigned int layer = 0; layer < m_supportPerimters.size(); ++layer)
			cutOutInterfaceSupport(layer);
	}
}

void SupportManager::removeTopSupportLayers(unsigned int layerID)
{
	// If this entire layer is gaurenteed to be removed
	if (m_supportPerimters.size() - layerID <= Settings::GetSingleton().getDoubleValue("emptySupportLayers") + 1)
		m_supportPerimters[layerID].clear();
	//clipper AND this support poly with the next layer, carry result to next layer
	for (int i = 1; i < Settings::GetSingleton().getDoubleValue("emptySupportLayers") + 1; i++)
	{
		if (layerID + i >= m_supportPerimters.size())
			break;
		RoVaClipperLib::Clipper andResult;
		andResult.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::ptSubject, true);
		andResult.AddPaths(m_supportPerimters[layerID + i], RoVaClipperLib::ptClip, true);
		m_supportPerimters[layerID].clear();
		andResult.Execute(RoVaClipperLib::ctIntersection, m_supportPerimters[layerID]);
	}
	if (GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers") > 0 &&
		!m_supportPerimters[layerID].empty())
	{
		// layerNumber+1 is guarenteed to be less than the array length; see start of fuinction
		RoVaClipperLib::Paths nonInterface = m_supportPerimters[layerID + 1];
		for (int i = 2; i < GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers") + 1; i++)
		{
			if (layerID + i >= m_supportPerimters.size())
				break;
			RoVaClipperLib::Clipper andResult;
			andResult.AddPaths(nonInterface, RoVaClipperLib::ptSubject, true);
			andResult.AddPaths(m_supportPerimters[layerID + i], RoVaClipperLib::ptClip, true);
			andResult.Execute(RoVaClipperLib::ctIntersection, nonInterface);
			if (m_supportInterfacePolys[layerID].empty() == false)
			{
				wxLogDebug("Started creating interface layer");
			}
		}
		RoVaClipperLib::Clipper interfacer;
		interfacer.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::ptSubject, true);
		interfacer.AddPaths(nonInterface, RoVaClipperLib::ptClip, true);
		RoVaClipperLib::Paths temp;
		interfacer.Execute(RoVaClipperLib::ctDifference, temp);
		interfacer.Clear();
		interfacer.AddPaths(m_supportInterfacePolys[layerID], RoVaClipperLib::ptSubject, true);
		interfacer.AddPaths(temp, RoVaClipperLib::ptClip, true);
		interfacer.Execute(RoVaClipperLib::ctUnion, m_supportInterfacePolys[layerID], RoVaClipperLib::pftNonZero);
	}
}

void SupportManager::removeBottomSupportLayers(unsigned int layerNumber)
{
	//Dont remove bottom layers(that is layers that touch the build plate)
	if (Settings::GetSingleton().getDoubleValue("emptySupportLayers") > 0)
	{
		if (layerNumber > Settings::GetSingleton().getDoubleValue("emptySupportLayers"))
		{
			for (int i = 1; i < Settings::GetSingleton().getDoubleValue("emptySupportLayers") + 1; ++i)
			{
				if (layerNumber - i <= Settings::GetSingleton().getDoubleValue("emptySupportLayers"))
					break;

				RoVaClipperLib::Clipper andResult;
				andResult.AddPaths(m_supportPerimters[layerNumber], RoVaClipperLib::ptSubject, true);
				andResult.AddPaths(m_supportPerimters[layerNumber - i], RoVaClipperLib::ptClip, true);
				m_supportPerimters[layerNumber].clear();
				andResult.Execute(RoVaClipperLib::ctIntersection, m_supportPerimters[layerNumber]);
			}
		}
	}

	if (GET_DOUBLE("supportInterfaceLayers")+GET_DOUBLE("baseMaterialInterfaceLayers") > 0 &&
		!m_supportPerimters[layerNumber].empty())
	{
		if (layerNumber <= GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers"))
			return;
		// layerNumber+1 is guarenteed to be less than the array length; see start of fuinction
		RoVaClipperLib::Paths nonInterface = m_supportPerimters[layerNumber - 1];
		for (int i = 1; i < GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers") + 1; i++)
		{
			if (layerNumber - i <= GET_DOUBLE("supportInterfaceLayers") + GET_DOUBLE("baseMaterialInterfaceLayers"))
				break;
			RoVaClipperLib::Clipper andResult;
			andResult.AddPaths(nonInterface, RoVaClipperLib::ptSubject, true);
			//andResult.AddPaths(m_supportInterfacePoly, RoVaClipperLib::ptSubject, true);
			//andResult.AddPaths(layers[layerNumber - i]->m_supportInterfacePoly, RoVaClipperLib::ptClip, true);
			andResult.AddPaths(m_supportPerimters[layerNumber - i], RoVaClipperLib::ptClip, true);
			andResult.Execute(RoVaClipperLib::ctIntersection, nonInterface);
		}
		RoVaClipperLib::Clipper interfacer;
		interfacer.AddPaths(m_supportPerimters[layerNumber], RoVaClipperLib::ptSubject, true);
		interfacer.AddPaths(nonInterface, RoVaClipperLib::ptClip, true);
		RoVaClipperLib::Paths temp;
		interfacer.Execute(RoVaClipperLib::ctDifference, temp);
		interfacer.Clear();
		interfacer.AddPaths(m_supportInterfacePolys[layerNumber], RoVaClipperLib::ptSubject, true);
		interfacer.AddPaths(temp, RoVaClipperLib::ptClip, true);
		interfacer.Execute(RoVaClipperLib::ctUnion, m_supportInterfacePolys[layerNumber], RoVaClipperLib::pftNonZero);
	}
}

void SupportManager::cutOutInterfaceSupport(unsigned int layerID)
{
	RoVaClipperLib::Clipper cutout;
	cutout.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::ptSubject, true);
	cutout.AddPaths(m_supportInterfacePolys[layerID], RoVaClipperLib::ptClip, true);
	cutout.Execute(RoVaClipperLib::ctDifference, m_supportPerimters[layerID]);
}

void SupportManager::prepareSupport()
{
	generateSupportInterfaceLayers();
	// ---Generate prototypes
		double spacing = (Settings::getSingleton().getDoubleValue("extrusionWidth")) / (GET_DOUBLE("supportInfillDensity") / 100.0);
		generateInfillPrototype(m_supportPerimters, spacing, normalInfillPrototype,false);
		spacing = (Settings::getSingleton().getDoubleValue("extrusionWidth")) / (GET_DOUBLE("supportInfillDensityFull") / 100.0);
		generateInfillPrototype(m_supportInterfacePolys, spacing, interfaceInfillPrototype,true);
	// ---
	infillSupportPerimeters();

	m_visualizerPtr = std::make_shared<SupportGCodeVisualizer>();
}

void SupportManager::generateInfillPrototype(std::vector<RoVaClipperLib::Paths> &sourcePolys, double spacing, RoVaClipperLib::Paths &prototype, bool dir)
{
	//Reg Support poly
	RoVaClipperLib::Clipper joiner;
	RoVaClipperLib::Paths ultimateOutline;
	if (Settings::GetSingleton().getBoolValue("preciseSupportCut"))
	{
		ultimateOutline = totalExtentPath;
	}
	else
	{
		joiner.AddPaths(sourcePolys[0], RoVaClipperLib::ptSubject, true);
		for (int i = 1; i < sourcePolys.size(); i++)
			joiner.AddPaths(sourcePolys[i], RoVaClipperLib::ptClip, true);
		joiner.Execute(RoVaClipperLib::ctUnion, ultimateOutline, RoVaClipperLib::pftNonZero);
	}
	auto bottomLeft = RoVaClipperLib::getMinPoint(ultimateOutline);
	auto topRight = RoVaClipperLib::getMaxPoint(ultimateOutline);
	//Generate lines fitting these bounds
	Lines temp;
	if(dir == false)
		Line::generateLines(temp, spacing, bottomLeft.X/DEDOUBLER, topRight.X/DEDOUBLER, bottomLeft.Y/DEDOUBLER, topRight.Y/DEDOUBLER);
	else
		Line::generateLinesB(temp, spacing, bottomLeft.X / DEDOUBLER, topRight.X / DEDOUBLER, bottomLeft.Y / DEDOUBLER, topRight.Y / DEDOUBLER);
	RoVaClipperLib::Paths source;
	for (auto lineIT : temp)
	{
		RoVaClipperLib::Path newPath;
		newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt1.x*DEDOUBLER, lineIT.pt1.y*DEDOUBLER));
		newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt2.x*DEDOUBLER, lineIT.pt2.y*DEDOUBLER));
		source.push_back(newPath);
	}

	RoVaClipperLib::Clipper protoCut;
	protoCut.AddPaths(source, RoVaClipperLib::ptSubject, false);
	protoCut.AddPaths(ultimateOutline, RoVaClipperLib::ptClip, true);
	RoVaClipperLib::PolyTree out;
	protoCut.Execute(RoVaClipperLib::ctIntersection, out);
	RoVaClipperLib::PolyTreeToPaths(out, prototype);
}

void SupportManager::infillSupportPerimeters()
{
	unsigned int topLayer = std::max(m_supportPerimters.size(), m_supportInterfacePolys.size());
	for (unsigned int i = 0; i < topLayer; i++)
	{
		infillLayer(i);
	}
}

void SupportManager::infillLayer(unsigned int layerID)
{
	if (layerID < m_supportPerimters.size() && !m_supportPerimters[layerID].empty())
	{
		if(m_sortedLinesPerLayer.size()<=layerID)
			m_sortedLinesPerLayer.resize(layerID+1);
		Infill supportIsland(0.0, &normalInfillPrototype, RoVaClipperLib::Paths(), true, false);
		supportIsland.cut(m_supportPerimters[layerID]);
		//Inflate the support poly so the infill lines dont touch it?
		RoVaClipperLib::ClipperOffset offsetter;
		RoVaClipperLib::Paths inflateSupportPoly;
		offsetter.AddPaths(m_supportPerimters[layerID], RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
		//This positive offset is needed to ensure the towers join and dont create overlapping infill lines
		offsetter.Execute(inflateSupportPoly, 1);
		supportIsland.orderLines(inflateSupportPoly, true);
		m_sortedLinesPerLayer[layerID] = supportIsland.sortedLines;

		Colour supportColour;
		int interfaceTool = Settings::getSingleton().getDoubleValue("mainSupportExtruder");
		if (interfaceTool == 0)
			supportColour = Settings::getSingleton().getColourValue("supportColour");
		else if (interfaceTool == 1)
			supportColour = Settings::getSingleton().getColourValue("t1Colour");
		else if (interfaceTool == 2)
			supportColour = Settings::getSingleton().getColourValue("t2Colour");
		for (auto &lineSetIT : m_sortedLinesPerLayer[layerID])
			Line::setLineSetToColour(lineSetIT, supportColour);
	}
	//Fix this copy and paste
	if (layerID < m_supportInterfacePolys.size() && !m_supportInterfacePolys[layerID].empty())
	{
		if (m_sortedInterfaceLinesPerLayer.size() <= layerID)
			m_sortedInterfaceLinesPerLayer.resize(layerID + 1);
		Infill supportIsland(0.0, &interfaceInfillPrototype, RoVaClipperLib::Paths(), true, false);
		supportIsland.cut(m_supportInterfacePolys[layerID]);
		//Inflate the support poly so the infill lines dont touch it?
		RoVaClipperLib::ClipperOffset offsetter;
		RoVaClipperLib::Paths inflateSupportPoly;
		offsetter.AddPaths(m_supportInterfacePolys[layerID], RoVaClipperLib::jtMiter, RoVaClipperLib::etClosedPolygon);
		//This positive offset is needed to ensure the towers join and dont create overlapping infill lines
		offsetter.Execute(inflateSupportPoly, 1);
		supportIsland.orderLines(inflateSupportPoly, true);
		m_sortedInterfaceLinesPerLayer[layerID] = supportIsland.sortedLines;

		Colour supportColour;
		int interfaceTool = Settings::getSingleton().getDoubleValue("supportInterfaceExtruder");
		if (interfaceTool == 0)
			supportColour = Settings::getSingleton().getColourValue("supportColour");
		else if (interfaceTool == 1)
			supportColour = Settings::getSingleton().getColourValue("t1Colour");
		else if (interfaceTool == 2)
			supportColour = Settings::getSingleton().getColourValue("t2Colour");
		for (auto &lineSetIT : m_sortedInterfaceLinesPerLayer[layerID])
			Line::setLineSetToColour(lineSetIT, supportColour);
	}
}