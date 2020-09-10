#pragma once
/* Stores objects placed on the print bed: IE models, coasters, as well as the associated visualization*/
#include <memory>
#include <list>
#include <unordered_set>
#include "Singleton.h"
#include "Geometry.h"

class iDrawable;
class iSlicable;
class SupportManager;
class UserMesh;
class CoasterPrint;
#ifndef _CONSOLE
class GCodeVisualizer;
class MainFrame;
class wxFrame;

class ModelViewPane;
class GCodeViewPane;

class wxCommandEvent;
#endif
class PrintBed : public Singleton<PrintBed>
{
private:
	std::list<std::shared_ptr<iDrawable>> objectsOnBed;
	std::shared_ptr<SupportManager> m_supportManagerPtr;

	std::shared_ptr<iDrawable> selectedObject;

	// Todo: Arrange pre sliced gcode copies onto the bed to reduce the slicing time
#ifndef _CONSOLE
	wxFrame* baseWindow;

	ModelViewPane *modelPane;
	GCodeViewPane *gcodePane;
#endif
	void updateDisplay();

	bool simpleTexture = false;
	bool wireFrame = false;
	bool perspectiveFrame = true;
	bool unsavedChanges = false;

	
	void sliceSupport();
public:
#ifndef _CONSOLE
	void registerViewPanes(ModelViewPane* mod, GCodeViewPane* gcode);
#endif


#ifndef _CONSOLE
	PrintBed(wxFrame* baseWindow);
#else
	PrintBed();
#endif
	~PrintBed();
#ifdef _CONSOLE
	void cutSupport(/*wxCommandEvent &burp commented by freelancer latino*/);
#else
	void cutSupport(wxCommandEvent &burp);
#endif
	std::vector<Colour> getColoursOnBed();
	std::shared_ptr<iDrawable> getDrawable();

	void clearBed();
	// Call this when global settings are changed
	void invalidateAllSlices();

	void switchToSimpleTexture();
	void switchToWireFrame();
	void switchToPerspective(bool newPerspective);

	void forceDisplayUpdate();

	void centreMesh(std::shared_ptr<iDrawable> mesh);

	void rotateMesh(double angle, BodySplitter::AXIS axis);
	void scaleMesh(double x, double y, double z);

	std::shared_ptr<UserMesh> loadMesh(const std::string &filePath, bool dontCentre = false);
	std::shared_ptr<CoasterPrint> loadImage(const std::string &filePath);

	void removeObject(std::shared_ptr<iDrawable> object);

	void updateSelectedObject(std::shared_ptr<iDrawable> newObject);
	std::shared_ptr<iDrawable> getSelectedObject() const;

	void updateTextureColour(uint16_t count);

	std::size_t getMaxLayer();

#ifndef _CONSOLE
	void generateSupport(wxCommandEvent &);
#else
	void generateSupport();
#endif

	std::list<std::shared_ptr<iDrawable>> getDrawables();

	void meshReslice();
	bool hasSupport();
#ifndef _CONSOLE
	void sliceBedContents(MainFrame* frameToMessage);
#else
	void sliceBedContents();
#endif

	std::shared_ptr<SupportManager> getSupportManager() {
		return m_supportManagerPtr;
	}

	std::unordered_set<Colour> getColoursOnLayer(unsigned int layerID) const;
	void clearSupport();
	bool isWireFrame() const { return wireFrame; }
	bool isPerspective() const { return perspectiveFrame; }
	bool hasUnsavedChanges() const { return unsavedChanges; }
	void changesNeedSaving() { unsavedChanges = true; }

	bool exportBed(const std::string &outPath);
	bool importBed(const std::string &filePath, bool importSettings = true);

	unsigned int getLayer(double zHeight) const;

	void addMeshToBed(std::shared_ptr<iDrawable> objToAdd);
};

