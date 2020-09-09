///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PROPPANEL_H__
#define __PROPPANEL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/radiobut.h>
#include <wx/clrpicker.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/scrolwin.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class PropertiesPanel
///////////////////////////////////////////////////////////////////////////////
class PropertiesPanel : public wxPanel
{
private:

protected:
	wxScrolledWindow* m_scrolledWindow1;
	wxTextCtrl* m_DrawbleName;
	wxChoice* m_ToolChoice;
	wxRadioButton* m_UseTexture;
	wxRadioButton* m_UseCustomColour;
	wxColourPickerCtrl* m_CustomColourPick;
	wxButton* m_PlaceOnBed;
	wxSpinCtrlDouble* m_XOffset;
	wxSpinCtrlDouble* m_YOffset;
	wxSpinCtrlDouble* m_ZOffset;
	wxButton* m_ResetPosition;
	wxSpinCtrlDouble* m_XRotation;
	wxSpinCtrlDouble* m_YRotation;
	wxSpinCtrlDouble* m_ZRotation;
	wxButton* m_ResetRotation;
	wxSpinCtrlDouble* m_XScale;
	wxSpinCtrlDouble* m_YScale;
	wxSpinCtrlDouble* m_ZScale;
	wxCheckBox* m_uniformScale;
	wxButton* m_ResetScale;
	wxButton* m_scaleMax;
	wxButton* m_Dupe;
	wxButton* m_DeleteObj;
	wxButton* m_hideObj;

	// Virtual event handlers, overide them in your derived class
	virtual void UpdateName(wxFocusEvent& event) { event.Skip(); }
	virtual void ToolChoose(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUseTexture(wxCommandEvent& event) { event.Skip(); }
	virtual void OnUseCustomColour(wxCommandEvent& event) { event.Skip(); }
	virtual void ColourChosen(wxColourPickerEvent& event) { event.Skip(); }
	virtual void PlaceOnBed(wxCommandEvent& event) { event.Skip(); }
	virtual void XOffsetUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void YOffsetUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void ZOffsetUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void ResetPosition(wxCommandEvent& event) { event.Skip(); }
	virtual void XRotationUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void YRotationUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void ZRotationUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void ResetRotation(wxCommandEvent& event) { event.Skip(); }
	virtual void XScaleUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void YScaleUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void ZScaleUpdate(wxSpinDoubleEvent& event) { event.Skip(); }
	virtual void UniformScaleCheck(wxCommandEvent& event) { event.Skip(); }
	virtual void ResetScale(wxCommandEvent& event) { event.Skip(); }
	virtual void scaleMaxButton(wxCommandEvent& event) { event.Skip(); }
	virtual void DupeObject(wxCommandEvent& event) { event.Skip(); }
	virtual void DeleteObj(wxCommandEvent& event) { event.Skip(); }
	virtual void OnHideButton(wxCommandEvent& event) { event.Skip(); }
	virtual void HideProperties(wxCommandEvent& event) { event.Skip(); }


public:
	wxButton* m_HideProperties;

	PropertiesPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(270, 687), long style = wxTAB_TRAVERSAL);
	~PropertiesPanel();

};

#endif //__PROPPANEL_H__
