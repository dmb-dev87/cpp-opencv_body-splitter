///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ABOUTDIALOG_H__
#define __ABOUTDIALOG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class AboutDialog
///////////////////////////////////////////////////////////////////////////////
class AboutDialog : public wxDialog
{
private:

protected:
	wxBoxSizer* bSizer2;
	wxStaticBitmap* m_bitmap1;
	wxStaticText* m_versionString;
	wxStaticText* m_CopyRightString;
	wxHyperlinkCtrl* m_hyperlink4;
	wxHyperlinkCtrl* m_hyperlink5;
	wxTextCtrl* m_textCtrl1;
	wxButton* m_button1;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCommandEvent& event) { event.Skip(); }


public:

	AboutDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About BodySplitter"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(537, 555), long style = wxDEFAULT_DIALOG_STYLE);
	~AboutDialog();

};

#endif //__ABOUTDIALOG_H__
