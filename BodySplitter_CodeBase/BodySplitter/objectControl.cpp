///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "objectControl.h"

///////////////////////////////////////////////////////////////////////////

slicableMenu::slicableMenu(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Model Preferences")), wxVERTICAL);

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer7->SetFlexibleDirection(wxBOTH);
	fgSizer7->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_useTexture = new wxCheckBox(sbSizer5->GetStaticBox(), wxID_ANY, wxT("Use Texture"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer7->Add(m_useTexture, 0, wxALL, 5);


	fgSizer7->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText14 = new wxStaticText(sbSizer5->GetStaticBox(), wxID_ANY, wxT("Colour:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText14->Wrap(-1);
	fgSizer7->Add(m_staticText14, 0, wxALL, 5);

	m_objectColour = new wxColourPickerCtrl(sbSizer5->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	fgSizer7->Add(m_objectColour, 0, wxALL, 5);


	sbSizer5->Add(fgSizer7, 1, wxEXPAND, 5);


	bSizer6->Add(sbSizer5, 1, wxEXPAND, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bSizer6->Add(m_sdbSizer1, 0, wxEXPAND, 5);


	this->SetSizer(bSizer6);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_useTexture->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(slicableMenu::UseTextureCheck), NULL, this);
}

slicableMenu::~slicableMenu()
{
	// Disconnect Events
	m_useTexture->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(slicableMenu::UseTextureCheck), NULL, this);

}
