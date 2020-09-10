#pragma once
#include <wx/wx.h>
#include <wx/sizer.h>

class wxImagePanel : public wxPanel
{
	wxBitmap image;

public:
	wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format);
	wxImagePanel(wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr);
	wxImagePanel(wxFrame* parent, wxImage *image);
	wxImagePanel(wxFrame* parent);
	void paintEvent(wxPaintEvent & evt);
	void paintNow();
	bool hasImageSet = false;
	void setImage(wxImage &img);
	void render(wxDC& dc);

	// some useful events
	/*
	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);
	*/

	DECLARE_EVENT_TABLE()
};

