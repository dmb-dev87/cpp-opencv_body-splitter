#include "Settings.h"
#include "wx/wxprec.h"
#include <wx/textfile.h>
#include <wx/cmdline.h>
#ifndef STORE_FILES_WITH_EXE
	#include <wx/stdpaths.h>
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "RovaSettings.h"

#include <fstream>
#include <spdlog/spdlog.h>

#ifdef CONSOLE

namespace defaultSettings {
	const double	LAYER_THICKNESS = 0.3;
	const double	FIRST_LAYER_THICKNESS = 0.3;
	const int		BOTTOM_SOLID = 1;
	const int		TOP_SOLID = 1;
	const int		INFILL_DENSITY = 25;
	const int		PERIM_COUNT = 1;
	const wxColour	INFILL_COLOUR(255, 255, 255);
	const wxColour	PERIM_COLOUR(255, 255, 255);
	const wxColour	T1COLOUR(255, 20, 20); //Flexible
	const wxColour	T2COLOUR(240, 240, 240); // Support material

	const bool		ROVA5COMPAT = false;
	const double	EXTRUDER_COUNT = 5;

	const bool		USE_K_MEANS = false;
	const int		K_MEANS_COLOUR = 5;
	const bool		USE_TRAVIS_SIMPLIFICATION = true;
	const double	COLOUR_CHANGE_THRESHOLD = 2.3;
	const double	MIN_LENGTH_COLOUR_LINE = 1.0;
	const double	COLOUR_BIN_THRESHOLD = 10.5;

	const int		COASTER_BASE = 1;
	const int		COASTER_TOP = 1;
	const wxColour	COASTER_BASE_COLOUR(255, 255, 255);
	const int		BASE_OPACITY = 100;
	const int		SURFACE_OPACITY = 100;
	const double	NOZZLE_D = 0.4;
	const double	EXT_WIDTH = 0.6;
	const double	MIN_EXT_WIDTH = NOZZLE_D;
	const double	TOP_SOLID_EXT_WIDTH = NOZZLE_D;
	const double	FILAMENT_D = 1.75;

	const int		T0 = 210;
	const int		T1 = 210;
	const int		T2 = 210;
	const int		T3 = 210;
	const int		T4 = 210;
	const int		BED = 70;

	const bool		INFILL_FIRST = false;
	const bool		PERIMS_INSIDE_OUT = false;

	const bool		SIMPLE_GCODE = false;
	const int		GRID_WIDTH = 330;
	const int		GRID_DEPTH = 330;
	const int		GRID_SPACING = 20;

	const int		KEEP_ALIVE_TIME = 30;

	const std::string EMPTY_STRING = "EMPTY";

	// User hidden settings
	const double CAMERA_ORIGIN_X = GRID_WIDTH / 2.0;
	const double CAMERA_ORIGIN_Y = GRID_WIDTH / 2.0;
	const double CAMERA_ORIGIN_Z = 20;
	const double CAMERA_Z = 20;
	const double INFILL_OVERLAP = 10;
	const double TOP_SURFACE_INFILL = 100;
	const double BOTTOM_SURFACE = 100;
	const double MM_PIXEL = 0.1;
	const double REMOVE_SHORT_LINES = 0.1;
	const double THREAD_COUNT = 5;

	const bool USE_INFILL = true;
	const bool SIMPLIFY_COLOUR_PER_LINE = false;
	const bool CONSIDER_SURFACE = false;
	const bool COLOUR_PREVIEW_BY_TYPE = false;
	const double MIN_LINE_LENGTH = 0.01;
	const double END_Z = -1.0000;
	const bool USE_CRAZY_INFILL = true;
	const bool GENERATRE_PRIMING_TOWERS = false;

};

void fillDefaultSettings(SettingValues *defaultSettings)
{
		//  	auto my_logger = spdlog::basic_logger_mt("settingslog", "settings.txt");

		defaultSettings->doubleValues["layerThickness"] = defaultSettings::LAYER_THICKNESS;
		defaultSettings->doubleValues["firstLayerThickness"] = defaultSettings::FIRST_LAYER_THICKNESS;
		defaultSettings->doubleValues["firsyLayerExtrusion"] = 100;
		defaultSettings->doubleValues["bottomSolidLayers"] = defaultSettings::BOTTOM_SOLID;
		defaultSettings->doubleValues["topSolidLayers"] = defaultSettings::TOP_SOLID;
		defaultSettings->doubleValues["infillDensity"] = defaultSettings::INFILL_DENSITY;
		defaultSettings->doubleValues["perimeterCount"] = defaultSettings::PERIM_COUNT;
		defaultSettings->doubleValues["XYOffset"] = 0.0;

		defaultSettings->colourValues["infillColour"] = Colour(defaultSettings::INFILL_COLOUR.Red(),
			defaultSettings::INFILL_COLOUR.Green(),
			defaultSettings::INFILL_COLOUR.Blue());
		defaultSettings->colourValues["perimeterColour"] = Colour(defaultSettings::PERIM_COLOUR.Red(),
			defaultSettings::PERIM_COLOUR.Green(),
			defaultSettings::PERIM_COLOUR.Blue());

		defaultSettings->colourValues["t1Colour"] = Colour(defaultSettings::T1COLOUR.Red(),
			defaultSettings::T1COLOUR.Green(),
			defaultSettings::T1COLOUR.Blue());

		defaultSettings->colourValues["t2Colour"] = Colour(defaultSettings::T2COLOUR.Red(),
			defaultSettings::T2COLOUR.Green(),
			defaultSettings::T2COLOUR.Blue());

		defaultSettings->boolValues["rova5Compat"] = defaultSettings::ROVA5COMPAT;
		defaultSettings->doubleValues["compatExtruderCount"] = defaultSettings::EXTRUDER_COUNT;

		defaultSettings->boolValues["useKMeans"] = defaultSettings::USE_K_MEANS;
		defaultSettings->doubleValues["kMeansColourCount"] = defaultSettings::K_MEANS_COLOUR;
		defaultSettings->boolValues["travisSimplification"] = defaultSettings::USE_TRAVIS_SIMPLIFICATION;
		defaultSettings->doubleValues["colourChangeThreshold"] = defaultSettings::COLOUR_CHANGE_THRESHOLD;
		defaultSettings->doubleValues["minLineLengthTravisSimplification"] = defaultSettings::MIN_LENGTH_COLOUR_LINE;
		defaultSettings->doubleValues["coasterBaseLayers"] = defaultSettings::COASTER_BASE;
		defaultSettings->colourValues["coasterBaseColour"] = Colour(defaultSettings::COASTER_BASE_COLOUR.Red(),
			defaultSettings::COASTER_BASE_COLOUR.Green(),
			defaultSettings::COASTER_BASE_COLOUR.Blue());
		defaultSettings->doubleValues["colourBinThreshold"] = defaultSettings::COLOUR_BIN_THRESHOLD;
		defaultSettings->doubleValues["baseOpacity"] = defaultSettings::BASE_OPACITY;
		defaultSettings->doubleValues["coasterSurfaceLayers"] = defaultSettings::COASTER_TOP;
		defaultSettings->doubleValues["coasterSurfaceOpacity"] = defaultSettings::SURFACE_OPACITY;
		defaultSettings->doubleValues["nozzleD"] = defaultSettings::NOZZLE_D;
		defaultSettings->doubleValues["extrusionWidth"] = defaultSettings::EXT_WIDTH;
		defaultSettings->doubleValues["minExtrusionWidth"] = defaultSettings::MIN_EXT_WIDTH;
		defaultSettings->doubleValues["topSolidExtrusionWidth"] = defaultSettings::TOP_SOLID_EXT_WIDTH;
		defaultSettings->doubleValues["filamentD"] = defaultSettings::FILAMENT_D;
		defaultSettings->doubleValues["fullInfillSelfOverlap"] = 25;

		defaultSettings->doubleValues["T0"] = defaultSettings::T0;
		defaultSettings->doubleValues["T1"] = defaultSettings::T1;
		defaultSettings->doubleValues["T2"] = defaultSettings::T2;
		defaultSettings->doubleValues["T3"] = defaultSettings::T3;
		defaultSettings->doubleValues["T4"] = defaultSettings::T4;
		defaultSettings->doubleValues["bedTemp"] = defaultSettings::BED;
		defaultSettings->boolValues["heatAndWait"] = true;

		defaultSettings->boolValues["keepFanAlwaysOn"] = false;
		defaultSettings->doubleValues["minFanSpeed"] = 30;
		defaultSettings->doubleValues["maxFanSpeed"] = 100;
		defaultSettings->boolValues["reducePrintSpeedSmallLayers"] = false;
		defaultSettings->doubleValues["minLayerTime"] = 10;
		defaultSettings->doubleValues["minLayerTimeFan"] = 60;
		defaultSettings->doubleValues["minSlowDownFeedrate"] = 200;

		defaultSettings->boolValues["simpleGCodePreview"] = defaultSettings::SIMPLE_GCODE;
		defaultSettings->doubleValues["maxX"] = defaultSettings::GRID_WIDTH;
		defaultSettings->doubleValues["maxY"] = defaultSettings::GRID_DEPTH;
		defaultSettings->doubleValues["gridSize"] = defaultSettings::GRID_SPACING;

		// USER HIDDEN SETTINGS
		defaultSettings->doubleValues["cameraOriginX"] = defaultSettings::CAMERA_ORIGIN_X;
		defaultSettings->doubleValues["cameraOriginY"] = defaultSettings::CAMERA_ORIGIN_Y;
		defaultSettings->doubleValues["cameraOriginZ"] = defaultSettings::CAMERA_ORIGIN_Z;
		defaultSettings->doubleValues["cameraZ"] = defaultSettings::CAMERA_Z;
		defaultSettings->doubleValues["minimumLineLength"] = defaultSettings::MIN_LINE_LENGTH;
		defaultSettings->doubleValues["endZ"] = defaultSettings::END_Z;
		defaultSettings->doubleValues["infillOutlineOverlapPercentage"] = defaultSettings::INFILL_OVERLAP;
		defaultSettings->doubleValues["topSurfaceInfillDensity"] = defaultSettings::TOP_SURFACE_INFILL;
		defaultSettings->doubleValues["topSolidExtrusionWidth"] = defaultSettings::NOZZLE_D;
		defaultSettings->doubleValues["surfaceInfillDensity"] = defaultSettings::TOP_SURFACE_INFILL;
		defaultSettings->doubleValues["bottomLayerInfill"] = defaultSettings::BOTTOM_SURFACE;
		defaultSettings->doubleValues["mmPerPixel"] = defaultSettings::MM_PIXEL;
		defaultSettings->doubleValues["removeShortLines"] = defaultSettings::REMOVE_SHORT_LINES;
		defaultSettings->doubleValues["threadCount"] = defaultSettings::THREAD_COUNT;
		defaultSettings->doubleValues["stepsPerMMX"] = 200;
		defaultSettings->doubleValues["stepsPerMMY"] = 200;
		defaultSettings->doubleValues["stepsPerMME"] = 744;
		defaultSettings->doubleValues["stepsPerMMZ"] = 8000;

		defaultSettings->doubleValues["texturePrintRes"] = 256;

		//Support
		defaultSettings->boolValues["useSupport"] = false;
		defaultSettings->doubleValues["supportRes"] = 4.0;
		defaultSettings->doubleValues["mainSupportExtruder"] = 0;		//Full colour tool
		defaultSettings->doubleValues["supportInterfaceExtruder"] = 2;	//Support tool(could really feed anything in here)
		defaultSettings->doubleValues["emptySupportLayers"] = 0;
		defaultSettings->doubleValues["supportInterfaceLayers"] = 3;
		defaultSettings->doubleValues["supportInfillDensityFull"] = 70;
		defaultSettings->doubleValues["supportInfillDensity"] = 30;
		defaultSettings->colourValues["supportColour"] = Colour((uint8_t)255, (uint8_t)255, (uint8_t)255);
		defaultSettings->boolValues["overrideInterfaceMaterialBasePlate"] = true;
		defaultSettings->doubleValues["baseMaterialInterfaceTool"] = 0;
		defaultSettings->doubleValues["maxUnsupportedAngle"] = 45;
		defaultSettings->doubleValues["supportOffsetFromPart"] = 0.3;
		defaultSettings->boolValues["preciseSupportCut"] = false;

		defaultSettings->doubleValues["skirtLoops"] = 0;
		defaultSettings->doubleValues["skirtOffset"] = 10.0;


		defaultSettings->boolValues["useCrazyInfill"] = defaultSettings::USE_CRAZY_INFILL;
		defaultSettings->boolValues["useInfill"] = defaultSettings::USE_INFILL;
		defaultSettings->boolValues["simplifyColourPerLine"] = defaultSettings::SIMPLIFY_COLOUR_PER_LINE;
		defaultSettings->boolValues["considerSurfaces"] = defaultSettings::CONSIDER_SURFACE;
		defaultSettings->boolValues["colourPreviewByType"] = defaultSettings::COLOUR_PREVIEW_BY_TYPE;
		defaultSettings->boolValues["savePerimetersToTestFile"] = false;
		defaultSettings->boolValues["generatePrimingTowers"] = defaultSettings::GENERATRE_PRIMING_TOWERS;

		defaultSettings->boolValues["quantizeMovement"] = false;
		defaultSettings->boolValues["useSupport"] = false;
		defaultSettings->boolValues["resetEAxis"] = true;
		defaultSettings->boolValues["addDebugComments"] = true;
		defaultSettings->boolValues["estimateTime"] = true;
		defaultSettings->boolValues["useConstantExtrusionSpeed"] = false;
		defaultSettings->boolValues["useSoftwareRetraction"] = true;
		defaultSettings->boolValues["retractOnLayerChange"] = false;

		defaultSettings->boolValues["infillBeforePerimeter"] = false;
		defaultSettings->boolValues["perimetersInsideOut"] = false;
		defaultSettings->boolValues["retractAllActive"] = false;
		defaultSettings->boolValues["noFanLayerOne"] = true;
		defaultSettings->boolValues["verbose"] = false;
		defaultSettings->boolValues["retactInfillOnlyCrossingPerim"] = true;

		defaultSettings->doubleValues["zLift"] = 0.0;
		defaultSettings->doubleValues["extrusionMultiplier"] = 1.0;
		defaultSettings->doubleValues["printSpeed"] = 2552;
		defaultSettings->doubleValues["travelSpeed"] = 2552;
		defaultSettings->doubleValues["travelDistanceWithoutRetract"] = 10;
		defaultSettings->doubleValues["printXOffset"] = 0;
		defaultSettings->doubleValues["printYOffset"] = 0;
		defaultSettings->doubleValues["mixerSpeed"] = 60;
		defaultSettings->doubleValues["coastLength"] = 0;
		defaultSettings->doubleValues["retractAmount"] = 0.0;
		defaultSettings->doubleValues["issueAmount"] = 0.0;
		defaultSettings->doubleValues["retractFeedrate"] = 4800;
		defaultSettings->doubleValues["issueFeedrate"] = 4800;
		defaultSettings->doubleValues["wipeDistance"] = 0.0;
		defaultSettings->doubleValues["wipeFeedrate"] = 1200;
		defaultSettings->doubleValues["fanSpeed"] = 127;
		defaultSettings->doubleValues["servoIssueTime"] = 100;
		defaultSettings->doubleValues["servoLiftTime"] = 100;

		defaultSettings->doubleValues["purgeTime"] = 100;
		defaultSettings->doubleValues["mixerSpeedOnPurge"] = 100;
		defaultSettings->doubleValues["kMeansAttemps"] = 5;
		defaultSettings->doubleValues["colourRes"] = 521;

		defaultSettings->doubleValues["thresholdVert"] = 1E-8;

		defaultSettings->doubleValues["keepAliveTime"] = defaultSettings::KEEP_ALIVE_TIME;

		defaultSettings->stringValues["onStartGCode"] = "";
		defaultSettings->stringValues["onEndGCode"] = "";
		defaultSettings->stringValues["onBeforeZGCode"] = "";
		defaultSettings->stringValues["onAfterZGCode"] = "";
		defaultSettings->stringValues["onRetractGCode"] = "";
		defaultSettings->stringValues["onIssueGCode"] = "";

		// Purge settings
		defaultSettings->boolValues["purgeAttribute"] = false;	//freelancer latino
		defaultSettings->stringValues["onPurgeAttribute"] = ""; //freelancer latino
		defaultSettings->boolValues["mCommand"] = false;
		defaultSettings->stringValues["mCommandText"] = "";

		defaultSettings->boolValues["alwaysPurge"] = false;
		defaultSettings->boolValues["useSoftwarePurge"] = false;
		defaultSettings->boolValues["purgeForceRetract"] = false;
		defaultSettings->doubleValues["purgeLength"] = 100;
		defaultSettings->doubleValues["purgeMixerSpeed"] = 100;
		defaultSettings->doubleValues["purgeWait"] = 1000;
		defaultSettings->doubleValues["purgeLocationX"] = 100;
		defaultSettings->doubleValues["purgeLocationY"] = 100;
		defaultSettings->doubleValues["purgeWidth"] = 50;
		defaultSettings->doubleValues["purgeHeight"] = 50;
		defaultSettings->doubleValues["purgeWipeX"] = 100;
		defaultSettings->doubleValues["purgeWipeWidth"] = 20;
		defaultSettings->doubleValues["purgeFeedrate"] = 2400;
		defaultSettings->boolValues["pauseOnFullPurgeBin"] = false;
		defaultSettings->doubleValues["purgeBinCapacity"] = 2000;

		defaultSettings->doubleValues["firstLayerSpeed"] = 30;
		defaultSettings->doubleValues["perimeterSpeed"] = 60;
		defaultSettings->doubleValues["infillSpeed"] = 80;
		defaultSettings->doubleValues["travelSpeed"] = 120;
}

#endif

SettingValues::SettingValues()
{
}

bool SettingValues::removeEscape(std::string &output)
{
	bool result = (output.back() == '"' && output.at(output.length() - 1) != '\\');
	
	//Now remove the escape characters
	for (std::size_t i = 0; i < output.length(); i++)
	{
		if (output.at(i) == '\\' && output.at(i + 1) == '"')
		{
			output.erase(i,1);
		}
	}
	return result;
}

bool SettingValues::loadFrom(const char* buffer, unsigned long length)
{
	std::istringstream ss(std::string(buffer, length));
	return loadFrom(ss);
}

bool SettingValues::loadFrom(std::istream &inStream)
{
	std::string currentLine;
	std::getline(inStream, currentLine);
	if (currentLine.find("BodySplitter Settings") == std::string::npos)
		return false;
	int type = 0;
	while (!inStream.eof())
	{
		std::getline(inStream, currentLine);
		if (currentLine.find("[double]") != std::string::npos)
			type = 1;
		else if (currentLine.find("[boolean]") != std::string::npos)
			type = 2;
		else if (currentLine.find("[string]") != std::string::npos)
			type = 3;
		else if (currentLine.find("[colours]") != std::string::npos)
			type = 4;
		else
		{
			//Actual line
			if (type == 1)
			{
				doubleValues[currentLine.substr(0, currentLine.find(' '))] = std::stod(currentLine.substr(currentLine.find('=') + 1, currentLine.length()));
			}
			else if (type == 2)
			{
				boolValues[currentLine.substr(0, currentLine.find(' '))] = currentLine.at(currentLine.find('=') + 2) == '1';
			}
			else if (type == 3)
			{
				//Read Key
				std::string key = currentLine.substr(0, currentLine.find(' '));
				if (key.empty())
					break;
				// Read until unescaped quotation
				std::string result;
				bool endLine = true;
				currentLine = currentLine.substr(currentLine.find('\"') + 1, currentLine.length());
				endLine = removeEscape(currentLine);
				result += currentLine;
				while (endLine == false)
				{
					std::getline(inStream, currentLine);
					endLine = removeEscape(currentLine);
					if (currentLine[0] == '\0')
						endLine = true;
					result += "\n" + currentLine;
				}
				stringValues[key].assign(result.begin(), result.end() - 1); // remove the closing "
			}
			else if (type == 4)
			{
				colourValues[currentLine.substr(0, currentLine.find(' '))] = Colour(currentLine.substr(currentLine.find('=') + 1));
			}
			else
			{
				//error
				return false;
			}
		}
	}
	return true;
}

bool SettingValues::loadFrom(const std::string &filename)
{
	try {
		std::ifstream inputFile;
		inputFile.open(filename.c_str());
		if (!inputFile.is_open()) {
#ifndef _CONSOLE
			wxMessageBox("Unable to find SliceSettings.ini, reverting to default settings", "Error");
#endif
			return false;
		}
		return loadFrom(inputFile);
	}
	catch (...)
	{
#ifndef _CONSOLE
		wxMessageBox("Unable to find SliceSettings.ini, reverting to default settings", "Error");
#endif
		return false;
	}
}
//static
std::string SettingValues::escapeQuotes(const std::string &in)
{
	std::string input(in);
	for (std::size_t i = 0; i < input.length(); i++)
	{
		if (input.at(i) == '"')
		{
			input.insert(i, "\\");
			i+=2;
		}
	}
	return input;
}

bool SettingValues::saveTo(std::ostream &outStream) const
{
	outStream << "BodySplitter Settings\n";
	outStream << "[double]\n";
	{
		std::vector<std::string> sortedKeys;
		for (auto it = doubleValues.begin(); it != doubleValues.end(); ++it)
		{
			sortedKeys.push_back(it->first);
		}
		std::sort(sortedKeys.begin(), sortedKeys.end());
		for (auto it : sortedKeys)
		{
			outStream << it << " = " << std::to_string(doubleValues.at(it)) << "\n";
		}
	}
	outStream << "[boolean]\n";
	{
		std::vector<std::string> sortedKeys;
		for (auto it = boolValues.begin(); it != boolValues.end(); ++it)
		{
			sortedKeys.push_back(it->first);
		}
		std::sort(sortedKeys.begin(), sortedKeys.end());
		for (auto it : sortedKeys)
		{
			outStream << it << " = " << std::to_string(boolValues.at(it)) << "\n";
		}
	}
	outStream << "[colours]\n";
	{
		std::vector<std::string> sortedKeys;
		for (auto it = colourValues.begin(); it != colourValues.end(); ++it)
		{
			sortedKeys.push_back(it->first);
		}
		std::sort(sortedKeys.begin(), sortedKeys.end());
		for (auto it : sortedKeys)
		{
			outStream << it << " = " << colourValues.at(it).getR<double>() << "," << colourValues.at(it).getG<double>() << "," << colourValues.at(it).getB<double>() << "\n";
		}
	}
	outStream << "[string]\n";
	{
		std::vector<std::string> sortedKeys;
		for (auto it = stringValues.begin(); it != stringValues.end(); ++it)
		{
			sortedKeys.push_back(it->first);
		}
		std::sort(sortedKeys.begin(), sortedKeys.end());
		for (auto it : sortedKeys)
		{
			outStream << it << " = \"" << (escapeQuotes(stringValues.at(it))) << "\"\n";
		}
	}
	return true;
}

bool SettingValues::saveTo(const std::string &filename) const
{
	std::ofstream outputFile;
	outputFile.open(filename.c_str());
	if (outputFile.bad())
		return false;
	outputFile.clear();
	if (saveTo(outputFile))
	{
		outputFile.close();
		return true;
	}
	else
		return false;
}

Settings::Settings()
{
	//try to load settings
	if (loadSettings())
	{

	}
	else
	{
		// Use default settings
	}
}



Settings::Settings(int argc, char **argv, wxString &source, wxString &output)
{
	wxDisableAsserts();
	//try to load settings
	loadSettings();
#ifdef CONSOLE
	fillDefaultSettings(&m_SliceSettings);
#endif

	static const wxCmdLineEntryDesc cmdLineDesc[] =
	{
		{ wxCMD_LINE_SWITCH, "v", "verbose", "be verbose" },
		{ wxCMD_LINE_OPTION, "o", "output",  "output file", wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "i", "input",   "input file" , wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY },
		{ wxCMD_LINE_OPTION, "s", "settings",   "settings file" , wxCMD_LINE_VAL_STRING },
		{ wxCMD_LINE_OPTION, "p", "purgefile.gcode",   "purgefile.gcode file" , wxCMD_LINE_VAL_STRING }, // freelancer latino
		{ wxCMD_LINE_OPTION, "m", "m.gcode",   "m.gcode file" , wxCMD_LINE_VAL_STRING }, // freelancer latino
		{ wxCMD_LINE_OPTION, "x", "purgeOnToolChange",   "purge On Tool Change "}, // freelancer latino

		{ wxCMD_LINE_SWITCH, NULL, "useCrazyInfill", "push colour inside"},
		{ wxCMD_LINE_SWITCH, NULL, "useInfill", "use infill" },
		{ wxCMD_LINE_SWITCH, NULL, "simplifyColourPerLine", "unsure" },
		{ wxCMD_LINE_SWITCH, NULL, "considerSurfaces", "horizontal surfaces" },
		{ wxCMD_LINE_SWITCH, NULL, "quantizeMovement", "ignore moves less than step size" },
		{ wxCMD_LINE_SWITCH, NULL, "useSupport", "automatically generate support" },
		{ wxCMD_LINE_SWITCH, NULL, "resetEAxis", "avoid large extrusion values" },
		{ wxCMD_LINE_SWITCH, NULL, "addDebugComments", "adds comments to the output gcode" },
		{ wxCMD_LINE_SWITCH, NULL, "useSoftwareRetraction", "use software retract instead of G10/G11" },
		{ wxCMD_LINE_SWITCH, NULL, "retractOnLayerChange", "retract on layer change" },
		{ wxCMD_LINE_SWITCH, NULL, "infillBeforePerimeter", "infill before outer perimeters" },
		{ wxCMD_LINE_SWITCH, NULL, "perimetersInsideOut", "start with innermost perimeter" },
		{ wxCMD_LINE_SWITCH, NULL, "retractAllActive", "retract equal amount all active tools" },
		{ wxCMD_LINE_SWITCH, NULL, "noFanLayerOne", "disable fan on layer one" },

		{ wxCMD_LINE_SWITCH, NULL, "rova5Compat", "enable compatability with 5 extruders" },
		{ wxCMD_LINE_SWITCH, NULL, "useKMeans", "simplify colour entire model" },
		{ wxCMD_LINE_SWITCH, NULL, "travisSimplification", "simplify colour by line" },
		{ wxCMD_LINE_SWITCH, NULL, "useSupport", "enable automatic support generation" },
		{ wxCMD_LINE_SWITCH, NULL, "overrideInterfaceMaterialBasePlate", "dont use dissolvable support on baseplate touch layer" },
		{ wxCMD_LINE_SWITCH, NULL, "preciseSupportCut", "cut support to only needed amt, reduce material usage >> slicing time" },

		//double
		{ wxCMD_LINE_OPTION, NULL, "layerThickness",  "default layer thickness", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "firstLayerThickness",  "first layer", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "bottomSolidLayers",  "solid layers on part bottom", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "topSolidLayers",  "solid layers on part top", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "infillDensity",  "infill density", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "perimeterCount",  "number of perimeters", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "XYOffset",  "compress or inflate objects based on this distnace(no colour information)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "compatExtruderCount",  "number of extruders to use in traditional compatability mode", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "kMeansColourCount",  "colours to use in k-means", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "colourChangeThreshold",  "threshold to change colours (travis simplification)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "minLineLengthTravisSimplification",  "threshold to change colours (travis simplification)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "coasterBaseLayers",  "number of layers of fixed colour on coaster", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "colourBinThreshold",  "unknown", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "baseOpacity",  "opactity of coaster base layer", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "coasterSurfaceLayers",  "number of top layers on coaster", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "coasterSurfaceOpacity",  "opactity of coaster top layers", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "nozzleD",  "nozzle diameter", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "extrusionWidth",  "extrusion width", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "minExtrusionWidth",  "under extrude to create finer features", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "filamentD",  "filament diameter", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "minimumLineLength",  "ignore lines smaller than this(mm -experimental)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "endZ",  "print to this height (mm)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "infillOutlineOverlapPercentage",  "overlap infill perimeters", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "mmPerPixel",  "mm per pixel when pushing colour inside", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "threadCount",  "threads to use while slicing", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "stepsPerMMX",  "use for removing too short movements", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "stepsPerMMY",  "use for removing too short movements", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "stepsPerMME",  "use for removing too short movements", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "stepsPerMMZ",  "use for removing too short movements", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportRes",  "resolution of support towers", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "mainSupportExtruder",  "tool to use for support material", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportInterfaceExtruder",  "tool to use for support interface material", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "emptySupportLayers",  "layers to leave empty", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportInterfaceLayers",  "layers to interface material", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportInfillDensityFull",  "support infill density on full layers", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportInfillDensity",  "support infill density", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "baseMaterialInterfaceTool",  "tool to use on base layer", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "maxUnsupportedAngle",  "angle before considering support", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "supportOffsetFromPart",  "support offset from part", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "zLift",  "z lift on retract", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "extrusionMultiplier",  "extrusion amt multiplier", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "printSpeed",  "speed during printing moves (mm/min)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "travelSpeed",  "speed during travel moves (mm/min)", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "travelDistanceWithoutRetract",  "distance without retract", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "retractAmount",  "distance to retract", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "issueAmount",  "distance to issue", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "retractFeedrate",  "retractFeedrate", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "issueFeedrate",  "issue feedrate", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "servoLiftpos",  "Servo position on lift", wxCMD_LINE_VAL_DOUBLE },
		{ wxCMD_LINE_OPTION, NULL, "servoIssuepos",  "servo position on issue", wxCMD_LINE_VAL_DOUBLE },
		//{ wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE },
		{ wxCMD_LINE_NONE }
	};
	/*
	defaultSettings->doubleValues["zLift"] = 0.0;
	defaultSettings->doubleValues["extrusionMultiplier"] = 1.0;
	defaultSettings->doubleValues["printSpeed"] = 2552;
	defaultSettings->doubleValues["travelSpeed"] = 2552;
	defaultSettings->doubleValues["travelDistanceWithoutRetract"] = 10;
	defaultSettings->doubleValues["printXOffset"] = 0;
	defaultSettings->doubleValues["printYOffset"] = 0;
	defaultSettings->doubleValues["mixerSpeed"] = 60;
	defaultSettings->doubleValues["coastLength"] = 0;
	defaultSettings->doubleValues["retractAmount"] = 0.0;
	defaultSettings->doubleValues["issueAmount"] = 0.0;
	defaultSettings->doubleValues["retractFeedrate"] = 4800;
	defaultSettings->doubleValues["issueFeedrate"] = 4800;
	defaultSettings->doubleValues["wipeDistance"] = 0.0;
	defaultSettings->doubleValues["fanSpeed"] = 127;

	int i = 0;
	for (auto it : m_SliceSettings.doubleValues)
	{
		i++;
		if (i > 10)
			break;
		cmdLineDesc.push_back({ wxCMD_LINE_OPTION, NULL, "hmm", "desciption", wxCMD_LINE_VAL_DOUBLE });
	}
	*/
	wxCmdLineParser parser;

	parser.SetDesc(cmdLineDesc);
	//for (auto it : m_SliceSettings.boolValues)
	//{
	//	parser.AddSwitch(it.first);
	//}
	parser.SetCmdLine(argc, argv);
	//std::cout << parser.GetUsageString() << "\n";
	switch (parser.Parse())
	{
	case(-1):
		std::cerr << "No option\n";
		std::cerr << parser.GetUsageString() << "\n";
		return;
		break;
	case (0):
		std::cerr << "cmd ok\n";
		break;
	default:
		std::cerr << "Cmd line error\n";
		std::cerr << parser.GetUsageString() << "\n";
		return;
	}
	parser.Found("input", &source);
	parser.Found("output", &output);




	wxString settingsFile;
	if (parser.Found("settings", &settingsFile))
	{
		m_SliceSettings.loadFrom(settingsFile.ToStdString());
		if (parser.Found("verbose"))	//verbose gets a special true override
			m_SliceSettings.boolValues["verbose"] = true;
		// Also check for double value overrides
		for (auto &it : m_SliceSettings.doubleValues)
		{
			double temp = 100;
			if (parser.Found(it.first, &temp))
			{
				if (m_SliceSettings.boolValues["verbose"])
					std::cerr << "Setting: " << it.first << " to " << temp << "\n";
				it.second = temp;
			}
		}
	}
	else
	{
		for (auto &it : m_SliceSettings.boolValues)
		{
			it.second = false;
		}
		for (auto &it : m_SliceSettings.boolValues)
		{
			if (parser.Found(it.first))
				it.second = true;
		}
		for (auto &it : m_SliceSettings.doubleValues)
		{
			double temp;
			if (parser.Found(it.first, &temp))
				it.second = temp;
		}
	}


	if (parser.Found("purgeOnToolChange")) //freelancer latino
		m_SliceSettings.boolValues["purgeOnToolChange"] = true;

	wxString purge_gcode;//freelancer latino
	if (parser.Found("purgefile.gcode", &purge_gcode))//freelancer latino
	{
#include <string>
#include <fstream>
#include <streambuf>
		using namespace std;
		m_SliceSettings.boolValues["purgeAttribute"] = true;	//freelancer latino
		m_SliceSettings.stringValues["onPurgeAttribute"] = purge_gcode.ToStdString();  //freelancer latino
		try
		{
			ifstream inFile;
			inFile.open(purge_gcode.ToStdString());//open the input file
			stringstream strStream;
			strStream << inFile.rdbuf();//read the file
			string str = strStream.str();
			if (str.back() != '\n' && str.back() != '\r')
				str += "\n";
			m_SliceSettings.stringValues["onPurgeAttribute"] =
				";begin purge file\n" + str + "; end purge file\n";
		}
		catch (...)
		{
			m_SliceSettings.boolValues["purgeAttribute"] = false;
			std::cerr << "Cannot load " << purge_gcode.ToStdString() << endl;
		}
	}


	wxString mGcode;//freelancer latino
	if (parser.Found("m.gcode", &mGcode))//freelancer latino
	{
#include <string>
#include <fstream>
#include <streambuf>
		using namespace std;
		m_SliceSettings.boolValues["mCommand"] = true;	//freelancer latino
		m_SliceSettings.stringValues["mCommandText"] = mGcode.ToStdString();  //freelancer latino
		try
		{
			ifstream inFile;
			inFile.open(mGcode.ToStdString());//open the input file
			stringstream strStream;
			strStream << inFile.rdbuf();//read the file
			string str = strStream.str();
			if (str.back() != '\n' && str.back() != '\r')
				str += "\n";
			m_SliceSettings.stringValues["mCommandText"] = str;
		}
		catch (...)
		{
			m_SliceSettings.boolValues["mCommand"] = false;
			std::cerr << "Cannot load " << mGcode.ToStdString() << endl;
		}
	}

	
}

int Settings::OpenSettingsWindow()
{
	
	RovaSettings* dialog = new RovaSettings(&this->m_SliceSettings); //switch to RAII
	dialog->populateBoxes(&this->m_SliceSettings);
	int result = dialog->ShowModal();
	if (result != wxID_OK)//if (result == wxCANCEL)
	{
		delete dialog;
		return false;
	}
	else// if (result == wxID_OK)
	{
		dialog->saveSettings();
		saveSettings();
		delete dialog;
		return true;
	}
}

Settings::~Settings()
{

}

bool Settings::loadSettings()
{
#ifndef STORE_FILES_WITH_EXE
	if (!m_SliceSettings.loadFrom(wxStandardPaths::Get().GetUserConfigDir().ToStdString() + "\\SliceSettings.ini"))
	{
		auto logger = spdlog::get("DEBUG_FILE");
		logger->info("Couldnt load user settings:" + wxStandardPaths::Get().GetUserConfigDir().ToStdString() + "\\SliceSettings.ini using default");
	}
	else
		return true;
#endif
	return
		m_SliceSettings.loadFrom("SliceSettings.ini");
	
}

bool Settings::loadNewSettings(std::istream &inStream)
{
	return m_SliceSettings.loadFrom(inStream);
}

bool Settings::saveSettingsTo(std::ostream &outStream)
{
	return m_SliceSettings.saveTo(outStream);
}

bool Settings::saveColourSubMap(std::ostream &outStream)
{
	outStream << "[startOfColourSub]\n";
	for (auto it : colourSubMap)
	{
		outStream << it.first;
		outStream << "=";
		outStream << it.second;
	}
	outStream << "[endOfColourSub]\n";
	return true;
}
bool Settings::loadColourSubMap(std::istream &inStream)
{
	colourSubMap.clear();
	std::string currentLine;
	std::getline(inStream, currentLine);
	if (currentLine.find("[startOfColourSub]") == std::string::npos)
		return false;
	std::getline(inStream, currentLine);
	while (currentLine.find("[endOfColourSub]") == std::string::npos)
	{
		uint32_t colour32bit;
		Colour::ColourSpace::CMYKWT colour;
		colour32bit = std::stoul(currentLine);
		int index = currentLine.find("=") + 1;
		colour.c = std::stod(currentLine.substr(index));
		index = currentLine.find(",", index) + 1;
		colour.m = std::stod(currentLine.substr(index));
		index = currentLine.find(",", index) + 1;
		colour.y = std::stod(currentLine.substr(index));
		index = currentLine.find(",", index) + 1;
		colour.k = std::stod(currentLine.substr(index));
		index = currentLine.find(",", index) + 1;
		colour.w = std::stod(currentLine.substr(index));
		index = currentLine.find(",", index) + 1;
		colour.t = std::stod(currentLine.substr(index));
		colourSubMap[colour32bit] = colour;
		std::getline(inStream, currentLine);
	}
	return true;
}

bool Settings::saveSettings()
{
#ifndef STORE_FILES_WITH_EXE
	return
		m_SliceSettings.saveTo(wxStandardPaths::Get().GetUserConfigDir().ToStdString() + "\\SliceSettings.ini");
#endif
	return
		m_SliceSettings.saveTo("SliceSettings.ini");
}

double Settings::getDoubleValue(const std::string &key) const
{
	if(m_SliceSettings.doubleValues.count(key))
		return m_SliceSettings.doubleValues.at(key);
	else
		return RovaSettings::getDefaultDouble(key);
}

std::string Settings::getStringValue(const std::string &key) const
{
	if (m_SliceSettings.stringValues.count(key))
		return m_SliceSettings.stringValues.at(key);
	else
		return RovaSettings::getDefaultString(key);
}

bool Settings::getBoolValue(const std::string &key) const
{
	if (m_SliceSettings.boolValues.count(key))
		return m_SliceSettings.boolValues.at(key);
	else
		return RovaSettings::getDefaultBool(key);
}

void Settings::setBoolValue(const std::string& key, bool value)
{
  m_SliceSettings.boolValues[key] = value;
}


Colour Settings::getColourValue(const std::string &key) const
{
	if (m_SliceSettings.colourValues.count(key))
		return m_SliceSettings.colourValues.at(key);
	else
		return RovaSettings::getDefaultColour(key);
}

bool GET_BOOL(const std::string &key)
{
	return Settings::getSingleton().getBoolValue(key);
}
double GET_DOUBLE(const std::string &key)
{
	return Settings::getSingleton().getDoubleValue(key);
}
Colour GET_COLOUR(const std::string &key)
{
	return Settings::getSingleton().getColourValue(key);
}

