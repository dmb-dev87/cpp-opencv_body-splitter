#include "ModelPropPanel.h"
#include "iDrawable.h"
#include "iSlicable.h"
#include "PrintBed.h"
#include <iomanip>
#include "ModelViewPane.h"

#ifndef _CONSOLE

ModelPropPanel::ModelPropPanel(ModelViewPane* pane, std::shared_ptr<iDrawable> mesh,wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : PropertiesPanel(parent, id, pos, size, style)
{
	viewPane = pane;
	m_XOffset->SetDigits(3);
	setNewDrawable(mesh);
	m_uniformScale->SetValue(true);
}

void ModelPropPanel::modelMoved(wxCommandEvent &ev)
{
	if (activeModel)
	{
		updatePositionValues();
		updateRotationValues();
		updateScaleValues();
	}
}

void ModelPropPanel::setNewDrawable(std::shared_ptr<iDrawable> newDrawable)
{
	if (newDrawable == activeModel)
		return;
	activeModel = newDrawable;
	m_ToolChoice->SetSelection(static_cast<int>(activeModel->getTool()));
	m_DrawbleName->SetValue(activeModel->getName());
	m_UseTexture->Enable(activeModel->hasTexture());
	if(activeModel->isUsingTexture())
		m_UseTexture->SetValue(activeModel->isUsingTexture());
	else
		m_UseCustomColour->SetValue(activeModel->getColour().isSet());
	m_CustomColourPick->Enable(activeModel->getColour().isSet());
	if (activeModel->isUsingTexture())
	{
		m_UseCustomColour->Enable(true);
		m_CustomColourPick->Enable(false);
	}
	else
	{
		m_UseCustomColour->SetValue(true);
		m_CustomColourPick->Enable();
	}
	m_CustomColourPick->SetColour(wxColour(activeModel->getColour().getR<uint8_t>(),
		activeModel->getColour().getG<uint8_t>(),
		activeModel->getColour().getB<uint8_t>()));

	//Offsets and rotation and scaling
	updatePositionValues();
	updateRotationValues();
	updateScaleValues();
	updateColourTextureOptions();
}

void ModelPropPanel::ToolChoose(wxCommandEvent& event)
{
	if (activeModel->getTool() == static_cast<iDrawable::ToolType>(
		m_ToolChoice->GetSelection()))
		return;
	resliceRequire();
	activeModel->setTool(static_cast<iDrawable::ToolType>(
		m_ToolChoice->GetSelection()));
	updateColourTextureOptions();
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::UpdateName(wxFocusEvent& event)
{
	activeModel->setName(m_DrawbleName->GetValue().ToStdString());
}

void ModelPropPanel::updatePositionValues()
{
	m_XOffset->SetValue(activeModel->getOffsetX());
	m_YOffset->SetValue(activeModel->getOffsetY());
	if(fabs(activeModel->getOffsetZ()) > 0.001)
		m_ZOffset->SetValue(activeModel->getOffsetZ());
	else
		m_ZOffset->SetValue(0.00);
}

void ModelPropPanel::updateRotationValues()
{
	m_XRotation->SetValue(activeModel->getXRotation());
	m_YRotation->SetValue(activeModel->getYRotation());
	m_ZRotation->SetValue(activeModel->getZRotation());
}

void ModelPropPanel::updateScaleValues()
{
	m_XScale->SetValue(activeModel->getXScale()*100.0);
	m_YScale->SetValue(activeModel->getYScale()*100.0);
	m_ZScale->SetValue(activeModel->getZScale()*100.0);
}

void ModelPropPanel::XRotationUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setXRotation(m_XRotation->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
	
}
void ModelPropPanel::YRotationUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setYRotation(m_YRotation->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
	
}
void ModelPropPanel::ZRotationUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setZRotation(m_ZRotation->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
	
}

void ModelPropPanel::XScaleUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setXScale(m_XScale->GetValue()/100.0);
	
	if (m_uniformScale->GetValue())
	{
		m_YScale->SetValue(m_XScale->GetValue());
		m_ZScale->SetValue(m_XScale->GetValue());
		activeModel->setYScale(m_YScale->GetValue() / 100.0);
		activeModel->setZScale(m_ZScale->GetValue() / 100.0);
	}
	PrintBed::getSingleton().forceDisplayUpdate();
}
void ModelPropPanel::YScaleUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setYScale(m_YScale->GetValue()/100.0);
	if (m_uniformScale->GetValue())
	{
		m_XScale->SetValue(m_YScale->GetValue());
		m_ZScale->SetValue(m_YScale->GetValue());
		activeModel->setXScale(m_XScale->GetValue() / 100.0);
		activeModel->setZScale(m_ZScale->GetValue() / 100.0);
	}
	PrintBed::getSingleton().forceDisplayUpdate();
	
}
void ModelPropPanel::ZScaleUpdate(wxSpinDoubleEvent& event) {
	resliceRequire();
	activeModel->setZScale(m_ZScale->GetValue()/100.0);
	if (m_uniformScale->GetValue())
	{
		m_XScale->SetValue(m_ZScale->GetValue());
		m_YScale->SetValue(m_ZScale->GetValue());
		activeModel->setXScale(m_XScale->GetValue() / 100.0);
		activeModel->setYScale(m_YScale->GetValue() / 100.0);
	}
	PrintBed::getSingleton().forceDisplayUpdate();
	
}

void ModelPropPanel::XOffsetUpdate(wxSpinDoubleEvent& event)
{
	activeModel->setOFfsetX(m_XOffset->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::YOffsetUpdate(wxSpinDoubleEvent& event)
{
	activeModel->setOFfsetY(m_YOffset->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::ZOffsetUpdate(wxSpinDoubleEvent& event)
{
	resliceRequire();
	activeModel->setOFfsetZ(m_ZOffset->GetValue());
	PrintBed::getSingleton().forceDisplayUpdate();
	
}


void ModelPropPanel::OnUseTexture(wxCommandEvent& event)
{
	resliceRequire();
	activeModel->setUsingTexture(m_UseTexture->GetValue());
	if (activeModel->isUsingTexture())
	{
		//m_UseCustomColour->Enable(false);
		m_CustomColourPick->Enable(false);
	}
	else
	{
		m_UseCustomColour->Enable(true);
		if(activeModel->getColour().isSet())
			m_CustomColourPick->Enable(true);
		else
			m_CustomColourPick->Enable(false);
	}
	PrintBed::getSingleton().forceDisplayUpdate();
	
}

void ModelPropPanel::updateColourTextureOptions()
{
	if (activeModel->getTool() == iDrawable::ToolType::FULL_COLOUR)
	{
		m_UseCustomColour->Enable();
		m_UseTexture->Enable(activeModel->hasTexture());
		wxCommandEvent tempCommand;
		OnUseTexture(tempCommand);
	}
	else
	{
		m_UseCustomColour->Enable(false);
		m_UseTexture->Enable(false);
	}
}

void ModelPropPanel::OnUseCustomColour(wxCommandEvent &event)
{
	resliceRequire();
	if (m_UseCustomColour->GetValue())
	{
		activeModel->setUsingTexture(false);
		m_CustomColourPick->Enable();
		activeModel->setColour(Colour(m_CustomColourPick->GetColour().Red(),
			m_CustomColourPick->GetColour().Green(),
			m_CustomColourPick->GetColour().Blue()));
	}
	else
	{
		m_CustomColourPick->Enable(false);
		activeModel->setColour(Colour());
	}
	
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::ColourChosen(wxColourPickerEvent& event)
{
	resliceRequire();
	activeModel->setColour(Colour(m_CustomColourPick->GetColour().Red(),
		m_CustomColourPick->GetColour().Green(),
		m_CustomColourPick->GetColour().Blue()));
	PrintBed::getSingleton().forceDisplayUpdate();
	
}

void ModelPropPanel::DeleteObj(wxCommandEvent& event)
{
	PrintBed::getSingleton().removeObject(activeModel);
	activeModel.reset();
	viewPane->HidePropertiesPanel();
	this->Hide();
}

void ModelPropPanel::OnHideButton(wxCommandEvent& event)
{
	activeModel->SetHidden(!activeModel->isHidden());
	if (activeModel->isHidden())
		m_hideObj->SetLabelText("Show");
	else
		m_hideObj->SetLabelText("Hide");
}

void ModelPropPanel::PlaceOnBed(wxCommandEvent& event)
{
	resliceRequire();
	activeModel->setToBed();
	m_ZOffset->SetValue(activeModel->getOffsetZ());
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::ResetScale(wxCommandEvent& event)
{
	m_XScale->SetValue(100);
	m_YScale->SetValue(100);
	m_ZScale->SetValue(100);
	activeModel->setXScale(m_XScale->GetValue() / 100.0);
	activeModel->setYScale(m_YScale->GetValue() / 100.0);
	activeModel->setZScale(m_ZScale->GetValue() / 100.0);
	resliceRequire();
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::scaleMaxButton(wxCommandEvent& event)
{
	activeModel->setMaxSize();
	resliceRequire();
	PrintBed::getSingleton().forceDisplayUpdate();
	updateScaleValues();
}

void ModelPropPanel::ResetRotation(wxCommandEvent& event)
{
	m_XRotation->SetValue(0);
	m_YRotation->SetValue(0);
	m_ZRotation->SetValue(0);

	activeModel->setXRotation(m_XRotation->GetValue());
	activeModel->setYRotation(m_YRotation->GetValue());
	activeModel->setZRotation(m_ZRotation->GetValue());

	resliceRequire();
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::ResetPosition(wxCommandEvent& event)
{
	double oldZ = activeModel->getOffsetZ();
	PrintBed::GetSingleton().centreMesh(activeModel);
	m_XOffset->SetValue(activeModel->getOffsetX());
	m_YOffset->SetValue(activeModel->getOffsetY());
	m_ZOffset->SetValue(activeModel->getOffsetZ());
	if (oldZ != m_ZOffset->GetValue())
		resliceRequire();
	PrintBed::getSingleton().forceDisplayUpdate();
}

void ModelPropPanel::resliceRequire()
{
	auto tempSlicible = std::dynamic_pointer_cast<iSlicable>(activeModel);
	if (tempSlicible->isSliced())
		tempSlicible->invalidateSlice();
}

ModelPropPanel::~ModelPropPanel()
{
}

wxBEGIN_EVENT_TABLE(ModelPropPanel, wxPanel)
EVT_COMMAND(MODEL_MOVED_ID, wxEVT_COMMAND_TEXT_UPDATED, ModelPropPanel::modelMoved)
wxEND_EVENT_TABLE()

#endif // _CONSOLE