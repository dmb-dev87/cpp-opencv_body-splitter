///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "CompatForm.h"

///////////////////////////////////////////////////////////////////////////

MultiExtCompat::MultiExtCompat(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_scrolledWindow1 = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxVSCROLL);
	m_scrolledWindow1->SetScrollRate(5, 5);
	addNozzlesHere = new wxFlexGridSizer(0, 2, 0, 0);
	addNozzlesHere->SetFlexibleDirection(wxBOTH);
	addNozzlesHere->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	m_scrolledWindow1->SetSizer(addNozzlesHere);
	m_scrolledWindow1->Layout();
	addNozzlesHere->Fit(m_scrolledWindow1);
	bSizer1->Add(m_scrolledWindow1, 1, wxEXPAND | wxALL, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	bSizer1->Add(m_sdbSizer1, 0, wxEXPAND, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);
}

MultiExtCompat::~MultiExtCompat()
{
}
