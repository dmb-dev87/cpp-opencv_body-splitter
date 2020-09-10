#include "Layer.h"
#include "Slicer.h"
#include "Settings.h"
#include <thread>
#include "ThreadPool/ThreadPool.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include "MainFrame.h"
#include "PrintBed.h"
#include "gCodeControlWindow.h"

Slicer::Slicer()
{
}


Slicer::~Slicer()
{
}

#ifndef _CONSOLE
bool Slicer::initSlicer(std::shared_ptr<iSlicable> slice, MainFrame* statusbar)
#else
bool Slicer::initSlicer(std::shared_ptr<iSlicable> slice)
#endif
{
	slicable = slice;
#ifndef _CONSOLE
	baseFrame = statusbar;
#else	// freelancer latino
	baseFrame = NULL; // freelancer latino
#endif
	updateStatusBar("Begin Slice");
	return true;
}

void Slicer::slice()
{
	long long startTime = wxGetUTCTimeMillis().GetValue();
	if (slicable)
	{
		slicable->startSlice(baseFrame);
		sliceCount = slicable->getLayerCount();//(slicable->getMaxZ() - slicable->getMinZ()) / 
		auto logger = spdlog::get("DEBUG_FILE");
		logger->info("Slice begin, Layers: " + std::to_string(sliceCount));
		for (unsigned int i = 0; i < sliceCount; i++)
		{
			slicable->createSlice(i);
		}
#ifdef CONSOLE
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Slicing perimeters\n";
#endif
		this->slicePerimeters();

		//
		//SEL Jan 2020   I believe this is the correct location to be able to export all the polygons for all the layers for the BodySplitter
		// I think I will add a Mesh::LogPolygons member functiton  and call it like this: slicable->LogPerimeters in which I will use the
		// member function getPerimeters
		slicable->LogPerimeters();
		if (Settings::getSingleton().getBoolValue("exportPolygons"))
		{
			goto SKIP_REST_IF_EXPORTING_POLYGONS;
		}
		//SEL done

		if (Settings::getSingleton().getDoubleValue("skirtLoops") > 0)
			slicable->generateSkirt();
#ifdef CONSOLE
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Slicing Surfaces\n";
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Slicing infill\n";
#endif
		if (Settings::getSingleton().getBoolValue("useInfill") && Settings::getSingleton().getDoubleValue("infillDensity")>=1.0)
			this->infillSlices();
		slicable->makeSliced();
		this->completeSlice();

SKIP_REST_IF_EXPORTING_POLYGONS:
		startTime = wxGetUTCTimeMillis().GetValue() - startTime;
		logger->info("Slice complete, time taken: " + std::to_string(startTime / (1000)) + " seconds");
#ifdef CONSOLE
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Slice complete\n";
#endif
	}
}

void Slicer::slicePerimeters()
{
	std::string progressText;
#ifndef NDEBUG
	ThreadPool threads(1);
#else
	ThreadPool threads(Settings::GetSingleton().getDoubleValue("threadCount"));
#endif
	for (unsigned int i = 0; i < sliceCount; i++)
	{
		threads.enqueue(&iSlicable::slicePerimeters, slicable, i);
	}
	
	using namespace std::chrono_literals;
	while (threads.queueSize() > 0)
	{
		std::this_thread::sleep_for(1s);
		int i = sliceCount - threads.queueSize();
		progressText = "Generating Perimeters: " + std::to_string((int)((float)i / (float)sliceCount * 100)) + "%";
		updateStatusBar(progressText, (int)((float)i / (float)sliceCount * 100));
		//baseFrame->SetStatusText(progressText);
	}
}

void Slicer::infillSlices()
{
	std::string progressText;
	//Generate infill lines
	for (int i = 0; i < 6; ++i)
		generateInfillLines(i, slicable->getTool());
#ifndef NDEBUG
	ThreadPool threads(1);
#else
	ThreadPool threads(Settings::GetSingleton().getDoubleValue("threadCount"));
#endif
	for (unsigned int i = 0; i < sliceCount; i ++)
	{
		threads.enqueue(&Slicer::infillSlice, this, i);
	}
	using namespace std::chrono_literals;
	while (threads.queueSize() > 0)
	{
		std::this_thread::sleep_for(1s);
		int i = sliceCount - threads.queueSize();
		progressText = "Generating Infill: " + std::to_string((int)((float)i / (float)sliceCount * 100)) + "%";
		updateStatusBar(progressText, (int)((float)i / (float)sliceCount * 100));
	}
}

void Slicer::infillSlice(unsigned int layerID)
{
	
	//if (Settings::getSingleton().getDoubleValue("topSolidLayers")>=1 || Settings::GetSingleton().getDoubleValue("bottomSolidLayers") >= 1)
	{
		if (layerID + Settings::GetSingleton().getDoubleValue("topSolidLayers") + 1 > sliceCount ||
			layerID < Settings::GetSingleton().getDoubleValue("bottomSolidLayers")) //Layers above this or below are all full infill
			slicable->fillInfill(layerID, RoVaClipperLib::Paths(), infillSources);	//Blank path makes for full infill
		else //Partial full infll
		{
			RoVaClipperLib::Paths not100(slicable->getLayer(layerID)->clipperlib_perimeters);
			RoVaClipperLib::Clipper clip;
			//Top layers
			for (unsigned int j = 1; j < (Settings::getSingleton().getDoubleValue("topSolidLayers") + 1); j++)
			{
				RoVaClipperLib::Clipper layerClip;
				layerClip.AddPaths(not100, RoVaClipperLib::ptSubject, true);
				layerClip.AddPaths(slicable->getLayer(layerID + j)->clipperlib_perimeters, RoVaClipperLib::ptClip, true);
				layerClip.Execute(RoVaClipperLib::ctIntersection, not100);
				//clip.AddPaths(not100, RoVaClipperLib::ptSubject, true);
			}
			//Bottom layers
			for (unsigned int j = 1; j < (Settings::getSingleton().getDoubleValue("bottomSolidLayers") + 1); j++)
			{
				RoVaClipperLib::Clipper layerClip;
				layerClip.AddPaths(not100, RoVaClipperLib::ptSubject, true);
				layerClip.AddPaths(slicable->getLayer(layerID - j)->clipperlib_perimeters, RoVaClipperLib::ptClip, true);
				layerClip.Execute(RoVaClipperLib::ctIntersection, not100);
			}
			slicable->fillInfill(layerID, not100, infillSources);
		}
	}
	//else
		//slicable->fillInfill(layerID, slicable->getLayer(layerID)->clipperlib_perimeters, infillSources, true);
}


void Slicer::generateSurfaces()
{
	std::string progressText;
	
	for (unsigned int i = 0; i < sliceCount; i++)
	{
		slicable->createSurfaces(i);
		progressText = "Generating Surfaces: " + std::to_string((int)((float)i / (float)sliceCount * 100)) + "%";
		updateStatusBar(progressText, (int)((float)i / (float)sliceCount * 100));
	}
}

void Slicer::completeSlice()
{	
	for (unsigned int i = 0; i < sliceCount; i++)
	{
		slicable->completeLayer(i);
	}
#ifndef CONSOLE
	PrintBed::GetSingleton().meshReslice();
	updateStatusBar("Slice complete");
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_DISPLAY_ID);
	baseFrame->GetEventHandler()->AddPendingEvent(event);
	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_MESH);
	gCodeControlWindow::GetSingleton().GetEventHandler()->AddPendingEvent(controlWindowEvent);
#endif
}


void Slicer::updateStatusBar(const std::string& text, int val)
{
#ifndef _CONSOLE
	if(baseFrame)
		baseFrame->SetStatusBarText(text, val);
#endif
}

void Slicer::generateInfillLines(int type, iSlicable::ToolType tool)
{
	double spacing = Settings::getSingleton().getDoubleValue("T"+std::to_string((int)tool)+"extrusionWidth");
	if (type <= 1) //Normal infill
	{
		spacing = spacing / (GET_DOUBLE("infillDensity") / 100.0);
	}
	else if( type <= 3) //base layer infill/anything with something below
		spacing = spacing / (GET_DOUBLE("bottomLayerInfill") / 100.0);
	else //bridging? full infill
	{
		spacing = spacing / (GET_DOUBLE("topSurfaceInfillDensity") / 100.0);
		//spacing = spacing / (GET_DOUBLE("infillDensity") / 100.0);
	}
	Lines temp;
	double dimMax = std::max(slicable->getWidth(), slicable->getHeight());
	Line::generateLines(temp, spacing, -dimMax*2.0, dimMax*2.0, -dimMax*2.0, dimMax*2.0);
	double angle;
	if (type % 2)
		angle = 45;
	else
		angle = 360 - 45;
	Line::rotateLines(temp, angle);
	RoVaClipperLib::Paths source;
	for (auto lineIT : temp)
	{

			RoVaClipperLib::Path newPath;
			newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt1.x*DEDOUBLER, lineIT.pt1.y*DEDOUBLER));
			newPath.push_back(RoVaClipperLib::IntPoint(lineIT.pt2.x*DEDOUBLER, lineIT.pt2.y*DEDOUBLER));
			source.push_back(newPath);
		
	}
	RoVaClipperLib::Path clip;
	/*
	X->
	 Y
	\|/

	*/
	// 0 ------ 1
	// -        -
	// -        -
	// 3 ------ 2

	clip.push_back(RoVaClipperLib::IntPoint(0, 0));
	clip.push_back(RoVaClipperLib::IntPoint(dimMax*DEDOUBLER, 0));
	clip.push_back(RoVaClipperLib::IntPoint(dimMax*DEDOUBLER, dimMax*DEDOUBLER));
	clip.push_back(RoVaClipperLib::IntPoint(0, dimMax*DEDOUBLER));

	RoVaClipperLib::Clipper clipper;
	clipper.AddPaths(source, RoVaClipperLib::ptSubject, false);
	clipper.AddPath(clip, RoVaClipperLib::ptClip, true);

	RoVaClipperLib::PolyTree out;

	clipper.Execute(RoVaClipperLib::ctIntersection, out);

	RoVaClipperLib::OpenPathsFromPolyTree(out, infillSources[type]);
	for (auto &pathIT : infillSources[type])
	{
		for (auto &pointIT : pathIT)
		{
			pointIT.X += slicable->getMinX()*DEDOUBLER;// +slicable->getWidth()*DEDOUBLER / 8.0;
			pointIT.Y += slicable->getMinY()*DEDOUBLER;// +slicable->getHeight()*DEDOUBLER / 8.0;
		}
	}
}