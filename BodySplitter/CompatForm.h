///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __COMPATFORM_H__
#define __COMPATFORM_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/scrolwin.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MultiExtCompat
///////////////////////////////////////////////////////////////////////////////
class MultiExtCompat : public wxDialog
{
private:

protected:
	wxScrolledWindow* m_scrolledWindow1;
	wxFlexGridSizer* addNozzlesHere;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

public:

	MultiExtCompat(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MultiExtruder Compatibility"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(316, 394), long style = wxDEFAULT_DIALOG_STYLE);
	~MultiExtCompat();

};

#endif //__COMPATFORM_H__
