#include "ModelViewPane.h"
#include "Mesh.h"
#include "UserMesh.h"
#include "MainFrame.h"
#include "CoasterPrint.h"
#include "CameraControl.h"
#include "PrintBed.h"
#include "ModelPropPanel.h"
#include "SupportTower.h"
#include "Settings.h"
#include <spdlog/spdlog.h>
#include <iostream>

#ifndef CONSOLE

ModelViewPane::ModelViewPane(wxFrame* parent, int* args) : View3DPane(parent,args)
{
	this->DragAcceptFiles(true);
	this->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(ModelViewPane::dragDropHandler), NULL, this);
}


ModelViewPane::~ModelViewPane()
{
}

std::pair<std::shared_ptr<SupportTower>,std::shared_ptr<UserMesh>> ModelViewPane::isMoveOverSupport(int xx, int yy) {
	// Create a list of all the bed's support towers: todo: cache this probably
	std::unordered_map<Colour, std::pair<std::shared_ptr<SupportTower>, std::shared_ptr<UserMesh>>> colourSupportMap;
	uint32_t colourCounter = 10;
	for (auto it : PrintBed::GetSingleton().getDrawables())
	{
		auto mesh_ptr = std::dynamic_pointer_cast<UserMesh>(it);
		if (mesh_ptr) 
		{
			auto sTowerList = mesh_ptr->getSupportTowers();
			for (auto sIt : sTowerList) {
				sIt->highLight(false);
				colourSupportMap[Colour(colourCounter++)] = std::pair<std::shared_ptr<SupportTower>, std::shared_ptr<UserMesh>>(sIt, mesh_ptr);
			}
		}
	}
	initializeDrawing(DrawingOptions::BLACK_BACKGROUND | DrawingOptions::NOGRID);

	for (auto it : colourSupportMap)
	{
		// cMap->(Support/Mesh)->Support
		glTranslatef(it.second.second->getOffsetX(), it.second.second->getOffsetY(), 0);
		it.second.first->draw(iDrawable::DrawingOptions::OBJECT_PICK, &(it.first));
		glTranslatef(-it.second.second->getOffsetX(), -it.second.second->getOffsetY(), 0);
	}
	unsigned char res[4];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(xx, viewport[3] - yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
	for (auto it : colourSupportMap)
	{
		//if (res[0] != 0)
		//{
		//	int r = res[0];
		//	int g = res[1];
		//	int b = res[2];

		//}
		if (Colour(res[0], res[1], res[2]) == it.first)
		{
			int r = it.first.getR<uint8_t>();
			int g = it.first.getG<uint8_t>();
			int b = it.first.getB<uint8_t>();

			return it.second;
		}
	}
	return std::pair<std::shared_ptr<SupportTower>,std::shared_ptr<UserMesh>>(nullptr,nullptr);
}

std::shared_ptr<iDrawable> ModelViewPane::isClickOnMesh(int xx, int yy)
{
	auto drawables = PrintBed::getSingleton().getDrawables();
	if (drawables.empty())
		return nullptr;
	//Create colour object map
	std::unordered_map<Colour, std::shared_ptr<iDrawable>> colourObjMap;
	uint32_t colourCounter = 1;
	for (auto it : drawables)
	{
		colourObjMap[Colour(colourCounter++)] = it;
	}
	initializeDrawing(DrawingOptions::BLACK_BACKGROUND | DrawingOptions::NOGRID);
	for (auto it : colourObjMap)
	{
		it.second->draw(iDrawable::DrawingOptions::OBJECT_PICK, &(it.first));
	}
	unsigned char res[4];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(xx, viewport[3] - yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
	for(auto it : colourObjMap)
	{
		if (Colour(res[0], res[1], res[2]) == it.first)
		{
			return it.second;
		}
	}
	return nullptr;
}

void ModelViewPane::updateControlMode(ControlModes newMode)
{
	if (controlState == newMode)
		controlState = ControlModes::NONE;
	else
		controlState = newMode;
	MainFrame::getSingleton().updateControlButton(controlState);
}

void ModelViewPane::addSupportMode(wxCommandEvent &event) {
	if (controlState == ControlModes::ADDSUPPORT) {
		supportTowerDrawing.reset();
		for (auto it : PrintBed::GetSingleton().getDrawables())
		{
			auto mesh_ptr = std::dynamic_pointer_cast<Mesh>(it);
			if (mesh_ptr)
			{
				mesh_ptr->undoTransformations();
			}
		}
		PrintBed::getSingleton().forceDisplayUpdate();
	}
	else
	{
		supportTowerDrawing = std::make_unique<SupportTower>(glm::vec3(0, 0, 0), 100,Settings::getSingleton().getDoubleValue("supportRes"));
		for (auto it : PrintBed::GetSingleton().getDrawables())
		{
			auto mesh_ptr = std::dynamic_pointer_cast<Mesh>(it);
			if (mesh_ptr)
			{
				mesh_ptr->applyTransformations();
			}
		}
		PrintBed::getSingleton().forceDisplayUpdate();
	}
	updateControlMode(ControlModes::ADDSUPPORT);
}

void ModelViewPane::mouseEvent(const wxMouseEvent &mouse)
{
	static bool bHadHighlighted = false; // Needed to force a display update if the mouse moves off support towers
	std::pair<std::shared_ptr<SupportTower>, std::shared_ptr<UserMesh>> mouseOverSupport;
	if (mouse.ButtonDClick())
	{
		auto meshClickedOn = isClickOnMesh(mouse.GetX(), mouse.GetY());
		if (meshClickedOn)
		{
			//meshClickedOn->doubleClickOn();
			OpenPropertiesPanel(meshClickedOn);
			propertiesPanelOpen = true;
			PrintBed::getSingleton().updateSelectedObject(meshClickedOn);
			//draw(); // This is probably going to crash linux
		}
	}
	else if (!mouse.LeftIsDown() && controlState == ControlModes::REMOVESUPPORT)
	{
		//Check if mouse is over a support tower
		mouseOverSupport = isMoveOverSupport(mouse.GetX(), mouse.GetY());
		if (mouseOverSupport.first) {
			mouseOverSupport.first->highLight(true);
			bHadHighlighted = true;
			PrintBed::getSingleton().forceDisplayUpdate();
		}
		else if (bHadHighlighted)
		{
			PrintBed::getSingleton().forceDisplayUpdate();
			bHadHighlighted = false;
		}
	}
	else if (mouse.LeftDown())
	{
		//User has gone and clicked the left mouse oh shit whaddup
		//Check if click startes on mesh
		if (controlState != ControlModes::NONE || mouse.GetModifiers() & (wxMOD_CONTROL | wxMOD_ALT | wxMOD_SHIFT))// && isClickOnMesh(mouse.GetX(),mouse.GetY()))
		{
			std::shared_ptr<iDrawable> clickedMesh;
			if (mouse.GetModifiers() & (wxMOD_CONTROL | wxMOD_ALT | wxMOD_SHIFT))
			{
				if (PrintBed::getSingleton().getSelectedObject())
					draggingObject = clickedMesh = PrintBed::getSingleton().getSelectedObject();
				else
				{
					if (PrintBed::getSingleton().getDrawables().empty())
					{
						//Nope
					}
					else
						clickedMesh = PrintBed::getSingleton().getDrawables().back();
				}
			}
			else
				clickedMesh = isClickOnMesh(mouse.GetX(), mouse.GetY());
			
			if (controlState == ControlModes::ADDSUPPORT && supportTowerDrawing->getTouchingMesh())
			{
				supportTowerDrawing->getTouchingMesh()->addSupport(std::make_shared<SupportTower>(*supportTowerDrawing.get()));
			}
			else if (controlState == ControlModes::REMOVESUPPORT && isMoveOverSupport(mouse.GetX(), mouse.GetY()).first)
			{
				mouseOverSupport = isMoveOverSupport(mouse.GetX(), mouse.GetY());
				if (mouseOverSupport.first) {
					mouseOverSupport.second->removeSupport(mouseOverSupport.first);
					PrintBed::getSingleton().forceDisplayUpdate();
				}
			}
			else if (clickedMesh)
			{
				//Start of a click and drag operation
				//is dragging
				isDraggingObject = true;
				if (clickedMesh)
					draggingObject = clickedMesh;
				if (mouse.GetModifiers() & wxMOD_CONTROL)
					keyBoardControlState = ControlModes::MOVE;
				if (mouse.GetModifiers() & wxMOD_ALT)
					keyBoardControlState = ControlModes::ROTATE;
				if (mouse.GetModifiers() & wxMOD_SHIFT)
					keyBoardControlState = ControlModes::SCALE;
				if (this->controlState == ControlModes::MOVE || keyBoardControlState == ControlModes::MOVE)
				{
					m_ptrCamera->mouseToWorld(mouse.GetX(), getHeight() - mouse.GetY(), mouseXStart, mouseYStart);
					xPosStart = clickedMesh->getOffsetX();
					yPosStart = clickedMesh->getOffsetY();
					xPosTemp = xPosStart;
					yPosTemp = yPosStart;
					draggingObject->moveTo(0, 0, 0);

				}
				else if (this->controlState == ControlModes::ROTATE || this->keyBoardControlState == ControlModes::ROTATE)
				{
					draggingObject->setZRotation(0);
					xPosTemp = clickedMesh->getOffsetX();
					yPosTemp = clickedMesh->getOffsetY();
					clickedMesh->moveTo(0, 0, 0);
				}
				else if (this->controlState == ControlModes::SCALE || this->keyBoardControlState == ControlModes::SCALE)
				{
					xPosTemp = clickedMesh->getOffsetX();
					yPosTemp = clickedMesh->getOffsetY();
					double tempX = xPosTemp + (clickedMesh->getXExtent() / 2.0);
					double tempY = yPosTemp + (clickedMesh->getYExtent() / 2.0);
					m_ptrCamera->mouseToWorld(mouse.GetX(), getHeight() - mouse.GetY(), mouseXStart, mouseYStart);
					startDistance = sqrt((tempX - mouseXStart)*(tempX - mouseXStart) + (tempY - mouseYStart)*(tempY - mouseYStart));
					clickedMesh->moveTo(0, 0, 0);
				}
			}
			else
			{
				draggingObject.reset();
				defaultMouse(mouse);
			}
			//wxLogDebug("Mouse clicked on mesh");
		}
		else
		{
			draggingObject.reset();
			isDraggingObject = false;
			keyBoardControlState = ControlModes::NONE;
			// Do default and return
			defaultMouse(mouse);
			return;
		}
	}
	if (mouse.Dragging() && isDraggingObject) //Continuation of a click and drag
	{
		double mouseXWorld, mouseYWorld;
		m_ptrCamera->mouseToWorld(mouse.GetX(), getHeight()-mouse.GetY(), mouseXWorld, mouseYWorld);
		if (this->controlState == ControlModes::MOVE || keyBoardControlState == ControlModes::MOVE)
		{
			this->xPosTemp = xPosStart+(mouseXWorld-mouseXStart);
			this->yPosTemp = yPosStart+(mouseYWorld-mouseYStart);
		}
		else if(this->controlState == ControlModes::ROTATE || this->keyBoardControlState == ControlModes::ROTATE)
		{
			zRotation = -AI_RAD_TO_DEG(atan2(mouseXWorld - (xPosTemp),
				mouseYWorld - (yPosTemp)));
			zRotation = fmod(zRotation, 360.000);
		//	m_ptrDrawable->applyRotation(zRotation);
		}
		else if (this->controlState == ControlModes::SCALE || this->keyBoardControlState == ControlModes::SCALE)
		{
			//Find distance to object centre
			double tempX = xPosTemp;
			double tempY = yPosTemp;
			tempX += (draggingObject->getXExtent() / 2.0);
			tempY += (draggingObject->getYExtent() / 2.0);
			m_ptrCamera->mouseToWorld(mouse.GetX(), getHeight() - mouse.GetY(), mouseXStart, mouseYStart);
			double endDistance = sqrt((tempX - mouseXStart)*(tempX - mouseXStart) + (tempY - mouseYStart)*(tempY - mouseYStart));
			scaleAmt = endDistance / startDistance;
		}

		//std::cout << "Mouse X:"<<xPosTemp<<"MouseY:"<<yPosTemp<<"\n";
	}
	else if (mouse.Dragging() && !isDraggingObject)
	{
		defaultMouse(mouse);
		return;
	}
	if (mouse.LeftUp() && isDraggingObject) // end of a click and drag
	{
			isDraggingObject = false;
			//std::cout << "Object moving to: "<<xPosTemp<<","<<yPosTemp<<"\n";
			if (this->controlState == ControlModes::MOVE || keyBoardControlState == ControlModes::MOVE)
			{
				draggingObject->moveTo(xPosTemp, yPosTemp);
				if (propertiesPanelOpen)
				{
					wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MODEL_MOVED_ID);
					propertiesPanel->GetEventHandler()->AddPendingEvent(event);
				}
			}
			else if (this->controlState == ControlModes::ROTATE || this->keyBoardControlState == ControlModes::ROTATE)
			{
				auto sliceTemp = std::dynamic_pointer_cast<iSlicable>(draggingObject);
				if (sliceTemp->isSliced())
					sliceTemp->invalidateSlice();
				draggingObject->setZRotation(fmod(zRotation, 360.0));
				draggingObject->moveTo(xPosTemp, yPosTemp,
					0);
				if (propertiesPanelOpen)
				{
					wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MODEL_MOVED_ID);
					propertiesPanel->GetEventHandler()->AddPendingEvent(event);
				}
			}
			else if (this->controlState == ControlModes::SCALE || this->keyBoardControlState == ControlModes::SCALE)
			{
				auto sliceTemp = std::dynamic_pointer_cast<iSlicable>(draggingObject);
				if (sliceTemp->isSliced())
					sliceTemp->invalidateSlice();
				draggingObject->setXScale(scaleAmt*draggingObject->getXScale());
				draggingObject->setYScale(scaleAmt*draggingObject->getYScale());
				draggingObject->setZScale(scaleAmt*draggingObject->getZScale());
				draggingObject->moveTo(xPosTemp, yPosTemp, 0);
				if (propertiesPanelOpen)
				{
					wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, MODEL_MOVED_ID);
					propertiesPanel->GetEventHandler()->AddPendingEvent(event);
				}
			}
			draggingObject.reset();
			keyBoardControlState = ControlModes::NONE;
	}
	//else 
	if (isDraggingObject)
	{
		draw();
	}
	if (mouse.LeftIsDown() == false)	// Default behaviour to right click
	{
		if (this->controlState == ControlModes::ADDSUPPORT)
		{
			updateSupportTowerPosition(mouse.GetX(), getHeight() - mouse.GetY());
			PrintBed::getSingleton().forceDisplayUpdate();
		}
		defaultMouse(mouse);
		return;
	}
	
}

void ModelViewPane::updateSupportTowerPosition(int mouseX, int mouseY)
{
	double outX, outY;
	m_ptrCamera->mouseToWorld(mouseX, mouseY, outX, outY);
	//Need a list of points this ray passes through, starting with in intersection in xyPlane
	std::vector<glm::vec3> intersectionPoints{ glm::vec3(outX,outY,0) };
	glm::vec3 rayDir = glm::normalize(intersectionPoints.front() - m_ptrCamera->getPos());
	for (auto &it : PrintBed::GetSingleton().getDrawables())
	{
		auto meshPtr = std::dynamic_pointer_cast<Mesh>(it);
		if (meshPtr)
		{
			for (auto triangleIT : meshPtr->getTriangles())
			{
				glm::vec3 result;
				for (int i = 0; i < 3; i++)
				{
					triangleIT[i].x += meshPtr->getOffsetX();
					triangleIT[i].y += meshPtr->getOffsetY();
					//triangleIT[i].z += meshPtr->getOffsetZ();
				}
				if (BodySplitter::rayIntersectsTriangle(triangleIT, m_ptrCamera->getPos(), rayDir, result))
				{
					intersectionPoints.push_back(result);
				}
			}
		}
	}
	//Find closest pt
	auto closestPt = intersectionPoints.begin();
	double closestDist = glm::length(m_ptrCamera->getPos() - (*closestPt));
	for (auto it = intersectionPoints.begin(); it != intersectionPoints.end(); ++it)
	{
		if (glm::length(m_ptrCamera->getPos() - (*it)) < closestDist)
		{
			closestDist = glm::length(m_ptrCamera->getPos() - (*it));
			closestPt = it;
		}
	}
	supportTowerDrawing->updatePosition(*closestPt);
}

void ModelViewPane::dragDropHandler(wxDropFilesEvent& event)
{
	double xOff, yOff;
	m_ptrCamera->mouseToWorld(event.m_pos.x, getHeight() - event.m_pos.y, xOff, yOff);
	for (int i = 0; i < event.m_noFiles; i++)
	{
		if (UserMesh::isValidExtension(event.m_files[i].ToStdString()))
		{
			auto meshPtr = PrintBed::getSingleton().loadMesh(event.m_files[i].ToStdString(),event.m_noFiles>1);
			if (meshPtr)
			{
				meshPtr->moveTo(xOff, yOff, 0);
			}
		}
		else if (CoasterPrint::isValidExtension(event.m_files[i].ToStdString()))
		{
			auto coasterPtr = PrintBed::getSingleton().loadImage(event.m_files[i].ToStdString());
			if (coasterPtr)
			{
				coasterPtr->moveTo(xOff, yOff, 0);
			}
		}
	}
	PrintBed::GetSingleton().forceDisplayUpdate();
}

void ModelViewPane::draw()
{
	initializeDrawing();
	if (supportTowerDrawing && this->controlState == ControlModes::ADDSUPPORT)
	{
		glPushMatrix();
		supportTowerDrawing->draw(0,nullptr);
		glPopMatrix();
	}
	glPushMatrix();
	if (!PrintBed::getSingleton().getDrawables().empty())
	{
	 // std::cout << "XPOS:"<<xPosTemp<<" YPOS:"<<yPosTemp<<"\n";
		if (isDraggingObject && (this->controlState == ControlModes::MOVE || this->keyBoardControlState == ControlModes::MOVE))
			glTranslated(xPosTemp, yPosTemp, 0);
		else if (isDraggingObject && (this->controlState == ControlModes::ROTATE || this->keyBoardControlState == ControlModes::ROTATE))
		{
		//  glTranslated(m_ptrDrawable->getXExtent()/2.0,m_ptrDrawable->getYExent()/2.0,0);
		  glTranslated(xPosTemp, yPosTemp, 0);
			glRotated(zRotation, 0, 0, 1);
			
			//glTranslated(-m_ptrDrawable->getXExtent()/2.0,-m_ptrDrawable->getYExent()/2.0,0);
		}
		else if (isDraggingObject && (this->controlState == ControlModes::SCALE || this->keyBoardControlState == ControlModes::SCALE))
		{
			glTranslated(xPosTemp, yPosTemp, 0);
			glScaled(scaleAmt, scaleAmt, scaleAmt);
			//
		}
		if (isDraggingObject)
		{
			if (draggingObject) // Sanity check
			{
				draggingObject->draw();
				if (this->controlState == ControlModes::MOVE || this->keyBoardControlState == ControlModes::MOVE)
					glTranslated(-xPosTemp, -yPosTemp, 0);
				else if (this->controlState == ControlModes::ROTATE || this->keyBoardControlState == ControlModes::ROTATE)
				{
					glRotated(-zRotation, 0, 0, 1);
					glTranslated(-xPosTemp, -yPosTemp, 0);
				}
				else if (this->controlState == ControlModes::SCALE || this->keyBoardControlState == ControlModes::SCALE)
				{

					glScaled(1.0 / scaleAmt, 1.0 / scaleAmt, 1.0 / scaleAmt);
					glTranslated(-xPosTemp, -yPosTemp, 0);
				}
			}
			else
				isDraggingObject = false;
		}
		auto drawables = PrintBed::getSingleton().getDrawables();
		for (auto it : drawables)
		{
			if (it == draggingObject)
				continue;
			it->draw();
		}
	  //m_ptrDrawable->draw();
	  /*
	  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	  m_ptrDrawable->draw(iDrawable::DrawingOptions::OBJECT_PICK);
	  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	  */
		//glTranslated(xPosTemp, yPosTemp, 0);
	}
	glPopMatrix();
	finalizeDrawing();

}

void ModelViewPane::OpenPropertiesPanel(std::shared_ptr<iDrawable> drawable)
{
	if (!propertiesPanelOpen)
		propertiesPanelOpen = true;
	if (propertiesPanel)
	{
		propertiesPanel->setNewDrawable(drawable);
	}
	else
	{
		propertiesPanel = std::make_unique<ModelPropPanel>(this,drawable, this->GetParent());
		modelSizer->Insert(1, propertiesPanel.get(), 1, wxEXPAND | wxALL, 1);
		//modelSizer->Add(propertiesPanel.get(), 1, wxEXPAND | wxALL, 1);
		propertiesPanel->m_HideProperties->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ModelViewPane::HidePropertiesPanel), NULL, this);
	}
	propertiesPanel->Show();
	modelSizer->Layout();
	MainFrame::GetSingleton().setPropertiesPanelState(true);
}

void ModelViewPane::HidePropertiesPanel(wxCommandEvent &ev)
{
	HidePropertiesPanel();
}

void ModelViewPane::HidePropertiesPanel()
{
	propertiesPanel->Hide();
	modelSizer->Layout();
	propertiesPanelOpen = false;
	MainFrame::GetSingleton().setPropertiesPanelState(false);
}

void ModelViewPane::openPropertiesButton(wxCommandEvent &event)
{
	if (!propertiesPanelOpen)
	{
		if (!PrintBed::getSingleton().getDrawables().empty())
		{
			OpenPropertiesPanel(PrintBed::getSingleton().getDrawables().back());
		}
		else
			MainFrame::GetSingleton().setPropertiesPanelState(false);
	}
}

void ModelViewPane::closePropertiesButton(wxCommandEvent &event)
{
	if (propertiesPanelOpen)
	{
		HidePropertiesPanel();
	}
}

BEGIN_EVENT_TABLE(ModelViewPane, View3DPane)
EVT_COMMAND(0xFEEF, wxEVT_COMMAND_TOOL_CLICKED, ModelViewPane::moveMode)
EVT_COMMAND(0xFEEEEF, wxEVT_COMMAND_TOOL_CLICKED, ModelViewPane::rotateMode)
//EVT_COMMAND(0xFEEEEF, wxEVT_COMMAND_TOOL_CLICKED, ModelViewPane::scaleMode)
END_EVENT_TABLE()

#endif