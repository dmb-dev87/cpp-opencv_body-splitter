///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "EULADialog.h"
#include "EULA.h"

///////////////////////////////////////////////////////////////////////////

EULA::EULA(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_licenseText = new wxTextCtrl(this, wxID_ANY, bin2c_ordterms_txt, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);
	bSizer3->Add(m_licenseText, 1, wxALL | wxEXPAND, 5);

	m_AgreeCheck = new wxCheckBox(this, wxID_ANY, wxT("Yes, I have read and understood the above terms and conditions"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(m_AgreeCheck, 0, wxALL, 5);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	m_Disagree = new wxButton(this, wxID_ANY, wxT("I Disagree"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(m_Disagree, 0, wxALIGN_CENTER | wxALL, 5);

	m_Agree = new wxButton(this, wxID_ANY, wxT("I Agree"), wxDefaultPosition, wxDefaultSize, 0);
	m_Agree->Enable(false);

	bSizer4->Add(m_Agree, 0, wxALIGN_CENTER | wxALL, 5);


	bSizer3->Add(bSizer4, 0, wxALIGN_CENTER, 5);


	this->SetSizer(bSizer3);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EULA::OnClose));
	m_AgreeCheck->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(EULA::OnCheck), NULL, this);
	m_Disagree->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EULA::OnDisagree), NULL, this);
	m_Agree->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EULA::OnAgree), NULL, this);
}

EULA::~EULA()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(EULA::OnClose));
	m_AgreeCheck->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(EULA::OnCheck), NULL, this);
	m_Disagree->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EULA::OnDisagree), NULL, this);
	m_Agree->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(EULA::OnAgree), NULL, this);

}
