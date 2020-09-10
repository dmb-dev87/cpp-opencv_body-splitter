#pragma once
/* Model view, gcode view and paint view are all derived from this */
#include <GL/glew.h>
#include <wx/glcanvas.h>
#include <wx/event.h>
#include <memory>

class CameraControl;
class GridView;
class TrackBall;

class View3DPane : public wxGLCanvas
{
private:
	
protected:
	double zoomAmtPerspective = 10.0f;
	double zoomAmtOrtho = 1.0;
	static wxGLContext*					m_context;
	std::unique_ptr<CameraControl>	m_ptrCamera;
	std::unique_ptr<GridView>		m_ptrGrid;
	std::unique_ptr<TrackBall>		m_ptrTrackball;
	void prepareViewport(unsigned int opts);
	virtual void draw();
	void defaultMouse(const wxMouseEvent &mouse);
	virtual void mouseEvent(const wxMouseEvent &mouse);
	void initializeDrawing(unsigned int options = 0); // Draw grid, set viewport etc(not in that order)
	void finalizeDrawing(unsigned int options = 0);
public:
	View3DPane(wxFrame* parent, int* args);
	virtual ~View3DPane();

	virtual void createTrackball();
	virtual int getHeight();
	virtual int getWidth();

	// Events
	DECLARE_EVENT_TABLE();
	void render(wxPaintEvent& evt);
	void resize(wxSizeEvent& evt);
	void OnMouse(wxMouseEvent& event);
	//void OnKeyDown(wxKeyEvent& event);
	//void OnKeyUp(wxKeyEvent& event);
	struct sCameraOpts{
		static const int SOLID			= 0x04;
		static const int WIREFRAME		= 0x08;
	};
protected:
	struct DrawingOptions {
		static const unsigned int NONE				= 0;
		static const unsigned int NOGRID			= 1;
		static const unsigned int BLACK_BACKGROUND	= 2;
		//static const unsigned int NONE = 0;
	};
	unsigned int cameraOpts = sCameraOpts::SOLID;
};

