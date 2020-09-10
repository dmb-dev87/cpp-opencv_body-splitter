#pragma once
/* Rather traditional opengl visualizer for the mesh objects */

#include <memory>
#include "View3DPane.h"

class wxBoxSizer;
class iDrawable;
class ModelPropPanel;
class SupportTower;
class Mesh;
class UserMesh;

class ModelViewPane :
	public View3DPane
{
public:
	enum class ControlModes { NONE = 0, MOVE, ROTATE, CLIP, SCALE, ADDSUPPORT, REMOVESUPPORT };
private:
	friend class ModelPropPanel;
	
	double xPosTemp = 0;
	double yPosTemp = 0;
	double xPosStart = 0;
	double yPosStart = 0;
	double mouseXStart = 0;
	double mouseYStart = 0;
	double zRotation = 0;
	double startDistance = 0;
	double scaleAmt = 1;
	ControlModes controlState =ControlModes::NONE;
	ControlModes keyBoardControlState = ControlModes::NONE;
	std::shared_ptr<iDrawable> isClickOnMesh(int xx, int yy);
	std::pair<std::shared_ptr<SupportTower>,std::shared_ptr<UserMesh>> isMoveOverSupport(int xx, int yy);
	std::shared_ptr<iDrawable> draggingObject;
	bool isDraggingObject = false;
	bool isRotating = false;
	void dragDropHandler(wxDropFilesEvent &event);
	wxBoxSizer* modelSizer;
	std::unique_ptr<ModelPropPanel> propertiesPanel;
	bool propertiesPanelOpen = false;
	void OpenPropertiesPanel(std::shared_ptr<iDrawable> drawable);
	void HidePropertiesPanel(wxCommandEvent &evnt);
	void HidePropertiesPanel();
	// This being unique requires it to be copied to the mesh object
	std::unique_ptr<SupportTower> supportTowerDrawing;
	void updateSupportTowerPosition(int mouseX, int mouseY);
	void updateControlMode(ControlModes newMode);
protected:
	virtual void mouseEvent(const wxMouseEvent &mouse) override;
	
public:
	ModelViewPane(wxFrame* parent, int* args);
	virtual ~ModelViewPane();

	void registerSizer(wxBoxSizer* sizer)
	{
		modelSizer = sizer;
	}

	void draw() override;

	virtual void moveMode(wxCommandEvent& event) {
		updateControlMode(ControlModes::MOVE);
	};
	virtual void rotateMode(wxCommandEvent& event) {
		updateControlMode(ControlModes::ROTATE);
	}
	virtual void scaleMode(wxCommandEvent& event) {
		updateControlMode(ControlModes::SCALE);
	}
	virtual void clipMode(wxCommandEvent& event) {
		updateControlMode(ControlModes::CLIP);
	}
	virtual void removeSupportMode(wxCommandEvent& event) {
		updateControlMode(ControlModes::REMOVESUPPORT);
	}
	virtual void addSupportMode(wxCommandEvent &event);
	virtual void openPropertiesButton(wxCommandEvent &event);
	virtual void closePropertiesButton(wxCommandEvent &event);

	DECLARE_EVENT_TABLE();
	//virtual void render(wxPaintEvent& evt);
};

