#pragma once
#include "iSlicable.h"
#include "iDrawable.h"
#include "Layer.h"
#include "ClipperLib/RoVaclipper.hpp"

class SupportGCodeVisualizer;

/*Responsible for slicing the support perimeters generated in mesh*/

class SupportManager
{
private:
	std::shared_ptr<SupportGCodeVisualizer> m_visualizerPtr;
	std::vector<std::vector<Lines>> m_sortedLinesPerLayer;
	std::vector<std::vector<Lines>> m_sortedInterfaceLinesPerLayer;
	std::vector<std::vector<Lines>> m_sortedBaseInterfaceLinesPerLayer;

	RoVaClipperLib::Paths totalExtentPath;

	RoVaClipperLib::Paths normalInfillPrototype;
	RoVaClipperLib::Paths interfaceInfillPrototype;
	std::vector<RoVaClipperLib::Paths> m_supportPerimters;

	std::vector<RoVaClipperLib::Paths> m_supportInterfacePolys;

	void addSupportPerimter(unsigned int layer, RoVaClipperLib::Paths &inPaths, double xOff, double yOff);
	void cutOutPerimeter(unsigned int layerID, const RoVaClipperLib::Paths &perimters, double xOff, double yOff);
	void generateSupportInterfaceLayers();

	void removeTopSupportLayers(unsigned int layerID);
	void removeBottomSupportLayers(unsigned int layerID);
	void cutOutInterfaceSupport(unsigned int layerID);

	void generateInfillPrototype(std::vector<RoVaClipperLib::Paths> &sourcePolys, double spacing, RoVaClipperLib::Paths &prototype, bool dir);

	void infillSupportPerimeters();
	void infillLayer(unsigned int layerID);
public:
	SupportManager();
	~SupportManager();

	void addObjects(std::list<std::shared_ptr<iDrawable>> objs);
	void prepareSupport();
	unsigned int getNonInterfaceLayerCount() const {
		return m_sortedLinesPerLayer.size();
	}
	const std::vector<Lines>& getSortedLinesPerLayer(unsigned int layerID) const {
		return m_sortedLinesPerLayer[layerID];
	}

	unsigned int getBaseMaterialInterfaceLayerCount() const {
		return m_sortedBaseInterfaceLinesPerLayer.size();
	}
	const std::vector<Lines>& getSortedBaseInterfaceLinesPerLayer(unsigned int layerID) const {
		return m_sortedBaseInterfaceLinesPerLayer[layerID];
	}

	unsigned int getInterfaceLayerCount() const {
		return m_sortedInterfaceLinesPerLayer.size();
	}
	const std::vector<Lines>& getSortedInterfaceLinesPerLayer(unsigned int layerID) const {
		return m_sortedInterfaceLinesPerLayer[layerID];
	}
	unsigned int getLayerCount() const {
		return std::max(m_sortedLinesPerLayer.size(), m_sortedInterfaceLinesPerLayer.size());
	}
	std::shared_ptr<SupportGCodeVisualizer> getVisualizer() {
		return m_visualizerPtr;
	}
};

