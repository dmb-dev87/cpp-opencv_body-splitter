///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OBJECTCONTROL_H__
#define __OBJECTCONTROL_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class slicableMenu
///////////////////////////////////////////////////////////////////////////////
class slicableMenu : public wxDialog
{
private:

protected:
	wxCheckBox* m_useTexture;
	wxStaticText* m_staticText14;
	wxColourPickerCtrl* m_objectColour;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

	// Virtual event handlers, overide them in your derived class
	virtual void UseTextureCheck(wxCommandEvent& event) { event.Skip(); }


public:

	slicableMenu(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Object Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(317, 374), long style = wxDEFAULT_DIALOG_STYLE);
	~slicableMenu();

};

#endif //__OBJECTCONTROL_H__
