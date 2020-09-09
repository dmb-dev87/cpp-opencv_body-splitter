///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE! - TOO late I did
///////////////////////////////////////////////////////////////////////////

#ifndef __GCODEVISUALIZERWX_H__
#define __GCODEVISUALIZERWX_H__

//#include <wx/artprov.h>
//#include <wx/xrc/xmlres.h>
class GCodeViewPane;

#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/slider.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class gCodeWXPanel
///////////////////////////////////////////////////////////////////////////////
class gCodeWXPanel : public wxPanel
{
private:

protected:
	wxPanel* g_codeStatsPanel;
	wxTextCtrl* m_filamentStats;
	wxTextCtrl* m_timeToPrint;
	wxTextCtrl* m_printCost;
	wxSlider* m_layerSelectSlider;
	wxTextCtrl* m_layerSelectText;
	wxButton* m_showLayer;
	wxButton* m_LayerUp;
	wxButton* m_Layerdown;
	wxTextCtrl* m_ColoursOnLayer;
	wxButton* m_colourSub;
	wxButton* m_Print;

	// Virtual event handlers, overide them in your derived class
	virtual void ShowLayerScroll(wxScrollEvent& event) { event.Skip(); }
	virtual void ShowLayer(wxCommandEvent& event) { event.Skip(); }
	virtual void OnLayerUp(wxCommandEvent& event) { event.Skip(); }
	virtual void OnLayerDown(wxCommandEvent& event) { event.Skip(); }
	virtual void OnColourSub(wxCommandEvent& event) { event.Skip(); }
	virtual void OnPrintButton(wxCommandEvent& event) { event.Skip(); }


public:
	GCodeViewPane* gcode_previewPanel;

	gCodeWXPanel(wxWindow* parent,int *args, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(894, 601), long style = wxTAB_TRAVERSAL);
	~gCodeWXPanel();

};

#endif //__GCODEVISUALIZERWX_H__
