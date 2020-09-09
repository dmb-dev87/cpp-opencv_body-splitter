#include "View3DPane.h"
#include <wx/window.h>
#include <wx/wx.h>

#include "CameraControl.h"
#include "GridView.h"
#include "OpenglExt.h"
#include "Settings.h"
#include "PrintBed.h"

#ifndef CONSOLE

wxGLContext* View3DPane::m_context = nullptr;

static bool hasInited = false;

View3DPane::View3DPane(wxFrame* parent, int* args) :
	wxGLCanvas((wxWindow*)parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	//if (!hasInited)
	//{
	//	hasInited = true;

	//}
	this->DragAcceptFiles(true);
	m_ptrCamera = std::make_unique<CameraControl>();
	m_ptrGrid = std::make_unique<GridView>();

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

View3DPane::~View3DPane()
{
	//This doesnt seem to work...
	//if (m_context)
	//	delete m_context;
}

int View3DPane::getWidth() {
	return GetSize().x;
}

int View3DPane::getHeight() {
	return GetSize().y;
}

void View3DPane::createTrackball()
{
	m_ptrTrackball = std::make_unique<TrackBall>(100.0, getWidth(), getHeight());
}

void View3DPane::prepareViewport(unsigned int opts)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glClearDepth(1.0f);	// Depth Buffer Setup
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glViewport(0, 0, getWidth(), getHeight());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float ratio_w_h = (float)(getWidth()) / (float)(getHeight());
	if(PrintBed::getSingleton().isPerspective())
		gluPerspective(zoomAmtPerspective /*view angle*/, ratio_w_h, 400 /*clip close*/, 4000 /*clip far*/);
	else
		glOrtho(-(getWidth() / zoomAmtOrtho) / 2.0, (getWidth()/zoomAmtOrtho)/2.0,
			-(getHeight() / zoomAmtOrtho) / 2.0, (getHeight()/zoomAmtOrtho)/2.0, 200, 5000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0, 0, 0, 1);
	glColor3f(1, 0, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Set antialiasing / multisampling

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_COLOR);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	if (opts &= DrawingOptions::BLACK_BACKGROUND)
	{
	}
	else
	{
		glBegin(GL_QUADS);
		//glTexCoord2f(0, 0);
		glColor3f(0.1, 0.1, 0.2);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, -1.0f);
		//glTexCoord2f(1, 1);
		glColor3f(10.0f / 255.0f, 98.0f / 255.0f, 144.0f / 255.0f);
		glVertex2f(1.0f, 1.0f);
		//glTexCoord2f(0, 1);
		glVertex2f(-1.0f, 1.0f);

		glEnd();
	}
	glDisable(GL_COLOR);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);

	glClearDepth(1.0f);	// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// ambient lighting
	float ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// directional source light w!=0
	float pos[] = { 1.0f,1.0f,0.0f,0.0f };
	float spec[] = { 0.8f, 0.8f,0.8f,0.8f };
	float difuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, pos);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, difuse);

	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void View3DPane::draw()
{
	initializeDrawing();
}

void View3DPane::initializeDrawing(unsigned int options)
{
	//Init view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	prepareViewport(options);
	glLoadIdentity();

	//draw background
	//drawBackground();
	// Position camera
	m_ptrCamera->look();
	glGetDoublev(GL_MODELVIEW_MATRIX, m_ptrCamera->modelview);       // Retrieve The Modelview Matrixm_ptrCamera
	glGetDoublev(GL_PROJECTION_MATRIX, m_ptrCamera->projection);     // Retrieve The Projection Matrix
	glGetIntegerv(GL_VIEWPORT, m_ptrCamera->viewport);
	glEnable(GL_LIGHTING);
	if (PrintBed::getSingleton().isWireFrame())
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void View3DPane::finalizeDrawing(unsigned int options)
{
	if ((options & DrawingOptions::NOGRID) > 0)
	{

	}
	else
		m_ptrGrid->draw();

	glFlush();
	SwapBuffers();
}

void View3DPane::resize(wxSizeEvent& evt)
{
	Refresh(); // Calls paint
}

void View3DPane::render(wxPaintEvent& evt)
{
	if (!IsShown())
		return;
	if (m_context == nullptr)
	{
		m_context = new wxGLContext(this);

		//Create the opengl extion thing here???
		wxGLCanvas::SetCurrent(*m_context);
		auto m_pExtensionManager = new OpenglExt();
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::string errorStr((char*)glewGetErrorString(err));
			/* Problem: glewInit failed, something is seriously wrong. */
			wxLogDebug("Error glewInit: %s", errorStr);
			//fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		if (m_pExtensionManager->getMajorVersion() == 1 && m_pExtensionManager->getMinorVersion() < 5)
		{
			//unsupport opengl
			wxMessageBox("BodySplitter requires OpenGL 1.5 and up");
			exit(1);
		}
	}
	else
	  wxGLCanvas::SetCurrent(*m_context);
	wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
	draw();
}

void View3DPane::OnMouse(wxMouseEvent& event)
{
	mouseEvent(event);
}

void View3DPane::mouseEvent(const wxMouseEvent &event)
{
	defaultMouse(event);
}

void View3DPane::defaultMouse(const wxMouseEvent& event)
{
	static bool hasDragged = false;
	static double x1 = event.GetX();
	static double y1 = (getHeight() - event.GetY());
	static glm::quat prevQuat;
	static int startX = 0;
	static int startY = 0;
	this->SetFocus();
	if (event.MiddleDown())
	{
		m_ptrCamera->resetCamera();
		zoomAmtPerspective = 10.0f;
		zoomAmtOrtho = 1.0f;
		Refresh(false);
	}
	if (event.GetWheelRotation())
	{
	//	m_ptrCamera->zoom(-event.GetWheelRotation() / 10.0);
		if (PrintBed::getSingleton().isPerspective())
		{
			if (event.GetModifiers() & wxMOD_SHIFT)
				zoomAmtPerspective += (-event.GetWheelRotation() / 800.0);
			else
				zoomAmtPerspective += (-event.GetWheelRotation() / 400.0);
			zoomAmtPerspective = std::max(0.1, zoomAmtPerspective);
		}
		else
		{
			if (event.GetModifiers() & wxMOD_SHIFT)
				zoomAmtOrtho += (event.GetWheelRotation() / 8000.0);
			else
				zoomAmtOrtho += (event.GetWheelRotation() / 4000.0);
			zoomAmtOrtho = std::max(0.1, zoomAmtOrtho);
		}

		Refresh(false);
	}
	if (event.RightDown() || event.LeftDown())
	{
		//Fixes a "bug" where wxwidgets passes on a double click event
		hasDragged = true;
		startX = event.GetX();
		startY = (getHeight() - event.GetY());
		prevQuat = m_ptrCamera->getQuat();

		x1 = event.GetX();
		y1 = (getHeight() - event.GetY());
		
		double worldX, worldY;
		m_ptrCamera->mouseToWorld(startX,startY,worldX,worldY);
		std::cerr << "worldX: "<<worldX<<" worldY: "<<worldY<<"\n";
	}
	if (event.Dragging() && event.LeftIsDown() &&hasDragged)
	{

#ifdef TURNTABLE
		//if (event.GetX() == x1 && (getHeight() - event.GetY()) == y1)
		//{
		//	//Mouse is being held, reset the start position
		//	startX = x1;
		//	startY = y1;
		//}
		//x1 = event.GetX();
		//y1 = event.GetY();
		m_ptrCamera->rotate((event.GetX() - x1)/*/(double)getWidth()*/, ((getHeight() - event.GetY()) - y1)/*/(double)getHeight()*/);
		x1 = event.GetX();
		y1 = getHeight() - event.GetY();
#else
		hasDragged = true;
		glm::vec3 v1 = glm::normalize(m_ptrTrackBall->getVector(x1, y1));
		glm::vec3 v2 = glm::normalize(m_ptrTrackBall->getVector(event.GetX(), getHeight() - event.GetY()));

		glm::quat delta(v1, v2);

		//Quaternion delta = Quaternion::getQuaternion(v1, v2);
		m_ptrCamera->setQuat(delta * prevQuat);
#endif
		/* orientation has changed, redraw mesh */
		Refresh(false);
	}
	else if (event.Dragging() && event.RightIsDown()) // Pan event
	{
		//Update camera
		int x2 = event.GetX();
		int y2 = (getHeight() - event.GetY());
		m_ptrCamera->pan(x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
		//getWidth();
		Refresh(false);
	}
	else if (event.LeftUp() || event.RightUp())
	{
		if (!hasDragged)
		{
		}
		else
			hasDragged = false;
	}
}
BEGIN_EVENT_TABLE(View3DPane, wxGLCanvas)
EVT_MOUSE_EVENTS(View3DPane::OnMouse)
EVT_SIZE(View3DPane::resize)
EVT_MOUSEWHEEL(View3DPane::OnMouse)
EVT_PAINT(View3DPane::render)
END_EVENT_TABLE()

#endif //if !console