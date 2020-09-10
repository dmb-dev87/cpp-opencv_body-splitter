///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __CONTROLPANEL_H__
#define __CONTROLPANEL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/gauge.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class controlPanel
///////////////////////////////////////////////////////////////////////////////
class controlPanel : public wxPanel
{
private:

protected:
	wxComboBox* m_SerialPort;
	wxStaticText* m_staticText141;
	wxComboBox* m_baudRate;
	wxButton* m_Connect;
	wxButton* m_Refresh;
	wxButton* m_PlusY;
	wxButton* m_MinusX;
	wxButton* m_Home;
	wxButton* m_PlusX;
	wxButton* m_MinusY;
	wxStaticText* m_staticText75;
	wxTextCtrl* m_T0_Temp;
	wxStaticText* m_staticText76;
	wxButton* m_MixerSet;
	wxButton* m_MixTempOff;
	wxStaticText* m_staticText77;
	wxTextCtrl* m_T1_Temp;
	wxStaticText* m_staticText78;
	wxButton* m_SupportSet;
	wxButton* m_SupportTempOff;
	wxStaticText* m_staticText79;
	wxTextCtrl* m_T2_Temp;
	wxStaticText* m_staticText80;
	wxButton* m_FlexSet;
	wxButton* m_FlexTempOff;
	wxStaticText* m_staticText81;
	wxTextCtrl* m_Bed_Temp;
	wxStaticText* m_staticText82;
	wxButton* m_BedSet;
	wxButton* m_BedTempOff;
	wxStaticText* m_105Result;
	wxStaticText* m_staticText12;
	wxSlider* m_PrintSpeed;
	wxSpinCtrl* m_printSpeedSpinCtrl;
	wxButton* m_SetPrintSpeed;
	wxStaticText* m_staticText14;
	wxSlider* m_ExtSpeed;
	wxSpinCtrl* m_extSpeedSpinCtrl;
	wxButton* m_SetMultiple;
	wxButton* m_StartPrint;
	wxButton* m_PausePrint;
	wxButton* m_Abort;
	wxButton* m_LoadGCode;
	wxCheckBox* m_Verbose;
	wxGauge* m_Progress;
	wxStaticText* m_MixerTempReadout;
	wxStaticText* m_BedTempReadout;
	wxTextCtrl* m_GCodeConsole;
	wxTextCtrl* m_StringToSend;
	wxButton* m_SendString;

	// Virtual event handlers, overide them in your derived class
	virtual void ConnectToggle(wxCommandEvent& event) { event.Skip(); }
	virtual void RefreshPorts(wxCommandEvent& event) { event.Skip(); }
	virtual void Plus10Y(wxCommandEvent& event) { event.Skip(); }
	virtual void Minus10X(wxCommandEvent& event) { event.Skip(); }
	virtual void Home(wxCommandEvent& event) { event.Skip(); }
	virtual void Plus10X(wxCommandEvent& event) { event.Skip(); }
	virtual void Minus10Y(wxCommandEvent& event) { event.Skip(); }
	virtual void SetMixerTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void OffMixerTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void SetSupportTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void OffSupportTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void SetFlexibleTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void OffFlexibleTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void SetBedTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void SetOffTemp(wxCommandEvent& event) { event.Skip(); }
	virtual void OnScrollPrintSpeed(wxScrollEvent& event) { event.Skip(); }
	virtual void OnSetPrintSpeedSpinner(wxFocusEvent& event) { event.Skip(); }
	virtual void OnPrintSpeedSpinCtrl(wxSpinEvent& event) { event.Skip(); }
	virtual void SetPrintSpeed(wxCommandEvent& event) { event.Skip(); }
	virtual void OnScrollExtMultiplier(wxScrollEvent& event) { event.Skip(); }
	virtual void OnSetExtrusionSpinner(wxFocusEvent& event) { event.Skip(); }
	virtual void OnExtrusionSpinCtrl(wxSpinEvent& event) { event.Skip(); }
	virtual void ExtMultiplier(wxCommandEvent& event) { event.Skip(); }
	virtual void StartPrint(wxCommandEvent& event) { event.Skip(); }
	virtual void PuaseToggle(wxCommandEvent& event) { event.Skip(); }
	virtual void Abort(wxCommandEvent& event) { event.Skip(); }
	virtual void LoadGCodeFile(wxCommandEvent& event) { event.Skip(); }
	virtual void VerboseToggle(wxCommandEvent& event) { event.Skip(); }
	virtual void DownHistory(wxKeyEvent& event) { event.Skip(); }
	virtual void sendCommandEnter(wxCommandEvent& event) { event.Skip(); }
	virtual void sendCommand(wxCommandEvent& event) { event.Skip(); }


public:

	controlPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(827, 766), long style = wxTAB_TRAVERSAL);
	~controlPanel();

};

#endif //__CONTROLPANEL_H__
