///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "PropPanel.h"

///////////////////////////////////////////////////////////////////////////

PropertiesPanel::PropertiesPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	this->SetMinSize(wxSize(270, -1));
	this->SetMaxSize(wxSize(270, -1));

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer(wxVERTICAL);

	m_scrolledWindow1 = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	m_scrolledWindow1->SetScrollRate(5, 5);
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* m_staticText23;
	m_staticText23 = new wxStaticText(m_scrolledWindow1, wxID_ANY, wxT("Item Name:"), wxDefaultPosition, wxSize(70, -1), 0);
	m_staticText23->Wrap(-1);
	m_staticText23->SetMinSize(wxSize(70, -1));
	m_staticText23->SetMaxSize(wxSize(70, -1));

	bSizer6->Add(m_staticText23, 0, wxALL, 2);

	m_DrawbleName = new wxTextCtrl(m_scrolledWindow1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), 0);
	m_DrawbleName->SetToolTip(wxT("Name of the object"));

	bSizer6->Add(m_DrawbleName, 1, wxALL, 2);


	bSizer16->Add(bSizer6, 0, 0, 5);

	wxStaticBoxSizer* sbSizer20;
	sbSizer20 = new wxStaticBoxSizer(new wxStaticBox(m_scrolledWindow1, wxID_ANY, wxT("Colour and Texture")), wxVERTICAL);

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer13->SetFlexibleDirection(wxBOTH);
	fgSizer13->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText26;
	m_staticText26 = new wxStaticText(sbSizer20->GetStaticBox(), wxID_ANY, wxT("Tool:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText26->Wrap(-1);
	fgSizer13->Add(m_staticText26, 0, wxALL, 2);

	wxString m_ToolChoiceChoices[] = { wxT("Full Colour"), wxT("Flexible"), wxT("Support"), wxT("T4 - Generic Tool"), wxT("T3 - Generic Tool") };
	int m_ToolChoiceNChoices = sizeof(m_ToolChoiceChoices) / sizeof(wxString);
	m_ToolChoice = new wxChoice(sbSizer20->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ToolChoiceNChoices, m_ToolChoiceChoices, 0);
	m_ToolChoice->SetSelection(0);
	m_ToolChoice->SetToolTip(wxT("Select Tool to use for which material is being printed"));
	m_ToolChoice->SetMaxSize(wxSize(90, -1));

	fgSizer13->Add(m_ToolChoice, 0, wxALL, 2);

	m_UseTexture = new wxRadioButton(sbSizer20->GetStaticBox(), wxID_ANY, wxT("Use Texture"), wxDefaultPosition, wxDefaultSize, 0);
	m_UseTexture->SetToolTip(wxT("Use the texture included with the model for colour information"));
	fgSizer13->Add(m_UseTexture, 0, wxALL, 5);


	fgSizer13->Add(0, 0, 1, wxEXPAND, 5);

	m_UseCustomColour = new wxRadioButton(sbSizer20->GetStaticBox(), wxID_ANY, wxT("Use Custom Colour"), wxDefaultPosition, wxDefaultSize, 0);
	m_UseCustomColour->SetToolTip(wxT("Select specific colour to use for print"));
	fgSizer13->Add(m_UseCustomColour, 0, wxALL, 5);


	fgSizer13->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText75;
	m_staticText75 = new wxStaticText(sbSizer20->GetStaticBox(), wxID_ANY, wxT("Custom Colour:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText75->Wrap(-1);
	fgSizer13->Add(m_staticText75, 0, wxALL, 5);

	m_CustomColourPick = new wxColourPickerCtrl(sbSizer20->GetStaticBox(), wxID_ANY, wxColour(255, 255, 255), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_CustomColourPick->SetMaxSize(wxSize(70, -1));

	fgSizer13->Add(m_CustomColourPick, 0, wxALL, 5);


	sbSizer20->Add(fgSizer13, 1, wxEXPAND, 5);


	bSizer16->Add(sbSizer20, 0, wxEXPAND, 2);

	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer(new wxStaticBox(m_scrolledWindow1, wxID_ANY, wxT("Position and Rotation")), wxVERTICAL);

	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer14->SetFlexibleDirection(wxBOTH);
	fgSizer14->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_PlaceOnBed = new wxButton(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Place on Bed"), wxDefaultPosition, wxDefaultSize, 0);
	m_PlaceOnBed->SetToolTip(wxT("Places the object so the lowest Z point is on the bed"));

	fgSizer14->Add(m_PlaceOnBed, 0, wxALL, 2);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText76;
	m_staticText76 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("X Offset:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText76->Wrap(-1);
	fgSizer14->Add(m_staticText76, 0, wxALL, 2);

	m_XOffset = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -3000, 3000, 0);
	m_XOffset->SetToolTip(wxT("Change the position of the Object on the grid"));
	m_XOffset->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_XOffset, 0, wxALL, 2);

	wxStaticText* m_staticText77;
	m_staticText77 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText77->Wrap(-1);
	fgSizer14->Add(m_staticText77, 0, wxALL, 2);

	wxStaticText* m_staticText78;
	m_staticText78 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Y Offset:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText78->Wrap(-1);
	fgSizer14->Add(m_staticText78, 0, wxALL, 2);

	m_YOffset = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -3000, 3000, 0);
	m_YOffset->SetToolTip(wxT("Change the position of the Object on the grid"));
	m_YOffset->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_YOffset, 0, wxALL, 2);

	wxStaticText* m_staticText79;
	m_staticText79 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText79->Wrap(-1);
	fgSizer14->Add(m_staticText79, 0, wxALL, 2);

	wxStaticText* m_staticText80;
	m_staticText80 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Z Offset:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText80->Wrap(-1);
	fgSizer14->Add(m_staticText80, 0, wxALL, 2);

	m_ZOffset = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -30000, 30000, 0);
	m_ZOffset->SetToolTip(wxT("Change the position of the Object on the grid"));
	m_ZOffset->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_ZOffset, 0, wxALL, 2);

	wxStaticText* m_staticText82;
	m_staticText82 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText82->Wrap(-1);
	fgSizer14->Add(m_staticText82, 0, wxALL, 2);

	m_ResetPosition = new wxButton(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Reset Positon"), wxDefaultPosition, wxDefaultSize, 0);
	m_ResetPosition->SetToolTip(wxT("Repositions object to original position"));

	fgSizer14->Add(m_ResetPosition, 0, wxALL, 2);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText83;
	m_staticText83 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("X Rotation:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText83->Wrap(-1);
	fgSizer14->Add(m_staticText83, 0, wxALL, 2);

	m_XRotation = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -360, 360, 0);
	m_XRotation->SetToolTip(wxT("Rotate the object on either the X, Y or Z axis"));
	m_XRotation->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_XRotation, 0, wxALL, 2);

	wxStaticText* m_staticText84;
	m_staticText84 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("degrees"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText84->Wrap(-1);
	fgSizer14->Add(m_staticText84, 0, wxALL, 2);

	wxStaticText* m_staticText85;
	m_staticText85 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Y Rotation:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText85->Wrap(-1);
	fgSizer14->Add(m_staticText85, 0, wxALL, 2);

	m_YRotation = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -360, 360, 0);
	m_YRotation->SetToolTip(wxT("Rotate the object on either the X, Y or Z axis"));
	m_YRotation->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_YRotation, 0, wxALL, 2);

	wxStaticText* m_staticText86;
	m_staticText86 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("degrees"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText86->Wrap(-1);
	fgSizer14->Add(m_staticText86, 0, wxALL, 2);

	wxStaticText* m_staticText87;
	m_staticText87 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Z Rotation:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText87->Wrap(-1);
	fgSizer14->Add(m_staticText87, 0, wxALL, 2);

	m_ZRotation = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -360, 360, 0);
	m_ZRotation->SetToolTip(wxT("Rotate the object on either the X, Y or Z axis"));
	m_ZRotation->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_ZRotation, 0, wxALL, 2);

	wxStaticText* m_staticText88;
	m_staticText88 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("degrees"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText88->Wrap(-1);
	fgSizer14->Add(m_staticText88, 0, wxALL, 2);

	m_ResetRotation = new wxButton(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Reset Rotation"), wxDefaultPosition, wxDefaultSize, 0);
	m_ResetRotation->SetToolTip(wxT("Repositions object to original rotation"));

	fgSizer14->Add(m_ResetRotation, 0, wxALL, 2);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText89;
	m_staticText89 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("X Scale:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText89->Wrap(-1);
	fgSizer14->Add(m_staticText89, 0, wxALL, 2);

	m_XScale = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -1000000, 1000000, 100);
	m_XScale->SetToolTip(wxT("Modify the size of the Object"));
	m_XScale->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_XScale, 0, wxALL, 2);

	wxStaticText* m_staticText90;
	m_staticText90 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText90->Wrap(-1);
	fgSizer14->Add(m_staticText90, 0, wxALL, 2);

	wxStaticText* m_staticText91;
	m_staticText91 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Y Scale:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText91->Wrap(-1);
	fgSizer14->Add(m_staticText91, 0, wxALL, 2);

	m_YScale = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100000, 100000, 100);
	m_YScale->SetToolTip(wxT("Modify the size of the Object"));
	m_YScale->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_YScale, 0, wxALL, 2);

	wxStaticText* m_staticText92;
	m_staticText92 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText92->Wrap(-1);
	fgSizer14->Add(m_staticText92, 0, wxALL, 2);

	wxStaticText* m_staticText93;
	m_staticText93 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Z Scale:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText93->Wrap(-1);
	fgSizer14->Add(m_staticText93, 0, wxALL, 2);

	m_ZScale = new wxSpinCtrlDouble(sbSizer21->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -100000, 100000, 100);
	m_ZScale->SetToolTip(wxT("Modify the size of the Object"));
	m_ZScale->SetMaxSize(wxSize(70, -1));

	fgSizer14->Add(m_ZScale, 0, wxALL, 2);

	wxStaticText* m_staticText94;
	m_staticText94 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText94->Wrap(-1);
	fgSizer14->Add(m_staticText94, 0, wxALL, 2);

	m_uniformScale = new wxCheckBox(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Uniform Scale"), wxDefaultPosition, wxDefaultSize, 0);
	m_uniformScale->SetToolTip(wxT("Scale object the same amount in each direction"));

	fgSizer14->Add(m_uniformScale, 0, wxALL, 2);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);


	sbSizer21->Add(fgSizer14, 1, wxEXPAND, 2);

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer(wxHORIZONTAL);

	m_ResetScale = new wxButton(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Reset Scale"), wxDefaultPosition, wxDefaultSize, 0);
	m_ResetScale->SetToolTip(wxT("Resets object to original size"));

	bSizer41->Add(m_ResetScale, 0, wxALL, 2);

	m_scaleMax = new wxButton(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Maximize Scale"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer41->Add(m_scaleMax, 0, wxALL, 2);


	sbSizer21->Add(bSizer41, 0, 0, 0);


	bSizer16->Add(sbSizer21, 0, wxEXPAND, 2);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	m_Dupe = new wxButton(m_scrolledWindow1, wxID_ANY, wxT("Duplicate Object"), wxDefaultPosition, wxDefaultSize, 0);
	m_Dupe->Hide();

	bSizer4->Add(m_Dupe, 0, wxALL, 2);

	m_DeleteObj = new wxButton(m_scrolledWindow1, wxID_ANY, wxT("Delete Object"), wxDefaultPosition, wxDefaultSize, 0);
	m_DeleteObj->SetToolTip(wxT("Deletes object when selected and erases Properties window. Double click object to reopen"));

	bSizer4->Add(m_DeleteObj, 0, wxALL, 2);

	m_hideObj = new wxButton(m_scrolledWindow1, wxID_ANY, wxT("Hide Object"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(m_hideObj, 0, wxALL, 2);

	bSizer16->Add(bSizer4, 1, wxEXPAND, 5);

	m_HideProperties = new wxButton(m_scrolledWindow1, wxID_ANY, wxT("Hide Properties"), wxDefaultPosition, wxDefaultSize, 0);
	m_HideProperties->SetToolTip(wxT("Closes this window. Double click object to reopen"));

	bSizer16->Add(m_HideProperties, 0, wxALL | wxEXPAND, 5);


	m_scrolledWindow1->SetSizer(bSizer16);
	m_scrolledWindow1->Layout();
	bSizer16->Fit(m_scrolledWindow1);
	bSizer15->Add(m_scrolledWindow1, 1, wxEXPAND | wxALL, 2);


	this->SetSizer(bSizer15);
	this->Layout();

	// Connect Events
	m_DrawbleName->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(PropertiesPanel::UpdateName), NULL, this);
	m_ToolChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(PropertiesPanel::ToolChoose), NULL, this);
	m_UseTexture->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PropertiesPanel::OnUseTexture), NULL, this);
	m_UseCustomColour->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PropertiesPanel::OnUseCustomColour), NULL, this);
	m_CustomColourPick->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(PropertiesPanel::ColourChosen), NULL, this);
	m_PlaceOnBed->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::PlaceOnBed), NULL, this);
	m_XOffset->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XOffsetUpdate), NULL, this);
	m_YOffset->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YOffsetUpdate), NULL, this);
	m_ZOffset->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZOffsetUpdate), NULL, this);
	m_ResetPosition->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetPosition), NULL, this);
	m_XRotation->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XRotationUpdate), NULL, this);
	m_YRotation->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YRotationUpdate), NULL, this);
	m_ZRotation->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZRotationUpdate), NULL, this);
	m_ResetRotation->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetRotation), NULL, this);
	m_XScale->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XScaleUpdate), NULL, this);
	m_YScale->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YScaleUpdate), NULL, this);
	m_ZScale->Connect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZScaleUpdate), NULL, this);
	m_uniformScale->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PropertiesPanel::UniformScaleCheck), NULL, this);
	m_ResetScale->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetScale), NULL, this);
	m_scaleMax->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::scaleMaxButton), NULL, this);
	m_Dupe->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::DupeObject), NULL, this);
	m_DeleteObj->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::DeleteObj), NULL, this);
	m_hideObj->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::OnHideButton), NULL, this);
	m_HideProperties->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::HideProperties), NULL, this);
}

PropertiesPanel::~PropertiesPanel()
{
	// Disconnect Events
	m_DrawbleName->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(PropertiesPanel::UpdateName), NULL, this);
	m_ToolChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(PropertiesPanel::ToolChoose), NULL, this);
	m_UseTexture->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PropertiesPanel::OnUseTexture), NULL, this);
	m_UseCustomColour->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(PropertiesPanel::OnUseCustomColour), NULL, this);
	m_CustomColourPick->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(PropertiesPanel::ColourChosen), NULL, this);
	m_PlaceOnBed->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::PlaceOnBed), NULL, this);
	m_XOffset->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XOffsetUpdate), NULL, this);
	m_YOffset->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YOffsetUpdate), NULL, this);
	m_ZOffset->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZOffsetUpdate), NULL, this);
	m_ResetPosition->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetPosition), NULL, this);
	m_XRotation->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XRotationUpdate), NULL, this);
	m_YRotation->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YRotationUpdate), NULL, this);
	m_ZRotation->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZRotationUpdate), NULL, this);
	m_ResetRotation->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetRotation), NULL, this);
	m_XScale->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::XScaleUpdate), NULL, this);
	m_YScale->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::YScaleUpdate), NULL, this);
	m_ZScale->Disconnect(wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler(PropertiesPanel::ZScaleUpdate), NULL, this);
	m_uniformScale->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(PropertiesPanel::UniformScaleCheck), NULL, this);
	m_ResetScale->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::ResetScale), NULL, this);
	m_scaleMax->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::scaleMaxButton), NULL, this);
	m_Dupe->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::DupeObject), NULL, this);
	m_DeleteObj->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::DeleteObj), NULL, this);
	m_hideObj->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::OnHideButton), NULL, this);
	m_HideProperties->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PropertiesPanel::HideProperties), NULL, this);

}
