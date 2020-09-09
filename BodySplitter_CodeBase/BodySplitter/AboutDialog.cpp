///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AboutDialog.h"
#include "images/rsimages.h"
#include "EULA.h"
///////////////////////////////////////////////////////////////////////////

AboutDialog::AboutDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	bSizer2 = new wxBoxSizer(wxVERTICAL);

	m_bitmap1 = new wxStaticBitmap(this, wxID_ANY, wxBITMAP_PNG_FROM_DATA(bin2c_rovalogo), wxDefaultPosition, wxSize(248, 100), 0);
	bSizer2->Add(m_bitmap1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	m_versionString = new wxStaticText(this, wxID_ANY, wxT("Version: 0.1.0"), wxDefaultPosition, wxDefaultSize, 0);
	m_versionString->Wrap(-1);
	bSizer2->Add(m_versionString, 0, wxLEFT, 5);

	m_CopyRightString = new wxStaticText(this, wxID_ANY, wxT("Copyright ORD Solutions"), wxDefaultPosition, wxDefaultSize, 0);
	m_CopyRightString->Wrap(-1);
	bSizer2->Add(m_CopyRightString, 0, wxALIGN_LEFT | wxLEFT, 5);

	m_hyperlink4 = new wxHyperlinkCtrl(this, wxID_ANY, wxT("Designed for the RoVa4D Full Colour Printer"), wxT("http://www.ordsolutions.com/rova4d-full-color-3d-printer/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizer2->Add(m_hyperlink4, 0, wxALIGN_LEFT | wxLEFT, 5);

	m_hyperlink5 = new wxHyperlinkCtrl(this, wxID_ANY, wxT("Full Colour Filament"), wxT("http://www.ordsolutions.com/trublend/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	bSizer2->Add(m_hyperlink5, 0, wxALIGN_LEFT | wxLEFT, 5);

	m_textCtrl1 = new wxTextCtrl(this, wxID_ANY, bin2c_ordterms_txt, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	bSizer2->Add(m_textCtrl1, 1, wxALL | wxEXPAND, 5);

	m_button1 = new wxButton(this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_button1, 0, wxALIGN_RIGHT | wxALL, 5);


	this->SetSizer(bSizer2);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_button1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialog::OnClose), NULL, this);
}

AboutDialog::~AboutDialog()
{
	// Disconnect Events
	m_button1->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(AboutDialog::OnClose), NULL, this);

}
