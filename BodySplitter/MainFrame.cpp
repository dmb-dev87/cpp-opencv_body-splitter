#include <thread>

#include "MainFrame.h"


#ifndef _CONSOLE

#if defined(_WIN32) || defined(__WXMAC__)
#include <wx\appprogress.h>
#endif
#include <wx/notebook.h>

#include "gCodeControlWindow.h"
#include "ColourAdjustWindow.h"
#include "Rova5Compat.h"
#include "PrintBed.h"
#include "Settings.h"
#include "Dialogs.h"
#include "GCodeWriter.h"
#include "iSlicable.h"
#include "iDrawable.h"
#include "UserMesh.h"
#include "CoasterPrint.h"
#include "Slicer.h"
#include "machineControl.h"
#include "AboutDialogOverride.h"

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	this->SetIcon(wxIcon(wxT("frame_icon")));
	menuFile->Append(ID_CLEAR, "&New...\tCtrl-N", "Clear the bed contents");
	menuFile->Append(ID_IMPORT_BED, "&Open Print Project\tCtrl-O", "Open an existing print bed and settings file");
	menuFile->Append(ID_EXPORT_BED, "&Save Print Project\tCtrl-E", "Save the bed layout and print settings to a file");
	menuFile->AppendSeparator();
	menuFile->Append(ID_OPEN, "&Import Model\tCtrl-A",
		"Add a file to the bed");
	menuFile->Append(ID_OPEN_IMAGE, "Import &Image\tCtrl-I",
		"Add an image to the bed");
	sliceButton = menuFile->Append(ID_SLICE, "&Slice\tCtrl-S",
		"Slice file");
	sliceButton->Enable(false);

	menuFile->Append(ID_SETTINGS, "S&ettings\tCtrl-Shift-S", "Slice Settings");
	writeButton = menuFile->Append(ID_WRITE_GCODE, "&Write GCode\tCtrl-P", "Write GCode to file");
	printButton = menuFile->Append(ID_PRINT_GCODE, "&Print GCode\tCtrl-Shift-P", "Print from BodySplitter");
	writeButton->Enable(false);
	printButton->Enable(false);
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	//menuHelp->Append(ID_SUBMIT_FEEDBACK, "&Submit Feedback\tCtrl-F", "Submit Feedback to ORD");
	//menuHelp->Append(ID_GET_HELP, "&Get Help\tCtrl-H", "Request Support from ORD");
	menuHelp->Append(wxID_ABOUT);

	wxMenu* menuView = new wxMenu;
	viewSimpleButton = menuView->Append(ID_SWITCH_TEXTURE, "&Switch Texture", "View Simple Texture");
	viewAsWireFrame = menuView->Append(ID_SWITCH_TO_WIREFRAME, "&Switch to wireframe", "View wire frame");
	viewSwitchPerspective = menuView->Append(ID_SWITCH_PERSPECTIVE, "&Switch to orthogonal view", "Switch View");

	wxMenuBar *menuBar = new wxMenuBar;

	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuView, "&View");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
#if defined(_WIN32) || defined(__WXMAC__)
	taskBarIndicator = nullptr;
#endif
	CreateStatusBar();
	SetStatusText("BodySplitter 0.1.0");
}

void MainFrame::OnUpdateStatusBar(wxCommandEvent& event)
{
	SetStatusText(event.GetString());
	//IF the slice is complete switch automatically to the gcode preview
	if (event.GetString() == "Slice complete") //This is kinda hacky
	{
		ptr_notebook->SetSelection(1);
	}
#if defined(_WIN32) || defined(__WXMAC__)
	if (event.GetInt() > -1)
	{
		if (taskBarIndicator)
			taskBarIndicator->SetValue(event.GetInt());
	}
#endif
}

void MainFrame::SetStatusBarText(const std::string &text, int val)
{
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, STATUS_UPDATE_ID);
	event.SetString(text);
	event.SetInt(val);
	this->GetEventHandler()->AddPendingEvent(event);
}

void MainFrame::OnExit(wxCommandEvent& event)
{
	Close(false);	
}

void MainFrame::OnClose(wxCloseEvent& event)
{
	if (event.CanVeto())
	{
		if (PrintBed::getSingleton().hasUnsavedChanges())
		{
			wxMessageDialog temp(nullptr, "Save changes before exiting?", "Discard", wxYES | wxNO | wxCANCEL);
			int result = temp.ShowModal();
			if (result == wxID_CANCEL)
			{
				return;
			}
			else if (result == wxID_YES)
			{
				wxString outputFileName = activeFile.SubString(0, activeFile.Last('.'));
				outputFileName += "rova";
				wxFileDialog writeFileDialog(this, _("Write Bed File"), "", outputFileName,
					"BodySplitter Files (*.rova)|*.rova", wxFD_SAVE);
				if (writeFileDialog.ShowModal() == wxID_CANCEL)
				{
					return;
				}
				PrintBed::getSingleton().exportBed(writeFileDialog.GetPath().ToStdString());
			}
		}
	}
	event.Skip();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	AboutDialogOverride dialog(this);
	dialog.ShowModal();
}

void MainFrame::OnSimpleTexture(wxCommandEvent& event)
{
	PrintBed::getSingleton().switchToSimpleTexture();
}

void MainFrame::OnSwitchToWire(wxCommandEvent& event)
{
	PrintBed::getSingleton().switchToWireFrame();
	if (PrintBed::getSingleton().isWireFrame())
		viewAsWireFrame->SetItemLabel("&Switch to filled polygons");
	else
		viewAsWireFrame->SetItemLabel("&Switch to wire frame");
}

void MainFrame::OnSwitchPerspective(wxCommandEvent& event)
{
	static bool isPerspective = true;
	isPerspective = !isPerspective;
	if(isPerspective)
		viewSwitchPerspective->SetItemLabel("&Switch to orthogonal view");
	else
		viewSwitchPerspective->SetItemLabel("&Switch to perspective view");
	PrintBed::getSingleton().switchToPerspective(isPerspective);
}

void MainFrame::OnSettings(wxCommandEvent& event)
{
	if (Settings::getSingleton().OpenSettingsWindow())
	{
		//Settings changed, invalidate the slice, TODO: only invalidate the slice is slice settings change
		PrintBed::getSingleton().invalidateAllSlices();
	}
}

void MainFrame::OnColourAdjust(wxCommandEvent &event)
{
	ColourAdjustWindow dialog(this);
	dialog.ShowModal();
}

void  MainFrame::OnWriteGCode(wxCommandEvent& event)
{
	std::unique_ptr<GCodeWriter> writer;
	std::shared_ptr<iSlicable> slicable = std::dynamic_pointer_cast<iSlicable>(PrintBed::getSingleton().getDrawable());
	if (!slicable)
		return;

	std::string outputName;
	if (event.GetId() == ID_WRITE_GCODE)
	{
		wxString outputFileName = activeFile.SubString(0, activeFile.Last('.'));
		outputFileName += "gcode";
		wxFileDialog writeFileDialog(this, _("Write GCode File"), "", outputFileName,
			"GCode Files (*.gcode)|*.gcode", wxFD_SAVE);

		if (writeFileDialog.ShowModal() == wxID_CANCEL)
		{
			return;
		}
		writer = std::make_unique<GCodeWriter>(writeFileDialog.GetPath().ToStdString());
		for (auto it : PrintBed::getSingleton().getDrawables())
		{
			auto temp = std::dynamic_pointer_cast<iSlicable>(it);
			writer->addSlicable(temp);
		}
		outputName = writeFileDialog.GetFilename();
	}
	else if (event.GetId() == ID_PRINT_GCODE)
	{
		if (ptr_machineControl->autoConnect())
		{
			writer = std::make_unique<GCodeWriter>(ptr_machineControl);
		}
		else
		{
			// Messagebox failed
			return;
		}
		for (auto it : PrintBed::getSingleton().getDrawables())
		{
			auto temp = std::dynamic_pointer_cast<iSlicable>(it);
			writer->addSlicable(temp);
		}
	}
	if (slicable->isSliced() == false)
	{
		// Blocking slice
		SetStatusText("Slicing...");
		sliceThread();
	}
	wxString progressText = "Writing to GCode: 0%";
	double tenPercentUpdate = writer->getLayerCount() / 10.0;
	SetStatusText(progressText);
	double lastUpdateAmount = 0;
	int tenPercentCounter = 1;
	writer->startWriter();
	for (int i = 0; i < writer->getLayerCount(); i++)
	{
		writer->writeLayer(i);
		if (i - lastUpdateAmount > tenPercentUpdate)
		{
			progressText = "Writing to GCode: " + wxString(std::to_string(tenPercentCounter * 10)) + "%";
			SetStatusText(progressText);
			tenPercentCounter++;
			lastUpdateAmount = i;
		}
	}
	writer->endWriter();
	progressText = "GCode written to :" + outputName + " Print time: " + writer->getTimeToPrint();
	gCodeControlWindow::getSingleton().setFilamentStats(writer->getFilamentUsageEstimatePrint());
	gCodeControlWindow::getSingleton().setPrintTime(writer->getTimeToPrint());
	gCodeControlWindow::GetSingleton().setPrintCost(writer->getPrintCost());
	SetStatusText(progressText);
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog
		openFileDialog(this, _("Open Model file"), "", "",
			"3D Model Files (*.obj;*.dae;*.stl)|*.obj;*.stl;*dae", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	activeFile = openFileDialog.GetFilename();
	auto tmpMesh = PrintBed::getSingleton().loadMesh(openFileDialog.GetPath().ToStdString());
	if (!tmpMesh)
	{
		wxMessageBox("Error opening Model", "Error");
	}
	else
	{
		PrintBed::getSingleton().centreMesh(std::dynamic_pointer_cast<iDrawable>(tmpMesh));
		tmpMesh->setToBed();
	}

	sliceButton->Enable(true);
	writeButton->Enable(true);
	printButton->Enable(true);
	if (tmpMesh->hasTexture())
		m_toolBar->EnableTool(m_ColourAdjust->GetId(), true);
}

void MainFrame::bedEmptied() {
	sliceButton->Enable(false);
	writeButton->Enable(false);
	printButton->Enable(false);
}

void MainFrame::OnClearSupport(wxCommandEvent& ev)
{
	PrintBed::getSingleton().clearSupport();
}

void MainFrame::bedReady(wxCommandEvent& event)
{
	sliceButton->Enable(true);
	writeButton->Enable(true);
	printButton->Enable(true);
	for (auto &it : PrintBed::GetSingleton().getDrawables())
	{
		if(it->hasTexture())
			m_toolBar->EnableTool(m_ColourAdjust->GetId(), true);
	}
}

void MainFrame::clearBed(wxCommandEvent& event)
{
	sliceButton->Enable(false);
	PrintBed::getSingleton().clearBed();
	m_toolBar->EnableTool(m_ColourAdjust->GetId(), false);
}

void MainFrame::OnOpenImage(wxCommandEvent& event)
{
	wxString fileName;
	wxFileDialog
		openFileDialog(this, _("Open image file"), "", "",
			"PNG and jpeg files (*.png;*.jpg;*.jpeg;*.bmp;*.tif)|*.png;*.jpg;*.jpeg;*.bmp;*.tif", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	activeFile = openFileDialog.GetFilename();
	auto tempMesh = PrintBed::GetSingleton().loadImage(openFileDialog.GetPath().ToStdString());
	if (!tempMesh)
	{
		wxMessageBox("Failed to open image", "Error");
	}
	else
	{
		sliceButton->Enable(true);
		PrintBed::getSingleton().centreMesh(std::dynamic_pointer_cast<iDrawable>(tempMesh));
	}
	sliceButton->Enable(true);
	writeButton->Enable(true);
	printButton->Enable(true);
	m_toolBar->EnableTool(m_ColourAdjust->GetId(), true);
}

void MainFrame::sliceThread()
{
	PrintBed::GetSingleton().sliceBedContents(this);
}

void MainFrame::OnSlice(wxCommandEvent& event)
{
	if (GET_BOOL("rova5Compat"))
	{
		auto colours = PrintBed::getSingleton().getColoursOnBed();
		if (colours.size() == 1)
		{
			Settings::getSingleton().resetColourToolMap();
			Settings::getSingleton().addColourToolPair(colours.front(),0);
		}
		else if (!colours.empty()) // Dont bring up compatibility window when no texture mesh presnet TODO: decide what to happen when slicing manual colouring
		{
			std::unique_ptr<Rova5Compat> compatFrame = std::make_unique<Rova5Compat>(this, colours);
			int result = compatFrame->ShowModal();
			if (result != wxID_OK)
				return;
			compatFrame->populateSettingsToolMap();
		}
	}
	std::thread sliceThread(&MainFrame::sliceThread, this);
	sliceThread.detach();
}

void MainFrame::setMachineControlPanel(machineControl* machine)
{
	ptr_machineControl = machine;
}

void MainFrame::setNotebook(wxNotebook* notebook)
{
	ptr_notebook = notebook;
}

void MainFrame::OnUpdateDisplay(wxCommandEvent &event)
{
	Refresh();
}

void MainFrame::OnNewBed(wxCommandEvent& event)
{
	if (!PrintBed::GetSingleton().getDrawables().empty())
	{
		//Add yes no box
		int answer = wxMessageBox("Discard current print bed?", "Clear Bed", wxYES_NO);
		if (answer == wxYES)
			PrintBed::GetSingleton().clearBed();
	}
}

void MainFrame::OnImportBed(wxCommandEvent& event)
{
	wxString fileName;
	wxFileDialog
		openFileDialog(this, _("Open print Bed"), "", "",
			"BodySplitter files (*.rova)|*.rova", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	activeFile = openFileDialog.GetFilename();
	int answer = wxMessageBox("Import print settings?", "Print Settings", wxYES_NO);
	PrintBed::GetSingleton().importBed(openFileDialog.GetPath().ToStdString(), answer == wxYES);
	if (PrintBed::GetSingleton().getDrawables().empty() == false)
	{
		sliceButton->Enable(true);
		writeButton->Enable(true);
		printButton->Enable(true);
		for (auto &it : PrintBed::GetSingleton().getDrawables())
		{
			if (it->hasTexture())
				m_toolBar->EnableTool(m_ColourAdjust->GetId(), true);
		}
	}
}

void MainFrame::OnExportBed(wxCommandEvent& event)
{
	wxString outputFileName = activeFile.SubString(0, activeFile.Last('.'));
	outputFileName += "rova";
	wxFileDialog writeFileDialog(this, _("Write Bed File"), "", outputFileName,
		"BodySplitter Files (*.rova)|*.rova", wxFD_SAVE);
	if (writeFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}

	PrintBed::getSingleton().exportBed(writeFileDialog.GetPath().ToStdString());
}

void MainFrame::updateControlButton(ModelViewPane::ControlModes mode)
{
	m_toolBar->ToggleTool(m_moveTool->GetId(), mode == ModelViewPane::ControlModes::MOVE);
	m_toolBar->ToggleTool(m_rotateTool->GetId(), mode == ModelViewPane::ControlModes::ROTATE);
	m_toolBar->ToggleTool(m_scaleTool->GetId(), mode == ModelViewPane::ControlModes::SCALE);
	m_toolBar->ToggleTool(m_AddSupport->GetId(), mode == ModelViewPane::ControlModes::ADDSUPPORT);
	m_toolBar->ToggleTool(m_RemoveSupport->GetId(), mode == ModelViewPane::ControlModes::REMOVESUPPORT);
}

void MainFrame::setPropertiesPanelState(bool state)
{
	m_toolBar->ToggleTool(m_leftChevron->GetId(), state);
	m_toolBar->ToggleTool(m_rightChevron->GetId(), !state);
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(ID_CLEAR, MainFrame::OnNewBed)
EVT_MENU(ID_IMPORT_BED, MainFrame::OnImportBed)
EVT_MENU(ID_EXPORT_BED, MainFrame::OnExportBed)
EVT_MENU(ID_OPEN, MainFrame::OnOpen)
EVT_MENU(ID_SLICE, MainFrame::OnSlice)
EVT_MENU(ID_SETTINGS, MainFrame::OnSettings)
EVT_MENU(ID_OPEN_IMAGE, MainFrame::OnOpenImage)
EVT_MENU(ID_WRITE_GCODE, MainFrame::OnWriteGCode)
EVT_MENU(ID_PRINT_GCODE, MainFrame::OnWriteGCode)
EVT_MENU(ID_SWITCH_TEXTURE, MainFrame::OnSimpleTexture)
EVT_MENU(ID_SWITCH_PERSPECTIVE, MainFrame::OnSwitchPerspective)
EVT_MENU(ID_SWITCH_TO_WIREFRAME, MainFrame::OnSwitchToWire)
EVT_MENU(wxID_EXIT, MainFrame::OnExit)
EVT_MENU(wxID_CLOSE, MainFrame::OnExit)
EVT_CLOSE(MainFrame::OnClose)
EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
EVT_MENU(ID_REMOVE_SUPPORT, MainFrame::OnClearSupport)
EVT_COMMAND(STATUS_UPDATE_ID, wxEVT_COMMAND_TEXT_UPDATED, MainFrame::OnUpdateStatusBar)
EVT_COMMAND(UPDATE_DISPLAY_ID, wxEVT_COMMAND_TEXT_UPDATED, MainFrame::OnUpdateDisplay)
EVT_COMMAND(ID_BED_READY, wxEVT_COMMAND_TEXT_UPDATED, MainFrame::bedReady)
wxEND_EVENT_TABLE()

#endif