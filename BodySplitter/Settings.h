#pragma once
/* Loads settings from files and dialogs and provides access to the rest of the program*/
#include <opencv2/core/core.hpp>
#include <string>
#include <wx/wx.h>
#include <string>
#include <unordered_map>
#include <fstream>
#include "colour.h"
#include "Singleton.h"
#include "AABoundingBox.h"
#include "colour.h"

#ifdef _WIN32
#ifdef _CONSOLE
#define CONSOLE
#endif
#endif

#define SHOW_SPLASH_SCREEN
#define STORE_FILES_WITH_EXE
#define BUILD_VERSION = "0.1.0"

#define PI 3.141592654
//#define LAYER_THICKNESS Settings::getSingleton().getDoubleValue("layerThickness")	//mm
#define MIN_AREA_SURFACE 10		//mm^2
//#define DUMP_TO_TEST_FILES
#define SETTINGS_DOUBLE(in) Settings::getSingleton().getDoubleValue(in)

enum StartPoint{RANDOM=0,CLOSEST,LINEAR};
class SettingValues
{
private:
	static std::string escapeQuotes(const std::string &in);
	// Returns true if quote at end
	bool removeEscape(std::string &in);
public:
	std::unordered_map<std::string, double> doubleValues;
	std::unordered_map<std::string, bool>	boolValues;
	std::unordered_map<std::string, std::string> stringValues;
	std::unordered_map<std::string, Colour> colourValues;
	
	SettingValues();
	bool saveTo(std::ostream &output) const;
	bool saveTo(const std::string &filename) const;
	bool loadFrom(std::istream &inStream);
	bool loadFrom(const std::string &filename);
	bool loadFrom(const char *buffer, unsigned long length);
};
class Settings : public Singleton<Settings>
{
public:

private:
	friend class SettingsDialog;
	SettingValues m_SliceSettings;
	//SettingValues m_GCodeSettings;
	//SettingValues m_UISettings;

	bool loadSettings();
	bool saveSettings();
	std::string ValidateConfig();

	uint16_t colours = 5;

	//Used for rova5 compat only, doesnt save
	std::unordered_map<uint32_t, uint16_t> colourToolMap;
	// Colour substition
	std::unordered_map<uint32_t, Colour::ColourSpace::CMYKWT> colourSubMap;
public:
	Settings();
	Settings(int argc, char **argv, wxString &input, wxString &output);
	~Settings();
	//Returns >0 if settings are changed
	int OpenSettingsWindow();

	static std::string VerifySettings(const SettingValues &newValues);

	double getDoubleValue(const std::string &input) const;
	bool getBoolValue(const std::string &input) const;
	void setBoolValue(const std::string &key, bool value);
	std::string getStringValue(const std::string &input) const;
	Colour getColourValue(const std::string &input) const;

	//Adds global control of the colour slider
	void setColours(unsigned int newColours) { colours = newColours; }
	uint16_t getColours() { return colours; }

	void resetColourToolMap() {
		colourToolMap.clear();
	}
	void resetColourSub() {
		colourSubMap.clear();
	}
	void addColourToolPair(uint32_t colour, uint16_t tool) {
		colourToolMap[colour] = tool;
	}
	uint16_t getToolByColour(uint32_t colour) {
		if (colourToolMap.empty())
			return 0;
		assert(colourToolMap.count(colour));
		return colourToolMap[colour];
	}
	void addColourSub(uint32_t colour, const Colour::ColourSpace::CMYKWT &col)
	{
		colourSubMap[colour] = col;
	}
	Colour::ColourSpace::CMYKWT getColourSub(uint32_t colour)
	{
		return colourSubMap[colour];
	}
	bool saveColourSubMap(std::ostream &outStream);
	bool loadColourSubMap(std::istream &inStream);

	bool loadNewSettings(std::istream &inStream);
	bool saveSettingsTo(std::ostream &outStream);

	bool hasColourSubs() {
		return !colourSubMap.empty();
	}

	std::unordered_map<uint32_t, uint16_t> &getColourToolMap() {
		return colourToolMap;
	}
};

bool GET_BOOL(const std::string &key);
double GET_DOUBLE(const std::string &key);
Colour GET_COLOUR(const std::string &key);