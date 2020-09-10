///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxImagePanel.h"

#include "ColourAdjustDialog.h"

///////////////////////////////////////////////////////////////////////////

ColourAdjustDialog::ColourAdjustDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer20->SetFlexibleDirection(wxBOTH);
	fgSizer20->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer(wxVERTICAL);

	m_MeshChoice = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL);
	bSizer17->Add(m_MeshChoice, 1, wxALL | wxEXPAND, 5);

	wxFlexGridSizer* fgSizer18;
	fgSizer18 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer18->SetFlexibleDirection(wxBOTH);
	fgSizer18->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText93 = new wxStaticText(this, wxID_ANY, wxT("Colours:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText93->Wrap(-1);
	fgSizer18->Add(m_staticText93, 0, wxALL, 5);

	m_ColourCount = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 32, 5);
	m_ColourCount->SetToolTip(wxT("The target number of colours to adjust the texture to contain. A larger value will take longer to analyze the texture"));
	fgSizer18->Add(m_ColourCount, 1, wxALL, 5);

	m_HoldRatio = new wxCheckBox(this, wxID_ANY, wxT("Preserve Ratio"), wxDefaultPosition, wxDefaultSize, 0);
	m_HoldRatio->SetValue(true);
	fgSizer18->Add(m_HoldRatio, 0, wxALL, 5);


	fgSizer18->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText94 = new wxStaticText(this, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText94->Wrap(-1);
	fgSizer18->Add(m_staticText94, 0, wxALL, 5);

	m_PixWidth = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 32, 2048, 512);
	fgSizer18->Add(m_PixWidth, 1, wxALL, 5);

	m_staticText95 = new wxStaticText(this, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText95->Wrap(-1);
	fgSizer18->Add(m_staticText95, 0, wxALL, 5);

	m_PixHeight = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 32, 2048, 512);
	fgSizer18->Add(m_PixHeight, 1, wxALL, 5);

	m_UpdatePreview = new wxButton(this, wxID_ANY, wxT("Update Preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_UpdatePreview->SetToolTip(wxT("Updates the texture preview showing the changes"));

	fgSizer18->Add(m_UpdatePreview, 0, wxALL, 5);

	m_RestoreToSource = new wxButton(this, wxID_ANY, wxT("Revert To Source"), wxDefaultPosition, wxDefaultSize, 0);
	m_RestoreToSource->SetToolTip(wxT(" Undo all changes and restore the original texture"));

	fgSizer18->Add(m_RestoreToSource, 0, wxALL, 5);


	bSizer17->Add(fgSizer18, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer24;
	sbSizer24 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Current Properties")), wxVERTICAL);

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer21->SetFlexibleDirection(wxBOTH);
	fgSizer21->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText96 = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Colours:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText96->Wrap(-1);
	fgSizer21->Add(m_staticText96, 0, wxALL, 5);

	m_currentColours = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_currentColours->Wrap(-1);
	fgSizer21->Add(m_currentColours, 0, wxALL, 5);

	m_staticText97 = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText97->Wrap(-1);
	fgSizer21->Add(m_staticText97, 0, wxALL, 5);

	m_currentWidth = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_currentWidth->Wrap(-1);
	fgSizer21->Add(m_currentWidth, 0, wxALL, 5);

	m_staticText98 = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Height:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText98->Wrap(-1);
	fgSizer21->Add(m_staticText98, 0, wxALL, 5);

	m_currentHeight = new wxStaticText(sbSizer24->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_currentHeight->Wrap(-1);
	fgSizer21->Add(m_currentHeight, 0, wxALL, 5);


	sbSizer24->Add(fgSizer21, 1, wxEXPAND, 5);


	bSizer17->Add(sbSizer24, 1, wxEXPAND, 5);


	fgSizer20->Add(bSizer17, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer(wxHORIZONTAL);

	m_PreviewImage = new wxImagePanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_PreviewImage->SetMinSize(wxSize(512, 512));

	bSizer18->Add(m_PreviewImage, 1, wxALL | wxEXPAND, 5);


	fgSizer20->Add(bSizer18, 1, wxEXPAND, 5);


	bSizer16->Add(fgSizer20, 1, wxEXPAND, 5);

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

	// Connect Events
	m_MeshChoice->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(ColourAdjustDialog::ChangeMesh), NULL, this);
	m_HoldRatio->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnPreserveRatio), NULL, this);
	m_PixWidth->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ColourAdjustDialog::OnWidthChange), NULL, this);
	m_PixWidth->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ColourAdjustDialog::OnWidthChange), NULL, this);
	m_PixHeight->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ColourAdjustDialog::OnHeightChange), NULL, this);
	m_PixHeight->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ColourAdjustDialog::OnHeightChange), NULL, this);
	m_UpdatePreview->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::UpdatePreviewImage), NULL, this);
	m_RestoreToSource->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnRevertToSource), NULL, this);
	m_sdbSizer2OK->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnOKClick), NULL, this);
}

ColourAdjustDialog::~ColourAdjustDialog()
{
	// Disconnect Events
	m_MeshChoice->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(ColourAdjustDialog::ChangeMesh), NULL, this);
	m_HoldRatio->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnPreserveRatio), NULL, this);
	m_PixWidth->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ColourAdjustDialog::OnWidthChange), NULL, this);
	m_PixWidth->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ColourAdjustDialog::OnWidthChange), NULL, this);
	m_PixHeight->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(ColourAdjustDialog::OnHeightChange), NULL, this);
	m_PixHeight->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ColourAdjustDialog::OnHeightChange), NULL, this);
	m_UpdatePreview->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::UpdatePreviewImage), NULL, this);
	m_RestoreToSource->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnRevertToSource), NULL, this);
	m_sdbSizer2OK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ColourAdjustDialog::OnOKClick), NULL, this);

}
