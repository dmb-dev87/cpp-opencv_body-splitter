///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 17 2015)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "controlPanel.h"

///////////////////////////////////////////////////////////////////////////

controlPanel::controlPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer(0, 2, 0, 0);
	fgSizer12->AddGrowableCol(1);
	fgSizer12->AddGrowableRow(0);
	fgSizer12->SetFlexibleDirection(wxBOTH);
	fgSizer12->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer13->SetFlexibleDirection(wxBOTH);
	fgSizer13->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_SerialPort = new wxComboBox(this, wxID_ANY, wxT("Port"), wxDefaultPosition, wxSize(80, -1), 0, NULL, 0);
	fgSizer13->Add(m_SerialPort, 0, wxALL, 5);

	m_staticText141 = new wxStaticText(this, wxID_ANY, wxT("BaudRate:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText141->Wrap(-1);
	fgSizer13->Add(m_staticText141, 0, wxALL, 5);

	m_baudRate = new wxComboBox(this, wxID_ANY, wxT("250000"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_baudRate->Append(wxT("2400"));
	m_baudRate->Append(wxT("9600"));
	m_baudRate->Append(wxT("19200"));
	m_baudRate->Append(wxT("38400"));
	m_baudRate->Append(wxT("57600"));
	m_baudRate->Append(wxT("115200"));
	m_baudRate->Append(wxT("250000"));
	m_baudRate->Append(wxT("500000"));
	m_baudRate->Append(wxT("1000000"));
	m_baudRate->SetSelection(6);
	fgSizer13->Add(m_baudRate, 0, wxALL, 5);

	m_Connect = new wxButton(this, wxID_ANY, wxT("Connect"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer13->Add(m_Connect, 0, wxALL, 5);


	fgSizer13->Add(0, 0, 1, wxEXPAND, 5);

	m_Refresh = new wxButton(this, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer13->Add(m_Refresh, 0, wxALL, 5);


	bSizer15->Add(fgSizer13, 0, wxEXPAND, 5);

	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer(3, 3, 0, 0);
	fgSizer14->SetFlexibleDirection(wxBOTH);
	fgSizer14->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	m_PlusY = new wxButton(this, wxID_ANY, wxT("+10 Y"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer14->Add(m_PlusY, 0, wxALL, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	m_MinusX = new wxButton(this, wxID_ANY, wxT("-10 X"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer14->Add(m_MinusX, 0, wxALL, 5);

	m_Home = new wxButton(this, wxID_ANY, wxT("Home"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer14->Add(m_Home, 0, wxALL, 5);

	m_PlusX = new wxButton(this, wxID_ANY, wxT("+10 X"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer14->Add(m_PlusX, 0, wxALL, 5);


	fgSizer14->Add(0, 0, 1, wxEXPAND, 5);

	m_MinusY = new wxButton(this, wxID_ANY, wxT("-10 Y"), wxDefaultPosition, wxSize(80, -1), 0);
	fgSizer14->Add(m_MinusY, 0, wxALL, 5);


	bSizer15->Add(fgSizer14, 0, 0, 5);

	wxStaticBoxSizer* sbSizer27;
	sbSizer27 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Temperatures")), wxVERTICAL);

	wxFlexGridSizer* fgSizer16;
	fgSizer16 = new wxFlexGridSizer(0, 5, 0, 0);
	fgSizer16->SetFlexibleDirection(wxBOTH);
	fgSizer16->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText75 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Mixing:"), wxDefaultPosition, wxSize(55, -1), 0);
	m_staticText75->Wrap(-1);
	fgSizer16->Add(m_staticText75, 0, wxALL, 5);

	m_T0_Temp = new wxTextCtrl(sbSizer27->GetStaticBox(), wxID_ANY, wxT("210"), wxDefaultPosition, wxSize(60, -1), 0);
	fgSizer16->Add(m_T0_Temp, 0, wxALL, 5);

	m_staticText76 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("C"), wxDefaultPosition, wxSize(10, -1), 0);
	m_staticText76->Wrap(-1);
	fgSizer16->Add(m_staticText76, 0, wxALL, 5);

	m_MixerSet = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_MixerSet, 0, wxALL, 5);

	m_MixTempOff = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Off"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_MixTempOff, 0, wxALL, 5);

	m_staticText77 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Support:"), wxDefaultPosition, wxSize(55, -1), 0);
	m_staticText77->Wrap(-1);
	fgSizer16->Add(m_staticText77, 0, wxALL, 5);

	m_T1_Temp = new wxTextCtrl(sbSizer27->GetStaticBox(), wxID_ANY, wxT("210"), wxDefaultPosition, wxSize(60, -1), 0);
	fgSizer16->Add(m_T1_Temp, 0, wxALL, 5);

	m_staticText78 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("C"), wxDefaultPosition, wxSize(10, -1), 0);
	m_staticText78->Wrap(-1);
	fgSizer16->Add(m_staticText78, 0, wxALL, 5);

	m_SupportSet = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_SupportSet, 0, wxALL, 5);

	m_SupportTempOff = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Off"), wxDefaultPosition, wxSize(40, -1), 0);
	m_SupportTempOff->SetMinSize(wxSize(40, -1));

	fgSizer16->Add(m_SupportTempOff, 0, wxALL, 5);

	m_staticText79 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Flexible:"), wxDefaultPosition, wxSize(55, -1), 0);
	m_staticText79->Wrap(-1);
	fgSizer16->Add(m_staticText79, 0, wxALL, 5);

	m_T2_Temp = new wxTextCtrl(sbSizer27->GetStaticBox(), wxID_ANY, wxT("210"), wxDefaultPosition, wxSize(60, -1), 0);
	fgSizer16->Add(m_T2_Temp, 0, wxALL, 5);

	m_staticText80 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("C"), wxDefaultPosition, wxSize(10, -1), 0);
	m_staticText80->Wrap(-1);
	fgSizer16->Add(m_staticText80, 0, wxALL, 5);

	m_FlexSet = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_FlexSet, 0, wxALL, 5);

	m_FlexTempOff = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Off"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_FlexTempOff, 0, wxALL, 5);

	m_staticText81 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Bed:"), wxDefaultPosition, wxSize(55, -1), 0);
	m_staticText81->Wrap(-1);
	fgSizer16->Add(m_staticText81, 0, wxALL, 5);

	m_Bed_Temp = new wxTextCtrl(sbSizer27->GetStaticBox(), wxID_ANY, wxT("70"), wxDefaultPosition, wxSize(60, -1), 0);
	fgSizer16->Add(m_Bed_Temp, 0, wxALL, 5);

	m_staticText82 = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("C"), wxDefaultPosition, wxSize(10, -1), 0);
	m_staticText82->Wrap(-1);
	fgSizer16->Add(m_staticText82, 0, wxALL, 5);

	m_BedSet = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_BedSet, 0, wxALL, 5);

	m_BedTempOff = new wxButton(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Off"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer16->Add(m_BedTempOff, 0, wxALL, 5);

	m_105Result = new wxStaticText(sbSizer27->GetStaticBox(), wxID_ANY, wxT("M105 Result"), wxDefaultPosition, wxDefaultSize, 0);
	m_105Result->Wrap(-1);
	fgSizer16->Add(m_105Result, 0, wxALL, 5);


	sbSizer27->Add(fgSizer16, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer(new wxStaticBox(sbSizer27->GetStaticBox(), wxID_ANY, wxT("Printer Overrides (%)")), wxVERTICAL);

	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer(0, 4, 0, 0);
	fgSizer6->SetFlexibleDirection(wxBOTH);
	fgSizer6->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText12 = new wxStaticText(sbSizer4->GetStaticBox(), wxID_ANY, wxT("Print Speed:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12->Wrap(-1);
	fgSizer6->Add(m_staticText12, 0, wxBOTTOM | wxTOP, 5);

	m_PrintSpeed = new wxSlider(sbSizer4->GetStaticBox(), wxID_ANY, 100, 1, 400, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	fgSizer6->Add(m_PrintSpeed, 0, wxBOTTOM | wxTOP, 5);

	m_printSpeedSpinCtrl = new wxSpinCtrl(sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 400, 100);
	m_printSpeedSpinCtrl->SetMaxSize(wxSize(50, -1));

	fgSizer6->Add(m_printSpeedSpinCtrl, 0, wxBOTTOM | wxTOP, 5);

	m_SetPrintSpeed = new wxButton(sbSizer4->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer6->Add(m_SetPrintSpeed, 0, wxBOTTOM | wxTOP, 5);

	m_staticText14 = new wxStaticText(sbSizer4->GetStaticBox(), wxID_ANY, wxT("Extrusion:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText14->Wrap(-1);
	fgSizer6->Add(m_staticText14, 0, wxBOTTOM | wxTOP, 5);

	m_ExtSpeed = new wxSlider(sbSizer4->GetStaticBox(), wxID_ANY, 100, 1, 400, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
	fgSizer6->Add(m_ExtSpeed, 0, wxBOTTOM | wxTOP, 5);

	m_extSpeedSpinCtrl = new wxSpinCtrl(sbSizer4->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 500, 100);
	m_extSpeedSpinCtrl->SetMaxSize(wxSize(50, -1));

	fgSizer6->Add(m_extSpeedSpinCtrl, 0, wxBOTTOM | wxTOP, 5);

	m_SetMultiple = new wxButton(sbSizer4->GetStaticBox(), wxID_ANY, wxT("Set"), wxDefaultPosition, wxSize(40, -1), 0);
	fgSizer6->Add(m_SetMultiple, 0, wxBOTTOM | wxTOP, 5);


	sbSizer4->Add(fgSizer6, 1, wxEXPAND, 5);


	sbSizer27->Add(sbSizer4, 0, wxEXPAND, 5);


	bSizer15->Add(sbSizer27, 1, wxEXPAND, 5);

	wxStaticBoxSizer* sbPrintControl;
	sbPrintControl = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Print Control")), wxVERTICAL);

	wxFlexGridSizer* fgSizer17;
	fgSizer17 = new wxFlexGridSizer(0, 3, 0, 0);
	fgSizer17->SetFlexibleDirection(wxBOTH);
	fgSizer17->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_StartPrint = new wxButton(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Start"), wxDefaultPosition, wxSize(80, -1), 0);
	m_StartPrint->Enable(false);

	fgSizer17->Add(m_StartPrint, 0, wxALL, 5);

	m_PausePrint = new wxButton(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Pause"), wxDefaultPosition, wxSize(80, -1), 0);
	m_PausePrint->Enable(false);

	fgSizer17->Add(m_PausePrint, 0, wxALL, 5);

	m_Abort = new wxButton(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Abort"), wxDefaultPosition, wxSize(80, -1), 0);
	m_Abort->Enable(false);

	fgSizer17->Add(m_Abort, 0, wxALL, 5);


	sbPrintControl->Add(fgSizer17, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	m_LoadGCode = new wxButton(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Load GCode File"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(m_LoadGCode, 0, wxALL, 5);

	m_Verbose = new wxCheckBox(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Verbose Output"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(m_Verbose, 0, wxALL, 5);

	m_Progress = new wxGauge(sbPrintControl->GetStaticBox(), wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	m_Progress->SetValue(0);
	bSizer5->Add(m_Progress, 0, wxALL, 5);


	sbPrintControl->Add(bSizer5, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer(new wxStaticBox(sbPrintControl->GetStaticBox(), wxID_ANY, wxT("Temperature Readings")), wxVERTICAL);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	m_MixerTempReadout = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Mixer Tool: ??/0"), wxDefaultPosition, wxDefaultSize, 0);
	m_MixerTempReadout->Wrap(-1);
	bSizer6->Add(m_MixerTempReadout, 0, wxALL, 5);

	m_BedTempReadout = new wxStaticText(sbSizer3->GetStaticBox(), wxID_ANY, wxT("Bed Temp: ??/0"), wxDefaultPosition, wxDefaultSize, 0);
	m_BedTempReadout->Wrap(-1);
	bSizer6->Add(m_BedTempReadout, 0, wxALL, 5);


	sbSizer3->Add(bSizer6, 1, wxEXPAND, 5);


	sbPrintControl->Add(sbSizer3, 1, wxEXPAND, 5);


	bSizer15->Add(sbPrintControl, 1, wxEXPAND, 5);


	fgSizer12->Add(bSizer15, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer(wxVERTICAL);

	m_GCodeConsole = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CHARWRAP | wxTE_MULTILINE | wxTE_READONLY);
	bSizer16->Add(m_GCodeConsole, 1, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer(wxHORIZONTAL);

	m_StringToSend = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	bSizer17->Add(m_StringToSend, 1, wxALL | wxEXPAND, 5);

	m_SendString = new wxButton(this, wxID_ANY, wxT("Send"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer17->Add(m_SendString, 0, wxALL, 5);


	bSizer16->Add(bSizer17, 0, wxEXPAND, 5);


	fgSizer12->Add(bSizer16, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer12);
	this->Layout();

	// Connect Events
	m_Connect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::ConnectToggle), NULL, this);
	m_Refresh->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::RefreshPorts), NULL, this);
	m_PlusY->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Plus10Y), NULL, this);
	m_MinusX->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Minus10X), NULL, this);
	m_Home->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Home), NULL, this);
	m_PlusX->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Plus10X), NULL, this);
	m_MinusY->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Minus10Y), NULL, this);
	m_MixerSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetMixerTemp), NULL, this);
	m_MixTempOff->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffMixerTemp), NULL, this);
	m_SupportSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetSupportTemp), NULL, this);
	m_SupportTempOff->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffSupportTemp), NULL, this);
	m_FlexSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetFlexibleTemp), NULL, this);
	m_FlexTempOff->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffFlexibleTemp), NULL, this);
	m_BedSet->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetBedTemp), NULL, this);
	m_BedTempOff->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetOffTemp), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_printSpeedSpinCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(controlPanel::OnSetPrintSpeedSpinner), NULL, this);
	m_printSpeedSpinCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(controlPanel::OnPrintSpeedSpinCtrl), NULL, this);
	m_SetPrintSpeed->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetPrintSpeed), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_TOP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_extSpeedSpinCtrl->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(controlPanel::OnSetExtrusionSpinner), NULL, this);
	m_extSpeedSpinCtrl->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(controlPanel::OnExtrusionSpinCtrl), NULL, this);
	m_SetMultiple->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::ExtMultiplier), NULL, this);
	m_StartPrint->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::StartPrint), NULL, this);
	m_PausePrint->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::PuaseToggle), NULL, this);
	m_Abort->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Abort), NULL, this);
	m_LoadGCode->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::LoadGCodeFile), NULL, this);
	m_Verbose->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(controlPanel::VerboseToggle), NULL, this);
	m_StringToSend->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(controlPanel::DownHistory), NULL, this);
	m_StringToSend->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(controlPanel::sendCommandEnter), NULL, this);
	m_SendString->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::sendCommand), NULL, this);
}

controlPanel::~controlPanel()
{
	// Disconnect Events
	m_Connect->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::ConnectToggle), NULL, this);
	m_Refresh->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::RefreshPorts), NULL, this);
	m_PlusY->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Plus10Y), NULL, this);
	m_MinusX->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Minus10X), NULL, this);
	m_Home->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Home), NULL, this);
	m_PlusX->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Plus10X), NULL, this);
	m_MinusY->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Minus10Y), NULL, this);
	m_MixerSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetMixerTemp), NULL, this);
	m_MixTempOff->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffMixerTemp), NULL, this);
	m_SupportSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetSupportTemp), NULL, this);
	m_SupportTempOff->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffSupportTemp), NULL, this);
	m_FlexSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetFlexibleTemp), NULL, this);
	m_FlexTempOff->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::OffFlexibleTemp), NULL, this);
	m_BedSet->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetBedTemp), NULL, this);
	m_BedTempOff->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetOffTemp), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_TOP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_PrintSpeed->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(controlPanel::OnScrollPrintSpeed), NULL, this);
	m_printSpeedSpinCtrl->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(controlPanel::OnSetPrintSpeedSpinner), NULL, this);
	m_printSpeedSpinCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(controlPanel::OnPrintSpeedSpinCtrl), NULL, this);
	m_SetPrintSpeed->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::SetPrintSpeed), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_TOP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_BOTTOM, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_LINEUP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_PAGEUP, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_ExtSpeed->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(controlPanel::OnScrollExtMultiplier), NULL, this);
	m_extSpeedSpinCtrl->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(controlPanel::OnSetExtrusionSpinner), NULL, this);
	m_extSpeedSpinCtrl->Disconnect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler(controlPanel::OnExtrusionSpinCtrl), NULL, this);
	m_SetMultiple->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::ExtMultiplier), NULL, this);
	m_StartPrint->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::StartPrint), NULL, this);
	m_PausePrint->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::PuaseToggle), NULL, this);
	m_Abort->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::Abort), NULL, this);
	m_LoadGCode->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::LoadGCodeFile), NULL, this);
	m_Verbose->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(controlPanel::VerboseToggle), NULL, this);
	m_StringToSend->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(controlPanel::DownHistory), NULL, this);
	m_StringToSend->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(controlPanel::sendCommandEnter), NULL, this);
	m_SendString->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(controlPanel::sendCommand), NULL, this);

}
