#pragma once
#include <wx/wx.h>
#include "newSettings.h"
#include "Geometry.h"
#include "objectControl.h"

/* For the most part these are not used, the side panel control
is much more functional and intuitive */

class ScaleDialog :
	public wxDialog
{
private:
	double xAmt = 1; double yAmt = 1; double zAmt = 1;
	wxTextCtrl *textControlX;
	wxTextCtrl* textControlY;
	wxTextCtrl* textControlZ;
public:
	ScaleDialog();
	~ScaleDialog();
	double getXScale()const;
	double getYScale()const;
	double getZScale()const;


};

class RotationDialog : public wxDialog
{
private:
	wxTextCtrl* rotationAmount;

public:
	RotationDialog(BodySplitter::AXIS axis);
	~RotationDialog();
	double getRotationAmount()const;
};

class MeshControl : public slicableMenu
{
private:
public:
	MeshControl(bool isTextureUse,bool hasTexture,const Colour &col);
	~MeshControl();

	void UseTextureCheck(wxCommandEvent& event);

	bool getUsingTexture();
	Colour getColour();
};