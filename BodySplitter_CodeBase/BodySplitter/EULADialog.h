///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __EULADIALOG_H__
#define __EULADIALOG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EULA
///////////////////////////////////////////////////////////////////////////////
class EULA : public wxDialog
{
private:

protected:
	wxTextCtrl* m_licenseText;
	wxCheckBox* m_AgreeCheck;
	wxButton* m_Disagree;
	wxButton* m_Agree;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event) { event.Skip(); }
	virtual void OnCheck(wxCommandEvent& event) { event.Skip(); }
	virtual void OnDisagree(wxCommandEvent& event) { event.Skip(); }
	virtual void OnAgree(wxCommandEvent& event) { event.Skip(); }


public:

	EULA(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("BodySplitter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(499, 358), long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
	~EULA();

};

#endif //__EULADIALOG_H__
