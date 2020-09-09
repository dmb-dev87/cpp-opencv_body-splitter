#ifndef __COLOURSUBSTITIONWX_H__
#define __COLOURSUBSTITIONWX_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

#include "colour.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class colourSubDialog
///////////////////////////////////////////////////////////////////////////////
class colourSubDialog : public wxDialog
{
private:

protected:
	std::vector<Colour> colours;
	wxPanel* m_panel1;
	wxFlexGridSizer* m_colourSubHolder;
	std::vector<wxColourPickerCtrl*> m_colour;
	std::vector<wxStaticText*> m_staticText1;
	std::vector<wxSpinCtrl*> m_C;
	std::vector<wxStaticText*> m_staticText2;
	std::vector<wxSpinCtrl*> m_M;
	std::vector<wxStaticText*> m_staticText3;
	std::vector<wxSpinCtrl*> m_Y;
	std::vector<wxStaticText*> m_staticText4;
	std::vector<wxSpinCtrl*> m_K;
	std::vector<wxStaticText*> m_staticText5;
	std::vector<wxSpinCtrl*> m_W;
	std::vector<wxStaticText*> m_staticText6;
	std::vector<wxSpinCtrl*> m_T;
	wxStdDialogButtonSizer* m_sdbSizer2;
	wxButton* m_sdbSizer2OK;
	wxButton* m_sdbSizer2Cancel;
	wxButton* m_RestoreColours;
	virtual void OnRestoreColours(wxCommandEvent& event);
public:

	colourSubDialog(wxWindow* parent,const std::vector<Colour> &colours, wxWindowID id = wxID_ANY, const wxString& title = wxT("Colour Substitution"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(700, 449), long style = wxDEFAULT_DIALOG_STYLE);
	~colourSubDialog();

	void saveColourSub();
};

#endif //__COLOURSUBSTITIONWX_H__
