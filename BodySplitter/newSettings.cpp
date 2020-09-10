///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newSettings.h"

///////////////////////////////////////////////////////////////////////////

settingsDialog::settingsDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_notebook1 = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	sliceSettings = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	sliceSettings->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_listbook1 = new wxListbook(sliceSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_LEFT);
	m_listbook1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_layerSettings = new wxPanel(m_listbook1, wxID_ANY, wxPoint(-1, -1), wxDefaultSize, wxTAB_TRAVERSAL);
	m_layerSettings->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(m_layerSettings, wxID_ANY, wxT("Layer Thickness")), wxVERTICAL);

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer6->SetFlexibleDirection(wxBOTH);
	fgSizer6->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticLayerThickness = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("Normal Layer Thickness:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticLayerThickness->Wrap(-1);
	m_staticLayerThickness->SetMinSize(wxSize(200, -1));

	fgSizer6->Add(m_staticLayerThickness, 0, wxALL, 5);

	m_layerThickness = new wxTextCtrl(sbSizer1->GetStaticBox(), wxID_ANY, wxT("0.200"), wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL);
	m_layerThickness->SetToolTip(wxT("Except for the first layer, this is how thick each printed layer will be"));
	m_layerThickness->SetMaxSize(wxSize(80, -1));

	fgSizer6->Add(m_layerThickness, 0, wxALL, 5);

	m_staticText33 = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText33->Wrap(-1);
	fgSizer6->Add(m_staticText33, 0, wxALL, 5);

	m_staticText3 = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("First Layer Thickness:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	fgSizer6->Add(m_staticText3, 0, wxALL, 5);

	m_firstLayerThickbess = new wxTextCtrl(sbSizer1->GetStaticBox(), wxID_ANY, wxT("0.200"), wxDefaultPosition, wxDefaultSize, 0);
	m_firstLayerThickbess->SetToolTip(wxT("This is how thick your first printed layer will be"));
	m_firstLayerThickbess->SetMaxSize(wxSize(80, -1));

	fgSizer6->Add(m_firstLayerThickbess, 0, wxALL, 5);

	m_staticText34 = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText34->Wrap(-1);
	fgSizer6->Add(m_staticText34, 0, wxALL, 5);

	m_staticText1472 = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("First Layer Extrusion Multiplier:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1472->Wrap(-1);
	fgSizer6->Add(m_staticText1472, 0, wxALL, 5);

	m_firstLayerExtMultiplier = new wxSpinCtrl(sbSizer1->GetStaticBox(), wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 300, 0);
	m_firstLayerExtMultiplier->SetToolTip(wxT("Can be used to increase the extrusion amount on the first layer to help with print bed adhesion"));
	m_firstLayerExtMultiplier->SetMaxSize(wxSize(80, -1));

	fgSizer6->Add(m_firstLayerExtMultiplier, 0, wxALL, 5);

	m_staticText1482 = new wxStaticText(sbSizer1->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1482->Wrap(-1);
	fgSizer6->Add(m_staticText1482, 0, wxALL, 5);


	sbSizer1->Add(fgSizer6, 1, wxEXPAND, 5);


	bSizer3->Add(sbSizer1, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer(new wxStaticBox(m_layerSettings, wxID_ANY, wxT("Infill")), wxVERTICAL);

	wxFlexGridSizer* fgSizer20;
	fgSizer20 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer20->SetFlexibleDirection(wxBOTH);
	fgSizer20->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticLayerThickness1 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("Bottom Solid Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticLayerThickness1->Wrap(-1);
	m_staticLayerThickness1->SetMinSize(wxSize(200, -1));

	fgSizer20->Add(m_staticLayerThickness1, 0, wxALL, 5);

	m_BottomSolidLayers = new wxSpinCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 1);
	m_BottomSolidLayers->SetToolTip(wxT("The number of 100% infilled layers at the beginning of the print, before infill is used"));
	m_BottomSolidLayers->SetMaxSize(wxSize(80, -1));

	fgSizer20->Add(m_BottomSolidLayers, 0, wxALL, 5);


	fgSizer20->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText31 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("Top Solid Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText31->Wrap(-1);
	fgSizer20->Add(m_staticText31, 0, wxALL, 5);

	m_topSolidLayers = new wxSpinCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 1);
	m_topSolidLayers->SetToolTip(wxT("The number of 100% infilled layers at the end of the print"));
	m_topSolidLayers->SetMaxSize(wxSize(80, -1));

	fgSizer20->Add(m_topSolidLayers, 0, wxALL, 5);


	fgSizer20->Add(0, 0, 1, wxEXPAND, 5);

	m_advanced5 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("Infill Overlap Percentage:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced5->Wrap(-1);
	fgSizer20->Add(m_advanced5, 0, wxALL, 5);

	m_infillOverlap = new wxTextCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0);
	m_infillOverlap->SetToolTip(wxT("Amount of overlap on the perimters as a percentage of the extrusion width. Set this value higher if the adhesion between your infill and perimeters is poor or there is a visible gap on solid layers."));
	m_infillOverlap->SetMaxSize(wxSize(80, -1));

	fgSizer20->Add(m_infillOverlap, 0, wxALL, 5);

	m_advanced6 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced6->Wrap(-1);
	fgSizer20->Add(m_advanced6, 0, wxALL, 5);

	m_staticText8 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("Infill Percentage:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText8->Wrap(-1);
	fgSizer20->Add(m_staticText8, 0, wxALL, 5);

	m_infillPercentage = new wxComboBox(sbSizer2->GetStaticBox(), wxID_ANY, wxT("10%"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_infillPercentage->Append(wxT("0%"));
	m_infillPercentage->Append(wxT("10%"));
	m_infillPercentage->Append(wxT("15%"));
	m_infillPercentage->Append(wxT("20%"));
	m_infillPercentage->Append(wxT("25%"));
	m_infillPercentage->Append(wxT("30%"));
	m_infillPercentage->Append(wxT("40%"));
	m_infillPercentage->Append(wxT("50%"));
	m_infillPercentage->Append(wxT("60%"));
	m_infillPercentage->Append(wxT("70%"));
	m_infillPercentage->Append(wxT("80%"));
	m_infillPercentage->Append(wxT("90%"));
	m_infillPercentage->Append(wxT("100%"));
	m_infillPercentage->SetToolTip(wxT("The amount of area covered within each infill layer"));

	fgSizer20->Add(m_infillPercentage, 0, wxALL, 5);


	fgSizer20->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText245 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("Infill Extrusion Multiplier:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText245->Wrap(-1);
	fgSizer20->Add(m_staticText245, 0, wxALL, 5);

	m_InfillExtrusionMultiplier = new wxSpinCtrl(sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 500, 0);
	m_InfillExtrusionMultiplier->SetMaxSize(wxSize(80, -1));

	fgSizer20->Add(m_InfillExtrusionMultiplier, 0, wxALL, 5);

	m_staticText246 = new wxStaticText(sbSizer2->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText246->Wrap(-1);
	fgSizer20->Add(m_staticText246, 0, wxALL, 5);


	sbSizer2->Add(fgSizer20, 1, wxEXPAND, 5);


	bSizer3->Add(sbSizer2, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer(new wxStaticBox(m_layerSettings, wxID_ANY, wxT("Perimeters")), wxVERTICAL);

	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer12->SetFlexibleDirection(wxBOTH);
	fgSizer12->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText9 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Number of Perimeters:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText9->Wrap(-1);
	m_staticText9->SetMinSize(wxSize(200, -1));

	fgSizer12->Add(m_staticText9, 0, wxALL, 5);

	m_PerimeterNumber = new wxSpinCtrl(sbSizer3->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
	m_PerimeterNumber->SetToolTip(wxT("The number of solid outer shells surrounding the infill"));
	m_PerimeterNumber->SetMaxSize(wxSize(80, -1));

	fgSizer12->Add(m_PerimeterNumber, 0, wxALL, 5);


	fgSizer12->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText76;
	m_staticText76 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("XY Offset:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText76->Wrap(-1);
	fgSizer12->Add(m_staticText76, 0, wxALL, 5);

	m_XYOffset = new wxTextCtrl(sbSizer3->GetStaticBox(), wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, 0);
	m_XYOffset->SetToolTip(wxT("Allows islands to be resized to compensate for calibration issues. This only works for parts without texture mapping"));
	m_XYOffset->SetMaxSize(wxSize(80, -1));

	fgSizer12->Add(m_XYOffset, 0, wxALL, 5);

	wxStaticText* m_staticText77;
	m_staticText77 = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText77->Wrap(-1);
	fgSizer12->Add(m_staticText77, 0, wxALL, 5);

	m_infillFirst = new wxCheckBox(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Infill Before Perimeters"), wxDefaultPosition, wxDefaultSize, 0);
	m_infillFirst->SetToolTip(wxT("Prints the infill first and then the perimeters (Usually to prime before printing colour exterior)"));

	fgSizer12->Add(m_infillFirst, 0, wxALL, 5);


	fgSizer12->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer12->Add(0, 0, 1, wxEXPAND, 5);

	m_insideOut = new wxCheckBox(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Perimeters Inside Out"), wxDefaultPosition, wxDefaultSize, 0);
	m_insideOut->SetToolTip(wxT("Print the inside perimeter first and then the outside perimeter"));

	fgSizer12->Add(m_insideOut, 0, wxALL, 5);


	fgSizer12->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer12->Add(0, 0, 1, wxEXPAND, 5);

	m_NoSequenceIslands = new wxCheckBox(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Do all Perimeters/Infill at start"), wxDefaultPosition, wxDefaultSize, 0);
	m_NoSequenceIslands->SetToolTip(wxT("Print all perimters on the layer in the target colour before moving onto infill. If infill before perimeters is on, this is reversed."));

	fgSizer12->Add(m_NoSequenceIslands, 0, wxALL, 5);


	sbSizer3->Add(fgSizer12, 1, wxEXPAND, 5);


	bSizer3->Add(sbSizer3, 0, wxEXPAND, 5);


	bSizer6->Add(bSizer3, 1, wxEXPAND, 5);


	m_layerSettings->SetSizer(bSizer6);
	m_layerSettings->Layout();
	bSizer6->Fit(m_layerSettings);
	m_listbook1->AddPage(m_layerSettings, wxT("Layers"), true);
	m_colourSettings = new wxPanel(m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_colourSettings->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbColourSimplification;
	sbColourSimplification = new wxStaticBoxSizer(new wxStaticBox(m_colourSettings, wxID_ANY, wxT("Colour Simplification")), wxVERTICAL);

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer8->SetFlexibleDirection(wxBOTH);
	fgSizer8->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_fullModelSimplify = new wxCheckBox(sbColourSimplification->GetStaticBox(), wxID_ANY, wxT("Simplify Full Model"), wxDefaultPosition, wxDefaultSize, 0);
	m_fullModelSimplify->SetToolTip(wxT("Simplify the texture for the whole model using K-means (algorithm to group colours together)"));

	fgSizer8->Add(m_fullModelSimplify, 0, wxALL, 5);


	fgSizer8->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer8->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText36 = new wxStaticText(sbColourSimplification->GetStaticBox(), wxID_ANY, wxT("Full Model Colours:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText36->Wrap(-1);
	m_staticText36->SetMinSize(wxSize(200, -1));

	fgSizer8->Add(m_staticText36, 0, wxALL, 5);

	m_kMeansColours = new wxSpinCtrl(sbColourSimplification->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 200, 0);
	fgSizer8->Add(m_kMeansColours, 0, wxLEFT | wxRIGHT, 5);


	bSizer8->Add(fgSizer8, 1, wxEXPAND, 5);


	sbColourSimplification->Add(bSizer8, 1, wxEXPAND, 5);


	bSizer5->Add(sbColourSimplification, 0, wxEXPAND, 5);


	m_colourSettings->SetSizer(bSizer5);
	m_colourSettings->Layout();
	bSizer5->Fit(m_colourSettings);
	m_listbook1->AddPage(m_colourSettings, wxT("Colours"), false);
	m_coasterSettings = new wxPanel(m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_coasterSettings->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer81;
	sbSizer81 = new wxStaticBoxSizer(new wxStaticBox(m_coasterSettings, wxID_ANY, wxT("Base")), wxVERTICAL);

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer4->SetFlexibleDirection(wxBOTH);
	fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText21 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT("Base Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText21->Wrap(-1);
	m_staticText21->SetMinSize(wxSize(200, -1));

	fgSizer4->Add(m_staticText21, 0, wxALL, 5);

	m_coasterBase = new wxSpinCtrl(sbSizer81->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 1);
	m_coasterBase->SetToolTip(wxT("Prints solid layers of a single colour to help part adhere to the bed"));

	fgSizer4->Add(m_coasterBase, 0, wxALL, 5);

	m_staticText221 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT("Base Colour:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText221->Wrap(-1);
	fgSizer4->Add(m_staticText221, 0, wxALL, 5);

	m_coasterBaseColour = new wxColourPickerCtrl(sbSizer81->GetStaticBox(), wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_coasterBaseColour->SetToolTip(wxT("Colour of that base"));

	fgSizer4->Add(m_coasterBaseColour, 0, wxALL, 5);

	m_staticText23 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT("Base Opacity:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText23->Wrap(-1);
	fgSizer4->Add(m_staticText23, 0, wxALL, 5);

	m_baseOpacity = new wxComboBox(sbSizer81->GetStaticBox(), wxID_ANY, wxT("100%"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_baseOpacity->Append(wxT("0%"));
	m_baseOpacity->Append(wxT("10%"));
	m_baseOpacity->Append(wxT("20%"));
	m_baseOpacity->Append(wxT("30%"));
	m_baseOpacity->Append(wxT("40%"));
	m_baseOpacity->Append(wxT("50%"));
	m_baseOpacity->Append(wxT("60%"));
	m_baseOpacity->Append(wxT("70%"));
	m_baseOpacity->Append(wxT("80%"));
	m_baseOpacity->Append(wxT("90%"));
	m_baseOpacity->Append(wxT("100%"));
	m_baseOpacity->SetToolTip(wxT("How see through the base is"));

	fgSizer4->Add(m_baseOpacity, 0, wxALL, 5);


	sbSizer81->Add(fgSizer4, 1, wxEXPAND, 5);


	bSizer81->Add(sbSizer81, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer(new wxStaticBox(m_coasterSettings, wxID_ANY, wxT("Surface")), wxVERTICAL);

	wxFlexGridSizer* fgSizer51;
	fgSizer51 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer51->SetFlexibleDirection(wxBOTH);
	fgSizer51->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText24 = new wxStaticText(sbSizer9->GetStaticBox(), wxID_ANY, wxT("Surface Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText24->Wrap(-1);
	m_staticText24->SetMinSize(wxSize(200, -1));

	fgSizer51->Add(m_staticText24, 0, wxALL, 5);

	m_surfaceLayers = new wxSpinCtrl(sbSizer9->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0);
	m_surfaceLayers->SetToolTip(wxT("These are the textured layers with the image printed on them"));

	fgSizer51->Add(m_surfaceLayers, 0, wxALL, 5);

	m_staticText25 = new wxStaticText(sbSizer9->GetStaticBox(), wxID_ANY, wxT("Surface Opacity:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText25->Wrap(-1);
	fgSizer51->Add(m_staticText25, 0, wxALL, 5);

	m_surfaceOpacity = new wxComboBox(sbSizer9->GetStaticBox(), wxID_ANY, wxT("100%"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_surfaceOpacity->Append(wxT("0%"));
	m_surfaceOpacity->Append(wxT("10%"));
	m_surfaceOpacity->Append(wxT("20%"));
	m_surfaceOpacity->Append(wxT("30%"));
	m_surfaceOpacity->Append(wxT("40%"));
	m_surfaceOpacity->Append(wxT("50%"));
	m_surfaceOpacity->Append(wxT("60%"));
	m_surfaceOpacity->Append(wxT("70%"));
	m_surfaceOpacity->Append(wxT("80%"));
	m_surfaceOpacity->Append(wxT("90%"));
	m_surfaceOpacity->Append(wxT("100%"));
	m_surfaceOpacity->SetToolTip(wxT(" Percent of transparent filament to use"));

	fgSizer51->Add(m_surfaceOpacity, 0, wxALL, 5);


	sbSizer9->Add(fgSizer51, 1, wxEXPAND, 5);


	bSizer81->Add(sbSizer9, 0, wxEXPAND, 5);


	m_coasterSettings->SetSizer(bSizer81);
	m_coasterSettings->Layout();
	bSizer81->Fit(m_coasterSettings);
	m_listbook1->AddPage(m_coasterSettings, wxT("2.5D"), false);
	m_Support = new wxPanel(m_listbook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_Support->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	m_Support->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer(wxVERTICAL);

	m_checkSupport = new wxCheckBox(m_Support, wxID_ANY, wxT("Use Support"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkSupport->SetToolTip(wxT("Select whether support material is used or not"));

	bSizer15->Add(m_checkSupport, 0, wxALL, 5);

	wxStaticBoxSizer* sbSizer22;
	sbSizer22 = new wxStaticBoxSizer(new wxStaticBox(m_Support, wxID_ANY, wxT("Material")), wxVERTICAL);

	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer(0, 4, 0, 0);
	fgSizer14->SetFlexibleDirection(wxHORIZONTAL);
	fgSizer14->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	wxStaticText* m_staticText82;
	m_staticText82 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Main Extruder:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText82->Wrap(-1);
	fgSizer14->Add(m_staticText82, 0, wxALL, 5);

	wxString m_SupportExtChoices[] = { wxT("Full Colour"), wxT("Flexible"), wxT("Support") };
	int m_SupportExtNChoices = sizeof(m_SupportExtChoices) / sizeof(wxString);
	m_SupportExt = new wxChoice(sbSizer22->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_SupportExtNChoices, m_SupportExtChoices, 0);
	m_SupportExt->SetSelection(0);
	m_SupportExt->SetToolTip(wxT("Extruder to be used for producing support material"));

	fgSizer14->Add(m_SupportExt, 0, wxALL, 5);

	wxStaticText* m_staticText84;
	m_staticText84 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Interface Extruder:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText84->Wrap(-1);
	m_staticText84->SetMinSize(wxSize(90, -1));

	fgSizer14->Add(m_staticText84, 0, wxALL, 5);

	wxString m_InterfaceSupportChoices[] = { wxT("Full Colour"), wxT("Flexible"), wxT("Support") };
	int m_InterfaceSupportNChoices = sizeof(m_InterfaceSupportChoices) / sizeof(wxString);
	m_InterfaceSupport = new wxChoice(sbSizer22->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_InterfaceSupportNChoices, m_InterfaceSupportChoices, 0);
	m_InterfaceSupport->SetSelection(2);
	m_InterfaceSupport->SetToolTip(wxT("Material type to use as an interface between support and build"));

	fgSizer14->Add(m_InterfaceSupport, 0, wxALL, 5);

	wxStaticText* m_staticText86;
	m_staticText86 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Interface Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText86->Wrap(-1);
	m_staticText86->SetToolTip(wxT("Use a serperate material on the layers between the support pillar and the part. This allows one to save PVA for only the layers that need it."));

	fgSizer14->Add(m_staticText86, 0, wxALL, 5);

	m_InterfaceCount = new wxSpinCtrl(sbSizer22->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 500, 0);
	m_InterfaceCount->SetToolTip(wxT("Use a separate material on the layers between the support pillar and the part. This allows one to save PVA for only the layers that need it"));
	m_InterfaceCount->SetMaxSize(wxSize(60, -1));

	fgSizer14->Add(m_InterfaceCount, 0, wxALL, 5);

	wxStaticText* m_staticText88;
	m_staticText88 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Empty Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText88->Wrap(-1);
	fgSizer14->Add(m_staticText88, 0, wxALL, 5);

	m_EmptySupportLayers = new wxSpinCtrl(sbSizer22->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 500, 0);
	m_EmptySupportLayers->SetToolTip(wxT("Number of layers between support material and build"));
	m_EmptySupportLayers->SetMaxSize(wxSize(60, -1));

	fgSizer14->Add(m_EmptySupportLayers, 0, wxALL, 5);

	m_staticText252 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Base Material Interface Layers:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText252->Wrap(-1);
	fgSizer14->Add(m_staticText252, 0, wxALL, 5);

	m_baseMaterialInterface = new wxSpinCtrl(sbSizer22->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 500, 0);
	m_baseMaterialInterface->SetToolTip(wxT("Layers to use of the base material for interface before using the interface material"));
	m_baseMaterialInterface->SetMaxSize(wxSize(60, -1));

	fgSizer14->Add(m_baseMaterialInterface, 0, wxALL, 5);

	m_staticText95 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Infill Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText95->Wrap(-1);
	fgSizer14->Add(m_staticText95, 0, wxALL, 5);

	m_SupportDensity = new wxComboBox(sbSizer22->GetStaticBox(), wxID_ANY, wxT("25%"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_SupportDensity->Append(wxT("0%"));
	m_SupportDensity->Append(wxT("5%"));
	m_SupportDensity->Append(wxT("15%"));
	m_SupportDensity->Append(wxT("25%"));
	m_SupportDensity->Append(wxT("50%"));
	m_SupportDensity->Append(wxT("75%"));
	m_SupportDensity->Append(wxT("100%"));
	m_SupportDensity->SetToolTip(wxT("The amount of area covered within each layer of support"));

	fgSizer14->Add(m_SupportDensity, 0, wxALL, 5);

	wxStaticText* m_staticText891;
	m_staticText891 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Interface Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText891->Wrap(-1);
	fgSizer14->Add(m_staticText891, 0, wxALL, 5);

	m_InterfaceDensity = new wxComboBox(sbSizer22->GetStaticBox(), wxID_ANY, wxT("100%"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_InterfaceDensity->Append(wxT("0%"));
	m_InterfaceDensity->Append(wxT("5%"));
	m_InterfaceDensity->Append(wxT("15%"));
	m_InterfaceDensity->Append(wxT("25%"));
	m_InterfaceDensity->Append(wxT("50%"));
	m_InterfaceDensity->Append(wxT("75%"));
	m_InterfaceDensity->Append(wxT("100%"));
	m_InterfaceDensity->Enable(false);
	m_InterfaceDensity->SetToolTip(wxT("Density of infill of the interface"));

	fgSizer14->Add(m_InterfaceDensity, 0, wxALL, 5);

	wxStaticText* m_staticText87;
	m_staticText87 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Support Colour:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText87->Wrap(-1);
	fgSizer14->Add(m_staticText87, 0, wxALL, 5);

	m_SupportColour = new wxColourPickerCtrl(sbSizer22->GetStaticBox(), wxID_ANY, wxColour(255, 255, 255), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_SupportColour->SetToolTip(wxT("Specify a custom colour for the support materials when using the full colour extruder."));

	fgSizer14->Add(m_SupportColour, 0, wxALL, 5);


	sbSizer22->Add(fgSizer14, 1, wxEXPAND, 5);

	m_OverrideBaseInterface = new wxCheckBox(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Override Interface Layer on Base Plate"), wxDefaultPosition, wxDefaultSize, 0);
	m_OverrideBaseInterface->SetToolTip(wxT("If you have trouble with your interface material adhering to the bed surface you can specify a different material for that layer only."));

	sbSizer22->Add(m_OverrideBaseInterface, 0, wxALL, 5);

	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer16->SetFlexibleDirection(wxBOTH);
	fgSizer16->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText94;
	m_staticText94 = new wxStaticText(sbSizer22->GetStaticBox(), wxID_ANY, wxT("Base Extruder:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText94->Wrap(-1);
	m_staticText94->SetMinSize(wxSize(90, -1));

	fgSizer16->Add(m_staticText94, 0, wxALL, 5);

	wxString m_BaseInterfaceToolChoices[] = { wxT("Full Colour"), wxT("Flexible"), wxT("Support") };
	int m_BaseInterfaceToolNChoices = sizeof(m_BaseInterfaceToolChoices) / sizeof(wxString);
	m_BaseInterfaceTool = new wxChoice(sbSizer22->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_BaseInterfaceToolNChoices, m_BaseInterfaceToolChoices, 0);
	m_BaseInterfaceTool->SetSelection(0);
	m_BaseInterfaceTool->Enable(false);
	m_BaseInterfaceTool->SetToolTip(wxT("Extruder to be used for producing base material touching the build plate"));

	fgSizer16->Add(m_BaseInterfaceTool, 0, wxALL, 5);


	sbSizer22->Add(fgSizer16, 0, wxEXPAND, 5);


	bSizer15->Add(sbSizer22, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer241;
	sbSizer241 = new wxStaticBoxSizer(new wxStaticBox(m_Support, wxID_ANY, wxT("Geometry")), wxVERTICAL);

	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer15->SetFlexibleDirection(wxBOTH);
	fgSizer15->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText89;
	m_staticText89 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("Maximum Angle:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText89->Wrap(-1);
	fgSizer15->Add(m_staticText89, 0, wxALL, 5);

	m_maxSupportAngle = new wxTextCtrl(sbSizer241->GetStaticBox(), wxID_ANY, wxT("25"), wxDefaultPosition, wxDefaultSize, 0);
	m_maxSupportAngle->SetToolTip(wxT("Faces with an angle greater than this are considered for automatic support generation"));

	fgSizer15->Add(m_maxSupportAngle, 0, wxALL, 5);

	wxStaticText* m_staticText90;
	m_staticText90 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("degrees"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText90->Wrap(-1);
	fgSizer15->Add(m_staticText90, 0, wxALL, 5);

	m_staticText92 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("Offset From Part:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText92->Wrap(-1);
	fgSizer15->Add(m_staticText92, 0, wxALL, 5);

	m_SupportOffset = new wxTextCtrl(sbSizer241->GetStaticBox(), wxID_ANY, wxT("0.30"), wxDefaultPosition, wxDefaultSize, 0);
	m_SupportOffset->SetToolTip(wxT("Distance support material is set away from Build"));

	fgSizer15->Add(m_SupportOffset, 0, wxALL, 5);

	wxStaticText* m_staticText93;
	m_staticText93 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText93->Wrap(-1);
	fgSizer15->Add(m_staticText93, 0, wxALL, 5);

	m_staticText250 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("Minimum Hole Area:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText250->Wrap(-1);
	fgSizer15->Add(m_staticText250, 0, wxALL, 5);

	m_supportHoleArea = new wxTextCtrl(sbSizer241->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_supportHoleArea->SetToolTip(wxT("Holes with area less than this will be treated as solid when calculating the interface layer"));

	fgSizer15->Add(m_supportHoleArea, 0, wxALL, 5);

	m_staticText251 = new wxStaticText(sbSizer241->GetStaticBox(), wxID_ANY, wxT("mm2"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText251->Wrap(-1);
	fgSizer15->Add(m_staticText251, 0, wxALL, 5);

	m_PerciseCutSupport = new wxCheckBox(sbSizer241->GetStaticBox(), wxID_ANY, wxT("Precise Cut Object Borders"), wxDefaultPosition, wxDefaultSize, 0);
	m_PerciseCutSupport->SetToolTip(wxT("Avoid Creating support material that excedes the parts out most dimensions. Increases time to slice support."));

	fgSizer15->Add(m_PerciseCutSupport, 0, wxALL, 5);


	sbSizer241->Add(fgSizer15, 1, wxEXPAND, 5);


	bSizer15->Add(sbSizer241, 1, wxEXPAND, 5);


	m_Support->SetSizer(bSizer15);
	m_Support->Layout();
	bSizer15->Fit(m_Support);
	m_listbook1->AddPage(m_Support, wxT("Support"), false);
#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook1ListView = m_listbook1->GetListView();
	long m_listbook1Flags = m_listbook1ListView->GetWindowStyleFlag();
	if (m_listbook1Flags & wxLC_SMALL_ICON)
	{
		m_listbook1Flags = (m_listbook1Flags & ~wxLC_SMALL_ICON) | wxLC_ICON;
	}
	m_listbook1ListView->SetWindowStyleFlag(m_listbook1Flags);
#endif

	bSizer2->Add(m_listbook1, 1, wxALL | wxEXPAND, 5);


	sliceSettings->SetSizer(bSizer2);
	sliceSettings->Layout();
	bSizer2->Fit(sliceSettings);
	m_notebook1->AddPage(sliceSettings, wxT("Slice Settings"), true);
	m_printSettings = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_printSettings->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxVERTICAL);

	m_listbook2 = new wxListbook(m_printSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLB_DEFAULT);
	m_listbook2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	m_panel15 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel15->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(m_panel15, wxID_ANY, wxT("Printer Configuration")), wxVERTICAL);

	wxFlexGridSizer* fgSizer34;
	fgSizer34 = new wxFlexGridSizer(0, 5, 0, 0);
	fgSizer34->SetFlexibleDirection(wxBOTH);
	fgSizer34->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	fgSizer34->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText222;
	m_staticText222 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Full Colour"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText222->Wrap(-1);
	fgSizer34->Add(m_staticText222, 0, wxALL, 5);

	wxStaticText* m_staticText223;
	m_staticText223 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Support"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText223->Wrap(-1);
	fgSizer34->Add(m_staticText223, 0, wxALL, 5);

	wxStaticText* m_staticText224;
	m_staticText224 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Flex"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText224->Wrap(-1);
	fgSizer34->Add(m_staticText224, 0, wxALL, 5);


	fgSizer34->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText225;
	m_staticText225 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Nozzle Diameter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText225->Wrap(-1);
	fgSizer34->Add(m_staticText225, 0, wxALL, 5);

	m_t0Diameter = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("0.5"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t0Diameter, 0, wxALL, 5);

	m_t1Diameter = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("0.5"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t1Diameter, 0, wxALL, 5);

	m_t2Diameter = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t2Diameter, 0, wxALL, 5);

	wxStaticText* m_staticText226;
	m_staticText226 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText226->Wrap(-1);
	fgSizer34->Add(m_staticText226, 0, wxALL, 5);

	wxStaticText* m_staticText227;
	m_staticText227 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Extrusion Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText227->Wrap(-1);
	fgSizer34->Add(m_staticText227, 0, wxALL, 5);

	m_t0ExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("0.6"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t0ExtWidth, 0, wxALL, 5);

	m_t1ExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t1ExtWidth, 0, wxALL, 5);

	m_t2ExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t2ExtWidth, 0, wxALL, 5);

	wxStaticText* m_staticText228;
	m_staticText228 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText228->Wrap(-1);
	fgSizer34->Add(m_staticText228, 0, wxALL, 5);

	wxStaticText* m_staticText229;
	m_staticText229 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Min Extrusion Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText229->Wrap(-1);
	fgSizer34->Add(m_staticText229, 0, wxALL, 5);

	m_t0MinExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("0.4"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t0MinExtWidth, 0, wxALL, 5);

	m_t1MinExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t1MinExtWidth, 0, wxALL, 5);

	m_t2MinExtWidth = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t2MinExtWidth, 0, wxALL, 5);

	wxStaticText* m_staticText230;
	m_staticText230 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText230->Wrap(-1);
	fgSizer34->Add(m_staticText230, 0, wxALL, 5);

	wxStaticText* m_staticText231;
	m_staticText231 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Filament Diameter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText231->Wrap(-1);
	fgSizer34->Add(m_staticText231, 0, wxALL, 5);

	m_t0FilamentD = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t0FilamentD, 0, wxALL, 5);

	m_t1FilamentD = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t1FilamentD, 0, wxALL, 5);

	m_t2FilamentD = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t2FilamentD, 0, wxALL, 5);

	wxStaticText* m_staticText2321;
	m_staticText2321 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2321->Wrap(-1);
	fgSizer34->Add(m_staticText2321, 0, wxALL, 5);

	wxStaticText* m_staticText2331;
	m_staticText2331 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Extrusion Multiplier:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2331->Wrap(-1);
	fgSizer34->Add(m_staticText2331, 0, wxALL, 5);

	m_t0ExtMultiplier = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t0ExtMultiplier, 0, wxALL, 5);

	m_t1ExtMultiplier = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t1ExtMultiplier, 0, wxALL, 5);

	m_t2ExtMultiplier = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer34->Add(m_t2ExtMultiplier, 0, wxALL, 5);


	fgSizer34->Add(0, 0, 1, wxEXPAND, 5);


	sbSizer7->Add(fgSizer34, 1, wxEXPAND, 5);

	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer5->SetFlexibleDirection(wxBOTH);
	fgSizer5->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_purgeBinPrime = new wxCheckBox(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Use Purge Bin To Prime Tool"), wxDefaultPosition, wxDefaultSize, 0);
	m_purgeBinPrime->SetToolTip(wxT("Before using a tool for the first time in a print extrude material to the purge bin (RoVa4D)"));

	fgSizer5->Add(m_purgeBinPrime, 0, wxALL, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1241 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Distance to Extrude:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1241->Wrap(-1);
	fgSizer5->Add(m_staticText1241, 0, wxALL, 5);

	m_purgePrimeDistance = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgePrimeDistance->SetToolTip(wxT("Amount of material to purge before printing with the tool"));

	fgSizer5->Add(m_purgePrimeDistance, 0, wxALL, 5);

	m_staticText125 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText125->Wrap(-1);
	fgSizer5->Add(m_staticText125, 0, wxALL, 5);

	m_changeToolOverBin = new wxCheckBox(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Change Tool Over Purge Bin"), wxDefaultPosition, wxDefaultSize, 0);
	m_changeToolOverBin->SetValue(true);
	fgSizer5->Add(m_changeToolOverBin, 0, wxALL, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);

	m_purgeOnEachToolChange = new wxCheckBox(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Purge on Each Tool Change"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer5->Add(m_purgeOnEachToolChange, 0, wxALL, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer5->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText218 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Distance To purge On Tool Change:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText218->Wrap(-1);
	fgSizer5->Add(m_staticText218, 0, wxALL, 5);

	m_toolChangePurgeDistance = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer5->Add(m_toolChangePurgeDistance, 0, wxALL, 5);

	m_staticText219 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText219->Wrap(-1);
	fgSizer5->Add(m_staticText219, 0, wxALL, 5);

	m_staticText220 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("Tool Change Purge Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText220->Wrap(-1);
	m_staticText220->Hide();

	fgSizer5->Add(m_staticText220, 0, wxALL, 5);

	m_toolChangePurgeFeedrate = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_toolChangePurgeFeedrate->Hide();

	fgSizer5->Add(m_toolChangePurgeFeedrate, 0, wxALL, 5);

	m_staticText2211 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2211->Wrap(-1);
	m_staticText2211->Hide();

	fgSizer5->Add(m_staticText2211, 0, wxALL, 5);


	sbSizer7->Add(fgSizer5, 1, wxEXPAND, 5);


	m_panel15->SetSizer(sbSizer7);
	m_panel15->Layout();
	sbSizer7->Fit(m_panel15);
	m_listbook2->AddPage(m_panel15, wxT("Nozzle Configuration"), true);
	m_panel16 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel16->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer(new wxStaticBox(m_panel16, wxID_ANY, wxT("Print Configuration")), wxVERTICAL);

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer3->SetFlexibleDirection(wxBOTH);
	fgSizer3->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText15 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Colour Extruder Temperature:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15->Wrap(-1);
	m_staticText15->SetMinSize(wxSize(200, -1));

	fgSizer3->Add(m_staticText15, 0, wxALL, 5);

	m_T0_Temp = new wxTextCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxT("210"), wxDefaultPosition, wxDefaultSize, 0);
	m_T0_Temp->SetToolTip(wxT("Set temperature of the hotend for standard print material"));
	m_T0_Temp->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_T0_Temp, 0, wxALL, 5);

	m_staticText29 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("ºC"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText29->Wrap(-1);
	fgSizer3->Add(m_staticText29, 0, wxALL, 5);

	m_staticText17 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Support Temperature:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText17->Wrap(-1);
	fgSizer3->Add(m_staticText17, 0, wxALL, 5);

	m_T1_TEMP = new wxTextCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxT("200"), wxDefaultPosition, wxDefaultSize, 0);
	m_T1_TEMP->SetToolTip(wxT("Set temperature of the hotend for support material"));
	m_T1_TEMP->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_T1_TEMP, 0, wxALL, 5);

	m_staticText30 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("ºC"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText30->Wrap(-1);
	fgSizer3->Add(m_staticText30, 0, wxALL, 5);

	m_staticText19 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Flexible Material Temperature:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText19->Wrap(-1);
	fgSizer3->Add(m_staticText19, 0, wxALL, 5);

	m_T2_TEMP = new wxTextCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxT("200"), wxDefaultPosition, wxDefaultSize, 0);
	m_T2_TEMP->SetToolTip(wxT("Set temperature of the hotend for flexible material"));
	m_T2_TEMP->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_T2_TEMP, 0, wxALL, 5);

	m_staticText311 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("ºC"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText311->Wrap(-1);
	fgSizer3->Add(m_staticText311, 0, wxALL, 5);

	m_staticText20 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Bed Temperature:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText20->Wrap(-1);
	fgSizer3->Add(m_staticText20, 0, wxALL, 5);

	m_Bed_Temp = new wxTextCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxT("70"), wxDefaultPosition, wxDefaultSize, 0);
	m_Bed_Temp->SetToolTip(wxT("Set temperature of the print bed"));
	m_Bed_Temp->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_Bed_Temp, 0, wxALL, 5);

	m_staticText32 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("ºC"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText32->Wrap(-1);
	fgSizer3->Add(m_staticText32, 0, wxALL, 5);

	m_heatAndWait = new wxCheckBox(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Heat and Wait"), wxDefaultPosition, wxDefaultSize, 0);
	m_heatAndWait->SetToolTip(wxT("Set the temperature of the tool and bed, and wait for these temperatures to be reached before printing."));

	fgSizer3->Add(m_heatAndWait, 0, wxALL, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);

	m_useCoolingFan = new wxCheckBox(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Use Cooling Fan"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer3->Add(m_useCoolingFan, 0, wxALL, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);

	m_fanAlwaysOn = new wxCheckBox(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Keep Fan Always On Maximum"), wxDefaultPosition, wxDefaultSize, 0);
	m_fanAlwaysOn->SetToolTip(wxT("Sets fan to maximum power level"));

	fgSizer3->Add(m_fanAlwaysOn, 0, wxALL, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText170;
	m_staticText170 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("First Layer Fan Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText170->Wrap(-1);
	fgSizer3->Add(m_staticText170, 0, wxALL, 5);

	m_firstLayerFanSpeed = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	m_firstLayerFanSpeed->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_firstLayerFanSpeed, 0, wxALL, 5);

	wxStaticText* m_staticText171;
	m_staticText171 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText171->Wrap(-1);
	fgSizer3->Add(m_staticText171, 0, wxALL, 5);

	m_staticText1131 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Minimum Fan Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1131->Wrap(-1);
	fgSizer3->Add(m_staticText1131, 0, wxALL, 5);

	m_minFanSpeed = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	m_minFanSpeed->SetToolTip(wxT("Sets minimum operating level for fan "));
	m_minFanSpeed->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_minFanSpeed, 0, wxALL, 5);

	m_staticText1141 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1141->Wrap(-1);
	fgSizer3->Add(m_staticText1141, 0, wxALL, 5);

	m_staticText1111 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Maximum Fan Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1111->Wrap(-1);
	fgSizer3->Add(m_staticText1111, 0, wxALL, 5);

	m_fanSpeed = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0);
	m_fanSpeed->SetToolTip(wxT("Sets maximum operating level for fan "));
	m_fanSpeed->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_fanSpeed, 0, wxALL, 5);

	m_staticText1121 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1121->Wrap(-1);
	fgSizer3->Add(m_staticText1121, 0, wxALL, 5);

	m_reducePrintSpeed = new wxCheckBox(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Reduce Print Speed on Small Layers"), wxDefaultPosition, wxDefaultSize, 0);
	m_reducePrintSpeed->SetToolTip(wxT("Slows the print speed down for smaller parts"));

	fgSizer3->Add(m_reducePrintSpeed, 0, wxALL, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer3->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1151 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Minimum Layer Time:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1151->Wrap(-1);
	fgSizer3->Add(m_staticText1151, 0, wxALL, 5);

	m_minLayerTime = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 900, 10);
	m_minLayerTime->SetToolTip(wxT("Sets the minimum time spent to print smaller parts"));
	m_minLayerTime->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_minLayerTime, 0, wxALL, 5);

	m_staticText116 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("seconds"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText116->Wrap(-1);
	fgSizer3->Add(m_staticText116, 0, wxALL, 5);

	m_staticText117 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Minimum Fan Speed for layers longer:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText117->Wrap(-1);
	fgSizer3->Add(m_staticText117, 0, wxALL, 5);

	m_minLayerTimeFan = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 120);
	m_minLayerTimeFan->SetToolTip(wxT("Maintain minimum fan speed for layers taking longer than specified time to print"));
	m_minLayerTimeFan->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_minLayerTimeFan, 0, wxALL, 5);

	m_staticText118 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("seconds"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText118->Wrap(-1);
	fgSizer3->Add(m_staticText118, 0, wxALL, 5);

	m_staticText119 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Keep Printing Feedrates Above:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText119->Wrap(-1);
	fgSizer3->Add(m_staticText119, 0, wxALL, 5);

	m_minSlowDownFeedrate = new wxSpinCtrl(sbSizer8->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 200, 6000, 0);
	m_minSlowDownFeedrate->SetToolTip(wxT("Sets absolute minimum print speed"));
	m_minSlowDownFeedrate->SetMaxSize(wxSize(80, -1));

	fgSizer3->Add(m_minSlowDownFeedrate, 0, wxALL, 5);

	m_staticText120 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText120->Wrap(-1);
	fgSizer3->Add(m_staticText120, 0, wxALL, 5);


	sbSizer8->Add(fgSizer3, 1, wxEXPAND, 5);


	m_panel16->SetSizer(sbSizer8);
	m_panel16->Layout();
	sbSizer8->Fit(m_panel16);
	m_listbook2->AddPage(m_panel16, wxT("Temperatures"), false);
	m_panel17 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel17->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxStaticBoxSizer* sbSizer26;
	sbSizer26 = new wxStaticBoxSizer(new wxStaticBox(m_panel17, wxID_ANY, wxT("Colour Purge")), wxVERTICAL);

	wxFlexGridSizer* fgSizer19;
	fgSizer19 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer19->SetFlexibleDirection(wxBOTH);
	fgSizer19->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_AlwaysPurge = new wxCheckBox(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Always Purge"), wxDefaultPosition, wxDefaultSize, 0);
	m_AlwaysPurge->SetToolTip(wxT("Always purge on a colour change"));

	fgSizer19->Add(m_AlwaysPurge, 0, wxALL, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);

	m_useSoftwarePurge = new wxCheckBox(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Software Controlled Purge"), wxDefaultPosition, wxDefaultSize, 0);
	m_useSoftwarePurge->SetToolTip(wxT("Use software commands to direct the print head to the purge area"));

	fgSizer19->Add(m_useSoftwarePurge, 0, wxALL, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText100 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Length:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText100->Wrap(-1);
	fgSizer19->Add(m_staticText100, 0, wxALL, 5);

	m_purgeLength = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeLength->SetToolTip(wxT("Amount of filament to be extruded when purging"));

	fgSizer19->Add(m_purgeLength, 0, wxALL, 5);

	m_staticText101 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText101->Wrap(-1);
	fgSizer19->Add(m_staticText101, 0, wxALL, 5);

	m_staticText112 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText112->Wrap(-1);
	fgSizer19->Add(m_staticText112, 0, wxALL, 5);

	m_purgeFeedrate = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeFeedrate->SetToolTip(wxT("Feedrate of the purge movement, both movement within the purge bin and extrusion rate"));

	fgSizer19->Add(m_purgeFeedrate, 0, wxALL, 5);

	m_staticText113 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText113->Wrap(-1);
	fgSizer19->Add(m_staticText113, 0, wxALL, 5);

	m_staticText102 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Mixer Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText102->Wrap(-1);
	fgSizer19->Add(m_staticText102, 0, wxALL, 5);

	m_purgeMixerSpeed = new wxSpinCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 300, 0);
	m_purgeMixerSpeed->SetToolTip(wxT("Mixer speed during purge"));

	fgSizer19->Add(m_purgeMixerSpeed, 0, wxALL, 5);

	m_staticText103 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText103->Wrap(-1);
	fgSizer19->Add(m_staticText103, 0, wxALL, 5);

	m_staticText104 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Wait:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText104->Wrap(-1);
	fgSizer19->Add(m_staticText104, 0, wxALL, 5);

	m_purgeWait = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeWait->SetToolTip(wxT("Time to equalize pressure when purging"));

	fgSizer19->Add(m_purgeWait, 0, wxALL, 5);

	m_staticText105 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("milliseconds"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText105->Wrap(-1);
	fgSizer19->Add(m_staticText105, 0, wxALL, 5);

	m_staticText106 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Bin Location (x,y):"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText106->Wrap(-1);
	fgSizer19->Add(m_staticText106, 0, wxALL, 5);

	m_purgeLocationX = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeLocationX->SetToolTip(wxT("Purge bin location X-mm"));

	fgSizer19->Add(m_purgeLocationX, 0, wxALL, 5);

	m_purgeLocationY = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeLocationY->SetToolTip(wxT("Purge bin location Y-mm"));

	fgSizer19->Add(m_purgeLocationY, 0, wxALL, 5);

	m_staticText107 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Bin Size:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText107->Wrap(-1);
	fgSizer19->Add(m_staticText107, 0, wxALL, 5);

	m_purgeWidth = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeWidth->SetToolTip(wxT("Purge Bin Width (mm)"));

	fgSizer19->Add(m_purgeWidth, 0, wxALL, 5);

	m_purgeHeight = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeHeight->SetToolTip(wxT("Purge bin depth (mm)"));

	fgSizer19->Add(m_purgeHeight, 0, wxALL, 5);

	m_staticText108 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Wipe Location:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText108->Wrap(-1);
	fgSizer19->Add(m_staticText108, 0, wxALL, 5);

	m_purgwWipeX = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgwWipeX->SetToolTip(wxT("The location along the X-axis the wiper is located at"));

	fgSizer19->Add(m_purgwWipeX, 0, wxALL, 5);

	m_staticText109 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText109->Wrap(-1);
	fgSizer19->Add(m_staticText109, 0, wxALL, 5);

	m_staticText110 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Wiper Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText110->Wrap(-1);
	fgSizer19->Add(m_staticText110, 0, wxALL, 5);

	m_purgeWipeWidth = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeWipeWidth->SetToolTip(wxT("The width of the wiper (mm)"));

	fgSizer19->Add(m_purgeWipeWidth, 0, wxALL, 5);

	m_staticText111 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText111->Wrap(-1);
	fgSizer19->Add(m_staticText111, 0, wxALL, 5);

	m_staticText168 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Wipe Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText168->Wrap(-1);
	fgSizer19->Add(m_staticText168, 0, wxALL, 5);

	m_purgeWipeSpeed = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxT("4000"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer19->Add(m_purgeWipeSpeed, 0, wxALL, 5);

	m_staticText169 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText169->Wrap(-1);
	fgSizer19->Add(m_staticText169, 0, wxALL, 5);

	m_purgeForceRetract = new wxCheckBox(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Force Retract on Travel"), wxDefaultPosition, wxDefaultSize, 0);
	m_purgeForceRetract->SetToolTip(wxT("Always retract when moving to the purge bin regardless of distance"));

	fgSizer19->Add(m_purgeForceRetract, 0, wxALL, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);

	m_fullPurgeBinPause = new wxCheckBox(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Pause When Purge Bin Full"), wxDefaultPosition, wxDefaultSize, 0);
	m_fullPurgeBinPause->SetToolTip(wxT("Issue a pause command to printing if the purge bin becomes full"));

	fgSizer19->Add(m_fullPurgeBinPause, 0, wxALL, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer19->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1491 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("Purge Bin Capactity:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1491->Wrap(-1);
	fgSizer19->Add(m_staticText1491, 0, wxALL, 5);

	m_purgeBinCapacity = new wxTextCtrl(sbSizer26->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_purgeBinCapacity->SetToolTip(wxT("Amount of material the purge bin can hold"));

	fgSizer19->Add(m_purgeBinCapacity, 0, wxALL, 5);

	m_staticText150 = new wxStaticText(sbSizer26->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText150->Wrap(-1);
	fgSizer19->Add(m_staticText150, 0, wxALL, 5);


	sbSizer26->Add(fgSizer19, 1, wxEXPAND, 5);


	m_panel17->SetSizer(sbSizer26);
	m_panel17->Layout();
	sbSizer26->Fit(m_panel17);
	m_listbook2->AddPage(m_panel17, wxT("Purging"), false);
	m_panel18 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel18->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxFlexGridSizer* fgSizer191;
	fgSizer191 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer191->SetFlexibleDirection(wxBOTH);
	fgSizer191->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_LookAheadColourTrans = new wxCheckBox(m_panel18, wxID_ANY, wxT("Use Look Ahead Colour Transition"), wxDefaultPosition, wxDefaultSize, 0);
	m_LookAheadColourTrans->SetToolTip(wxT("Change the colour dynamically during print, to save time and material"));

	fgSizer191->Add(m_LookAheadColourTrans, 0, wxALL, 5);


	fgSizer191->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer191->Add(0, 0, 1, wxEXPAND, 5);

	m_purgeExcessColourChange = new wxCheckBox(m_panel18, wxID_ANY, wxT("Purge Excess"), wxDefaultPosition, wxDefaultSize, 0);
	m_purgeExcessColourChange->SetToolTip(wxT("If the colour change region is more material than can be used in the infill region, the remaining material will be purged in the bucket(RoVa4D). "));

	fgSizer191->Add(m_purgeExcessColourChange, 0, wxALL, 5);


	fgSizer191->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer191->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1191 = new wxStaticText(m_panel18, wxID_ANY, wxT("Distance before Colour Change Zone:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1191->Wrap(-1);
	fgSizer191->Add(m_staticText1191, 0, wxALL, 5);

	m_DistanceToColourChange = new wxTextCtrl(m_panel18, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_DistanceToColourChange->SetToolTip(wxT("This is the distance of filament that can be extruded before any change in colour is seen in the nozzle output"));

	fgSizer191->Add(m_DistanceToColourChange, 0, wxALL, 5);

	m_staticText1201 = new wxStaticText(m_panel18, wxID_ANY, wxT("mm of filament"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1201->Wrap(-1);
	fgSizer191->Add(m_staticText1201, 0, wxALL, 5);

	m_staticText121 = new wxStaticText(m_panel18, wxID_ANY, wxT("Distance of colour Change:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText121->Wrap(-1);
	fgSizer191->Add(m_staticText121, 0, wxALL, 5);

	m_ColourChangeMix = new wxTextCtrl(m_panel18, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_ColourChangeMix->SetToolTip(wxT("This is the distance in filament that it takes before the previous colour is cleared and the new colour remains"));

	fgSizer191->Add(m_ColourChangeMix, 0, wxALL, 5);

	m_staticText122 = new wxStaticText(m_panel18, wxID_ANY, wxT("mm of filament"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText122->Wrap(-1);
	fgSizer191->Add(m_staticText122, 0, wxALL, 5);


	m_panel18->SetSizer(fgSizer191);
	m_panel18->Layout();
	fgSizer191->Fit(m_panel18);
	m_listbook2->AddPage(m_panel18, wxT("Look Ahead"), false);
	m_panel20 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer23;
	fgSizer23 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer23->SetFlexibleDirection(wxBOTH);
	fgSizer23->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText1291 = new wxStaticText(m_panel20, wxID_ANY, wxT("Firmware Type:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1291->Wrap(-1);
	fgSizer23->Add(m_staticText1291, 0, wxALL, 5);

	wxString m_firmwareChoiceChoices[] = { wxT("RoVa4D"), wxT("RoVa3D"), wxT("Marlin") };
	int m_firmwareChoiceNChoices = sizeof(m_firmwareChoiceChoices) / sizeof(wxString);
	m_firmwareChoice = new wxChoice(m_panel20, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_firmwareChoiceNChoices, m_firmwareChoiceChoices, 0);
	m_firmwareChoice->SetSelection(0);
	m_firmwareChoice->SetToolTip(wxT("RoVa4D, RoVa3D or third party printer"));

	fgSizer23->Add(m_firmwareChoice, 0, wxALL, 5);


	fgSizer23->Add(0, 0, 1, wxEXPAND, 5);


	m_panel20->SetSizer(fgSizer23);
	m_panel20->Layout();
	fgSizer23->Fit(m_panel20);
	m_listbook2->AddPage(m_panel20, wxT("Firmware"), false);
	m_panel201 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel201->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer252;
	sbSizer252 = new wxStaticBoxSizer(new wxStaticBox(m_panel201, wxID_ANY, wxT("Material Properties")), wxVERTICAL);

	wxFlexGridSizer* fgSizer231;
	fgSizer231 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer231->AddGrowableCol(0);
	fgSizer231->AddGrowableCol(1);
	fgSizer231->SetFlexibleDirection(wxBOTH);
	fgSizer231->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticBoxSizer* sbSizer261;
	sbSizer261 = new wxStaticBoxSizer(new wxStaticBox(sbSizer252->GetStaticBox(), wxID_ANY, wxT("Colour Material")), wxVERTICAL);

	wxFlexGridSizer* fgSizer24;
	fgSizer24 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer24->SetFlexibleDirection(wxBOTH);
	fgSizer24->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText131 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("Material Cost:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText131->Wrap(-1);
	fgSizer24->Add(m_staticText131, 0, wxALL, 5);

	m_colourCost = new wxTextCtrl(sbSizer261->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_colourCost->SetToolTip(wxT("Cost per kg of filament, used to calculate print cost."));

	fgSizer24->Add(m_colourCost, 0, wxALL, 5);

	m_staticText132 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("$/kg"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText132->Wrap(-1);
	fgSizer24->Add(m_staticText132, 0, wxALL, 5);

	m_staticText135 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("Filament Diameter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText135->Wrap(-1);
	fgSizer24->Add(m_staticText135, 0, wxALL, 5);

	m_colourDiameter = new wxTextCtrl(sbSizer261->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	m_colourDiameter->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	m_colourDiameter->SetToolTip(wxT("The measured diameter of the filament, for best result find the average of multiple measurements"));

	fgSizer24->Add(m_colourDiameter, 0, wxALL, 5);

	m_staticText136 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText136->Wrap(-1);
	fgSizer24->Add(m_staticText136, 0, wxALL, 5);

	m_staticText139 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText139->Wrap(-1);
	fgSizer24->Add(m_staticText139, 0, wxALL, 5);

	m_colourDensity = new wxTextCtrl(sbSizer261->GetStaticBox(), wxID_ANY, wxT("1.25"), wxDefaultPosition, wxDefaultSize, 0);
	m_colourDensity->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
	m_colourDensity->SetToolTip(wxT("The density of the material in g/cm3"));

	fgSizer24->Add(m_colourDensity, 0, wxALL, 5);

	m_staticText140 = new wxStaticText(sbSizer261->GetStaticBox(), wxID_ANY, wxT("g/cm³"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText140->Wrap(-1);
	fgSizer24->Add(m_staticText140, 0, wxALL, 5);


	sbSizer261->Add(fgSizer24, 1, wxEXPAND, 5);


	fgSizer231->Add(sbSizer261, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer28;
	sbSizer28 = new wxStaticBoxSizer(new wxStaticBox(sbSizer252->GetStaticBox(), wxID_ANY, wxT("Support Material")), wxVERTICAL);

	wxFlexGridSizer* fgSizer25;
	fgSizer25 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer25->SetFlexibleDirection(wxBOTH);
	fgSizer25->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText133 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("Material Cost:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText133->Wrap(-1);
	fgSizer25->Add(m_staticText133, 0, wxALL, 5);

	m_supportCost = new wxTextCtrl(sbSizer28->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_supportCost->SetToolTip(wxT("Cost per kg of filament, used to calculate print cost."));

	fgSizer25->Add(m_supportCost, 0, wxALL, 5);

	m_staticText134 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("$/kg"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText134->Wrap(-1);
	fgSizer25->Add(m_staticText134, 0, wxALL, 5);

	m_staticText137 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("Filament Diameter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText137->Wrap(-1);
	fgSizer25->Add(m_staticText137, 0, wxALL, 5);

	m_supportDiameter = new wxTextCtrl(sbSizer28->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	m_supportDiameter->SetToolTip(wxT("The measured diameter of the filament, for best result find the average of multiple measurements"));

	fgSizer25->Add(m_supportDiameter, 0, wxALL, 5);

	m_staticText138 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText138->Wrap(-1);
	fgSizer25->Add(m_staticText138, 0, wxALL, 5);

	m_staticText141 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText141->Wrap(-1);
	fgSizer25->Add(m_staticText141, 0, wxALL, 5);

	m_supportDensity = new wxTextCtrl(sbSizer28->GetStaticBox(), wxID_ANY, wxT("1.19"), wxDefaultPosition, wxDefaultSize, 0);
	m_supportDensity->SetToolTip(wxT("The density of the material in g/cm3"));

	fgSizer25->Add(m_supportDensity, 0, wxALL, 5);

	m_staticText142 = new wxStaticText(sbSizer28->GetStaticBox(), wxID_ANY, wxT("g/cm³"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText142->Wrap(-1);
	fgSizer25->Add(m_staticText142, 0, wxALL, 5);


	sbSizer28->Add(fgSizer25, 1, wxEXPAND, 5);


	fgSizer231->Add(sbSizer28, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer29;
	sbSizer29 = new wxStaticBoxSizer(new wxStaticBox(sbSizer252->GetStaticBox(), wxID_ANY, wxT("Flexible Material")), wxVERTICAL);

	wxFlexGridSizer* fgSizer26;
	fgSizer26 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer26->SetFlexibleDirection(wxBOTH);
	fgSizer26->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText144 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("Material Cost:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText144->Wrap(-1);
	fgSizer26->Add(m_staticText144, 0, wxALL, 5);

	m_flexCost = new wxTextCtrl(sbSizer29->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_flexCost->SetToolTip(wxT("Cost per kg of filament, used to calculate print cost."));

	fgSizer26->Add(m_flexCost, 0, wxALL, 5);

	m_staticText145 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("$/kg"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText145->Wrap(-1);
	fgSizer26->Add(m_staticText145, 0, wxALL, 5);

	m_staticText146 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("Filament Diameter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText146->Wrap(-1);
	fgSizer26->Add(m_staticText146, 0, wxALL, 5);

	m_flexDiameter = new wxTextCtrl(sbSizer29->GetStaticBox(), wxID_ANY, wxT("1.75"), wxDefaultPosition, wxDefaultSize, 0);
	m_flexDiameter->SetToolTip(wxT("The measured diameter of the filament, for best result find the average of multiple measurements"));

	fgSizer26->Add(m_flexDiameter, 0, wxALL, 5);

	m_staticText147 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText147->Wrap(-1);
	fgSizer26->Add(m_staticText147, 0, wxALL, 5);

	m_staticText148 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText148->Wrap(-1);
	fgSizer26->Add(m_staticText148, 0, wxALL, 5);

	m_flexDensity = new wxTextCtrl(sbSizer29->GetStaticBox(), wxID_ANY, wxT("1.25"), wxDefaultPosition, wxDefaultSize, 0);
	m_flexDensity->SetToolTip(wxT("The density of the material in g/cm3"));

	fgSizer26->Add(m_flexDensity, 0, wxALL, 5);

	m_staticText149 = new wxStaticText(sbSizer29->GetStaticBox(), wxID_ANY, wxT("g/cm³"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText149->Wrap(-1);
	fgSizer26->Add(m_staticText149, 0, wxALL, 5);


	sbSizer29->Add(fgSizer26, 1, wxEXPAND, 5);


	fgSizer231->Add(sbSizer29, 1, wxEXPAND, 5);


	sbSizer252->Add(fgSizer231, 1, wxEXPAND, 5);


	bSizer16->Add(sbSizer252, 1, wxEXPAND, 5);


	m_panel201->SetSizer(bSizer16);
	m_panel201->Layout();
	bSizer16->Fit(m_panel201);
	m_listbook2->AddPage(m_panel201, wxT("Filament"), false);
	m_panel21 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_panel21->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer221;
	sbSizer221 = new wxStaticBoxSizer(new wxStaticBox(m_panel21, wxID_ANY, wxT("Multinozzle Compatibility")), wxVERTICAL);

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer17->SetFlexibleDirection(wxBOTH);
	fgSizer17->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_RovaCompat = new wxCheckBox(sbSizer221->GetStaticBox(), wxID_ANY, wxT("Enable Traditional Multitool Printing"), wxDefaultPosition, wxDefaultSize, 0);
	m_RovaCompat->SetToolTip(wxT("For Export to other multi-head printers"));

	fgSizer17->Add(m_RovaCompat, 0, wxALL, 5);


	fgSizer17->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText871;
	m_staticText871 = new wxStaticText(sbSizer221->GetStaticBox(), wxID_ANY, wxT("Extruders:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText871->Wrap(-1);
	fgSizer17->Add(m_staticText871, 0, wxALL, 5);

	m_ExtCount = new wxSpinCtrl(sbSizer221->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999, 0);
	m_ExtCount->SetToolTip(wxT("Number of Extruders to be used"));

	fgSizer17->Add(m_ExtCount, 0, wxLEFT, 5);


	sbSizer221->Add(fgSizer17, 0, wxEXPAND, 5);


	bSizer18->Add(sbSizer221, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer251;
	sbSizer251 = new wxStaticBoxSizer(new wxStaticBox(m_panel21, wxID_ANY, wxT("Colour Mixing Head")), wxVERTICAL);

	wxFlexGridSizer* fgSizer21;
	fgSizer21 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer21->SetFlexibleDirection(wxBOTH);
	fgSizer21->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText1251 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Head Type:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1251->Wrap(-1);
	fgSizer21->Add(m_staticText1251, 0, wxALL, 5);

	wxString m_headChoiceChoices[] = { wxT("RoVa4D - CMYK+WT"), wxT("Diamond Hotend - CMY"), wxT("Diamond Full Colour - CMYKW") };
	int m_headChoiceNChoices = sizeof(m_headChoiceChoices) / sizeof(wxString);
	m_headChoice = new wxChoice(sbSizer251->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_headChoiceNChoices, m_headChoiceChoices, 0);
	m_headChoice->SetSelection(0);
	m_headChoice->SetToolTip(wxT("Select mixing tool present. RoVa4D using Cyan, Magenta, Yellow, Black, White and transparent to print in full colour. Alternatively a diamond hotend can be used with Cyan, Magenta and Yellow filament to produce a limited spectrum of colours"));

	fgSizer21->Add(m_headChoice, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticText* m_staticText167;
	m_staticText167 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("CMY White:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText167->Wrap(-1);
	fgSizer21->Add(m_staticText167, 0, wxALL, 5);

	m_CMYWhiteSub = new wxColourPickerCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxColour(255, 255, 0), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_CMYWhiteSub->SetToolTip(wxT("CMY can't produce the colour white, choose a substition here"));

	fgSizer21->Add(m_CMYWhiteSub, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1451 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Mixer Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1451->Wrap(-1);
	fgSizer21->Add(m_staticText1451, 0, wxALL, 5);

	m_mixerSpeed = new wxSpinCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 400, 60);
	m_mixerSpeed->SetToolTip(wxT("Mixer speed when purge is complete"));

	fgSizer21->Add(m_mixerSpeed, 0, wxALL | wxEXPAND, 5);

	m_staticText1461 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1461->Wrap(-1);
	fgSizer21->Add(m_staticText1461, 0, wxALL, 5);

	m_stopMixerNotInUse = new wxCheckBox(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Stop Mixer When Not In Use"), wxDefaultPosition, wxDefaultSize, 0);
	m_stopMixerNotInUse->SetValue(true);
	m_stopMixerNotInUse->SetToolTip(wxT("Stop the mixer from running if printing using a different tool"));

	fgSizer21->Add(m_stopMixerNotInUse, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_useServo = new wxCheckBox(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Use Syringe Lift"), wxDefaultPosition, wxDefaultSize, 0);
	m_useServo->SetToolTip(wxT("Use the syringe control on the RoVa4D during retraction"));

	fgSizer21->Add(m_useServo, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText166 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Syringe Servo Index:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText166->Wrap(-1);
	fgSizer21->Add(m_staticText166, 0, wxALL, 5);

	m_syringeServoIndex = new wxSpinCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 4, 0);
	fgSizer21->Add(m_syringeServoIndex, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1471 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Syringe Lift Position:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1471->Wrap(-1);
	fgSizer21->Add(m_staticText1471, 0, wxALL, 5);

	m_ServoLift = new wxTextCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_ServoLift->SetToolTip(wxT("Syringe lift position"));

	fgSizer21->Add(m_ServoLift, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1481 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Syringe Issue Position:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1481->Wrap(-1);
	fgSizer21->Add(m_staticText1481, 0, wxALL, 5);

	m_ServoIssue = new wxTextCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_ServoIssue->SetToolTip(wxT("Syringe printing position"));

	fgSizer21->Add(m_ServoIssue, 0, wxALL, 5);


	fgSizer21->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1441 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Syringe Lift Time:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1441->Wrap(-1);
	fgSizer21->Add(m_staticText1441, 0, wxALL, 5);

	m_syringeLiftTime = new wxSpinCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 100);
	fgSizer21->Add(m_syringeLiftTime, 0, wxALL, 5);

	m_staticText1452 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("milliseconds"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1452->Wrap(-1);
	fgSizer21->Add(m_staticText1452, 0, wxALL, 5);

	m_staticText1462 = new wxStaticText(sbSizer251->GetStaticBox(), wxID_ANY, wxT("Syringe Issue Time:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1462->Wrap(-1);
	fgSizer21->Add(m_staticText1462, 0, wxALL, 5);

	m_syringeIssueTime = new wxSpinCtrl(sbSizer251->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 20000, 100);
	fgSizer21->Add(m_syringeIssueTime, 0, wxALL, 5);


	sbSizer251->Add(fgSizer21, 1, wxEXPAND, 5);


	bSizer18->Add(sbSizer251, 1, wxEXPAND, 5);


	m_panel21->SetSizer(bSizer18);
	m_panel21->Layout();
	bSizer18->Fit(m_panel21);
	m_listbook2->AddPage(m_panel21, wxT("Print Head"), false);
	m_panel22 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer181;
	bSizer181 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer291;
	sbSizer291 = new wxStaticBoxSizer(new wxStaticBox(m_panel22, wxID_ANY, wxT("First Layer Speed")), wxVERTICAL);

	wxFlexGridSizer* fgSizer27;
	fgSizer27 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer27->SetFlexibleDirection(wxBOTH);
	fgSizer27->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText152 = new wxStaticText(sbSizer291->GetStaticBox(), wxID_ANY, wxT("Print Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText152->Wrap(-1);
	m_staticText152->SetMinSize(wxSize(70, -1));

	fgSizer27->Add(m_staticText152, 0, wxALL, 5);

	m_firstLayerPrintSpeed = new wxTextCtrl(sbSizer291->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_firstLayerPrintSpeed->SetToolTip(wxT("All printing speeds on the first layer"));

	fgSizer27->Add(m_firstLayerPrintSpeed, 0, wxALL, 5);

	m_staticText153 = new wxStaticText(sbSizer291->GetStaticBox(), wxID_ANY, wxT("mm/s"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText153->Wrap(-1);
	fgSizer27->Add(m_staticText153, 0, wxALL, 5);


	sbSizer291->Add(fgSizer27, 1, wxEXPAND, 5);


	bSizer181->Add(sbSizer291, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer30;
	sbSizer30 = new wxStaticBoxSizer(new wxStaticBox(m_panel22, wxID_ANY, wxT("Print Speed")), wxVERTICAL);

	wxFlexGridSizer* fgSizer28;
	fgSizer28 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer28->SetFlexibleDirection(wxBOTH);
	fgSizer28->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText154 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("Perimeter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText154->Wrap(-1);
	m_staticText154->SetMinSize(wxSize(70, -1));

	fgSizer28->Add(m_staticText154, 0, wxALL, 5);

	m_perimSpeed = new wxTextCtrl(sbSizer30->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_perimSpeed->SetToolTip(wxT("Printing speed of perimeters"));

	fgSizer28->Add(m_perimSpeed, 0, wxALL, 5);

	m_staticText155 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("mm/s"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText155->Wrap(-1);
	fgSizer28->Add(m_staticText155, 0, wxALL, 5);

	m_staticText156 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("Infill:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText156->Wrap(-1);
	fgSizer28->Add(m_staticText156, 0, wxALL, 5);

	m_infillSpeed = new wxTextCtrl(sbSizer30->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_infillSpeed->SetToolTip(wxT("Printing Speed of infill"));

	fgSizer28->Add(m_infillSpeed, 0, wxALL, 5);

	m_staticText157 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("mm/s"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText157->Wrap(-1);
	fgSizer28->Add(m_staticText157, 0, wxALL, 5);

	m_staticText158 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("Travel:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText158->Wrap(-1);
	fgSizer28->Add(m_staticText158, 0, wxALL, 5);

	m_travelSpeed = new wxTextCtrl(sbSizer30->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_travelSpeed->SetToolTip(wxT("Speed of non printing moves"));

	fgSizer28->Add(m_travelSpeed, 0, wxALL, 5);

	m_staticText159 = new wxStaticText(sbSizer30->GetStaticBox(), wxID_ANY, wxT("mm/s"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText159->Wrap(-1);
	fgSizer28->Add(m_staticText159, 0, wxALL, 5);


	sbSizer30->Add(fgSizer28, 1, wxEXPAND, 5);


	bSizer181->Add(sbSizer30, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer31;
	sbSizer31 = new wxStaticBoxSizer(new wxStaticBox(m_panel22, wxID_ANY, wxT("Mixer Speed")), wxVERTICAL);

	wxFlexGridSizer* fgSizer29;
	fgSizer29 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer29->SetFlexibleDirection(wxBOTH);
	fgSizer29->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText1581 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("First Layer:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1581->Wrap(-1);
	m_staticText1581->SetMinSize(wxSize(70, -1));

	fgSizer29->Add(m_staticText1581, 0, wxALL, 5);

	m_firstLayerMixerSpeed = new wxTextCtrl(sbSizer31->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer29->Add(m_firstLayerMixerSpeed, 0, wxALL, 5);

	m_staticText1591 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1591->Wrap(-1);
	fgSizer29->Add(m_staticText1591, 0, wxALL, 5);

	m_staticText160 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("Perimeter:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText160->Wrap(-1);
	fgSizer29->Add(m_staticText160, 0, wxALL, 5);

	m_PerimeterMixerSpeed = new wxTextCtrl(sbSizer31->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer29->Add(m_PerimeterMixerSpeed, 0, wxALL, 5);

	m_staticText161 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText161->Wrap(-1);
	fgSizer29->Add(m_staticText161, 0, wxALL, 5);

	m_staticText164 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("Infill:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText164->Wrap(-1);
	fgSizer29->Add(m_staticText164, 0, wxALL, 5);

	m_InfillMixerSpeed = new wxTextCtrl(sbSizer31->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer29->Add(m_InfillMixerSpeed, 0, wxALL, 5);

	m_staticText165 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText165->Wrap(-1);
	fgSizer29->Add(m_staticText165, 0, wxALL, 5);

	m_staticText162 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("Travel:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText162->Wrap(-1);
	fgSizer29->Add(m_staticText162, 0, wxALL, 5);

	m_TravelMixerSpeed = new wxTextCtrl(sbSizer31->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer29->Add(m_TravelMixerSpeed, 0, wxALL, 5);

	m_staticText163 = new wxStaticText(sbSizer31->GetStaticBox(), wxID_ANY, wxT("RPM"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText163->Wrap(-1);
	fgSizer29->Add(m_staticText163, 0, wxALL, 5);


	sbSizer31->Add(fgSizer29, 1, wxEXPAND, 5);


	bSizer181->Add(sbSizer31, 1, wxEXPAND, 5);


	m_panel22->SetSizer(bSizer181);
	m_panel22->Layout();
	bSizer181->Fit(m_panel22);
	m_listbook2->AddPage(m_panel22, wxT("Speed"), false);
	m_panel25 = new wxPanel(m_listbook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer31->SetFlexibleDirection(wxBOTH);
	fgSizer31->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_EnableGCodeSplitUp = new wxCheckBox(m_panel25, wxID_ANY, wxT("Enable GCode Split Up"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer31->Add(m_EnableGCodeSplitUp, 0, wxALL, 5);


	fgSizer31->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer31->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText176 = new wxStaticText(m_panel25, wxID_ANY, wxT("Time Before Pause:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText176->Wrap(-1);
	fgSizer31->Add(m_staticText176, 0, wxALL, 5);

	m_GCodeFileTime = new wxTextCtrl(m_panel25, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer31->Add(m_GCodeFileTime, 0, wxALL, 5);

	m_staticText177 = new wxStaticText(m_panel25, wxID_ANY, wxT("hours"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText177->Wrap(-1);
	fgSizer31->Add(m_staticText177, 0, wxALL, 5);

	m_PuasePurgeBin = new wxCheckBox(m_panel25, wxID_ANY, wxT("Travel To Purge Bin"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer31->Add(m_PuasePurgeBin, 0, wxALL, 5);


	fgSizer31->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer31->Add(0, 0, 1, wxEXPAND, 5);

	m_purgeResume = new wxCheckBox(m_panel25, wxID_ANY, wxT("Purge On Resume"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer31->Add(m_purgeResume, 0, wxALL, 5);


	bSizer20->Add(fgSizer31, 1, wxEXPAND, 5);


	m_panel25->SetSizer(bSizer20);
	m_panel25->Layout();
	bSizer20->Fit(m_panel25);
	m_listbook2->AddPage(m_panel25, wxT("Print Suspend"), false);
#ifdef __WXGTK__ // Small icon style not supported in GTK
	wxListView* m_listbook2ListView = m_listbook2->GetListView();
	long m_listbook2Flags = m_listbook2ListView->GetWindowStyleFlag();
	if (m_listbook2Flags & wxLC_SMALL_ICON)
	{
		m_listbook2Flags = (m_listbook2Flags & ~wxLC_SMALL_ICON) | wxLC_ICON;
	}
	m_listbook2ListView->SetWindowStyleFlag(m_listbook2Flags);
#endif

	bSizer9->Add(m_listbook2, 1, wxEXPAND | wxALL, 5);


	m_printSettings->SetSizer(bSizer9);
	m_printSettings->Layout();
	bSizer9->Fit(m_printSettings);
	m_notebook1->AddPage(m_printSettings, wxT("Printer Settings"), false);
	m_UISettings = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer(new wxStaticBox(m_UISettings, wxID_ANY, wxT("UI Settings")), wxVERTICAL);

	wxFlexGridSizer* fgSizer61;
	fgSizer61 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer61->SetFlexibleDirection(wxBOTH);
	fgSizer61->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_simpleGCodeView = new wxCheckBox(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Low Quality GCode Preview"), wxDefaultPosition, wxDefaultSize, 0);
	m_simpleGCodeView->SetToolTip(wxT("Use a lower quality preview for the gocde visualization to reduce memory and CPU usage. Enable this if BodySplitter is crashing while generating the GCode Preview."));

	fgSizer61->Add(m_simpleGCodeView, 0, wxALL, 5);


	fgSizer61->Add(0, 0, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer(new wxStaticBox(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Grid Settings - Takes effect after restart")), wxVERTICAL);

	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer7->SetFlexibleDirection(wxBOTH);
	fgSizer7->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText291 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("Grid Width:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText291->Wrap(-1);
	m_staticText291->SetMinSize(wxSize(200, -1));

	fgSizer7->Add(m_staticText291, 0, wxALL, 5);

	m_gridWidth = new wxTextCtrl(sbSizer12->GetStaticBox(), wxID_ANY, wxT("340"), wxDefaultPosition, wxDefaultSize, 0);
	m_gridWidth->SetToolTip(wxT("X-Axis size setting"));

	fgSizer7->Add(m_gridWidth, 0, wxALL, 5);

	m_staticText301 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText301->Wrap(-1);
	fgSizer7->Add(m_staticText301, 0, wxALL, 5);

	m_staticText321 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("Grid Depth:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText321->Wrap(-1);
	fgSizer7->Add(m_staticText321, 0, wxALL, 5);

	m_gridDepth = new wxTextCtrl(sbSizer12->GetStaticBox(), wxID_ANY, wxT("340"), wxDefaultPosition, wxDefaultSize, 0);
	m_gridDepth->SetToolTip(wxT("Y-Axis size setting"));

	fgSizer7->Add(m_gridDepth, 0, wxALL, 5);

	m_staticText331 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText331->Wrap(-1);
	fgSizer7->Add(m_staticText331, 0, wxALL, 5);

	m_staticText123 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("Build Height:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText123->Wrap(-1);
	fgSizer7->Add(m_staticText123, 0, wxALL, 5);

	m_gridHeight = new wxTextCtrl(sbSizer12->GetStaticBox(), wxID_ANY, wxT("300"), wxDefaultPosition, wxDefaultSize, 0);
	m_gridHeight->SetToolTip(wxT("Z-Axis size setting"));

	fgSizer7->Add(m_gridHeight, 0, wxALL, 5);

	m_staticText124 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText124->Wrap(-1);
	fgSizer7->Add(m_staticText124, 0, wxALL, 5);

	m_staticText341 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("Grid Spacing:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText341->Wrap(-1);
	fgSizer7->Add(m_staticText341, 0, wxALL, 5);

	m_gridSpacing = new wxTextCtrl(sbSizer12->GetStaticBox(), wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, 0);
	m_gridSpacing->SetToolTip(wxT("Measured distance between grid lines"));

	fgSizer7->Add(m_gridSpacing, 0, wxALL, 5);

	m_staticText35 = new wxStaticText(sbSizer12->GetStaticBox(), wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText35->Wrap(-1);
	fgSizer7->Add(m_staticText35, 0, wxALL, 5);

	m_showAxis = new wxCheckBox(sbSizer12->GetStaticBox(), wxID_ANY, wxT("Show Axis"), wxDefaultPosition, wxDefaultSize, 0);
	m_showAxis->SetToolTip(wxT("Show’s Home for all Axes"));

	fgSizer7->Add(m_showAxis, 0, wxALL, 5);


	sbSizer12->Add(fgSizer7, 1, wxEXPAND, 5);


	fgSizer61->Add(sbSizer12, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer21;
	sbSizer21 = new wxStaticBoxSizer(new wxStaticBox(sbSizer10->GetStaticBox(), wxID_ANY, wxT("Custom Filament Colours")), wxVERTICAL);

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer13->SetFlexibleDirection(wxBOTH);
	fgSizer13->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText78;
	m_staticText78 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Flexible (T1):"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText78->Wrap(-1);
	fgSizer13->Add(m_staticText78, 0, wxALIGN_RIGHT | wxALL, 5);

	m_T1Colour = new wxColourPickerCtrl(sbSizer21->GetStaticBox(), wxID_ANY, wxColour(255, 0, 0), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_T1Colour->SetToolTip(wxT("Sets custom colour of flexible material used (GCode Preview)"));

	fgSizer13->Add(m_T1Colour, 0, wxALL, 5);

	wxStaticText* m_staticText80;
	m_staticText80 = new wxStaticText(sbSizer21->GetStaticBox(), wxID_ANY, wxT("Support (T2):"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText80->Wrap(-1);
	fgSizer13->Add(m_staticText80, 0, wxALIGN_RIGHT | wxALL, 5);

	m_T2Colour = new wxColourPickerCtrl(sbSizer21->GetStaticBox(), wxID_ANY, wxColour(240, 240, 240), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE);
	m_T2Colour->SetToolTip(wxT("Sets custom colour of support material used (GCode Preview)"));

	fgSizer13->Add(m_T2Colour, 0, wxALL, 5);


	sbSizer21->Add(fgSizer13, 1, wxEXPAND, 5);


	fgSizer61->Add(sbSizer21, 1, wxEXPAND, 5);


	sbSizer10->Add(fgSizer61, 1, wxEXPAND, 5);


	bSizer91->Add(sbSizer10, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer(new wxStaticBox(m_UISettings, wxID_ANY, wxT("Advanced/Experimental")), wxVERTICAL);

	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer(0, 1, 0, 0);
	fgSizer9->SetFlexibleDirection(wxBOTH);
	fgSizer9->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_checkExperimental = new wxCheckBox(sbSizer11->GetStaticBox(), wxID_ANY, wxT("Enable Experimental Features"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkExperimental->SetToolTip(wxT("The settings and features below are for testing/development only."));

	fgSizer9->Add(m_checkExperimental, 0, wxALL, 5);

	m_AdvancedPanel = new wxPanel(sbSizer11->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer(0, 6, 0, 0);
	fgSizer10->SetFlexibleDirection(wxBOTH);
	fgSizer10->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_advanced1 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("Minimum Line Length:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced1->Wrap(-1);
	fgSizer10->Add(m_advanced1, 0, wxALL, 5);

	m_minLineLength = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("0.0"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_minLineLength, 0, wxALL, 5);

	m_advanced2 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced2->Wrap(-1);
	fgSizer10->Add(m_advanced2, 0, wxALL, 5);

	m_advanced3 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("End Z:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced3->Wrap(-1);
	fgSizer10->Add(m_advanced3, 0, wxALL, 5);

	m_endZ = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("-1"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_endZ, 0, wxALL, 5);

	m_advanced4 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced4->Wrap(-1);
	fgSizer10->Add(m_advanced4, 0, wxALL, 5);

	m_advanced8 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("Top Surface Infill:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced8->Wrap(-1);
	fgSizer10->Add(m_advanced8, 0, wxALL, 5);

	m_topSurfaceInfill = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_topSurfaceInfill, 0, wxALL, 5);

	m_advanced9 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced9->Wrap(-1);
	fgSizer10->Add(m_advanced9, 0, wxALL, 5);

	m_advanced10 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("Surface Infill Density:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced10->Wrap(-1);
	fgSizer10->Add(m_advanced10, 0, wxALL, 5);

	m_SurfaceInfillDensity = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_SurfaceInfillDensity, 0, wxALL, 5);

	m_advanced11 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced11->Wrap(-1);
	fgSizer10->Add(m_advanced11, 0, wxALL, 5);

	m_advanced12 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("Bottom Layer Infill:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced12->Wrap(-1);
	fgSizer10->Add(m_advanced12, 0, wxALL, 5);

	m_BottomLayerInfill = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("100"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_BottomLayerInfill, 0, wxALL, 5);

	m_advanced13 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("%"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced13->Wrap(-1);
	fgSizer10->Add(m_advanced13, 0, wxALL, 5);

	m_advanced14 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("mm Per Pixel:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced14->Wrap(-1);
	m_advanced14->SetToolTip(wxT("Pixel grid size for insetting colour into model. A low value here will result in a very long slice."));

	fgSizer10->Add(m_advanced14, 0, wxALL, 5);

	m_mmPerPixel = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_mmPerPixel, 0, wxALL, 5);

	m_advanced15 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced15->Wrap(-1);
	fgSizer10->Add(m_advanced15, 0, wxALL, 5);

	m_advanced16 = new wxStaticText(m_AdvancedPanel, wxID_ANY, wxT("Thread Count:"), wxDefaultPosition, wxDefaultSize, 0);
	m_advanced16->Wrap(-1);
	fgSizer10->Add(m_advanced16, 0, wxALL, 5);

	m_ThreadCount = new wxTextCtrl(m_AdvancedPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer10->Add(m_ThreadCount, 0, wxALL, 5);


	fgSizer10->Add(0, 0, 1, wxEXPAND, 5);

	m_ColourPreviewByType = new wxCheckBox(m_AdvancedPanel, wxID_ANY, wxT("Colour Preview By Type"), wxDefaultPosition, wxDefaultSize, 0);
	m_ColourPreviewByType->SetToolTip(wxT("Changes the colour of the line in the gcode preview based on line type(infill,perimeter,support, etc)"));

	fgSizer10->Add(m_ColourPreviewByType, 0, wxALL, 5);


	fgSizer10->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer10->Add(0, 0, 1, wxEXPAND, 5);

	m_savePerimsToTest = new wxCheckBox(m_AdvancedPanel, wxID_ANY, wxT("Save Perimeters To Test File"), wxDefaultPosition, wxDefaultSize, 0);
	m_savePerimsToTest->Hide();

	fgSizer10->Add(m_savePerimsToTest, 0, wxALL, 5);


	fgSizer10->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer10->Add(0, 0, 1, wxEXPAND, 5);

	m_ConsiderSurfaces = new wxCheckBox(m_AdvancedPanel, wxID_ANY, wxT("Consider Surfaces"), wxDefaultPosition, wxDefaultSize, 0);
	m_ConsiderSurfaces->Hide();

	fgSizer10->Add(m_ConsiderSurfaces, 0, wxALL, 5);


	m_AdvancedPanel->SetSizer(fgSizer10);
	m_AdvancedPanel->Layout();
	fgSizer10->Fit(m_AdvancedPanel);
	fgSizer9->Add(m_AdvancedPanel, 1, wxEXPAND | wxALL, 5);


	sbSizer11->Add(fgSizer9, 1, wxEXPAND, 5);


	bSizer91->Add(sbSizer11, 0, wxEXPAND, 5);


	m_UISettings->SetSizer(bSizer91);
	m_UISettings->Layout();
	bSizer91->Fit(m_UISettings);
	m_notebook1->AddPage(m_UISettings, wxT("BodySplitter Settings"), false);
	mRetraction = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer(new wxStaticBox(mRetraction, wxID_ANY, wxT("Retraction")), wxVERTICAL);

	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer11->SetFlexibleDirection(wxBOTH);
	fgSizer11->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_fwRetraction = new wxCheckBox(sbSizer13->GetStaticBox(), wxID_ANY, wxT("Use Software Retraction"), wxDefaultPosition, wxDefaultSize, 0);
	m_fwRetraction->SetToolTip(wxT("Sets the software to control retraction functions as opposed to using the firmware commands G10/G11"));

	fgSizer11->Add(m_fwRetraction, 0, wxALL, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);

	m_retractLayerChange = new wxCheckBox(sbSizer13->GetStaticBox(), wxID_ANY, wxT("Retract On Layer Change"), wxDefaultPosition, wxDefaultSize, 0);
	m_retractLayerChange->SetToolTip(wxT("Always call a retraction when changing layers"));

	fgSizer11->Add(m_retractLayerChange, 0, wxALL, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);

	m_retractInfillCrossPerimOnly = new wxCheckBox(sbSizer13->GetStaticBox(), wxID_ANY, wxT("Retract Infill Only When Crossing Perimeters"), wxDefaultPosition, wxDefaultSize, 0);
	m_retractInfillCrossPerimOnly->SetToolTip(wxT("Only retract infill travel moves if the movement crosses a perimeter"));

	fgSizer11->Add(m_retractInfillCrossPerimOnly, 0, wxALL, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);

	m_onyRetractCrossingPerimeters = new wxCheckBox(sbSizer13->GetStaticBox(), wxID_ANY, wxT("Only Retract When Crossing Perimeters"), wxDefaultPosition, wxDefaultSize, 0);
	m_onyRetractCrossingPerimeters->SetToolTip(wxT("Only retract travel moves that cross a perimeter"));

	fgSizer11->Add(m_onyRetractCrossingPerimeters, 0, wxALL, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);


	fgSizer11->Add(0, 0, 1, wxEXPAND, 5);


	sbSizer13->Add(fgSizer11, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer(wxVERTICAL);

	m_notebook3 = new wxNotebook(sbSizer13->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_perToolRetraction = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer32;
	fgSizer32 = new wxFlexGridSizer(0, 6, 0, 0);
	fgSizer32->SetFlexibleDirection(wxBOTH);
	fgSizer32->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText58;
	m_staticText58 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Z Lift:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText58->Wrap(-1);
	fgSizer32->Add(m_staticText58, 0, wxALL, 5);

	m_ZLift0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_ZLift0->SetToolTip(wxT("Distance to lower bed/Raise the head on retraction, reduces likelihood of nozzle dragging"));

	fgSizer32->Add(m_ZLift0, 0, wxALL, 5);

	m_staticText59 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText59->Wrap(-1);
	fgSizer32->Add(m_staticText59, 0, wxALL, 5);

	m_staticText232 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Z Lift Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText232->Wrap(-1);
	fgSizer32->Add(m_staticText232, 0, wxALL, 5);

	m_ZLiftFeed0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer32->Add(m_ZLiftFeed0, 0, wxALL, 5);

	m_staticText233 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText233->Wrap(-1);
	fgSizer32->Add(m_staticText233, 0, wxALL, 5);

	m_staticText60 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Travel Distance Without Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText60->Wrap(-1);
	fgSizer32->Add(m_staticText60, 0, wxALL, 5);

	m_TravelWithoutRetract0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0);
	m_TravelWithoutRetract0->SetToolTip(wxT("Maximum non-printing distance allowed to travel without forcing filament retraction"));

	fgSizer32->Add(m_TravelWithoutRetract0, 0, wxALL, 5);

	wxStaticText* m_staticText61;
	m_staticText61 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText61->Wrap(-1);
	fgSizer32->Add(m_staticText61, 0, wxALL, 5);

	wxStaticText* m_staticText62;
	m_staticText62 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Coast Length:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText62->Wrap(-1);
	fgSizer32->Add(m_staticText62, 0, wxALL, 5);

	m_CoastLength0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_CoastLength0->SetToolTip(wxT("Stop extruding for this length of printing to relieve pressure on Bowden Tubes"));

	fgSizer32->Add(m_CoastLength0, 0, wxALL, 5);

	wxStaticText* m_staticText63;
	m_staticText63 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText63->Wrap(-1);
	fgSizer32->Add(m_staticText63, 0, wxALL, 5);

	m_staticText64 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Retract Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText64->Wrap(-1);
	fgSizer32->Add(m_staticText64, 0, wxALL, 5);

	m_RetractAmt0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractAmt0->SetToolTip(wxT("Length of filament to be retracted"));

	fgSizer32->Add(m_RetractAmt0, 0, wxALL, 5);

	wxStaticText* m_staticText65;
	m_staticText65 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText65->Wrap(-1);
	fgSizer32->Add(m_staticText65, 0, wxALL, 5);

	m_staticText66 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Issue Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText66->Wrap(-1);
	fgSizer32->Add(m_staticText66, 0, wxALL, 5);

	m_IssueAmt0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_IssueAmt0->SetToolTip(wxT("Filament to be re-extruded after a retraction"));

	fgSizer32->Add(m_IssueAmt0, 0, wxALL, 5);

	wxStaticText* m_staticText67;
	m_staticText67 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText67->Wrap(-1);
	fgSizer32->Add(m_staticText67, 0, wxALL, 5);

	m_staticText68 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Retract Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText68->Wrap(-1);
	fgSizer32->Add(m_staticText68, 0, wxALL, 5);

	m_RetractFeed0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractFeed0->SetToolTip(wxT("Speed of filament pull back on retraction"));

	fgSizer32->Add(m_RetractFeed0, 0, wxALL, 5);

	m_staticText69 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText69->Wrap(-1);
	fgSizer32->Add(m_staticText69, 0, wxALL, 5);

	wxStaticText* m_staticText70;
	m_staticText70 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Issue Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText70->Wrap(-1);
	fgSizer32->Add(m_staticText70, 0, wxALL, 5);

	m_Issue0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_Issue0->SetToolTip(wxT("Speed of filament extrusion"));

	fgSizer32->Add(m_Issue0, 0, wxALL, 5);

	wxStaticText* m_staticText72;
	m_staticText72 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText72->Wrap(-1);
	fgSizer32->Add(m_staticText72, 0, wxALL, 5);

	m_staticText73 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Wipe Distance:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText73->Wrap(-1);
	fgSizer32->Add(m_staticText73, 0, wxALL, 5);

	m_Wipe0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_Wipe0->SetToolTip(wxT("Distance to double back on the previous path before retracting"));

	fgSizer32->Add(m_Wipe0, 0, wxALL, 5);

	wxStaticText* m_staticText931;
	m_staticText931 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText931->Wrap(-1);
	fgSizer32->Add(m_staticText931, 0, wxALL, 5);

	m_staticText1561 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Wipe Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1561->Wrap(-1);
	fgSizer32->Add(m_staticText1561, 0, wxALL, 5);

	m_wipeFeedrate0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_wipeFeedrate0->SetToolTip(wxT("Feedrate of the wipe movement"));

	fgSizer32->Add(m_wipeFeedrate0, 0, wxALL, 5);

	m_staticText1571 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1571->Wrap(-1);
	fgSizer32->Add(m_staticText1571, 0, wxALL, 5);

	wxStaticText* m_staticText2341;
	m_staticText2341 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Tool Change Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2341->Wrap(-1);
	fgSizer32->Add(m_staticText2341, 0, wxALL, 5);

	m_ToolChangeRetractT0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer32->Add(m_ToolChangeRetractT0, 0, wxALL, 5);

	wxStaticText* m_staticText2351;
	m_staticText2351 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2351->Wrap(-1);
	fgSizer32->Add(m_staticText2351, 0, wxALL, 5);

	wxStaticText* m_staticText2361;
	m_staticText2361 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Tool Change Issue:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2361->Wrap(-1);
	fgSizer32->Add(m_staticText2361, 0, wxALL, 5);

	m_toolChangeIssueT0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer32->Add(m_toolChangeIssueT0, 0, wxALL, 5);

	wxStaticText* m_staticText2371;
	m_staticText2371 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2371->Wrap(-1);
	fgSizer32->Add(m_staticText2371, 0, wxALL, 5);

	m_staticText243 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("Print Distance Before Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText243->Wrap(-1);
	fgSizer32->Add(m_staticText243, 0, wxALL, 5);

	m_PrintDistanceRetractT0 = new wxTextCtrl(m_perToolRetraction, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer32->Add(m_PrintDistanceRetractT0, 0, wxALL, 5);

	wxStaticText* m_staticText244;
	m_staticText244 = new wxStaticText(m_perToolRetraction, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText244->Wrap(-1);
	fgSizer32->Add(m_staticText244, 0, wxALL, 5);


	m_perToolRetraction->SetSizer(fgSizer32);
	m_perToolRetraction->Layout();
	fgSizer32->Fit(m_perToolRetraction);
	m_notebook3->AddPage(m_perToolRetraction, wxT("Full Colour (T0)"), true);
	m_perToolRetraction1 = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer321;
	fgSizer321 = new wxFlexGridSizer(0, 6, 0, 0);
	fgSizer321->SetFlexibleDirection(wxBOTH);
	fgSizer321->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText581;
	m_staticText581 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Z Lift:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText581->Wrap(-1);
	fgSizer321->Add(m_staticText581, 0, wxALL, 5);

	m_ZLift1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_ZLift1->SetToolTip(wxT("Distance to lower bed/Raise the head on retraction, reduces likelihood of nozzle dragging"));

	fgSizer321->Add(m_ZLift1, 0, wxALL, 5);

	m_staticText591 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText591->Wrap(-1);
	fgSizer321->Add(m_staticText591, 0, wxALL, 5);

	m_staticText234 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Z Lift Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText234->Wrap(-1);
	fgSizer321->Add(m_staticText234, 0, wxALL, 5);

	m_ZLiftFeed1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer321->Add(m_ZLiftFeed1, 0, wxALL, 5);

	m_staticText235 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText235->Wrap(-1);
	fgSizer321->Add(m_staticText235, 0, wxALL, 5);

	m_staticText601 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Travel Distance Without Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText601->Wrap(-1);
	fgSizer321->Add(m_staticText601, 0, wxALL, 5);

	m_TravelWithoutRetract1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0);
	m_TravelWithoutRetract1->SetToolTip(wxT("Maximum non-printing distance allowed to travel without forcing filament retraction"));

	fgSizer321->Add(m_TravelWithoutRetract1, 0, wxALL, 5);

	wxStaticText* m_staticText611;
	m_staticText611 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText611->Wrap(-1);
	fgSizer321->Add(m_staticText611, 0, wxALL, 5);

	wxStaticText* m_staticText621;
	m_staticText621 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Coast Length:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText621->Wrap(-1);
	fgSizer321->Add(m_staticText621, 0, wxALL, 5);

	m_CoastLength1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_CoastLength1->SetToolTip(wxT("Stop extruding for this length of printing to relieve pressure on Bowden Tubes"));

	fgSizer321->Add(m_CoastLength1, 0, wxALL, 5);

	wxStaticText* m_staticText631;
	m_staticText631 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText631->Wrap(-1);
	fgSizer321->Add(m_staticText631, 0, wxALL, 5);

	m_staticText641 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Retract Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText641->Wrap(-1);
	fgSizer321->Add(m_staticText641, 0, wxALL, 5);

	m_RetractAmt1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractAmt1->SetToolTip(wxT("Length of filament to be retracted"));

	fgSizer321->Add(m_RetractAmt1, 0, wxALL, 5);

	wxStaticText* m_staticText651;
	m_staticText651 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText651->Wrap(-1);
	fgSizer321->Add(m_staticText651, 0, wxALL, 5);

	m_staticText661 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Issue Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText661->Wrap(-1);
	fgSizer321->Add(m_staticText661, 0, wxALL, 5);

	m_IssueAmt1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_IssueAmt1->SetToolTip(wxT("Filament to be re-extruded after a retraction"));

	fgSizer321->Add(m_IssueAmt1, 0, wxALL, 5);

	wxStaticText* m_staticText671;
	m_staticText671 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText671->Wrap(-1);
	fgSizer321->Add(m_staticText671, 0, wxALL, 5);

	m_staticText681 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Retract Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText681->Wrap(-1);
	fgSizer321->Add(m_staticText681, 0, wxALL, 5);

	m_RetractFeed1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractFeed1->SetToolTip(wxT("Speed of filament pull back on retraction"));

	fgSizer321->Add(m_RetractFeed1, 0, wxALL, 5);

	m_staticText691 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText691->Wrap(-1);
	fgSizer321->Add(m_staticText691, 0, wxALL, 5);

	wxStaticText* m_staticText701;
	m_staticText701 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Issue Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText701->Wrap(-1);
	fgSizer321->Add(m_staticText701, 0, wxALL, 5);

	m_Issue1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_Issue1->SetToolTip(wxT("Speed of filament extrusion"));

	fgSizer321->Add(m_Issue1, 0, wxALL, 5);

	wxStaticText* m_staticText721;
	m_staticText721 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText721->Wrap(-1);
	fgSizer321->Add(m_staticText721, 0, wxALL, 5);

	m_staticText732 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Wipe Distance:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText732->Wrap(-1);
	fgSizer321->Add(m_staticText732, 0, wxALL, 5);

	m_Wipe1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_Wipe1->SetToolTip(wxT("Distance to double back on the previous path before retracting"));

	fgSizer321->Add(m_Wipe1, 0, wxALL, 5);

	wxStaticText* m_staticText9311;
	m_staticText9311 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText9311->Wrap(-1);
	fgSizer321->Add(m_staticText9311, 0, wxALL, 5);

	m_staticText15611 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Wipe Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15611->Wrap(-1);
	fgSizer321->Add(m_staticText15611, 0, wxALL, 5);

	m_wipeFeedrate1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_wipeFeedrate1->SetToolTip(wxT("Feedrate of the wipe movement"));

	fgSizer321->Add(m_wipeFeedrate1, 0, wxALL, 5);

	m_staticText15711 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15711->Wrap(-1);
	fgSizer321->Add(m_staticText15711, 0, wxALL, 5);

	m_staticText239 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Tool Change Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText239->Wrap(-1);
	fgSizer321->Add(m_staticText239, 0, wxALL, 5);

	m_ToolChangeRetractT1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer321->Add(m_ToolChangeRetractT1, 0, wxALL, 5);

	wxStaticText* m_staticText242;
	m_staticText242 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText242->Wrap(-1);
	fgSizer321->Add(m_staticText242, 0, wxALL, 5);

	wxStaticText* m_staticText23611;
	m_staticText23611 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Tool Change Issue:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText23611->Wrap(-1);
	fgSizer321->Add(m_staticText23611, 0, wxALL, 5);

	m_toolChangeIssueT1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer321->Add(m_toolChangeIssueT1, 0, wxALL, 5);

	wxStaticText* m_staticText249;
	m_staticText249 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText249->Wrap(-1);
	fgSizer321->Add(m_staticText249, 0, wxALL, 5);

	wxStaticText* m_staticText2411;
	m_staticText2411 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("Print Distance Before Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2411->Wrap(-1);
	fgSizer321->Add(m_staticText2411, 0, wxALL, 5);

	m_PrintDistanceRetractT1 = new wxTextCtrl(m_perToolRetraction1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer321->Add(m_PrintDistanceRetractT1, 0, wxALL, 5);

	wxStaticText* m_staticText2421;
	m_staticText2421 = new wxStaticText(m_perToolRetraction1, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2421->Wrap(-1);
	fgSizer321->Add(m_staticText2421, 0, wxALL, 5);


	m_perToolRetraction1->SetSizer(fgSizer321);
	m_perToolRetraction1->Layout();
	fgSizer321->Fit(m_perToolRetraction1);
	m_notebook3->AddPage(m_perToolRetraction1, wxT("Flexible (T1)"), false);
	m_perToolRetraction2 = new wxPanel(m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxFlexGridSizer* fgSizer322;
	fgSizer322 = new wxFlexGridSizer(0, 6, 0, 0);
	fgSizer322->SetFlexibleDirection(wxBOTH);
	fgSizer322->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticText* m_staticText582;
	m_staticText582 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Z Lift:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText582->Wrap(-1);
	fgSizer322->Add(m_staticText582, 0, wxALL, 5);

	m_ZLift2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_ZLift2->SetToolTip(wxT("Distance to lower bed/Raise the head on retraction, reduces likelihood of nozzle dragging"));

	fgSizer322->Add(m_ZLift2, 0, wxALL, 5);

	m_staticText592 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText592->Wrap(-1);
	fgSizer322->Add(m_staticText592, 0, wxALL, 5);

	m_staticText236 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Z Lift Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText236->Wrap(-1);
	fgSizer322->Add(m_staticText236, 0, wxALL, 5);

	m_ZLiftFeed2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer322->Add(m_ZLiftFeed2, 0, wxALL, 5);

	m_staticText237 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText237->Wrap(-1);
	fgSizer322->Add(m_staticText237, 0, wxALL, 5);

	m_staticText602 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Travel Distance Without Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText602->Wrap(-1);
	fgSizer322->Add(m_staticText602, 0, wxALL, 5);

	m_TravelWithoutRetract2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, 0);
	m_TravelWithoutRetract2->SetToolTip(wxT("Maximum non-printing distance allowed to travel without forcing filament retraction"));

	fgSizer322->Add(m_TravelWithoutRetract2, 0, wxALL, 5);

	wxStaticText* m_staticText612;
	m_staticText612 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText612->Wrap(-1);
	fgSizer322->Add(m_staticText612, 0, wxALL, 5);

	wxStaticText* m_staticText622;
	m_staticText622 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Coast Length:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText622->Wrap(-1);
	fgSizer322->Add(m_staticText622, 0, wxALL, 5);

	m_CoastLength2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_CoastLength2->SetToolTip(wxT("Stop extruding for this length of printing to relieve pressure on Bowden Tubes"));

	fgSizer322->Add(m_CoastLength2, 0, wxALL, 5);

	wxStaticText* m_staticText632;
	m_staticText632 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText632->Wrap(-1);
	fgSizer322->Add(m_staticText632, 0, wxALL, 5);

	m_staticText642 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Retract Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText642->Wrap(-1);
	fgSizer322->Add(m_staticText642, 0, wxALL, 5);

	m_RetractAmt2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractAmt2->SetToolTip(wxT("Length of filament to be retracted"));

	fgSizer322->Add(m_RetractAmt2, 0, wxALL, 5);

	wxStaticText* m_staticText652;
	m_staticText652 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText652->Wrap(-1);
	fgSizer322->Add(m_staticText652, 0, wxALL, 5);

	m_staticText662 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Issue Amount:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText662->Wrap(-1);
	fgSizer322->Add(m_staticText662, 0, wxALL, 5);

	m_IssueAmt2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_IssueAmt2->SetToolTip(wxT("Filament to be re-extruded after a retraction"));

	fgSizer322->Add(m_IssueAmt2, 0, wxALL, 5);

	wxStaticText* m_staticText672;
	m_staticText672 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText672->Wrap(-1);
	fgSizer322->Add(m_staticText672, 0, wxALL, 5);

	m_staticText682 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Retract Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText682->Wrap(-1);
	fgSizer322->Add(m_staticText682, 0, wxALL, 5);

	m_RetractFeed2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_RetractFeed2->SetToolTip(wxT("Speed of filament pull back on retraction"));

	fgSizer322->Add(m_RetractFeed2, 0, wxALL, 5);

	m_staticText692 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText692->Wrap(-1);
	fgSizer322->Add(m_staticText692, 0, wxALL, 5);

	wxStaticText* m_staticText702;
	m_staticText702 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Issue Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText702->Wrap(-1);
	fgSizer322->Add(m_staticText702, 0, wxALL, 5);

	m_Issue2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("4800"), wxDefaultPosition, wxDefaultSize, 0);
	m_Issue2->SetToolTip(wxT("Speed of filament extrusion"));

	fgSizer322->Add(m_Issue2, 0, wxALL, 5);

	wxStaticText* m_staticText722;
	m_staticText722 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText722->Wrap(-1);
	fgSizer322->Add(m_staticText722, 0, wxALL, 5);

	m_staticText733 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Wipe Distance:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText733->Wrap(-1);
	fgSizer322->Add(m_staticText733, 0, wxALL, 5);

	m_Wipe2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_Wipe2->SetToolTip(wxT("Distance to double back on the previous path before retracting"));

	fgSizer322->Add(m_Wipe2, 0, wxALL, 5);

	wxStaticText* m_staticText9312;
	m_staticText9312 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText9312->Wrap(-1);
	fgSizer322->Add(m_staticText9312, 0, wxALL, 5);

	m_staticText15612 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Wipe Feedrate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15612->Wrap(-1);
	fgSizer322->Add(m_staticText15612, 0, wxALL, 5);

	m_wipeFeedrate2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_wipeFeedrate2->SetToolTip(wxT("Feedrate of the wipe movement"));

	fgSizer322->Add(m_wipeFeedrate2, 0, wxALL, 5);

	m_staticText15712 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm/min"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText15712->Wrap(-1);
	fgSizer322->Add(m_staticText15712, 0, wxALL, 5);

	m_staticText240 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Tool Change Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText240->Wrap(-1);
	fgSizer322->Add(m_staticText240, 0, wxALL, 5);

	m_ToolChangeRetractT2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer322->Add(m_ToolChangeRetractT2, 0, wxALL, 5);

	wxStaticText* m_staticText241;
	m_staticText241 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText241->Wrap(-1);
	fgSizer322->Add(m_staticText241, 0, wxALL, 5);

	wxStaticText* m_staticText23612;
	m_staticText23612 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Tool Change Issue:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText23612->Wrap(-1);
	fgSizer322->Add(m_staticText23612, 0, wxALL, 5);

	m_toolChangeIssueT2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer322->Add(m_toolChangeIssueT2, 0, wxALL, 5);

	wxStaticText* m_staticText248;
	m_staticText248 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText248->Wrap(-1);
	fgSizer322->Add(m_staticText248, 0, wxALL, 5);

	wxStaticText* m_staticText2391;
	m_staticText2391 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("Print Distance Before Retract:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2391->Wrap(-1);
	fgSizer322->Add(m_staticText2391, 0, wxALL, 5);

	m_PrintDistanceRetractT2 = new wxTextCtrl(m_perToolRetraction2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer322->Add(m_PrintDistanceRetractT2, 0, wxALL, 5);

	wxStaticText* m_staticText2401;
	m_staticText2401 = new wxStaticText(m_perToolRetraction2, wxID_ANY, wxT("mm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2401->Wrap(-1);
	fgSizer322->Add(m_staticText2401, 0, wxALL, 5);


	m_perToolRetraction2->SetSizer(fgSizer322);
	m_perToolRetraction2->Layout();
	fgSizer322->Fit(m_perToolRetraction2);
	m_notebook3->AddPage(m_perToolRetraction2, wxT("Support (T2)"), false);

	bSizer21->Add(m_notebook3, 1, wxEXPAND | wxALL, 5);


	sbSizer13->Add(bSizer21, 1, wxEXPAND, 5);


	bSizer11->Add(sbSizer13, 1, wxEXPAND, 5);


	mRetraction->SetSizer(bSizer11);
	mRetraction->Layout();
	bSizer11->Fit(mRetraction);
	m_notebook1->AddPage(mRetraction, wxT("Retraction"), false);
	m_gcodeStrings = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer(wxVERTICAL);

	m_notebook2 = new wxNotebook(m_gcodeStrings, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
	m_panel10 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbSizer17;
	sbSizer17 = new wxStaticBoxSizer(new wxStaticBox(m_panel10, wxID_ANY, wxT("Start GCode")), wxVERTICAL);

	m_OnStart = new wxTextCtrl(sbSizer17->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_OnStart->SetToolTip(wxT("Preset Gcode to perform at the beginning of any print"));

	sbSizer17->Add(m_OnStart, 1, wxALL | wxEXPAND, 5);


	m_panel10->SetSizer(sbSizer17);
	m_panel10->Layout();
	sbSizer17->Fit(m_panel10);
	m_notebook2->AddPage(m_panel10, wxT("Start"), false);
	m_panel11 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxStaticBoxSizer* sbSizer18;
	sbSizer18 = new wxStaticBoxSizer(new wxStaticBox(m_panel11, wxID_ANY, wxT("End GCode")), wxVERTICAL);

	m_OnEndGCode = new wxTextCtrl(sbSizer18->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_OnEndGCode->SetToolTip(wxT("Preset Gcode to perform at the end of any print"));

	sbSizer18->Add(m_OnEndGCode, 1, wxALL | wxEXPAND, 5);


	m_panel11->SetSizer(sbSizer18);
	m_panel11->Layout();
	sbSizer18->Fit(m_panel11);
	m_notebook2->AddPage(m_panel11, wxT("End"), false);
	m_panel12 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer24;
	sbSizer24 = new wxStaticBoxSizer(new wxStaticBox(m_panel12, wxID_ANY, wxT("Before Z Move")), wxVERTICAL);

	m_BeforeZMove = new wxTextCtrl(sbSizer24->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_BeforeZMove->SetToolTip(wxT("Preset Gcode to perform between layer changes"));

	sbSizer24->Add(m_BeforeZMove, 1, wxALL | wxEXPAND, 5);


	bSizer14->Add(sbSizer24, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer25;
	sbSizer25 = new wxStaticBoxSizer(new wxStaticBox(m_panel12, wxID_ANY, wxT("After Z Move")), wxVERTICAL);

	m_OnAfterZMove = new wxTextCtrl(sbSizer25->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	sbSizer25->Add(m_OnAfterZMove, 1, wxALL | wxEXPAND, 5);


	bSizer14->Add(sbSizer25, 1, wxEXPAND, 5);


	m_panel12->SetSizer(bSizer14);
	m_panel12->Layout();
	bSizer14->Fit(m_panel12);
	m_notebook2->AddPage(m_panel12, wxT("Layer Change"), false);
	m_panel13 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer14;
	sbSizer14 = new wxStaticBoxSizer(new wxStaticBox(m_panel13, wxID_ANY, wxT("On Retract")), wxVERTICAL);

	m_OnRetractStrings = new wxTextCtrl(sbSizer14->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_OnRetractStrings->SetToolTip(wxT("Preset Gcode to perform on retraction/extrusion"));

	sbSizer14->Add(m_OnRetractStrings, 1, wxALL | wxEXPAND, 5);


	bSizer13->Add(sbSizer14, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer16;
	sbSizer16 = new wxStaticBoxSizer(new wxStaticBox(m_panel13, wxID_ANY, wxT("On Issue")), wxVERTICAL);

	m_OnIssue = new wxTextCtrl(sbSizer16->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_OnIssue->SetToolTip(wxT("Preset Gcode to perform on retraction/extrusion"));

	sbSizer16->Add(m_OnIssue, 1, wxALL | wxEXPAND, 5);


	bSizer13->Add(sbSizer16, 1, wxEXPAND, 5);


	m_panel13->SetSizer(bSizer13);
	m_panel13->Layout();
	bSizer13->Fit(m_panel13);
	m_notebook2->AddPage(m_panel13, wxT("Retraction"), false);
	m_panel23 = new wxPanel(m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer33;
	sbSizer33 = new wxStaticBoxSizer(new wxStaticBox(m_panel23, wxID_ANY, wxT("On Pause")), wxVERTICAL);

	m_OnSuspendStrings = new wxTextCtrl(sbSizer33->GetStaticBox(), wxID_ANY, wxT("M104 T0 S0\nM140 S0"), wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	sbSizer33->Add(m_OnSuspendStrings, 1, wxALL | wxEXPAND, 5);


	bSizer19->Add(sbSizer33, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer34;
	sbSizer34 = new wxStaticBoxSizer(new wxStaticBox(m_panel23, wxID_ANY, wxT("On Resume")), wxVERTICAL);

	m_OnResumeStrings = new wxTextCtrl(sbSizer34->GetStaticBox(), wxID_ANY, wxT("G28 X\nG28 Y"), wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP | wxTE_MULTILINE);
	m_OnResumeStrings->SetToolTip(wxT("G92 Z<finish z> is already included"));

	sbSizer34->Add(m_OnResumeStrings, 1, wxALL | wxEXPAND, 5);


	bSizer19->Add(sbSizer34, 1, wxEXPAND, 5);


	m_panel23->SetSizer(bSizer19);
	m_panel23->Layout();
	bSizer19->Fit(m_panel23);
	m_notebook2->AddPage(m_panel23, wxT("Print Suspend"), true);

	bSizer121->Add(m_notebook2, 1, wxEXPAND | wxALL, 5);


	m_gcodeStrings->SetSizer(bSizer121);
	m_gcodeStrings->Layout();
	bSizer121->Fit(m_gcodeStrings);
	m_notebook1->AddPage(m_gcodeStrings, wxT("Custom Scripts"), false);

	bSizer1->Add(m_notebook1, 1, wxALL | wxEXPAND, 5);

	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer(0, 2, 0, 0);

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer(wxHORIZONTAL);

	m_saveToFile = new wxButton(this, wxID_ANY, wxT("Export Settings"), wxDefaultPosition, wxDefaultSize, 0);
	m_saveToFile->SetToolTip(wxT("Save Settings to a file"));

	bSizer12->Add(m_saveToFile, 0, wxLEFT | wxRIGHT, 5);

	m_importSettings = new wxButton(this, wxID_ANY, wxT("Import Settings"), wxDefaultPosition, wxDefaultSize, 0);
	m_importSettings->SetToolTip(wxT("Open an existing settings file"));

	bSizer12->Add(m_importSettings, 0, wxLEFT | wxRIGHT, 5);

	m_DefaultSetting = new wxButton(this, wxID_ANY, wxT("Default Settings"), wxDefaultPosition, wxDefaultSize, 0);
	m_DefaultSetting->SetToolTip(wxT("Restore default settings"));

	bSizer12->Add(m_DefaultSetting, 0, wxLEFT | wxRIGHT, 5);


	gSizer4->Add(bSizer12, 1, wxEXPAND, 5);

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton(this, wxID_OK);
	m_sdbSizer1->AddButton(m_sdbSizer1OK);
	m_sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer1->AddButton(m_sdbSizer1Cancel);
	m_sdbSizer1->Realize();

	gSizer4->Add(m_sdbSizer1, 0, wxALIGN_RIGHT, 5);


	bSizer1->Add(gSizer4, 0, wxEXPAND, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_layerThickness->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_firstLayerThickbess->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BottomSolidLayers->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_topSolidLayers->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillOverlap->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillPercentage->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_PerimeterNumber->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_XYOffset->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillFirst->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_insideOut->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_NoSequenceIslands->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fullModelSimplify->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::kMeansCheck), NULL, this);
	m_kMeansColours->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_coasterBase->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_coasterBaseColour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_baseOpacity->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_surfaceLayers->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_surfaceOpacity->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_checkSupport->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportExt->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::MainExtruderChoose), NULL, this);
	m_InterfaceSupport->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_InterfaceCount->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::UpdateInterfaceCount), NULL, this);
	m_EmptySupportLayers->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportDensity->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_InterfaceDensity->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportColour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OverrideBaseInterface->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OverrideBaseInterfaceCheck), NULL, this);
	m_BaseInterfaceTool->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_maxSupportAngle->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportOffset->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_PerciseCutSupport->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeBinPrime->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUsePurgeBinPrime), NULL, this);
	m_purgePrimeDistance->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T0_Temp->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T1_TEMP->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T2_TEMP->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Bed_Temp->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useCoolingFan->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUseCoolingFanCheck), NULL, this);
	m_fanAlwaysOn->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::FanMaximumAlways), NULL, this);
	m_minFanSpeed->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fanSpeed->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_reducePrintSpeed->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::reducePrintSpeedOnSmallLayers), NULL, this);
	m_minLayerTime->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_minLayerTimeFan->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_minSlowDownFeedrate->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useSoftwarePurge->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::onPurgeSoftwareToggle), NULL, this);
	m_purgeLength->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeFeedrate->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeMixerSpeed->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWait->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeLocationX->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeLocationY->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWidth->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeHeight->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgwWipeX->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWipeWidth->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeForceRetract->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fullPurgeBinPause->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnCheckPauseFullBin), NULL, this);
	m_LookAheadColourTrans->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnLookAheadColourTransition), NULL, this);
	m_purgeExcessColourChange->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_DistanceToColourChange->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ColourChangeMix->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RovaCompat->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::SwitchRovaCompat), NULL, this);
	m_ExtCount->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_headChoice->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnHeadChoice), NULL, this);
	m_CMYWhiteSub->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::CMYWhiteSub), NULL, this);
	m_mixerSpeed->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useServo->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUseSyringeLift), NULL, this);
	m_ServoLift->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ServoIssue->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_simpleGCodeView->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridWidth->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridDepth->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridHeight->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridSpacing->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T1Colour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T2Colour->Connect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_checkExperimental->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::ShowHideExperimental), NULL, this);
	m_minLineLength->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_endZ->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_topSurfaceInfill->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SurfaceInfillDensity->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BottomLayerInfill->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_mmPerPixel->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ThreadCount->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ColourPreviewByType->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_savePerimsToTest->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ConsiderSurfaces->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fwRetraction->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::FW_Retract), NULL, this);
	m_retractLayerChange->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe0->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe1->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe2->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnStart->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnEndGCode->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BeforeZMove->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnAfterZMove->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnRetractStrings->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnIssue->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_saveToFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::exportSettings), NULL, this);
	m_importSettings->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::importSettings), NULL, this);
	m_DefaultSetting->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::LoadDefaultSettings), NULL, this);
}

settingsDialog::~settingsDialog()
{
	// Disconnect Events
	m_layerThickness->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_firstLayerThickbess->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BottomSolidLayers->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_topSolidLayers->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillOverlap->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillPercentage->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_PerimeterNumber->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_XYOffset->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_infillFirst->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_insideOut->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_NoSequenceIslands->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fullModelSimplify->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::kMeansCheck), NULL, this);
	m_kMeansColours->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_coasterBase->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_coasterBaseColour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_baseOpacity->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_surfaceLayers->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_surfaceOpacity->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_checkSupport->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportExt->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::MainExtruderChoose), NULL, this);
	m_InterfaceSupport->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_InterfaceCount->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::UpdateInterfaceCount), NULL, this);
	m_EmptySupportLayers->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportDensity->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_InterfaceDensity->Disconnect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportColour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OverrideBaseInterface->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OverrideBaseInterfaceCheck), NULL, this);
	m_BaseInterfaceTool->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_maxSupportAngle->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SupportOffset->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_PerciseCutSupport->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeBinPrime->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUsePurgeBinPrime), NULL, this);
	m_purgePrimeDistance->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T0_Temp->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T1_TEMP->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T2_TEMP->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Bed_Temp->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useCoolingFan->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUseCoolingFanCheck), NULL, this);
	m_fanAlwaysOn->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::FanMaximumAlways), NULL, this);
	m_minFanSpeed->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fanSpeed->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_reducePrintSpeed->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::reducePrintSpeedOnSmallLayers), NULL, this);
	m_minLayerTime->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_minLayerTimeFan->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_minSlowDownFeedrate->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useSoftwarePurge->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::onPurgeSoftwareToggle), NULL, this);
	m_purgeLength->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeFeedrate->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeMixerSpeed->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWait->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeLocationX->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeLocationY->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWidth->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeHeight->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgwWipeX->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeWipeWidth->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_purgeForceRetract->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fullPurgeBinPause->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnCheckPauseFullBin), NULL, this);
	m_LookAheadColourTrans->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnLookAheadColourTransition), NULL, this);
	m_purgeExcessColourChange->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_DistanceToColourChange->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ColourChangeMix->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RovaCompat->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::SwitchRovaCompat), NULL, this);
	m_ExtCount->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_headChoice->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(settingsDialog::OnHeadChoice), NULL, this);
	m_CMYWhiteSub->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::CMYWhiteSub), NULL, this);
	m_mixerSpeed->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_useServo->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnUseSyringeLift), NULL, this);
	m_ServoLift->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ServoIssue->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_simpleGCodeView->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridWidth->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridDepth->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridHeight->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_gridSpacing->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T1Colour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_T2Colour->Disconnect(wxEVT_COMMAND_COLOURPICKER_CHANGED, wxColourPickerEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_checkExperimental->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::ShowHideExperimental), NULL, this);
	m_minLineLength->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_endZ->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_topSurfaceInfill->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_SurfaceInfillDensity->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BottomLayerInfill->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_mmPerPixel->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ThreadCount->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ColourPreviewByType->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_savePerimsToTest->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ConsiderSurfaces->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_fwRetraction->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::FW_Retract), NULL, this);
	m_retractLayerChange->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe0->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe1->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_ZLift2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_TravelWithoutRetract2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_CoastLength2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractAmt2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_IssueAmt2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_RetractFeed2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Issue2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_Wipe2->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnStart->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnEndGCode->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_BeforeZMove->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnAfterZMove->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnRetractStrings->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_OnIssue->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(settingsDialog::OnValueChanged), NULL, this);
	m_saveToFile->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::exportSettings), NULL, this);
	m_importSettings->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::importSettings), NULL, this);
	m_DefaultSetting->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(settingsDialog::LoadDefaultSettings), NULL, this);

}
