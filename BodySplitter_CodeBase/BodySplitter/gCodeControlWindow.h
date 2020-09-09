#pragma once

#include "GCodeVisualizerWX.h"
#include "Singleton.h"

/* This is the GCode preview tab, note:! gCodeWXPanel needs to be modified
since wxFormBuilder cant seem to change the constructor and we need to
pass int *args to the gcodeVisualizer window for opengl */

const int UPDATE_MESH = 34823;
const int UPDATE_LAYER_VIEW = 34824;
const int UPDATE_PRINT_PROGRESS = 34825;
const int FILL_PRINT_STATS = 34826;
const int FILL_PRINT_TIME = 34827;
const int FILL_PRINT_COST = 34728;

class gCodeControlWindow : public gCodeWXPanel, public Singleton<gCodeControlWindow>
{
private:
	virtual void ShowLayerScroll(wxScrollEvent& event);
	virtual void ShowLayer(wxCommandEvent& event);
	int m_layerToShow = -1;

	virtual void fillTime(wxCommandEvent &event);
	virtual void fillStats(wxCommandEvent &event);
	virtual void fillCost(wxCommandEvent &event);

	void OnLayerUp(wxCommandEvent& event);
	void OnLayerDown(wxCommandEvent& event);

	virtual void OnColourSub(wxCommandEvent& event);
public:
	gCodeControlWindow(wxWindow* parent, int* args, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(894, 601), long style = wxTAB_TRAVERSAL);
	~gCodeControlWindow();

	void setFilamentStats(const std::string &str);
	void setPrintTime(const std::string &str);
	void setPrintCost(double printCost);
	void setLayerRange(unsigned int count);

	void meshSliced(wxCommandEvent &event);
	void updateLayerView(wxCommandEvent &event);

	void updatePrintProgress(wxCommandEvent &event);

	wxDECLARE_EVENT_TABLE();
};