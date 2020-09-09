#include "PrintBed.h"
#include <spdlog/spdlog.h>
#include <unordered_set>

#include "Slicer.h"
#include "SupportManager.h"

#include "iSlicable.h"
#include "iDrawable.h"

#include "UserMesh.h"
#include "SupportMesh.h"
#include "CoasterPrint.h"

#ifndef _CONSOLE
#include "MainFrame.h"
#include "GCodeVisualizer.h"
#include "ModelViewPane.h"
#include "GCodeViewPane.h"
#include "gCodeControlWindow.h"

#endif
#include "PrintBedPackage.h"

#include "Settings.h"

#ifndef _CONSOLE
PrintBed::PrintBed(wxFrame* baseWindowIn) : baseWindow(baseWindowIn)
{
	//previewObjects.push_back(std::make_shared<GCodeVisualizer>());
}
#else
PrintBed::PrintBed()
{

}
#endif

void PrintBed::updateDisplay()
{
	// Prepare event for the base window to redraw the gl port
	// ToDo: Update display only if the model view pane is visible
#ifndef _CONSOLE
	wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, UPDATE_DISPLAY_ID);
	if(baseWindow)
		baseWindow->GetEventHandler()->AddPendingEvent(event);
#endif
}

std::shared_ptr<UserMesh> PrintBed::loadMesh(const std::string &filePath, bool dontCentre)
{
	auto logger = spdlog::get("DEBUG_FILE");
	logger->info("Loading file" + filePath);
	std::shared_ptr<UserMesh> tempMeshPtr = std::make_shared<UserMesh>();
	if (!tempMeshPtr->loadMesh(filePath, dontCentre))
	{
		return false;
	}
	objectsOnBed.push_back(tempMeshPtr);

	//tempMeshPtr->setToBed();
	
	updateDisplay();
#ifndef _CONSOLE
	wxCommandEvent readySlice(wxEVT_COMMAND_TEXT_UPDATED, ID_BED_READY);
	if(baseWindow)
		baseWindow->GetEventHandler()->AddPendingEvent(readySlice);
#endif
	unsavedChanges = true;
	return tempMeshPtr;
}

void PrintBed::addMeshToBed(std::shared_ptr<iDrawable> objToAdd)
{
	// Check if it already is on bed
	for (auto it : objectsOnBed)
	{
		if (it == objToAdd)
			return;
	}
	objectsOnBed.push_back(objToAdd);
}

void PrintBed::clearBed()
{
	objectsOnBed.clear();//hooray for smart ptrs
	selectedObject.reset();
	updateDisplay();
	unsavedChanges = false;
}

void PrintBed::invalidateAllSlices()
{
	for (auto it : objectsOnBed)
	{
		std::dynamic_pointer_cast<iSlicable>(it)->invalidateSlice();
	}
	if (m_supportManagerPtr)
		m_supportManagerPtr.reset();
	updateDisplay();
}
#ifndef _CONSOLE
void PrintBed::generateSupport(wxCommandEvent &ev)
#else
void PrintBed::generateSupport()
#endif
{
	for (auto it : objectsOnBed)
	{
		auto tempMesh = std::dynamic_pointer_cast<UserMesh>(it);
		if (tempMesh)
		{
			tempMesh->applyTransformations();
			tempMesh->generateSupport();
			tempMesh->undoTransformations();
		}

	}
	updateDisplay();
}

std::shared_ptr<CoasterPrint> PrintBed::loadImage(const std::string &filePath)
{
	std::shared_ptr<CoasterPrint> tempCoaster = std::make_shared<CoasterPrint>();
	if (!tempCoaster->loadImage(filePath))
		return false;
	objectsOnBed.push_back(tempCoaster);
	
	updateDisplay();
#ifndef _CONSOLE
	wxCommandEvent readySlice(wxEVT_COMMAND_TEXT_UPDATED, ID_BED_READY);
	if(baseWindow)
		baseWindow->GetEventHandler()->AddPendingEvent(readySlice);
#endif
	unsavedChanges = true;
	return tempCoaster;
}

std::shared_ptr<iDrawable> PrintBed::getDrawable()
{
	assert(!objectsOnBed.empty()); // This function makes no sense if both coaster and mesh are present
	return objectsOnBed.front();
}

void PrintBed::switchToSimpleTexture()
{
	simpleTexture = !simpleTexture;
	for(auto it : objectsOnBed)
	{
		std::dynamic_pointer_cast<iSlicable>(it)->setTextureSimplified(simpleTexture);
		updateDisplay();
	
	}
}

void PrintBed::switchToWireFrame()
{
	//Wire frame on a coaster makes no sense
	wireFrame = !wireFrame;
	updateDisplay();
}

void PrintBed::switchToPerspective(bool newPerspective)
{
	perspectiveFrame = newPerspective;
	updateDisplay();
}
#ifndef _CONSOLE
void PrintBed::registerViewPanes(ModelViewPane* mod, GCodeViewPane* gcode)
{
	modelPane = mod;
	gcodePane = gcode;
}
#endif
void PrintBed::forceDisplayUpdate()
{
#ifndef _CONSOLE
	updateDisplay();
#endif
}

void PrintBed::rotateMesh(double angle, BodySplitter::AXIS axis)
{
	// ToDo: Update rotate mesh to accept strongly typed axis information
	for(auto it : objectsOnBed)
	{
		int weakAxis = 0;
		if (axis == BodySplitter::AXIS::Y_AXIS)
			weakAxis = 1;
		else if (axis == BodySplitter::AXIS::Z_AXIS)
			weakAxis = 2;
		auto meshCast = std::dynamic_pointer_cast<Mesh>(it);
		meshCast->rotateMesh(angle, weakAxis);
		updateDisplay();
	}
	unsavedChanges = true;
}

void PrintBed::scaleMesh(double x, double y, double z)
{
	for (auto it : objectsOnBed)
	{

		std::dynamic_pointer_cast<Mesh>(it)->scaleMesh(x, y, z);
		updateDisplay();
	}
	unsavedChanges = true;
}

PrintBed::~PrintBed()
{
}

void PrintBed::updateTextureColour(uint16_t count)
{
	for (auto it : objectsOnBed)
		it->updateTextureColour(count);
	updateDisplay();
	unsavedChanges = true;
}

std::list<std::shared_ptr<iDrawable>> PrintBed::getDrawables()
{
	return objectsOnBed;
}

void PrintBed::meshReslice()
{
	for (auto it : objectsOnBed)
	{
		auto temp = std::dynamic_pointer_cast<iSlicable>(it);
#ifndef _CONSOLE
		if(temp->isSliced())
			temp->getVisualizer()->meshReslice();
#endif
	}
}

void PrintBed::centreMesh(std::shared_ptr<iDrawable> mesh)
{
	auto tempSlicable = std::dynamic_pointer_cast<iSlicable>(mesh);
	double midX = Settings::GetSingleton().getDoubleValue("maxX")/2.0;
	double midY = Settings::GetSingleton().getDoubleValue("maxY")/2.0;
	// Lets just assume the origin is in the cetnre like a good mesh
	mesh->setOFfsetX(midX);// -mesh->getXExtent() / 2.0);
	mesh->setOFfsetY(midY);// -mesh->getYExtent() / 2.0);
	unsavedChanges = true;
}

void PrintBed::removeObject(std::shared_ptr<iDrawable> object)
{
	if (object == selectedObject)
		selectedObject.reset();
	for (auto it = objectsOnBed.begin(); it != objectsOnBed.end(); ++it)
	{
		if (*it == object)
		{
			objectsOnBed.erase(it);
			break;
		}
	}
#ifndef _CONSOLE
	if (objectsOnBed.empty())
		MainFrame::getSingleton().bedEmptied();
#endif
	unsavedChanges = true;
	//Couldnt find this object to remove...
}

std::shared_ptr<iDrawable> PrintBed::getSelectedObject() const
{
	return selectedObject;
}

void PrintBed::updateSelectedObject(std::shared_ptr<iDrawable> newOb)
{
	selectedObject = newOb;
}

std::vector<Colour> PrintBed::getColoursOnBed()
{
	std::unordered_set<uint32_t> temp;
	for (auto it : objectsOnBed)
	{
		auto objectTemp = it->getColours();
		temp.insert(objectTemp.begin(), objectTemp.end());
	}
	return std::vector<Colour>(temp.begin(),temp.end());
}

// Determine if any mesh is using support
bool PrintBed::hasSupport()
{
	for (auto it : objectsOnBed)
	{
		auto mesh_ptr = std::dynamic_pointer_cast<Mesh>(it);
		if (mesh_ptr)
		{
			if (mesh_ptr->hasSupport())
				return true;
		}
	}
	return false;
}

void PrintBed::clearSupport()
{
	for (auto it : objectsOnBed)
	{
		auto mesh_ptr = std::dynamic_pointer_cast<UserMesh>(it);
		if (mesh_ptr)
		{
			if (mesh_ptr->hasSupport())
				mesh_ptr->clearSupport();
		}
	}
	if (m_supportManagerPtr)
		m_supportManagerPtr.reset();
	forceDisplayUpdate();
}

#ifndef _CONSOLE
void PrintBed::sliceBedContents(MainFrame *msgFrame)
#else
void PrintBed::sliceBedContents()
#endif
{
//	ControlWindow::getSingleton().startSlicing();
	for (auto it : objectsOnBed)
	{
		auto temp = std::dynamic_pointer_cast<iSlicable>(it);
		if (temp->isSliced())
			continue;
		std::shared_ptr<Slicer>m_ptrSlicer = std::make_shared<Slicer>();

		std::shared_ptr<iSlicable> toSlice(temp);
#ifndef _CONSOLE
		m_ptrSlicer->initSlicer(toSlice, msgFrame);
#else
		m_ptrSlicer->initSlicer(toSlice);
#endif
		m_ptrSlicer->slice();
	}
	if (hasSupport())
	{
#ifndef _CONSOLE
		msgFrame->SetStatusText("Generating Support...");
#endif
		sliceSupport();
		forceDisplayUpdate();
	}
	//msgFrame->SetStatusText("Slice complete");
	//ControlWindow::getSingleton().stopSlicing();

}

std::size_t PrintBed::getMaxLayer()
{
	unsigned int layerCount = 0;
	for (auto &it : objectsOnBed)
	{
		if (it->isSliced())
		{
			layerCount = std::max(layerCount,it->getLayerCount());
		}
	}
	return layerCount;
}

#ifdef _CONSOLE
void PrintBed::cutSupport(/*wxCommandEvent &burp commented by freelancer latino*/)
#else
void PrintBed::cutSupport(wxCommandEvent &burp)
#endif
{
	for (auto it : objectsOnBed)
	{
		auto slicePtr = std::dynamic_pointer_cast<UserMesh>(it);
		if (slicePtr)
		{
			slicePtr->cutOutSupport();
		}
	}
}

void PrintBed::sliceSupport()
{
	//for (auto it : objectsOnBed)
	//{
	//	auto slicePtr = std::dynamic_pointer_cast<UserMesh>(it);
	//	if (slicePtr) {
	//		if (slicePtr->hasSupport())
	//		{
	//			slicePtr->getSupportMesh()->startSlice(nullptr);
	//		}
	//	}
	//}
	//m_supportManagerPtr = std::make_shared<SupportManager>();
	//m_supportManagerPtr->addObjects(objectsOnBed);
	//m_supportManagerPtr->prepareSupport();
	// Populate the support polygons
	{
		
	}
	// Cut out the perimters
	{

	}
	// Infill the remaining polygons
	{

	}
}

std::unordered_set<Colour> PrintBed::getColoursOnLayer(unsigned int layerID) const
{
	std::unordered_set<Colour> output;
	for (auto it : objectsOnBed)
	{
		auto slicePtr = std::dynamic_pointer_cast<iSlicable>(it);
		if (layerID < slicePtr->getLayerCount())
		{
			auto coloursOnThisLayer = slicePtr->getLayer(layerID)->getColoursOnLayer();
			output.insert(coloursOnThisLayer.begin(), coloursOnThisLayer.end());
		}
	}

	return output;
}

bool PrintBed::exportBed(const std::string &filePath)
{
	//Create manifest
	PrintBedPackageExporter exporter(filePath, objectsOnBed);
	if (exporter.exportPrintBed())
	{
		unsavedChanges = false;
		return true;
	}
	return false;
}

bool PrintBed::importBed(const std::string &filePath,bool importSettings)
{
	PrintBedPackageImporter importer(filePath);
	//volatile bool isGood = importer.isGood();
	//volatile bool readZip = importer.ReadZip();
	if (importer.isGood() && importer.ReadZip(importSettings))
	{
		objectsOnBed = importer.getDrawables();
#ifdef _CONSOLE
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Imported from rovafile \n";
#endif
	}
	else
	{
#ifdef _CONSOLE
		if (Settings::getSingleton().getBoolValue("verbose"))
			std::cerr << "Failed to import from rovafile \n";
#endif
		return false;
	}
#ifndef _CONSOLE
	forceDisplayUpdate();
#endif
	unsavedChanges = false;
	return true;
}

unsigned int PrintBed::getLayer(double zHeight) const
{
	//Find layer from height
	zHeight -= Settings::GetSingleton().getDoubleValue("firstLayerThickness");
	if (zHeight <= 0)
		return 0;
	else
		return zHeight / GET_DOUBLE("layerThickness");
}