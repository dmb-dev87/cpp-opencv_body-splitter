///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __COLOURADJUSTDIALOG_H__
#define __COLOURADJUSTDIALOG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
class wxImagePanel;

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ColourAdjustDialog
///////////////////////////////////////////////////////////////////////////////
class ColourAdjustDialog : public wxDialog
{
private:

protected:
	wxListBox* m_MeshChoice;
	wxStaticText* m_staticText93;
	wxSpinCtrl* m_ColourCount;
	wxCheckBox* m_HoldRatio;
	wxStaticText* m_staticText94;
	wxSpinCtrl* m_PixWidth;
	wxStaticText* m_staticText95;
	wxSpinCtrl* m_PixHeight;
	wxButton* m_UpdatePreview;
	wxButton* m_RestoreToSource;
	wxStaticText* m_staticText96;
	wxStaticText* m_currentColours;
	wxStaticText* m_staticText97;
	wxStaticText* m_currentWidth;
	wxStaticText* m_staticText98;
	wxStaticText* m_currentHeight;
	wxImagePanel* m_PreviewImage;
	wxStdDialogButtonSizer* m_sdbSizer2;
	wxButton* m_sdbSizer2OK;
	wxButton* m_sdbSizer2Cancel;

	// Virtual event handlers, overide them in your derived class
	virtual void ChangeMesh(wxCommandEvent& event) { event.Skip(); }
	virtual void OnPreserveRatio(wxCommandEvent& event) { event.Skip(); }
	virtual void OnWidthChange(wxFocusEvent& event) { event.Skip(); }
	virtual void OnWidthChange(wxCommandEvent& event) { event.Skip(); }
	virtual void OnHeightChange(wxFocusEvent& event) { event.Skip(); }
	virtual void OnHeightChange(wxCommandEvent& event) { event.Skip(); }
	virtual void UpdatePreviewImage(wxCommandEvent& event) { event.Skip(); }
	virtual void OnRevertToSource(wxCommandEvent& event) { event.Skip(); }
	virtual void OnOKClick(wxCommandEvent& event) { event.Skip(); }


public:

	ColourAdjustDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Colour Adjustment"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(797, 584), long style = wxDEFAULT_DIALOG_STYLE);
	~ColourAdjustDialog();

};

#endif //__COLOURADJUSTDIALOG_H__
