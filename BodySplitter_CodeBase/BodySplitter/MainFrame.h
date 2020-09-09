#pragma once
#include <wx/wx.h>
#include <memory>

#include "Singleton.h"
#include "ModelViewPane.h"

/* Holds the notebook that has modelview,gcodeview and the menu */

class machineControl;
class wxNotebook;

//TODO central file to avoid duplicartion of message ids
const unsigned int UPDATE_DISPLAY_ID = 10001;

enum
{

	ID_SLICE = 2,
	ID_SETTINGS = 3,

	ID_OPEN_IMAGE = 5,
	ID_SCALE = 6,
	ID_ROTATE = 7,
	ID_SWITCH_TEXTURE = 8,
	ID_SWITCH_TO_WIREFRAME = 9,
	ID_SWITCH_PERSPECTIVE = 10,
	ID_SCROLL = 11,
	ID_ADD_MESH = 12,
	ID_UPDATE_COLOR = 13,
	ID_DELETE_LINE = 14,
	ID_ROTATE_X = 15,
	ID_ROTATE_Y = 16,
	ID_ROTATE_Z = 17,
	ID_REMOVE_SUPPORT = 18,

	ID_OPEN = 19,
	ID_CLEAR = 20,
	ID_IMPORT_BED = 21,
	ID_EXPORT_BED = 22,


	ID_TEST_PRINT = 40,
	ID_PRINT_COLOURS = 41,

	ID_WRITE_GCODE = 50,
	ID_PRINT_GCODE = 51,

	ID_SUBMIT_FEEDBACK = 60,
	ID_GET_HELP = 61,

	ID_BED_READY = 70
};


class MainFrame : public wxFrame, public Singleton<MainFrame>
{
public:
	MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	void OnSlice(wxCommandEvent& event);
	void clearBed(wxCommandEvent& event);
	void setMachineControlPanel(machineControl* machine);
	void setNotebook(wxNotebook* notebook);

	void OnColourAdjust(wxCommandEvent& event);
	void SetStatusBarText(const std::string &text, int val = -1);
	void bedEmptied();
	void updateControlButton(ModelViewPane::ControlModes mode);
	void setPropertiesPanelState(bool state);
private:
	friend class MyApp;
	wxToolBar* m_toolBar;
	wxToolBarToolBase* m_moveTool;
	wxToolBarToolBase* m_rotateTool;
	wxToolBarToolBase* m_scaleTool;
	wxToolBarToolBase* m_AddSupport;
	wxToolBarToolBase* m_RemoveSupport;
	wxToolBarToolBase* m_ColourAdjust;
	wxToolBarToolBase* m_GenSupport;
	wxToolBarToolBase* m_leftChevron;
	wxToolBarToolBase* m_rightChevron;

	void bedReady(wxCommandEvent &event);
	
	machineControl* ptr_machineControl;
	wxNotebook* ptr_notebook;

	void OnOpen(wxCommandEvent& event);
	//void OnOpenTestCircle(wxCommandEvent &event);
	//void OnAddMesh(wxCommandEvent& event);
	void OnOpenImage(wxCommandEvent& event);

	void OnNewBed(wxCommandEvent& event);
	void OnImportBed(wxCommandEvent& event);
	void OnExportBed(wxCommandEvent& event);

	void OnSettings(wxCommandEvent &event);
	void OnExit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnWriteGCode(wxCommandEvent& event);
	void OnSimpleTexture(wxCommandEvent& event);
	void OnSwitchToWire(wxCommandEvent& event);
	void OnSwitchPerspective(wxCommandEvent& event);

	void OnClearSupport(wxCommandEvent &ev);

	void OnUpdateStatusBar(wxCommandEvent &event);
	void OnUpdateDisplay(wxCommandEvent &event);
	wxMenuItem* sliceButton;
	wxMenuItem* printButton;
	wxMenuItem* writeButton;
	wxMenuItem* viewSimpleButton;
	wxMenuItem* viewAsWireFrame;
	wxMenuItem* viewSwitchPerspective;
#if defined(_WIN32) || defined(__WXMAC__)
	wxAppProgressIndicator* taskBarIndicator;
#endif
	void sliceThread();
	
	std::string fileNameMesh;

	wxDECLARE_EVENT_TABLE();
	wxString activeFile = "";
};
