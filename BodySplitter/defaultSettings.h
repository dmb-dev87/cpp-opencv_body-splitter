///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DEFAULTSETTINGS_H__
#define __DEFAULTSETTINGS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DefaultSettingsDialog
///////////////////////////////////////////////////////////////////////////////
class DefaultSettingsDialog : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText125;
	wxStdDialogButtonSizer* m_sdbSizer2;
	wxButton* m_sdbSizer2OK;
	wxButton* m_sdbSizer2Cancel;

public:
	wxChoice* m_hardwareChoice;

	DefaultSettingsDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Default Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(356, 128), long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);
	~DefaultSettingsDialog();

};

#endif //__DEFAULTSETTINGS_H__
