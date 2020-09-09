#pragma once
#include "GCodeVisualizer.h"
class SupportGCodeVisualizer :
	public GCodeVisualizer
{
protected:
	void createVBOs() override;
public:
	SupportGCodeVisualizer();
	~SupportGCodeVisualizer();
};

