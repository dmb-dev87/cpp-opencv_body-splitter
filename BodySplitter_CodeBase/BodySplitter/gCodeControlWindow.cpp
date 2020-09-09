#include "gCodeControlWindow.h"
#include "GCodeViewPane.h"
#include "PrintBed.h"
#include "colourSubstitionWX.h"
#ifndef _CONSOLE

gCodeControlWindow::gCodeControlWindow(wxWindow* parent, int* args, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : 
	gCodeWXPanel(parent,args,id,pos,size,style)
{

}

gCodeControlWindow::~gCodeControlWindow()
{

}

void gCodeControlWindow::ShowLayer(wxCommandEvent &event)
{
	m_layerToShow = std::stoi(m_layerSelectText->GetValue().ToStdString());
	m_layerSelectSlider->SetValue(m_layerToShow);

	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_LAYER_VIEW);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);
	gcode_previewPanel->setEndLayerDrawing(m_layerToShow);
	PrintBed::getSingleton().forceDisplayUpdate();
}

void gCodeControlWindow::ShowLayerScroll(wxScrollEvent &event)
{
	m_layerToShow = m_layerSelectSlider->GetValue();
	
	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_LAYER_VIEW);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);

	gcode_previewPanel->setEndLayerDrawing(m_layerToShow);
	PrintBed::getSingleton().forceDisplayUpdate();
}

void gCodeControlWindow::meshSliced(wxCommandEvent &event)
{
	int maxLayer = PrintBed::getSingleton().getMaxLayer();
	m_layerToShow = maxLayer-1;
	gcode_previewPanel->setEndLayerDrawing(m_layerToShow);
	m_layerSelectText->SetLabel(std::to_string(m_layerToShow));
	m_layerSelectSlider->SetMax(maxLayer-1);
	m_layerSelectSlider->SetMin(0);
	m_layerSelectSlider->SetValue(maxLayer);
	PrintBed::getSingleton().forceDisplayUpdate();
	m_colourSub->Enable(true);
}

void gCodeControlWindow::updateLayerView(wxCommandEvent &event)
{
	m_layerSelectText->SetLabel(std::to_string(m_layerToShow));
	m_ColoursOnLayer->SetLabel(std::to_string(PrintBed::GetSingleton().getColoursOnLayer(m_layerToShow).size()));
}

void gCodeControlWindow::updatePrintProgress(wxCommandEvent &event)
{

}

void gCodeControlWindow::setFilamentStats(const std::string &str)
{
	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, FILL_PRINT_STATS);
	controlWindowEvent.SetString(str);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);
}
void gCodeControlWindow::setPrintTime(const std::string &str)
{
	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, FILL_PRINT_TIME);
	controlWindowEvent.SetString(str);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);
}

void gCodeControlWindow::fillTime(wxCommandEvent &event)
{
	m_timeToPrint->SetValue(event.GetString());
}

void gCodeControlWindow::fillStats(wxCommandEvent &event)
{
	m_filamentStats->SetValue(event.GetString());
}

void gCodeControlWindow::fillCost(wxCommandEvent &event)
{
	m_printCost->SetValue(event.GetString());
}

void gCodeControlWindow::setPrintCost(double printCost)
{
	std::stringstream outputStream;
	std::fixed(outputStream);
	outputStream.precision(2);
	outputStream << "$" << printCost;

	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, FILL_PRINT_COST);
	controlWindowEvent.SetString(outputStream.str());
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);
}

void gCodeControlWindow::OnLayerUp(wxCommandEvent& event)
{
	m_layerToShow = std::min(m_layerToShow + 1, (int)PrintBed::getSingleton().getMaxLayer());

	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_LAYER_VIEW);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);

	gcode_previewPanel->setEndLayerDrawing(m_layerToShow);
	PrintBed::getSingleton().forceDisplayUpdate();
}

void gCodeControlWindow::OnLayerDown(wxCommandEvent& event)
{
	m_layerToShow = std::max(m_layerToShow - 1, 0);

	wxCommandEvent controlWindowEvent(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_LAYER_VIEW);
	this->GetEventHandler()->AddPendingEvent(controlWindowEvent);

	gcode_previewPanel->setEndLayerDrawing(m_layerToShow);
	PrintBed::getSingleton().forceDisplayUpdate();
}

void gCodeControlWindow::OnColourSub(wxCommandEvent& event)
{
	colourSubDialog subDialog(this, PrintBed::GetSingleton().getColoursOnBed());
	int result = subDialog.ShowModal();
	if (result == wxID_OK)
		subDialog.saveColourSub();
}

wxBEGIN_EVENT_TABLE(gCodeControlWindow, gCodeWXPanel)
EVT_COMMAND(UPDATE_MESH, wxEVT_COMMAND_TEXT_UPDATED, gCodeControlWindow::meshSliced)
EVT_COMMAND(UPDATE_LAYER_VIEW, wxEVT_COMMAND_TEXT_UPDATED, gCodeControlWindow::updateLayerView)
EVT_COMMAND(FILL_PRINT_TIME, wxEVT_COMMAND_TEXT_UPDATED, gCodeControlWindow::fillTime)
EVT_COMMAND(FILL_PRINT_STATS, wxEVT_COMMAND_TEXT_UPDATED, gCodeControlWindow::fillStats)
EVT_COMMAND(FILL_PRINT_COST, wxEVT_COMMAND_TEXT_UPDATED, gCodeControlWindow::fillCost)
wxEND_EVENT_TABLE()

#endif