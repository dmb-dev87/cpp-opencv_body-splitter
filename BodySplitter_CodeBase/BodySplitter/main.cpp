#include <memory>
#include <thread>
#include <spdlog/spdlog.h>

#include "Settings.h"
#include "Geometry.h"


#ifndef CONSOLE

// include OpenGL

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#ifdef _WIN32
	#pragma comment(lib,"glew32.lib")
	#pragma comment(lib,"opengl32.lib")
#include <wx/msw/registry.h>
#endif

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/glcanvas.h> // Muist occur after glew.h
#include <wx/notebook.h>
#include <wx/cmdline.h>
#include <wx/cmdargs.h>
#if defined(_WIN32) || defined(__WXMAC__)
#include <wx/appprogress.h>
#endif
#include <wx/splash.h>

#include "images/splashscreen.h"
#include "images/rsimages.h"

#include "PrintBed.h"

#include "EULAOverride.h"

#include "gCodeControlWindow.h"
#include "MainFrame.h"
#include "Dialogs.h"

#include "Slicer.h"
#include "TestPrint.h"
#include "machineControl.h"
#include "Mesh.h"
#include "CoasterPrint.h"

#include "ModelViewPane.h"
#include "PaintViewPane.h"
#include "GCodeViewPane.h"

#include "Settings.h"

// Store the config and log files in the same dir as EXE, can be used for portable impl
#ifndef STORE_FILES_WITH_EXE
#include <wx/stdpaths.h>
#endif

//Why is this a global variable, rob?
static machineControl *m_ptrMachinePanel = nullptr;

class MyApp : public wxApp, public Singleton<MyApp>
{
	virtual bool OnInit();
	virtual int OnExit() override;
	void OnEndSession(wxCloseEvent& event);

	MainFrame *frame = nullptr;
	//ModelTab*		m_ptrModelTab;
	ModelViewPane *modelView = nullptr;
	GCodeViewPane *gcodeView = nullptr;
	PaintViewPane *paintView = nullptr;
public:
	void idle(wxIdleEvent& evt);
	void force_refresh()
	{
	  frame->Refresh();
	}
	void redraw()
	{
		PrintBed::getSingleton().forceDisplayUpdate();
		force_refresh();
	}
	~MyApp()
	{

	}
	wxDECLARE_EVENT_TABLE();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	// Used to load the splash screen (note transparency is sadly not handled
	wxImage::AddHandler(new wxPNGHandler);
#ifndef STORE_FILES_WITH_EXE
	auto file_logger = spdlog::rotating_logger_mt("DEBUG_FILE", wxStandardPaths::Get().GetUserConfigDir().ToStdString() + "\\BodySplitterdebug.log", 1048576 * 5, 3);
#else
	auto file_logger = spdlog::rotating_logger_mt("DEBUG_FILE", "BodySplitterdebug.log", 1048576 * 5, 3);
#endif
#ifdef __WIN32
	file_logger->flush_on(spdlog::level::info);
#endif
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	Settings* pSettings = new Settings();
#ifdef _WIN32
	{
		wxRegKey tempKey(wxRegKey::HKCU, "Software\\");
		if (tempKey.HasSubKey("BodySplitterEULA"))
		{

		}
		else
		{
			//show eula
			EULAOverride euladialog(nullptr);
			int eularesult = euladialog.ShowModal();
			if (eularesult != wxID_YES)
				return 0;
			wxRegKey key(wxRegKey::HKCU, "Software\\BodySplitterEULA");
			key.Create();
			// Create a new value "MyValue" and set it to 12.
			key.SetValue("Version", "0.1.00");
		}
	}
#endif
	
#ifdef SHOW_SPLASH_SCREEN
	wxBitmap bitmap;
	bitmap = wxBITMAP_PNG_FROM_DATA(bin2c_rssplashscreen);
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
			wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT,
			5000, NULL, -1, wxDefaultPosition, wxDefaultSize,
			wxBORDER_SIMPLE | wxSTAY_ON_TOP);
	}
	wxYield();


#endif

#ifndef NDEBUG
	frame = new MainFrame(wxT("BodySplitter - DEBUG BUILD"), wxPoint(50, 50), wxSize(1024, 800));
#else
	frame = new MainFrame(wxT("BodySplitter"), wxPoint(50, 50), wxSize(1024, 800));
#endif
	frame->SetMaxSize(wxSize(-1, -1));
	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	PrintBed* pPrintBed = new PrintBed(frame);

	auto m_notebook1 = new wxNotebook(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
	m_notebook1->SetMaxSize(wxSize(-1, -1));
	auto m_NormalPanel = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_NormalPanel->SetMaxSize(wxSize(-1, -1));
	m_notebook1->AddPage(m_NormalPanel, wxT("3D View"), false);
	//auto m_GCodePanel = new wxPanel(m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	auto m_GCodePanel = new gCodeControlWindow(m_notebook1, args, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook1->AddPage(m_GCodePanel, wxT("GCode Preview"), false);
	sizer->Add(m_notebook1, 1, wxEXPAND | wxALL, 1);
	frame->SetSizer(sizer);
	{
		wxBoxSizer* modelSizer = new wxBoxSizer(wxHORIZONTAL);
		modelView = new ModelViewPane((wxFrame*)m_NormalPanel, args);
		modelView->registerSizer(modelSizer);
		modelView->createTrackball();
		frame->m_toolBar = new wxToolBar(m_NormalPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_VERTICAL);
		frame->m_toolBar->SetToolBitmapSize(wxSize(32, 32));
		//m_tool8 = m_toolBar2->AddTool(wxID_ANY, wxT("Move"), wxBitmap(wxT("C:\\Users\\Robert\\Documents\\Visual Studio 2015\\Projects\\BodySplitter\\BodySplitter\\move.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);
		frame->m_moveTool = frame->m_toolBar->AddTool(wxID_ANY, wxT("Move"), wxBITMAP_PNG_FROM_DATA(bin2c_move), wxNullBitmap, wxITEM_NORMAL,
			wxT("Click and hold on object; move mouse to reposition on grid. Click button again to deselect"), wxT("Click and hold on object; move mouse to reposition on grid. Click button again to deselect"), NULL);
		frame->m_rotateTool = frame->m_toolBar->AddTool(wxID_ANY, wxT("Rotate"), wxBITMAP_PNG_FROM_DATA(bin2c_rotate), wxNullBitmap, wxITEM_NORMAL,
			wxT("Click and hold on object: move mouse to rotate on grid. Click button again to deselect"), wxEmptyString, NULL);
		frame->m_scaleTool = frame->m_toolBar->AddTool(wxID_ANY, wxT("Scale"), wxBITMAP_PNG_FROM_DATA(bin2c_scale), wxNullBitmap, wxITEM_NORMAL, \
			wxT("Click and hold on object: move mouse to change size. Click button again to deselect"), wxEmptyString, NULL);
		//auto m_clipTool = toolBar->AddTool(wxID_ANY, wxT("Clip"), wxBitmap(wxT("clip.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxT("Clip"), wxEmptyString, NULL);
		auto m_sliceTool = frame->m_toolBar->AddTool(wxID_ANY, wxT("Slice"), wxBITMAP_PNG_FROM_DATA(bin2c_slice), wxNullBitmap, wxITEM_NORMAL,
			wxT("Slice"), wxEmptyString, NULL);
		auto m_clearTool = frame->m_toolBar->AddTool(wxID_ANY, wxT("Clear"), wxBITMAP_PNG_FROM_DATA(bin2c_clearbeds), wxNullBitmap, wxITEM_NORMAL,
			wxT("Remove objects from grid"), wxEmptyString, NULL);
		auto m_GenSupport = frame->m_toolBar->AddTool(wxID_ANY, wxT("Support"), wxBITMAP_PNG_FROM_DATA(bin2c_supportGen), wxNullBitmap, wxITEM_NORMAL,
			wxT("Auto generate support material. Click button again to deselect"), wxEmptyString, NULL);
		frame->m_AddSupport = frame->m_toolBar->AddTool(wxID_ANY, wxT("Add"), wxBITMAP_PNG_FROM_DATA(bin2c_addsupport), wxNullBitmap, wxITEM_NORMAL,
			wxT("Manually add individual support material. Bar turns green when placed in acceptable position. When in desired position click and bar turns lavender. Click button again to deselect"), wxEmptyString, NULL);
		frame->m_RemoveSupport = frame->m_toolBar->AddTool(wxID_ANY, wxT("Remove"), wxBITMAP_PNG_FROM_DATA(bin2c_removesupport), wxNullBitmap, wxITEM_NORMAL,
			wxT("Remove individual support material"), wxEmptyString, NULL);
		frame->m_ColourAdjust = frame->m_toolBar->AddTool(wxID_ANY, wxT("Colours"), wxBITMAP_PNG_FROM_DATA(bin2c_colour), wxNullBitmap, wxITEM_NORMAL,
			wxT("Used to manually adjust and preview changes to the object’s texture"), wxEmptyString, NULL);
		frame->m_ColourAdjust->Enable(false);
		frame->m_toolBar->AddSeparator();
		frame->m_leftChevron = frame->m_toolBar->AddTool(wxID_ANY, wxT("Open Properties"), wxBITMAP_PNG_FROM_DATA(bin2c_leftchevron), wxNullBitmap, wxITEM_NORMAL,
			wxT("Open object properties"), wxEmptyString, NULL);
		frame->m_rightChevron = frame->m_toolBar->AddTool(wxID_ANY, wxT("Close Properties"), wxBITMAP_PNG_FROM_DATA(bin2c_rightchevron), wxNullBitmap, wxITEM_NORMAL,
			wxT("Close object properties"), wxEmptyString, NULL);
		frame->m_toolBar->AddSeparator();
		auto cutSupport = frame->m_toolBar->AddTool(wxID_ANY, wxT("Cut Support"), wxBITMAP_PNG_FROM_DATA(bin2c_rightchevron), wxNullBitmap, wxITEM_NORMAL,
			wxT("Cut Support"), wxEmptyString, NULL);

		frame->m_moveTool->SetToggle(true);
		frame->m_rotateTool->SetToggle(true);
		frame->m_scaleTool->SetToggle(true);
		frame->m_AddSupport->SetToggle(true);
		frame->m_RemoveSupport->SetToggle(true);
		frame->m_leftChevron->SetToggle(true);
		frame->m_rightChevron->SetToggle(true);
		frame->m_toolBar->Realize();
		
		//Connect(0xFEEF, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ModelViewPane::moveMode));
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::moveMode, modelView, frame->m_moveTool->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MyApp::clickToolBarItem, this, m_moveTool->GetId());

		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::rotateMode, modelView, frame->m_rotateTool->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MyApp::clickToolBarItem, this, m_rotateTool->GetId());

		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrame::OnSlice, frame, m_sliceTool->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::scaleMode, modelView, frame->m_scaleTool->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MyApp::clickToolBarItem, this, m_scaleTool->GetId());

		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrame::clearBed, frame, m_clearTool->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &PrintBed::generateSupport, PrintBed::GetSingletonPtr(), m_GenSupport->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::addSupportMode, modelView, frame->m_AddSupport->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::removeSupportMode, modelView, frame->m_RemoveSupport->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MyApp::clickToolBarItem, this, m_AddSupport->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &MainFrame::OnColourAdjust, frame, frame->m_ColourAdjust->GetId());

		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::openPropertiesButton, modelView, frame->m_leftChevron->GetId());
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::closePropertiesButton, modelView, frame->m_rightChevron->GetId());

		
		m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &PrintBed::cutSupport, PrintBed::GetSingletonPtr(), cutSupport->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::clipMode, modelView, cutSupport->GetId());
		//m_NormalPanel->Bind(wxEVT_COMMAND_TOOL_CLICKED, &ModelViewPane::clipMode, modelView, m_clipTool->GetId());
		//m_NormalPanel->Connect(m_moveTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, &modelView->moveMode);
		//modelView->Connect(m_moveTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ModelViewPane::moveMode));
		//Connect(0xFEEEEF, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(ModelViewPane::rotateMode));

		frame->m_toolBar->Realize();
		frame->m_toolBar->SetMaxSize(wxSize(36, -1));
		
		modelSizer->Add(modelView, 1, wxEXPAND | wxALL, 1);
		modelSizer->Add(frame->m_toolBar, 1, wxEXPAND | wxALL, 1);
		m_NormalPanel->SetSizer(modelSizer);
	}
	{
		//Paint Tab
		//wxBoxSizer* paintPanesizer = new wxBoxSizer(wxHORIZONTAL);
		////controls = new ControlWindow(m_GCodePanel, -1, wxPoint(1024 - 125, 0), wxSize(100, 800), 0L, "controls");
		//paintView = new PaintViewPane((wxFrame*)m_PaintPanel, args);
		////paintView->SetPosition(wxPoint(0, 0));
		////paintView->SetSize(wxSize(1024, 800));
		//paintView->createTrackball();
		//paintPanesizer->Add(paintView, 1, wxEXPAND | wxALL, 1);
		//m_PaintPanel->SetSizer(paintPanesizer);
	}
	m_ptrMachinePanel = new machineControl(m_notebook1);
	m_notebook1->AddPage(m_ptrMachinePanel, "Printer Control", false);
	frame->setMachineControlPanel(m_ptrMachinePanel);
	frame->setNotebook(m_notebook1);
	frame->Show();
	PrintBed::GetSingleton().registerViewPanes(modelView, gcodeView);

	//auto degubFile = spdlog::create<spdlog::sinks::simple_file_sink_mt>("DEBUG_FILE", "debug.log", true);
	wxString cmdFilename;
	wxCmdLineParser cmdParser(argc, argv);
	// Check for a project filename
	if (cmdParser.GetParamCount() > 0)
	{
		cmdFilename = cmdParser.GetParam(0);

		// Under Windows when invoking via a document
		// in Explorer, we are passed the short form.
		// So normalize and make the long form.
		wxFileName fName(cmdFilename);
		fName.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS |
			wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE);
		cmdFilename = fName.GetFullPath();
		PrintBed::GetSingleton().importBed(cmdFilename.ToStdString());
	}
	return true;
}

int MyApp::OnExit()
{
	return 0;
}

/* End session means logout or shutdown */
void MyApp::OnEndSession(wxCloseEvent& event)
{
	// Either it can never be veto'd or this code is broken
	/*
	if (event.CanVeto() && !PrintBed::getSingleton().getDrawables().empty())
	{
		wxMessageDialog temp(nullptr, "Bed has objects on it, abort shutdown?", "Discard", wxYES_NO);
		if (temp.ShowModal() == wxID_YES)
		{
			event.Skip();
			return;
		}
	}
	*/
	// This is bad behaviour - but its speedy
	exit(0);
}

wxBEGIN_EVENT_TABLE(MyApp, wxApp)
EVT_END_SESSION(MyApp::OnEndSession)
END_EVENT_TABLE()

void MyApp::idle(wxIdleEvent& evt)
{
	evt.RequestMore();
}

void UPDATE_DISPLAY() {
	MyApp::getSingleton().redraw();
}

#else
/* Command line functionality - yes its hacky */
#include <wx/cmdline.h>
#include <wx/app.h>

#include "iSlicable.h"
#include "Slicer.h"
#include "Mesh.h"
#include "GCodeWriter.h"
#include "SupportManager.h"
#include "PrintBed.h"
#include "UserMesh.h" // added by freelancer latino

void slicePrintBed(PrintBed* bed, const std::string &output)
{
	bed->sliceBedContents();
#ifdef CONSOLE
	if (Settings::getSingleton().getBoolValue("verbose"))
		std::cerr << "Sliced bed contents\n";
#endif
	std::unique_ptr<GCodeWriter> writer = std::make_unique<GCodeWriter>(output);

	//SEL 2020 If we are simply exporting polygons for the program BodySlicer to put back together,
	// then we can exit right now and not bother generating all the GCode.
	if (Settings::getSingleton().getBoolValue("exportPolygons"))
	{
		goto SKIP_REST_IF_EXPORTING_POLYGONS;
	}
	//SEL done

	for (auto it : bed->getDrawables())
	{
		writer->addSlicable(it);
	}
	writer->startWriter();
#ifdef CONSOLE
	if (Settings::getSingleton().getBoolValue("verbose"))
		std::cerr << "Writing GCode\n";
#endif
	for (int i = 0; i < writer->getLayerCount(); i++)
	{
		std::cerr << "Layer: " << i << std::endl;
		writer->writeLayer(i);
	}
	writer->endWriter();

SKIP_REST_IF_EXPORTING_POLYGONS:

	std::cerr << "Slice complete\n";
}

void sliceMesh(std::shared_ptr<UserMesh> mesh, const std::string &output)
{
	auto temp = std::dynamic_pointer_cast<iSlicable>(mesh);
	double midX = Settings::GetSingleton().getDoubleValue("maxX") / 2.0;
	double midY = Settings::GetSingleton().getDoubleValue("maxY") / 2.0;
	mesh->setOFfsetX(midX - mesh->getXExtent() / 2.0);
	mesh->setOFfsetY(midY - mesh->getYExtent() / 2.0);

	if (Settings::getSingleton().getBoolValue("useSupport"))
	{
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Generating support towers\n";
		mesh->generateSupport();
	}

	std::shared_ptr<Slicer>m_ptrSlicer = std::make_shared<Slicer>();

	std::shared_ptr<iSlicable> toSlice(temp);

	m_ptrSlicer->initSlicer(toSlice);
	m_ptrSlicer->slice();
	auto m_supportManagerPtr = std::make_shared<SupportManager>();
	if (Settings::getSingleton().getBoolValue("useSupport"))
	{
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Preparing support\n";

		std::list<std::shared_ptr<iDrawable>> objects;
		objects.push_back(std::dynamic_pointer_cast<iDrawable>(mesh));
		m_supportManagerPtr->addObjects(objects);
		m_supportManagerPtr->prepareSupport();
	}

	std::unique_ptr<GCodeWriter> writer;
	writer = std::make_unique<GCodeWriter>(output);
	writer->addSlicable(toSlice);
	if (Settings::GetSingleton().getBoolValue("useSupport"))
		writer->registerSupportManager(m_supportManagerPtr);
	writer->startWriter();
#ifdef CONSOLE
	if (Settings::getSingleton().getBoolValue("verbose"))
		std::cerr << "Writing GCode\n";
#endif
	for (int i = 0; i < writer->getLayerCount(); i++)
	{
		std::cerr << "Layer: " << i << std::endl;
		writer->writeLayer(i);
	}
	writer->endWriter();
	std::cerr << "Slice complete\n";
}

int gl_nSEL = 0; //SEL debug.  Set this to non zero for verbose print  //SEL March 2020  I changed this to the
				 // layer ID for ease for future logging.  This does mean that I can't log layer 0.  I have never
				 // have done this so I will live with the limitation.   I could switch from zero based numbers
				 // but no, that would be too easy.

int main(int argc, char **argv)
{

	wxString input, output;
	PrintBed *ptr_PrintBed = new PrintBed();
	Settings* pSettings = new Settings(argc,argv,input,output);
	if (output.empty())
		output = "output.gcode";


	auto file_logger = spdlog::rotating_logger_mt("DEBUG_FILE", "debug.log", 1048576 * 5, 3);
#ifdef __WIN32
	file_logger->flush_on(spdlog::level::info);
#endif
	if (input.empty())
	{
		std::cerr << "Invalid parameters\n";
		return 0;
	}
	if (Settings::getSingleton().getBoolValue("verbose"))
		std::cerr << "Slicing: " << input << "\n";
	auto logger = spdlog::get("DEBUG_FILE");
	logger->info("Loading file " + input.ToStdString());


	/*
	Settings::getSingleton().addColourToolPair(Colour((uint8_t)255, 196, 150), 0);
	Settings::getSingleton().addColourToolPair(Colour((uint8_t)107,  69,  59), 1);
	Settings::getSingleton().addColourToolPair(Colour((uint8_t)176, 146, 126), 2);
	Settings::getSingleton().addColourToolPair(Colour((uint8_t)   0,   1,  0), 3);
	Settings::getSingleton().addColourToolPair(Colour((uint8_t)255, 255, 255), 4);
	*/


	if (input.substr(input.length() - 4) == "rova")
	{
		ptr_PrintBed->importBed(input.ToStdString());

		// all these code added by freelancer latino
		auto colours = PrintBed::getSingleton().getColoursOnBed();
		if (colours.size() == 1)
		{
			Settings::getSingleton().resetColourToolMap();
			Settings::getSingleton().addColourToolPair(colours.front(), 0);
		}
		else if (!colours.empty()) // Dont bring up compatibility window when no texture mesh presnet TODO: decide what to happen when slicing manual colouring
		{
			/*
			std::unique_ptr<Rova5Compat> compatFrame = std::make_unique<Rova5Compat>(this, colours);
			int result = compatFrame->ShowModal();
			if (result != wxID_OK)
			return;
			compatFrame->populateSettingsToolMap();
			*/
			Settings::getSingleton().resetColourToolMap();
			int n = colours.size();
			for (int i = 0; i < n; i++)
			{
				Settings::getSingleton().addColourToolPair(colours[i], i);
			}
		}


#ifdef _DEBUG	// added for freelancer latino for debugging only...
		std::vector<Colour> col = ptr_PrintBed->getColoursOnBed();
#endif
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Imported print bed: " << input << "\n";
		slicePrintBed(ptr_PrintBed, output.ToStdString());
	}
	else
	{/*
		std::shared_ptr<UserMesh> tempMeshPtr = std::make_shared<UserMesh>();
		//std::shared_ptr<Mesh> tempMeshPtr = std::make_shared<Mesh>(); // commented by freelancerlatino
		if (!tempMeshPtr->loadMesh(input.ToStdString()))
		{
			std::cout << "Error, failed to load mesh\n";
			return false;
		}

		sliceMesh(tempMeshPtr, output.ToStdString());
		*/
		PrintBed *pBed = &PrintBed::getSingleton();
		auto tmpMesh = pBed->loadMesh(input.ToStdString());
		if (!tmpMesh)
		{
			std::cerr << "Error opening Model" << std::endl;
		}
		else
		{
			pBed->centreMesh(std::dynamic_pointer_cast<iDrawable>(tmpMesh));
			tmpMesh->setToBed();
			if (tmpMesh->hasTexture())
			{
					//std::string tex_path = tmpMesh->getAssimpTexturePath();
					//pBed->loadImage(tex_path);
			}
		}
		//PrintBed::getSingleton().switchToSimpleTexture();
		//pBed->updateTextureColour(5); //!!!xxxxx  it looks like the number of colours is hard coded here
		//SEL  let's set the maximum number of colours from the ini file
		double dMaxColours = Settings::GetSingleton().getDoubleValue("MaximumColours");
		pBed->updateTextureColour((uint16_t)dMaxColours);

		auto colours = pBed->getColoursOnBed();
		if (colours.size() == 1)
		{
			Settings::getSingleton().resetColourToolMap();
			Settings::getSingleton().addColourToolPair(colours.front(), 0);
		}
		else if (!colours.empty()) // Dont bring up compatibility window when no texture mesh presnet TODO: decide what to happen when slicing manual colouring
		{
			/*
			std::unique_ptr<Rova5Compat> compatFrame = std::make_unique<Rova5Compat>(this, colours);
			int result = compatFrame->ShowModal();
			if (result != wxID_OK)
			return;
			compatFrame->populateSettingsToolMap();
			*/
			Settings::getSingleton().resetColourToolMap();
			int n = colours.size();
			std::cerr << "n colors on bed = " << n << std::endl;
			for (int i = 0; i < n; i++)
			{
				Settings::getSingleton().addColourToolPair(colours[i], i);
			}
		}


	#ifdef _DEBUG	// added for freelancer latino for debugging only...
		std::vector<Colour> col = ptr_PrintBed->getColoursOnBed();
	#endif
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Imported print bed: " << input << "\n";
		slicePrintBed(ptr_PrintBed, output.ToStdString());
	}
}

#endif