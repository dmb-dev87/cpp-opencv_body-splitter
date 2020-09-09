#pragma once
#include "Singleton.h"
#include "View3DPane.h"

#ifndef _CONSOLE

class GCodeVisualizer;
class SupportGCodeVisualizer;
/* Handlers the gcode view tab */
class GCodeViewPane :
	public View3DPane, public Singleton<GCodeViewPane>
{
private:

public:
	GCodeViewPane(wxFrame* parent, int* args);
	~GCodeViewPane();

	void draw() override;

	// The result of the slider, sets the layer to draw upto
	void setEndLayerDrawing(unsigned int layer);
	// Live print tracking - Use mm instead of a layer number
	void setPrintingLayer(double layerHeight);
};

#endif