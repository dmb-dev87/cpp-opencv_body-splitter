#pragma once
/* This is the side bar that holds rotation, scale, pos, colour etc*/
#include "PropPanel.h"
#include <memory>

class iDrawable;
class ModelViewPane;

const unsigned int MODEL_MOVED_ID = 0xDEADBEEF;

class ModelPropPanel :
	public PropertiesPanel
{
private:
	std::shared_ptr<iDrawable> activeModel;
	ModelViewPane* viewPane;
	//Disable texture/custom colour when not using full colour tool
	void updateColourTextureOptions();
	void updatePositionValues();
	void updateRotationValues();
	void updateScaleValues();
	void resliceRequire();
protected:
	virtual void ToolChoose(wxCommandEvent& event) override;
	virtual void UpdateName(wxFocusEvent& event) override;
	virtual void OnUseTexture(wxCommandEvent& event) override;
	virtual void OnUseCustomColour(wxCommandEvent& event) override;
	virtual void ColourChosen(wxColourPickerEvent& event) override;
	virtual void DeleteObj(wxCommandEvent& event) override;
	virtual void OnHideButton(wxCommandEvent& event) override;
	virtual void PlaceOnBed(wxCommandEvent& event) override;

	virtual void ResetPosition(wxCommandEvent& event) override;
	virtual void ResetRotation(wxCommandEvent& event) override;
	virtual void ResetScale(wxCommandEvent& event) override;
	virtual void scaleMaxButton(wxCommandEvent& event) override;

	void XOffsetUpdate(wxSpinDoubleEvent& event) override;
	void YOffsetUpdate(wxSpinDoubleEvent& event) override;
	void ZOffsetUpdate(wxSpinDoubleEvent& event)override;

	void XRotationUpdate(wxSpinDoubleEvent& event) override;
	void YRotationUpdate(wxSpinDoubleEvent& event) override;
	void ZRotationUpdate(wxSpinDoubleEvent& event) override;

	void XScaleUpdate(wxSpinDoubleEvent& event) override;
	void YScaleUpdate(wxSpinDoubleEvent& event) override;
	void ZScaleUpdate(wxSpinDoubleEvent& event) override;

	void modelMoved(wxCommandEvent &event);
public:
	void setNewDrawable(std::shared_ptr<iDrawable> newDrawable);
	ModelPropPanel(ModelViewPane* pane,std::shared_ptr<iDrawable> mesh,wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(293, 608), long style = wxTAB_TRAVERSAL);
	~ModelPropPanel();

	wxDECLARE_EVENT_TABLE();
};

