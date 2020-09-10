///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "defaultSettings.h"

///////////////////////////////////////////////////////////////////////////

DefaultSettingsDialog::DefaultSettingsDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer21->AddGrowableCol(1);
	fgSizer21->SetFlexibleDirection(wxBOTH);
	fgSizer21->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText125 = new wxStaticText(this, wxID_ANY, wxT("Hardware Version:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText125->Wrap(-1);
	fgSizer21->Add(m_staticText125, 0, wxALL, 5);

	wxString m_hardwareChoiceChoices[] = { wxT("RoVa4D - PLA"), wxT("RoVa4D - ABS"), wxT("RoVa3D - Direct Drive"), wxT("RoVa3D - Dual Extruder"), wxT("RoVa3D - Five Extruder"), wxT("RoVa3D - Diamond Hotend") };
	int m_hardwareChoiceNChoices = sizeof(m_hardwareChoiceChoices) / sizeof(wxString);
	m_hardwareChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_hardwareChoiceNChoices, m_hardwareChoiceChoices, 0);
	m_hardwareChoice->SetSelection(0);
	fgSizer21->Add(m_hardwareChoice, 1, wxALL | wxEXPAND, 5);


	bSizer16->Add(fgSizer21, 1, wxEXPAND, 5);

	m_sdbSizer2 = new wxStdDialogButtonSizer();
	m_sdbSizer2OK = new wxButton(this, wxID_OK);
	m_sdbSizer2->AddButton(m_sdbSizer2OK);
	m_sdbSizer2Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer2->AddButton(m_sdbSizer2Cancel);
	m_sdbSizer2->Realize();

	bSizer16->Add(m_sdbSizer2, 0, wxEXPAND, 5);


	this->SetSizer(bSizer16);
	this->Layout();

	this->Centre(wxBOTH);
}

DefaultSettingsDialog::~DefaultSettingsDialog()
{
}
