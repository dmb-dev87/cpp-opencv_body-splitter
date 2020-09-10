#include "GCodeViewPane.h"

#include "Settings.h"
#include "GCodeVisualizer.h"
#include "SupportGCodeVisualizer.h"
#include "PrintBed.h"
#include "iSlicable.h"
#include "iDrawable.h"
#include "SupportManager.h"

#ifndef CONSOLE

GCodeViewPane::GCodeViewPane(wxFrame* parent, int* args) : View3DPane(parent, args)
{
}


GCodeViewPane::~GCodeViewPane()
{
}

void GCodeViewPane::draw()
{
	initializeDrawing();
	auto visualizationObjects = PrintBed::getSingleton().getDrawables();
	
	for(auto it : visualizationObjects)
	{
		if (Settings::GetSingleton().getBoolValue("simpleGCodePreview"))
			glDisable(GL_CULL_FACE);
		//auto temp = std::dynamic_pointer_cast<iSlicable>(it);.
		auto temp = std::dynamic_pointer_cast<iSlicable>(it);
		if(temp->isSliced())
			temp->getVisualizer()->draw();
	}
	if (PrintBed::GetSingleton().hasSupport() && PrintBed::GetSingleton().getSupportManager())
	{
		//auto supportVisualizer = PrintBed::GetSingleton().getSupportManager()->getVisualizer();
		//if (supportVisualizer)
		//	supportVisualizer->draw();
	}
	finalizeDrawing();
}
//gcode_previewPanel = new GCodeViewPane((wxFrame*)this, args);
void GCodeViewPane::setEndLayerDrawing(unsigned int endLayer)
{
	auto visualizationObjects = PrintBed::getSingleton().getDrawables();
	for (auto it : visualizationObjects)
	{
		auto temp = std::dynamic_pointer_cast<iSlicable>(it);
		if(temp->getVisualizer())
			temp->getVisualizer()->setEndLayer(endLayer);
	}
	if (PrintBed::GetSingleton().hasSupport() && PrintBed::GetSingleton().getSupportManager())
	{
		auto supportVisualizer = PrintBed::GetSingleton().getSupportManager()->getVisualizer();
		if (supportVisualizer)
			supportVisualizer->setEndLayer(endLayer);
	}
}

void GCodeViewPane::setPrintingLayer(double layerZ)
{
	unsigned int layer = PrintBed::GetSingleton().getLayer(layerZ);
	setEndLayerDrawing(layer);
}

#endif