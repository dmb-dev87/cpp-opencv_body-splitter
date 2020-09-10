#pragma once
/* Does the actual slicing: convert stl to lines */
#include <wx/wx.h>	//Needed to update progress status bar - windows only i think
#include <memory>
#include "iSlicable.h"

class MainFrame;

const unsigned int STATUS_UPDATE_ID = 10000;


class Slicer
{
private:
	// 6 sets, non bridging fullx2, normal infillx2, bridging fulllx2
	RoVaClipperLib::Paths infillSources[6];
	std::shared_ptr<iSlicable> slicable;
	MainFrame *baseFrame;
	void slicePerimeters();

	void infillSlices();
	void completeSlice();
	void generateSurfaces();
	void infillSlice(unsigned int layerID);
	unsigned int sliceCount;
	void updateStatusBar(const std::string &text, int val = -1);

	void generateInfillLines(int type, iSlicable::ToolType tool);
public:
#ifndef _CONSOLE
	bool initSlicer(std::shared_ptr<iSlicable> target, MainFrame *baseFrame);
#else
	bool initSlicer(std::shared_ptr<iSlicable> target);
#endif
	void slice();
	Slicer();
	~Slicer();


};

