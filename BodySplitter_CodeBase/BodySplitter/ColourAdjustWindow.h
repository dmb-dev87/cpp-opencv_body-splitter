#pragma once
#include "ColourAdjustDialog.h"
class BodySplitterTexture;

/* Popup window when someone pushes the colour button, this is 
used for k-means simplification and visualizing the changes */

#include <memory>
#include <unordered_map>
#include <wx/string.h>

class iDrawable;
class BodySplitterTexture;

class ColourAdjustWindow :
	public ColourAdjustDialog
{
public:
	ColourAdjustWindow(wxWindow* parent);
	~ColourAdjustWindow();
	
private:
	unsigned int sourceX, sourceY;
	std::shared_ptr<BodySplitterTexture> workingTexture;

	std::unordered_map<std::string, std::shared_ptr<iDrawable>> drawableMap;
	void populateListBox();
	void updateBitmapImage();

	virtual void ChangeMesh(wxCommandEvent& event) override;
	virtual void OnPreserveRatio(wxCommandEvent& event) override;
	virtual void UpdatePreviewImage(wxCommandEvent& event) override;
	virtual void OnRevertToSource(wxCommandEvent& event) override;

	virtual void OnWidthChange(wxFocusEvent& event) override;
	virtual void OnWidthChange(wxCommandEvent& event) override;
	virtual void OnHeightChange(wxFocusEvent& event) override;
	virtual void OnHeightChange(wxCommandEvent& event) override;

	virtual void OnOKClick(wxCommandEvent& event);
};

