///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#ifndef _CONSOLE
#include "GCodeViewPane.h"

#include "GCodeVisualizerWX.h"

///////////////////////////////////////////////////////////////////////////

gCodeWXPanel::gCodeWXPanel(wxWindow* parent,int *args, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	wxBoxSizer* gcodeSizer;
	gcodeSizer = new wxBoxSizer(wxHORIZONTAL);

	gcode_previewPanel = new GCodeViewPane((wxFrame*)this, args);
	gcodeSizer->Add(gcode_previewPanel, 1, wxEXPAND, 5);

	g_codeStatsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	g_codeStatsPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	g_codeStatsPanel->SetMaxSize(wxSize(160, -1));

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sbSizer24;
	sbSizer24 = new wxStaticBoxSizer(new wxStaticBox(g_codeStatsPanel, wxID_ANY, wxT("Print Statistics")), wxVERTICAL);

	wxStaticBoxSizer* sbSizer25;
	sbSizer25 = new wxStaticBoxSizer(new wxStaticBox(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Filament Usage")), wxVERTICAL);

	sbSizer25->SetMinSize(wxSize(-1, 150));
	m_filamentStats = new wxTextCtrl(sbSizer25->GetStaticBox(), wxID_ANY, wxT("Calculation will occur after exporting gcode"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxNO_BORDER);
	m_filamentStats->SetMaxSize(wxSize(-1, 120));

	sbSizer25->Add(m_filamentStats, 1, wxEXPAND, 5);


	sbSizer24->Add(sbSizer25, 0, wxEXPAND, 0);

	wxStaticBoxSizer* sbSizer27;
	sbSizer27 = new wxStaticBoxSizer(new wxStaticBox(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Print Time")), wxVERTICAL);

	m_timeToPrint = new wxTextCtrl(sbSizer27->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	sbSizer27->Add(m_timeToPrint, 0, wxALL | wxEXPAND, 5);


	sbSizer24->Add(sbSizer27, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Print Cost")), wxVERTICAL);

	m_printCost = new wxTextCtrl(sbSizer7->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL | wxTE_READONLY);
	sbSizer7->Add(m_printCost, 0, wxALL | wxEXPAND, 5);


	sbSizer24->Add(sbSizer7, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer28;
	sbSizer28 = new wxStaticBoxSizer(new wxStaticBox(sbSizer24->GetStaticBox(), wxID_ANY, wxT("Layer Preview")), wxVERTICAL);

	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer(wxHORIZONTAL);

	m_layerSelectSlider = new wxSlider(sbSizer28->GetStaticBox(), wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_INVERSE | wxSL_VERTICAL);
	bSizer20->Add(m_layerSelectSlider, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer(wxVERTICAL);

	m_layerSelectText = new wxTextCtrl(sbSizer28->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	bSizer21->Add(m_layerSelectText, 0, wxALL, 5);

	m_showLayer = new wxButton(sbSizer28->GetStaticBox(), wxID_ANY, wxT("Show"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer21->Add(m_showLayer, 0, wxALL, 5);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);

	m_LayerUp = new wxButton(sbSizer28->GetStaticBox(), wxID_ANY, wxT("/\\"), wxDefaultPosition, wxDefaultSize, 0);
	m_LayerUp->SetMaxSize(wxSize(30, -1));

	bSizer6->Add(m_LayerUp, 0, wxALL, 5);

	m_Layerdown = new wxButton(sbSizer28->GetStaticBox(), wxID_ANY, wxT("\\/"), wxDefaultPosition, wxDefaultSize, 0);
	m_Layerdown->SetMaxSize(wxSize(30, -1));

	bSizer6->Add(m_Layerdown, 0, wxALL, 5);


	bSizer21->Add(bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5);


	bSizer20->Add(bSizer21, 1, wxALIGN_CENTER_VERTICAL, 5);


	bSizer22->Add(bSizer20, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer30;
	sbSizer30 = new wxStaticBoxSizer(new wxStaticBox(sbSizer28->GetStaticBox(), wxID_ANY, wxT("Colours On Layer")), wxVERTICAL);

	m_ColoursOnLayer = new wxTextCtrl(sbSizer30->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	sbSizer30->Add(m_ColoursOnLayer, 0, wxALL, 5);


	bSizer22->Add(sbSizer30, 0, wxEXPAND, 5);


	sbSizer28->Add(bSizer22, 1, wxEXPAND, 5);


	sbSizer24->Add(sbSizer28, 1, wxEXPAND, 5);


	bSizer19->Add(sbSizer24, 1, wxEXPAND, 5);
	m_colourSub = new wxButton(g_codeStatsPanel, wxID_ANY, wxT("Colour Substitution"), wxDefaultPosition, wxDefaultSize, 0);
	m_colourSub->Enable(false);
	bSizer19->Add(m_colourSub, 0, wxALL | wxEXPAND, 5);
	m_Print = new wxButton(g_codeStatsPanel, wxID_ANY, wxT("Print"), wxDefaultPosition, wxDefaultSize, 0);
	m_Print->Enable(false);

	bSizer19->Add(m_Print, 0, wxALL | wxEXPAND, 5);


	g_codeStatsPanel->SetSizer(bSizer19);
	g_codeStatsPanel->Layout();
	bSizer19->Fit(g_codeStatsPanel);
	gcodeSizer->Add(g_codeStatsPanel, 1, wxALL | wxEXPAND, 5);


	this->SetSizer(gcodeSizer);
	this->Layout();

	// Connect Events
	m_layerSelectSlider->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectText->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gCodeWXPanel::ShowLayer), NULL, this);
	m_showLayer->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::ShowLayer), NULL, this);
	m_LayerUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnLayerUp), NULL, this);
	m_Layerdown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnLayerDown), NULL, this);
	m_colourSub->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnColourSub), NULL, this);
	m_Print->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnPrintButton), NULL, this);
}

gCodeWXPanel::~gCodeWXPanel()
{
	// Disconnect Events
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_TOP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectSlider->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(gCodeWXPanel::ShowLayerScroll), NULL, this);
	m_layerSelectText->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gCodeWXPanel::ShowLayer), NULL, this);
	m_showLayer->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::ShowLayer), NULL, this);
	m_LayerUp->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnLayerUp), NULL, this);
	m_Layerdown->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnLayerDown), NULL, this);
	m_colourSub->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnColourSub), NULL, this);
	m_Print->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gCodeWXPanel::OnPrintButton), NULL, this);

}
#endif