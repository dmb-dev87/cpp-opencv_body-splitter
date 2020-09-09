#include "RovaSettings.h"
#include <spdlog/spdlog.h>

#include "Settings.h"
#include "defaultSettings.h"

SettingValues *RovaSettings::defaultSettings = nullptr;
std::mutex RovaSettings::defaultSettingWriter;

namespace defaultSettings {
	const double	LAYER_THICKNESS				= 0.3;
	const double	FIRST_LAYER_THICKNESS		= 0.3;
	const int		BOTTOM_SOLID				= 1;
	const int		TOP_SOLID					= 1;
	const int		INFILL_DENSITY				= 25;
	const int		PERIM_COUNT					= 1;
	const wxColour	INFILL_COLOUR				(255, 255, 255);
	const wxColour	PERIM_COLOUR				(255, 255, 255);
	const wxColour	T1COLOUR					(255, 20, 20); //Flexible
	const wxColour	T2COLOUR					(240, 240, 240); // Support material
	const wxColour	T3COLOUR					(240, 240, 240); // Other colour
	const wxColour	T4COLOUR					(240, 240, 240); // really rova3d compat only
	const wxColour	T5COLOUR					(240, 240, 240); // I think the multicolor I3 only does 4

	const bool		ROVA5COMPAT					= false;
	const double	EXTRUDER_COUNT				= 5;

	const bool		USE_K_MEANS					= true;
	const int		K_MEANS_COLOUR				= 5;
	const bool		USE_TRAVIS_SIMPLIFICATION	= true;
	const double	COLOUR_CHANGE_THRESHOLD		= 2.3;
	const double	MIN_LENGTH_COLOUR_LINE		= 1.0;
	const double	COLOUR_BIN_THRESHOLD		= 10.5;

	const int		COASTER_BASE				= 1;
	const int		COASTER_TOP					= 1;
	const wxColour	COASTER_BASE_COLOUR			(255, 255, 255);
	const int		BASE_OPACITY				= 100;
	const int		SURFACE_OPACITY				= 100;
	const double	NOZZLE_D					= 0.4;
	const double	EXT_WIDTH					= 0.6;
	const double	MIN_EXT_WIDTH				= NOZZLE_D;
	const double	TOP_SOLID_EXT_WIDTH			= NOZZLE_D;
	const double	FILAMENT_D					= 1.75;

	const int		T0							= 210;
	const int		T1							= 210;
	const int		T2							= 210;
	const int		T3							= 210;
	const int		T4							= 210;
	const int		BED							= 70;

	const bool		INFILL_FIRST				= false;
	const bool		PERIMS_INSIDE_OUT			= false;

	const bool		SIMPLE_GCODE				= false;
	const int		GRID_WIDTH					= 304;
	const int		GRID_DEPTH					= 304;
	const int		GRID_HEIGHT					= 495;
	const int		GRID_SPACING				= 20;

	const int		KEEP_ALIVE_TIME				= 30;

	const std::string EMPTY_STRING				= "EMPTY";

	// User hidden settings
	const double CAMERA_ORIGIN_X	= GRID_WIDTH/2.0;
	const double CAMERA_ORIGIN_Y	= GRID_WIDTH/2.0;
	const double CAMERA_ORIGIN_Z	= 20;
	const double CAMERA_Z			= 20;
	const double INFILL_OVERLAP		= 10;
	const double TOP_SURFACE_INFILL = 100;
	const double BOTTOM_SURFACE		= 100;
	const double MM_PIXEL			= 0.1;
	const double REMOVE_SHORT_LINES = 0.1;
	const double THREAD_COUNT		= 16+1;
	const double MAX_COASTER_COLOURS = 32;

	const bool USE_INFILL = true;
	const bool SIMPLIFY_COLOUR_PER_LINE = false;
	const bool CONSIDER_SURFACE = false;
	const bool COLOUR_PREVIEW_BY_TYPE = false;
	const double MIN_LINE_LENGTH = 0.01;
	const double END_Z = -1.0000;
	const bool USE_CRAZY_INFILL = true;
	const bool GENERATRE_PRIMING_TOWERS = false;

};

void RovaSettings::fillDefaultSettings() {
	std::lock_guard<std::mutex> lock(defaultSettingWriter);
	if (defaultSettings)
		return;
//  	auto my_logger = spdlog::basic_logger_mt("settingslog", "settings.txt");
	defaultSettings = new SettingValues();
	
	defaultSettings->stringValues["firmware"] = "RoVa4D";
	defaultSettings->boolValues["useChecksums"] = true;
	defaultSettings->boolValues["feedrateMMm"] = true;

	defaultSettings->doubleValues["layerThickness"] = defaultSettings::LAYER_THICKNESS;
	defaultSettings->doubleValues["firstLayerThickness"] = defaultSettings::FIRST_LAYER_THICKNESS;
	defaultSettings->doubleValues["firsyLayerExtrusion"] = 100;
	defaultSettings->doubleValues["bottomSolidLayers"] = defaultSettings::BOTTOM_SOLID;
	defaultSettings->doubleValues["topSolidLayers"] = defaultSettings::TOP_SOLID;
	defaultSettings->doubleValues["infillDensity"] = defaultSettings::INFILL_DENSITY;
	defaultSettings->doubleValues["perimeterCount"] = defaultSettings::PERIM_COUNT;
	defaultSettings->doubleValues["XYOffset"] = 0.0;

	defaultSettings->boolValues["printAllPerimsFirst"] = false;

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

	defaultSettings->colourValues["t3Colour"] = Colour(defaultSettings::T3COLOUR.Red(),
		defaultSettings::T3COLOUR.Green(),
		defaultSettings::T3COLOUR.Blue());

	defaultSettings->colourValues["t4Colour"] = Colour(defaultSettings::T4COLOUR.Red(),
		defaultSettings::T4COLOUR.Green(),
		defaultSettings::T4COLOUR.Blue());

	defaultSettings->colourValues["t5Colour"] = Colour(defaultSettings::T5COLOUR.Red(),
		defaultSettings::T5COLOUR.Green(),
		defaultSettings::T5COLOUR.Blue());

	defaultSettings->boolValues["rova5Compat"] = defaultSettings::ROVA5COMPAT;
	defaultSettings->doubleValues["compatExtruderCount"] = defaultSettings::EXTRUDER_COUNT;
	defaultSettings->stringValues["headChoice"] = "RoVa4D - CMYK+WT";
	defaultSettings->boolValues["mixerOnlyUse"] = true;

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

	defaultSettings->doubleValues["T0nozzleD"] = 0.5f;
	defaultSettings->doubleValues["T0extrusionWidth"] = 0.6f;
	defaultSettings->doubleValues["T0minExtrusionWidth"] = 0.4f;
	defaultSettings->doubleValues["T0filamentD"] = 1.75f;
	defaultSettings->doubleValues["T0extMulti"] = 1.0f;
	//
	defaultSettings->doubleValues["T1nozzleD"] = 0.5f;
	defaultSettings->doubleValues["T1extrusionWidth"] = 0.6f;
	defaultSettings->doubleValues["T1minExtrusionWidth"] = 0.4f;
	defaultSettings->doubleValues["T1filamentD"] = 1.75f;
	defaultSettings->doubleValues["T1extMulti"] = 1.0f;
	//
	defaultSettings->doubleValues["T2nozzleD"] = 0.5f;
	defaultSettings->doubleValues["T2extrusionWidth"] = 0.6f;
	defaultSettings->doubleValues["T2minExtrusionWidth"] = 0.4f;
	defaultSettings->doubleValues["T2filamentD"] = 1.75f;
	defaultSettings->doubleValues["T2extMulti"] = 1.0f;

	defaultSettings->doubleValues["fullInfillSelfOverlap"] = 25;
	defaultSettings->doubleValues["infillExtrusionMultiplier"] = 100;
	
	defaultSettings->doubleValues["primeToolPurgeLength"] = 20;

	defaultSettings->colourValues["CMYWhiteSub"] = Colour((uint8_t)255, 255, 0);

	defaultSettings->doubleValues["T0"] = defaultSettings::T0;
	defaultSettings->doubleValues["T1"] = defaultSettings::T1;
	defaultSettings->doubleValues["T2"] = defaultSettings::T2;
	defaultSettings->doubleValues["T3"] = defaultSettings::T3;
	defaultSettings->doubleValues["T4"] = defaultSettings::T4;
	defaultSettings->doubleValues["bedTemp"] = defaultSettings::BED;
	defaultSettings->boolValues["heatAndWait"] = true;

	//autocool
	defaultSettings->boolValues["useCoolingFan"] = false;
	defaultSettings->doubleValues["firstLayerFanSpeed"] = 0;
	defaultSettings->boolValues["keepFanAlwaysOn"] = false;
	defaultSettings->doubleValues["minFanSpeed"] = 30;
	defaultSettings->doubleValues["maxFanSpeed"] = 100;
	defaultSettings->boolValues["reducePrintSpeedSmallLayers"] =false;
	defaultSettings->doubleValues["minLayerTime"] = 10;
	defaultSettings->doubleValues["minLayerTimeFan"] = 60;
	defaultSettings->doubleValues["minSlowDownFeedrate"] = 200;

	defaultSettings->boolValues["simpleGCodePreview"] = defaultSettings::SIMPLE_GCODE;
	defaultSettings->doubleValues["maxX"] = defaultSettings::GRID_WIDTH;
	defaultSettings->doubleValues["maxY"] = defaultSettings::GRID_DEPTH;
	defaultSettings->doubleValues["maxZ"] = defaultSettings::GRID_HEIGHT;
	defaultSettings->doubleValues["gridSize"] = defaultSettings::GRID_SPACING;
	defaultSettings->boolValues["showAxis"] = true;

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
	defaultSettings->doubleValues["maxCoasterColours"] = defaultSettings::MAX_COASTER_COLOURS;
	defaultSettings->doubleValues["stepsPerMMX"] = 100;
	defaultSettings->doubleValues["stepsPerMMY"] = 100;
	defaultSettings->doubleValues["stepsPerMME"] = 372;
	defaultSettings->doubleValues["stepsPerMMZ"] = 800;

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
	defaultSettings->doubleValues["baseMaterialInterfaceLayers"] = 0;
	defaultSettings->doubleValues["minimumHoleAreaSupport"] = 5;

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
	defaultSettings->boolValues["retactInfillOnlyCrossingPerim"] = true;
	defaultSettings->boolValues["retractOnlyPerimeterCrossing"] = false;
	defaultSettings->boolValues["fanOffDuringRetract"] = true;

	defaultSettings->boolValues["infillBeforePerimeter"] = false;
	defaultSettings->boolValues["perimetersInsideOut"] = false;
	defaultSettings->boolValues["retractAllActive"] = false;
	defaultSettings->boolValues["noFanLayerOne"] = true;

	defaultSettings->doubleValues["printXOffset"] = 0;
	defaultSettings->doubleValues["printYOffset"] = 0;

	defaultSettings->doubleValues["zLiftT0"] = 0.0;
	defaultSettings->doubleValues["zLiftFeedT0"] = 20;
	defaultSettings->doubleValues["extrusionMultiplierT0"] = 1.0;
	defaultSettings->doubleValues["printSpeedT0"] = 60;
	defaultSettings->doubleValues["travelSpeedT0"] = 120;
	defaultSettings->doubleValues["travelDistanceWithoutRetractT0"] = 10;
	defaultSettings->doubleValues["coastLengthT0"] = 0;
	defaultSettings->doubleValues["retractAmountT0"] = 0.0;
	defaultSettings->doubleValues["issueAmountT0"] = 0.0;
	defaultSettings->doubleValues["retractFeedrateT0"] = 100;
	defaultSettings->doubleValues["issueFeedrateT0"] = 100;
	defaultSettings->doubleValues["wipeDistanceT0"] = 0.0;
	defaultSettings->doubleValues["wipeFeedrateT0"] = 120;
	defaultSettings->doubleValues["toolChangeRetractDistanceT0"] = 10;
	defaultSettings->doubleValues["toolChangeIssueDistanceT0"] = 10;
	defaultSettings->doubleValues["PrintDistanceBeforeRetractT0"] = 10;

	defaultSettings->doubleValues["zLiftT1"] = 0.0;
	defaultSettings->doubleValues["zLiftFeedT1"] = 20;
	defaultSettings->doubleValues["extrusionMultiplierT1"] = 1.0;
	defaultSettings->doubleValues["printSpeedT1"] = 60;
	defaultSettings->doubleValues["travelSpeedT1"] = 120;
	defaultSettings->doubleValues["travelDistanceWithoutRetractT1"] = 10;
	defaultSettings->doubleValues["coastLengthT1"] = 0;
	defaultSettings->doubleValues["retractAmountT1"] = 0.0;
	defaultSettings->doubleValues["issueAmountT1"] = 0.0;
	defaultSettings->doubleValues["retractFeedrateT1"] = 100;
	defaultSettings->doubleValues["issueFeedrateT1"] = 100;
	defaultSettings->doubleValues["wipeDistanceT1"] = 0.0;
	defaultSettings->doubleValues["wipeFeedrateT1"] = 120;
	defaultSettings->doubleValues["toolChangeRetractDistanceT1"] = 10;
	defaultSettings->doubleValues["toolChangeIssueDistanceT1"] = 10;
	defaultSettings->doubleValues["PrintDistanceBeforeRetractT1"] = 10;

	defaultSettings->doubleValues["zLiftT2"] = 0.0;
	defaultSettings->doubleValues["zLiftFeedT2"] = 20;
	defaultSettings->doubleValues["extrusionMultiplierT2"] = 1.0;
	defaultSettings->doubleValues["printSpeedT2"] = 60;
	defaultSettings->doubleValues["travelSpeedT2"] = 120;
	defaultSettings->doubleValues["travelDistanceWithoutRetractT2"] = 10;
	defaultSettings->doubleValues["coastLengthT2"] = 0;
	defaultSettings->doubleValues["retractAmountT2"] = 0.0;
	defaultSettings->doubleValues["issueAmountT2"] = 0.0;
	defaultSettings->doubleValues["retractFeedrateT2"] = 100;
	defaultSettings->doubleValues["issueFeedrateT2"] = 100;
	defaultSettings->doubleValues["wipeDistanceT2"] = 0.0;
	defaultSettings->doubleValues["wipeFeedrateT2"] = 120;
	defaultSettings->doubleValues["toolChangeRetractDistanceT2"] = 10;
	defaultSettings->doubleValues["toolChangeIssueDistanceT2"] = 10;
	defaultSettings->doubleValues["PrintDistanceBeforeRetractT2"] = 10;

	defaultSettings->doubleValues["mixerSpeed"] = 60;
	defaultSettings->boolValues["useServo"] = true;
	defaultSettings->doubleValues["syringeServoIndex"] = 0;
	defaultSettings->doubleValues["servoLift"] = 0.0;
	defaultSettings->doubleValues["servoIssue"] = 0.0;
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
	defaultSettings->doubleValues["purgeFeedrate"] = 100;
	defaultSettings->boolValues["pauseOnFullPurgeBin"] = false;
	defaultSettings->doubleValues["purgeBinCapacity"] = 2000;
	defaultSettings->doubleValues["purgeWipeSpeed"] = 80;
	defaultSettings->boolValues["primeToolPurgeBin"] = false;
	defaultSettings->boolValues["purgeOnToolChange"] = false;
	defaultSettings->boolValues["purgeAttribute"] = false; // adeed by freelancer latino
	defaultSettings->boolValues["mCommand"] = false; // adeed by freelancer latino
	defaultSettings->boolValues["changeToolOverPurgeBin"] = true;
	defaultSettings->doubleValues["toolChangePurgeDistance"] = 100;
	defaultSettings->doubleValues["toolChangePurgeFeedrate"] = 4000;

	//Look ahead colour change
	defaultSettings->boolValues["lookAheadPurge"] = false;
	defaultSettings->boolValues["purgeExcess"] = false;
	defaultSettings->doubleValues["distanceBeforeTransition"] = 100;
	defaultSettings->doubleValues["distanceOfTransition"] = 200;

	// Speed
	defaultSettings->doubleValues["firstLayerSpeed"] = 30;
	defaultSettings->doubleValues["perimeterSpeed"] = 60;
	defaultSettings->doubleValues["infillSpeed"] = 80;
	defaultSettings->doubleValues["travelSpeed"] = 120;
	//Mixer
	defaultSettings->doubleValues["firstLayerMixerSpeed"] = 60;
	defaultSettings->doubleValues["perimeterMixerSpeed"] = 60;
	defaultSettings->doubleValues["infillMixerSpeed"] = 70;
	defaultSettings->doubleValues["travelMixerSpeed"] = 20;

	// Filament settings
	// Colour
	defaultSettings->doubleValues["colourMaterialCost"] = 50;
	defaultSettings->doubleValues["colourDiameter"] = 1.75;
	defaultSettings->doubleValues["colourDensity"] = 1.25;
	// Support
	defaultSettings->doubleValues["supportMaterialCost"] = 50;
	defaultSettings->doubleValues["supportDiameter"] = 1.75;
	defaultSettings->doubleValues["supportDensity"] = 1.19;
	// Flex
	defaultSettings->doubleValues["flexMaterialCost"] = 50;
	defaultSettings->doubleValues["flexDiameter"] = 1.75;
	defaultSettings->doubleValues["flexDensity"] = 1.25;
	// Need colour tool map for diamond
	//defaultSettings->stringValues["tool0Colour"] = "Cyan";
	//defaultSettings->stringValues["tool1Colour"] = "Magenta";
	//defaultSettings->stringValues["tool2Colour"] = "Yellow";
	// GCode Suspend
	defaultSettings->doubleValues["gcodeSuspendTime"] = 4;
	defaultSettings->boolValues["enableGCodeSplitup"] = false;
	defaultSettings->boolValues["travelToPurgeOnSuspend"] = false;
	defaultSettings->boolValues["purgeOnResume"] = true;
	defaultSettings->stringValues["onSuspend"] = "M104 T0 S0\nM140 S0";
	defaultSettings->stringValues["onResume"] = "G28 X\nG28 Y";
}

Colour RovaSettings::getDefaultColour(const std::string &key)
{
	fillDefaultSettings();
	try {
		return defaultSettings->colourValues.at(key);
	}
	catch (std::out_of_range) {
		auto logger = spdlog::get("settingslog");
		logger->error("Colour: Setting key not found: " + key);
		return Colour((uint8_t)255, (uint8_t)255, (uint8_t)255); // user defined type literals please
	}
}

bool RovaSettings::getDefaultBool(const std::string &key)
{
	fillDefaultSettings();
	try {
		return defaultSettings->boolValues.at(key);
	}
	catch (std::out_of_range) {
		auto logger = spdlog::get("settingslog");
		logger->error("Bool: Setting key not found: " + key);
		return false;
	}
}

double RovaSettings::getDefaultDouble(const std::string &key)
{
	fillDefaultSettings();
	try {
		return defaultSettings->doubleValues.at(key);
	}
	catch (std::out_of_range) {
		auto logger = spdlog::get("settingslog");
		logger->error("Double: Setting key not found: " + key);
		return 1.0;
	}
}

const std::string& RovaSettings::getDefaultString(const std::string &key)
{
	fillDefaultSettings();
	try {
		return defaultSettings->stringValues.at(key);
	}
	catch (std::out_of_range) {
		auto logger = spdlog::get("settingslog");
		logger->error("String: Setting key not found: " + key);
		return defaultSettings::EMPTY_STRING;
	}
}

RovaSettings::RovaSettings(SettingValues *sliceValuesIn) : settingsDialog(nullptr),
sliceValues(sliceValuesIn)
{
	//m_sdbSizer1Apply->Enable(false);
	if (sliceValuesIn->stringValues["headChoice"] == "RoVa4D - CMYK+WT")
	{

	}
}

RovaSettings::~RovaSettings()
{

}

void RovaSettings::OnValueChanged(wxCommandEvent& event)
{
	changesNeedingSaving = true;
	//m_sdbSizer1Apply->Enable();
}
void RovaSettings::OnValueChanged(wxSpinEvent& event)
{
	changesNeedingSaving = true;
	//m_sdbSizer1Apply->Enable();
}

void RovaSettings::SettingsClose(wxCloseEvent& event)
{
	if (event.CanVeto())
	{
		wxMessageDialog temp(this, "Discard any settings changes?", "Discard", wxYES_NO);
		if (changesNeedingSaving && temp.ShowModal() == wxID_YES)
		{
			event.Skip();
		}
		else if (!changesNeedingSaving)
		{
			event.Skip(); //nothing to save, nothing to say
		}
	}
}

//void RovaSettings::setColoursManually(wxCommandEvent& event)
//{
//	m_infillDefault->Enable(m_manualColour->IsChecked());
//	m_perimColour->Enable(m_manualColour->IsChecked());
//}

void RovaSettings::kMeansCheck(wxCommandEvent& event)
{
	//m_kMeansColours->Enable(m_fullModelSimplify->IsChecked());
	changesNeedingSaving = true;
}

void RovaSettings::onPurgeSoftwareToggle(wxCommandEvent& event)
{
	bool state = m_useSoftwarePurge->GetValue();
	m_purgeLength->Enable(state);
	m_purgeMixerSpeed->Enable(state);
	m_mixerSpeed->Enable(state);
	m_purgeFeedrate->Enable(state);
	m_purgeWait->Enable(state);
	m_purgeLocationX->Enable(state);
	m_purgeLocationY->Enable(state);
	m_purgeWidth->Enable(state);
	m_purgeHeight->Enable(state);

	m_purgwWipeX->Enable(state);
	m_purgeWipeWidth->Enable(state);

	m_fullPurgeBinPause->Enable(state);
	m_purgeBinCapacity->Enable(state);
	changesNeedingSaving = true;
}

void RovaSettings::OnUsePurgeBinPrime(wxCommandEvent& event)
{
	bool state = m_purgeBinPrime->GetValue();
	m_purgePrimeDistance->Enable(state);
	changesNeedingSaving = true;
}

void RovaSettings::OnUseCoolingFanCheck(wxCommandEvent& event)
{
	m_fanAlwaysOn->Enable(m_useCoolingFan->GetValue());
	m_firstLayerFanSpeed->Enable(m_useCoolingFan->GetValue());
	m_minFanSpeed->Enable(m_useCoolingFan->GetValue());
	m_fanSpeed->Enable(m_useCoolingFan->GetValue());
	m_reducePrintSpeed->Enable(m_useCoolingFan->GetValue());
	
	m_minLayerTime->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());
	m_minLayerTimeFan->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());
	m_minSlowDownFeedrate->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());

}

void RovaSettings::reducePrintSpeedOnSmallLayers(wxCommandEvent& event)
{
	m_minLayerTime->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());
	m_minLayerTimeFan->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());
	m_minSlowDownFeedrate->Enable(m_useCoolingFan->GetValue() && m_reducePrintSpeed->GetValue());
}

void RovaSettings::travisSimplificationCheck(wxCommandEvent& event)
{
	//m_colourChangeThreshold->Enable(m_travisSimplification->IsChecked());
	//m_travisMinLineLength->Enable(m_travisSimplification->IsChecked());
//	m_ColourBinThreshold->Enable(m_travisSimplification->IsChecked());
	changesNeedingSaving = true;
}

void RovaSettings::exportSettings(wxCommandEvent& event)
{
	wxString outputFileName = "BodySplitterSettings";
	outputFileName += ".ini";
	wxFileDialog writeFileDialog(this, _("Write Settings File"), "", outputFileName,
		"RoVa Setting Files (*.ini)|*.ini", wxFD_SAVE);
	if (writeFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	SettingValues *outputItems = new SettingValues();
	boxesToSettingValues(outputItems);
	// Write these to the file
	outputItems->saveTo(writeFileDialog.GetPath().ToStdString());
	delete outputItems;
	return;
}
void RovaSettings::importSettings(wxCommandEvent& event)
{
	wxFileDialog
		openFileDialog(this, _("Open Settings file"), "", "",
			"RoVa Setting Files (*.ini)|*.ini", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL)
	{
		return;
	}
	SettingValues* newSettings = new SettingValues();
	newSettings->loadFrom(openFileDialog.GetPath().ToStdString());
	populateBoxes(newSettings);
	delete newSettings;
	return;
}
void RovaSettings::applyButton(wxCommandEvent& event)
{
	//m_sdbSizer1Apply->Enable(false);
	boxesToSettingValues(sliceValues);
	changesNeedingSaving = false;
}

void RovaSettings::SwitchRovaCompat(wxCommandEvent& event)
{
	m_ExtCount->Enable(m_RovaCompat->GetValue());
	m_headChoice->Enable(!m_RovaCompat->GetValue());
	if (m_RovaCompat->GetValue())
	{
		//m_travisSimplification->SetValue(false); //introduces new colours
		//m_fullModelSimplify->SetValue(true);
		travisSimplificationCheck(event);
	}
	changesNeedingSaving = true;
	OnHeadChoice(event);
}

void RovaSettings::fillCheck(wxCheckBox *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	if (sliceValuesIn->boolValues.count(key))
		box->SetValue(sliceValuesIn->boolValues.at(key));
	else
		box->SetValue(defaultSettings->boolValues.at(key));
}
void RovaSettings::fillNumber(wxTextCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	if (sliceValuesIn->doubleValues.count(key))
		box->SetValue(std::to_string(sliceValuesIn->doubleValues.at(key)));
	else
		box->SetValue(std::to_string(defaultSettings->doubleValues.at(key)));
}
void RovaSettings::fillNumber(wxTextCtrl *box, const std::string &key, const SettingValues *sliceValuesIn, int decimals) const
{
	double valueToFormat;
	if (sliceValuesIn->doubleValues.count(key))
		valueToFormat = (sliceValuesIn->doubleValues.at(key));
	else
		valueToFormat = (defaultSettings->doubleValues.at(key));
	std::stringstream decimalFormatter;
	std::fixed(decimalFormatter);
	decimalFormatter.precision(decimals);
	decimalFormatter << valueToFormat;
	box->SetValue(decimalFormatter.str());
}
void RovaSettings::fillNumber(wxSpinCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	if (sliceValuesIn->doubleValues.count(key))
		box->SetValue((sliceValuesIn->doubleValues.at(key)));
	else
		box->SetValue((defaultSettings->doubleValues.at(key)));
}
void RovaSettings::fillString(wxTextCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	if (sliceValuesIn->stringValues.count(key))
		box->SetValue((sliceValuesIn->stringValues.at(key)));
	else
		box->SetValue((defaultSettings->stringValues.at(key)));
}
void RovaSettings::fillColour(wxColourPickerCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	if (sliceValuesIn->colourValues.count(key))
		box->SetColour(wxColour(sliceValuesIn->colourValues.at(key).getR<uint8_t>()
			, sliceValuesIn->colourValues.at(key).getG<uint8_t>()
			, sliceValuesIn->colourValues.at(key).getB<uint8_t>()));
	else
		box->SetColour(wxColour(defaultSettings->colourValues.at(key).getR<uint8_t>()
			, defaultSettings->colourValues.at(key).getG<uint8_t>()
			, defaultSettings->colourValues.at(key).getB<uint8_t>()));
}

void RovaSettings::fillComboBoxNumber(wxComboBox *box, const std::string &key, const SettingValues *sliceValuesIn,
	const std::string &prefix, const std::string &postfix) const
{
	if (sliceValuesIn->doubleValues.count(key))
		box->SetValue(prefix+std::to_string((int)sliceValuesIn->doubleValues.at(key)) + postfix);
	else
		box->SetValue(std::to_string((int)defaultSettings->doubleValues.at(key)) + postfix);
}

void RovaSettings::fillChoiceBoxSelection(wxChoice *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	int selection = 0;
	if (sliceValuesIn->doubleValues.count(key))
		selection = (int)sliceValuesIn->doubleValues.at(key);
	else
		selection = (int)defaultSettings->doubleValues.at(key);
	box->SetSelection(selection);
}

void RovaSettings::fillChoiceBoxString(wxChoice *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	auto strings = box->GetStrings();
	std::string toFind;
	if (sliceValuesIn->stringValues.count(key))
		toFind = sliceValuesIn->stringValues.at(key);
	else
		toFind = defaultSettings->stringValues.at(key);
	for (int i = 0; i < strings.size(); i++)
	{
		if (strings[i] == toFind)
		{
			box->SetSelection(i);
			return;
		}
	}
	box->SetSelection(0);
}

void RovaSettings::fillFeedrate(wxTextCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	double feedRate;
	if (sliceValuesIn->doubleValues.count(key))
		feedRate = sliceValuesIn->doubleValues.at(key);
	else
		feedRate = defaultSettings->doubleValues.at(key);
	//if (GET_BOOL("feedrateMMm"))
	//	feedRate *= 60.0;
	std::stringstream decimalFormatter;
	std::fixed(decimalFormatter);
	decimalFormatter.precision(0);
	decimalFormatter << feedRate;
	box->SetValue(decimalFormatter.str());
}

void RovaSettings::fillFeedrate(wxSpinCtrl *box, const std::string &key, const SettingValues *sliceValuesIn) const
{
	double feedRate;
	if (sliceValuesIn->doubleValues.count(key))
		feedRate = sliceValuesIn->doubleValues.at(key);
	else
		feedRate = defaultSettings->doubleValues.at(key);
	//if (GET_BOOL("feedrateMMm"))
	//	feedRate *= 60.0;
	std::stringstream decimalFormatter;
	std::fixed(decimalFormatter);
	decimalFormatter.precision(0);
	decimalFormatter << feedRate;
	box->SetValue(decimalFormatter.str());
}

double RovaSettings::readFeedrate(wxTextCtrl *box) const
{
	double feedRate = std::stod(box->GetValue().ToStdString());
	//if (GET_BOOL("feedrateMMm"))
	//	feedRate /= 60.0;
	return feedRate;
}

double RovaSettings::readFeedrate(wxSpinCtrl *box) const
{
	double feedRate = box->GetValue();
	//if (GET_BOOL("feedrateMMm"))
	//	feedRate /= 60.0;
	return feedRate;
}

void RovaSettings::populateBoxes(SettingValues *sliceValuesIn)
{
	//Fill slice values
	// Slice Settings
	// Layers and Perimeters
		fillDefaultSettings();
		fillNumber(m_layerThickness, "layerThickness", sliceValuesIn,4);
		fillNumber(m_firstLayerThickbess, "firstLayerThickness", sliceValuesIn,4);
		fillNumber(m_firstLayerExtMultiplier, "firsyLayerExtrusion", sliceValuesIn);
		fillNumber(m_BottomSolidLayers, "bottomSolidLayers", sliceValuesIn);
		fillNumber(m_topSolidLayers, "topSolidLayers", sliceValuesIn);
		fillComboBoxNumber(m_infillPercentage, "infillDensity", sliceValuesIn,"","%");
		fillNumber(m_InfillExtrusionMultiplier, "infillExtrusionMultiplier", sliceValuesIn);
		fillNumber(m_PerimeterNumber, "perimeterCount", sliceValuesIn);
		fillNumber(m_XYOffset, "XYOffset", sliceValuesIn,4);
	//Colours
		fillColour(m_T1Colour, "t1Colour", sliceValuesIn);
		fillColour(m_T2Colour, "t2Colour", sliceValuesIn);
		fillColour(m_CMYWhiteSub, "CMYWhiteSub", sliceValuesIn);
		fillCheck(m_RovaCompat, "rova5Compat", sliceValuesIn);
		fillNumber(m_ExtCount, "compatExtruderCount", sliceValuesIn);
		fillNumber(m_kMeansColours, "kMeansColourCount", sliceValuesIn);
		fillChoiceBoxString(m_headChoice, "headChoice", sliceValuesIn);
		fillNumber(m_mixerSpeed, "mixerSpeed", sliceValuesIn);
		fillCheck(m_stopMixerNotInUse, "mixerOnlyUse", sliceValuesIn);
	//Coaster printing
		fillNumber(m_coasterBase, "coasterBaseLayers", sliceValuesIn);
		fillColour(m_coasterBaseColour, "coasterBaseColour", sliceValuesIn);
		fillComboBoxNumber(m_baseOpacity, "baseOpacity", sliceValuesIn,"","%");
		fillComboBoxNumber(m_surfaceOpacity, "coasterSurfaceOpacity", sliceValuesIn, "", "%");
		fillNumber(m_surfaceLayers, "coasterSurfaceLayers", sliceValuesIn);
	// Printer settings
		fillNumber(m_t0Diameter, "T0nozzleD", sliceValuesIn,2);
		fillNumber(m_t0ExtWidth, "T0extrusionWidth", sliceValuesIn,2);
		fillNumber(m_t0MinExtWidth, "T0minExtrusionWidth", sliceValuesIn,2);
		fillNumber(m_t0FilamentD, "T0filamentD", sliceValuesIn,2);
		fillNumber(m_t0ExtMultiplier, "T0extMulti", sliceValuesIn, 2);

		fillNumber(m_t1Diameter, "T1nozzleD", sliceValuesIn, 2);
		fillNumber(m_t1ExtWidth, "T1extrusionWidth", sliceValuesIn, 2);
		fillNumber(m_t1MinExtWidth, "T1minExtrusionWidth", sliceValuesIn, 2);
		fillNumber(m_t1FilamentD, "T1filamentD", sliceValuesIn, 2);
		fillNumber(m_t1ExtMultiplier, "T1extMulti", sliceValuesIn, 2);

		fillNumber(m_t2Diameter, "T2nozzleD", sliceValuesIn, 2);
		fillNumber(m_t2ExtWidth, "T2extrusionWidth", sliceValuesIn, 2);
		fillNumber(m_t2MinExtWidth, "T2minExtrusionWidth", sliceValuesIn, 2);
		fillNumber(m_t2FilamentD, "T2filamentD", sliceValuesIn, 2);
		fillNumber(m_t2ExtMultiplier, "T2extMulti", sliceValuesIn, 2);


		fillCheck(m_purgeBinPrime, "primeToolPurgeBin", sliceValuesIn);
		fillNumber(m_purgePrimeDistance, "primeToolPurgeLength", sliceValuesIn);
		fillCheck(m_changeToolOverBin, "changeToolOverPurgeBin", sliceValuesIn);
		fillCheck(m_purgeOnEachToolChange, "purgeOnToolChange", sliceValuesIn);
		fillNumber(m_toolChangePurgeDistance, "toolChangePurgeDistance", sliceValuesIn);
		fillNumber(m_toolChangePurgeFeedrate, "toolChangePurgeFeedrate", sliceValuesIn);
		fillNumber(m_T0_Temp, "T0", sliceValuesIn,0);
		fillNumber(m_T1_TEMP, "T1", sliceValuesIn,0);
		fillNumber(m_T2_TEMP, "T2", sliceValuesIn,0);
		fillNumber(m_Bed_Temp, "bedTemp", sliceValuesIn,0);
		fillCheck(m_heatAndWait, "heatAndWait", sliceValuesIn);
		fillCheck(m_infillFirst, "infillBeforePerimeter", sliceValuesIn);
		fillCheck(m_insideOut, "perimetersInsideOut", sliceValuesIn);
		fillCheck(m_NoSequenceIslands, "printAllPerimsFirst", sliceValuesIn);
	//autocool
		fillCheck(m_useCoolingFan, "useCoolingFan", sliceValuesIn);
		fillNumber(m_firstLayerFanSpeed, "firstLayerFanSpeed", sliceValuesIn);
		fillCheck(m_fanAlwaysOn, "keepFanAlwaysOn", sliceValuesIn);
		fillNumber(m_minFanSpeed, "minFanSpeed", sliceValuesIn);
		fillNumber(m_fanSpeed, "maxFanSpeed", sliceValuesIn);
		fillCheck(m_reducePrintSpeed, "reducePrintSpeedSmallLayers", sliceValuesIn);
		fillNumber(m_minLayerTime, "minLayerTime", sliceValuesIn);
		fillNumber(m_minLayerTimeFan, "minLayerTimeFan", sliceValuesIn);
		fillFeedrate(m_minSlowDownFeedrate, "minSlowDownFeedrate", sliceValuesIn);
	//Support
		fillCheck(m_checkSupport, "useSupport", sliceValuesIn);
		fillNumber(m_EmptySupportLayers, "emptySupportLayers", sliceValuesIn);
		fillNumber(m_InterfaceCount, "supportInterfaceLayers", sliceValuesIn);
		fillNumber(m_baseMaterialInterface, "baseMaterialInterfaceLayers", sliceValuesIn);
		fillComboBoxNumber(m_SupportDensity, "supportInfillDensity", sliceValuesIn,"","%");
		fillComboBoxNumber(m_InterfaceDensity, "supportInfillDensityFull", sliceValuesIn, "", "%");
		fillColour(m_SupportColour, "supportColour", sliceValuesIn);
		fillCheck(m_OverrideBaseInterface, "overrideInterfaceMaterialBasePlate", sliceValuesIn);
		fillNumber(m_maxSupportAngle, "maxUnsupportedAngle", sliceValuesIn,1);
		fillNumber(m_SupportOffset, "supportOffsetFromPart", sliceValuesIn,4);
		fillChoiceBoxSelection(m_SupportExt, "mainSupportExtruder", sliceValuesIn);
		fillChoiceBoxSelection(m_InterfaceSupport, "supportInterfaceExtruder", sliceValuesIn);
		fillChoiceBoxSelection(m_BaseInterfaceTool, "baseMaterialInterfaceTool", sliceValuesIn);
		fillNumber(m_supportHoleArea, "minimumHoleAreaSupport", sliceValuesIn, 2);
		fillCheck(m_PerciseCutSupport, "preciseSupportCut", sliceValuesIn);
	// UI Settings
		fillCheck(m_simpleGCodeView, "simpleGCodePreview", sliceValuesIn);
		fillNumber(m_gridWidth, "maxX", sliceValuesIn,0);
		fillNumber(m_gridDepth, "maxY", sliceValuesIn,0);
		fillNumber(m_gridHeight, "maxZ", sliceValuesIn,0);
		fillNumber(m_gridSpacing, "gridSize", sliceValuesIn,0);
		fillCheck(m_ColourPreviewByType, "colourPreviewByType", sliceValuesIn);
		fillCheck(m_showAxis, "showAxis", sliceValuesIn);
	//Advanced Settings
		//Numbers
		fillNumber(m_minLineLength, "minimumLineLength", sliceValuesIn,4);
		fillNumber(m_endZ, "endZ", sliceValuesIn,4);
		fillNumber(m_infillOverlap, "infillOutlineOverlapPercentage", sliceValuesIn,2);
		fillNumber(m_topSurfaceInfill, "topSurfaceInfillDensity", sliceValuesIn,2);
		fillNumber(m_SurfaceInfillDensity, "surfaceInfillDensity", sliceValuesIn,2);
		fillNumber(m_BottomLayerInfill, "bottomLayerInfill", sliceValuesIn,2);
		fillNumber(m_mmPerPixel, "mmPerPixel", sliceValuesIn,2);
		fillNumber(m_ThreadCount, "threadCount", sliceValuesIn,0);
		// bool values
		fillCheck(m_ConsiderSurfaces, "considerSurfaces", sliceValuesIn);
		fillCheck(m_ColourPreviewByType, "colourPreviewByType", sliceValuesIn);
		fillCheck(m_savePerimsToTest, "savePerimetersToTestFile", sliceValuesIn);
	//Retraction
		fillCheck(m_fwRetraction, "useSoftwareRetraction", sliceValuesIn);
		fillCheck(m_retractLayerChange, "retractOnLayerChange", sliceValuesIn);
		fillCheck(m_retractInfillCrossPerimOnly, "retactInfillOnlyCrossingPerim", sliceValuesIn);
		fillCheck(m_onyRetractCrossingPerimeters, "retractOnlyPerimeterCrossing", sliceValuesIn);
		//fillCheck(m_fanOffRetract, "fanOffDuringRetract", sliceValuesIn);
		fillRetractionSettings(sliceValuesIn);

		fillCheck(m_useServo, "useServo", sliceValuesIn);
		fillNumber(m_ServoLift, "servoLift", sliceValuesIn,0);
		fillNumber(m_ServoIssue, "servoIssue", sliceValuesIn,0);
		fillNumber(m_syringeIssueTime, "servoIssueTime", sliceValuesIn);
		fillNumber(m_syringeLiftTime, "servoLiftTime", sliceValuesIn);
		fillNumber(m_syringeServoIndex, "syringeServoIndex", sliceValuesIn);
	// Custom GCode Scripts
		fillString(m_OnStart, "onStartGCode", sliceValuesIn);
		fillString(m_OnEndGCode, "onEndGCode", sliceValuesIn);
		fillString(m_BeforeZMove, "onBeforeZGCode", sliceValuesIn);
		fillString(m_OnAfterZMove, "onAfterZGCode", sliceValuesIn);
		fillString(m_OnRetractStrings, "onRetractGCode", sliceValuesIn);
		fillString(m_OnIssue, "onIssueGCode", sliceValuesIn);
	// Purge settings
		fillCheck(m_AlwaysPurge, "alwaysPurge", sliceValuesIn);
		fillCheck(m_useSoftwarePurge, "useSoftwarePurge", sliceValuesIn);
		fillCheck(m_purgeForceRetract, "purgeForceRetract", sliceValuesIn);
		fillNumber(m_purgeLength, "purgeLength", sliceValuesIn,2);
		fillNumber(m_purgeMixerSpeed, "purgeMixerSpeed", sliceValuesIn);
		fillNumber(m_purgeWait, "purgeWait", sliceValuesIn,0);
		fillNumber(m_purgeLocationX, "purgeLocationX", sliceValuesIn,2);
		fillNumber(m_purgeLocationY, "purgeLocationY", sliceValuesIn,2);
		fillNumber(m_purgeWidth, "purgeWidth", sliceValuesIn,2);
		fillNumber(m_purgeHeight, "purgeHeight", sliceValuesIn,2);
		fillNumber(m_purgwWipeX, "purgeWipeX", sliceValuesIn,2);
		fillNumber(m_purgeWipeWidth, "purgeWipeWidth", sliceValuesIn,2);
		fillFeedrate(m_purgeFeedrate, "purgeFeedrate", sliceValuesIn);
		fillCheck(m_fullPurgeBinPause, "pauseOnFullPurgeBin", sliceValuesIn);
		fillNumber(m_purgeBinCapacity, "purgeBinCapacity", sliceValuesIn,2);
		fillNumber(m_purgeWipeSpeed, "purgeWipeSpeed", sliceValuesIn, 0);
	// Colour lookahead
		fillCheck(m_LookAheadColourTrans, "lookAheadPurge", sliceValuesIn);
		fillCheck(m_purgeExcessColourChange, "purgeExcess", sliceValuesIn);
		fillNumber(m_DistanceToColourChange, "distanceBeforeTransition", sliceValuesIn,2);
		fillNumber(m_ColourChangeMix, "distanceOfTransition", sliceValuesIn,2);
	// Firmware Settings
		//fillCheck(m_useCheckSums, "useChecksums", sliceValuesIn);
		fillChoiceBoxString(m_firmwareChoice, "firmware", sliceValuesIn);

	// Speed
		fillFeedrate(m_firstLayerPrintSpeed, "firstLayerSpeed", sliceValuesIn);
		fillFeedrate(m_perimSpeed, "perimeterSpeed", sliceValuesIn);
		fillFeedrate(m_infillSpeed, "infillSpeed", sliceValuesIn);
		fillFeedrate(m_travelSpeed, "travelSpeed", sliceValuesIn);
	// Mixer Speed
		fillNumber(m_firstLayerMixerSpeed, "firstLayerMixerSpeed", sliceValuesIn, 1);
		fillNumber(m_PerimeterMixerSpeed, "perimeterMixerSpeed", sliceValuesIn, 1);
		fillNumber(m_InfillMixerSpeed, "infillMixerSpeed", sliceValuesIn, 1);
		fillNumber(m_TravelMixerSpeed, "travelMixerSpeed", sliceValuesIn, 1);
	// Filament Settings
		fillNumber(m_colourCost, "colourMaterialCost", sliceValuesIn, 2);
		fillNumber(m_colourDiameter, "colourDiameter", sliceValuesIn, 2);
		fillNumber(m_colourDensity, "colourDensity", sliceValuesIn, 2);

		fillNumber(m_supportCost, "supportMaterialCost", sliceValuesIn, 2);
		fillNumber(m_supportDiameter, "supportDiameter", sliceValuesIn, 2);
		fillNumber(m_supportDensity, "supportDensity", sliceValuesIn, 2);

		fillNumber(m_flexCost, "flexMaterialCost", sliceValuesIn, 2);
		fillNumber(m_flexDiameter, "flexDiameter", sliceValuesIn, 2);
		fillNumber(m_flexDensity, "flexDensity", sliceValuesIn, 2);
	// Suspend
		fillCheck(m_EnableGCodeSplitUp, "enableGCodeSplitup", sliceValuesIn);
		fillCheck(m_PuasePurgeBin, "travelToPurgeOnSuspend", sliceValuesIn);
		fillCheck(m_purgeResume, "purgeOnResume", sliceValuesIn);
		fillNumber(m_GCodeFileTime, "gcodeSuspendTime", sliceValuesIn, 2);
		fillString(m_OnSuspendStrings, "onSuspend", sliceValuesIn);
		fillString(m_OnResumeStrings, "onResume", sliceValuesIn);
	// Trigger these checks
	wxCommandEvent fakeEvent;
	OverrideBaseInterfaceCheck(fakeEvent);
	wxSpinEvent fakeSpin;
	UpdateInterfaceCount(fakeSpin);
	SwitchRovaCompat(fakeEvent);
	kMeansCheck(fakeEvent);
	travisSimplificationCheck(fakeEvent);
	ShowHideExperimental(fakeEvent);
	onPurgeSoftwareToggle(fakeEvent);
	OnUsePurgeBinPrime(fakeEvent);
	OnHeadChoice(fakeEvent);
	OnCheckPauseFullBin(fakeEvent);
	OnUseCoolingFanCheck(fakeEvent);
	reducePrintSpeedOnSmallLayers(fakeEvent);
	changesNeedingSaving = false;
	//m_sdbSizer1Apply->Enable(false);
}

void RovaSettings::LoadDefaultSettings(wxCommandEvent& event)
{
	DefaultSettingsDialog* dialog = new DefaultSettingsDialog((wxWindow*)this); //switch to RAII
	int result = dialog->ShowModal();
	if (result != wxID_OK)//if (result == wxCANCEL)
	{
		delete dialog;
		return;
	}
	else
	{
		int selection = dialog->m_hardwareChoice->GetSelection();
		if (selection != wxNOT_FOUND)
		{
			//Load settings .ini based on name
			std::string settingsFile = dialog->m_hardwareChoice->GetString(selection) + ".ini";
			//wxMessageBox("Loading settings: DefaultSettings\\" + settingsFile);
			SettingValues newDefaults;
			if (newDefaults.loadFrom("DefaultSettings\\" + settingsFile))
			{
				populateBoxes(&newDefaults);
			}
		}
		else
		{
			fillDefaultSettings();

			populateBoxes(defaultSettings);
		}
	}
}

void RovaSettings::boxesToSettingValues(SettingValues *sliceValuesOut)
{
	sliceValuesOut->doubleValues["layerThickness"] = std::stod(m_layerThickness->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["firstLayerThickness"] = std::stod(m_firstLayerThickbess->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["firsyLayerExtrusion"] = m_firstLayerExtMultiplier->GetValue();
	sliceValuesOut->doubleValues["bottomSolidLayers"] = m_BottomSolidLayers->GetValue();
	sliceValuesOut->doubleValues["topSolidLayers"] = m_topSolidLayers->GetValue();
	sliceValuesOut->doubleValues["infillDensity"] = std::stod(m_infillPercentage->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["infillExtrusionMultiplier"] = m_InfillExtrusionMultiplier->GetValue();
	sliceValuesOut->doubleValues["perimeterCount"] = m_PerimeterNumber->GetValue();
	sliceValuesOut->doubleValues["XYOffset"] = std::stod(m_XYOffset->GetValue().ToStdWstring());

	sliceValuesOut->boolValues["infillBeforePerimeter"] = m_infillFirst->GetValue();
	sliceValuesOut->boolValues["perimetersInsideOut"] = m_insideOut->GetValue();
	sliceValuesOut->boolValues["printAllPerimsFirst"] = m_NoSequenceIslands->GetValue();


	sliceValuesOut->colourValues["t1Colour"] = Colour(m_T1Colour->GetColour().Red(),
		m_T1Colour->GetColour().Green(),
		m_T1Colour->GetColour().Blue());
	sliceValuesOut->colourValues["t2Colour"] = Colour(m_T2Colour->GetColour().Red(),
		m_T2Colour->GetColour().Green(),
		m_T2Colour->GetColour().Blue());
	// Support
	sliceValuesOut->doubleValues["emptySupportLayers"] = m_EmptySupportLayers->GetValue();
	sliceValuesOut->doubleValues["supportInterfaceLayers"] = m_InterfaceCount->GetValue();
	sliceValuesOut->doubleValues["baseMaterialInterfaceLayers"] = m_baseMaterialInterface->GetValue();
	sliceValuesOut->boolValues["useSupport"] = m_checkSupport->GetValue();
	sliceValuesOut->doubleValues["supportInfillDensity"] = std::stod(m_SupportDensity->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["supportInfillDensityFull"] = std::stod(m_InterfaceDensity->GetValue().ToStdWstring());
	sliceValuesOut->colourValues["supportColour"] = Colour(m_SupportColour->GetColour().Red(),
		m_SupportColour->GetColour().Green(),
		m_SupportColour->GetColour().Blue());
	sliceValuesOut->boolValues["overrideInterfaceMaterialBasePlate"] = m_OverrideBaseInterface->GetValue();
	sliceValuesOut->doubleValues["maxUnsupportedAngle"] = std::stod(m_maxSupportAngle->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["supportOffsetFromPart"] = std::stod(m_SupportOffset->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["mainSupportExtruder"] = m_SupportExt->GetSelection();
	sliceValuesOut->doubleValues["supportInterfaceExtruder"] = m_InterfaceSupport->GetSelection();
	sliceValuesOut->doubleValues["baseMaterialInterfaceTool"] = m_BaseInterfaceTool->GetSelection();
	sliceValuesOut->doubleValues["minimumHoleAreaSupport"] = std::stod(m_supportHoleArea->GetValue().ToStdString());
	sliceValuesOut->boolValues["preciseSupportCut"] = m_PerciseCutSupport->GetValue();

	sliceValuesOut->boolValues["rova5Compat"] = m_RovaCompat->GetValue();
	sliceValuesOut->doubleValues["compatExtruderCount"] = m_ExtCount->GetValue();
	sliceValuesOut->stringValues["headChoice"] = m_headChoice->GetString(m_headChoice->GetSelection());
	sliceValuesOut->doubleValues["mixerSpeed"] = m_mixerSpeed->GetValue();
	sliceValuesOut->boolValues["mixerOnlyUse"] = m_stopMixerNotInUse->GetValue();
	sliceValuesOut->colourValues["CMYWhiteSub"] = Colour(m_CMYWhiteSub->GetColour().Red(), m_CMYWhiteSub->GetColour().Green(), m_CMYWhiteSub->GetColour().Blue());
	sliceValuesOut->doubleValues["kMeansColourCount"] = m_kMeansColours->GetValue();
	sliceValuesOut->doubleValues["coasterBaseLayers"] = m_coasterBase->GetValue();
	sliceValuesOut->colourValues["coasterBaseColour"] = Colour(m_coasterBaseColour->GetColour().Red(),
		m_coasterBaseColour->GetColour().Green(),
		m_coasterBaseColour->GetColour().Blue());
	sliceValuesOut->doubleValues["baseOpacity"] = std::stod(m_baseOpacity->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["coasterSurfaceLayers"] = m_surfaceLayers->GetValue();
	sliceValuesOut->doubleValues["coasterSurfaceOpacity"] = std::stod(m_surfaceOpacity->GetValue().ToStdWstring());

	sliceValuesOut->doubleValues["T0nozzleD"] = std::stod(m_t0Diameter->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T0extrusionWidth"] = std::stod(m_t0ExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T0minExtrusionWidth"] = std::stod(m_t0MinExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T0filamentD"] = std::stod(m_t0FilamentD->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T0extMulti"] = std::stod(m_t0ExtMultiplier->GetValue().ToStdWstring());
	//
	sliceValuesOut->doubleValues["T1nozzleD"] = std::stod(m_t1Diameter->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T1extrusionWidth"] = std::stod(m_t1ExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T1minExtrusionWidth"] = std::stod(m_t1MinExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T1filamentD"] = std::stod(m_t1FilamentD->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T1extMulti"] = std::stod(m_t1ExtMultiplier->GetValue().ToStdWstring());
	//
	sliceValuesOut->doubleValues["T2nozzleD"] = std::stod(m_t2Diameter->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T2extrusionWidth"] = std::stod(m_t2ExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T2minExtrusionWidth"] = std::stod(m_t2MinExtWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T2filamentD"] = std::stod(m_t2FilamentD->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T2extMulti"] = std::stod(m_t2ExtMultiplier->GetValue().ToStdWstring());

	sliceValuesOut->boolValues["primeToolPurgeBin"] = m_purgeBinPrime->GetValue();
	sliceValuesOut->doubleValues["primeToolPurgeLength"] = std::stod(m_purgePrimeDistance->GetValue().ToStdString());
	sliceValuesOut->boolValues["purgeOnToolChange"] = m_purgeOnEachToolChange->GetValue();
	sliceValuesOut->boolValues["changeToolOverPurgeBin"] = m_changeToolOverBin->GetValue();
	sliceValuesOut->doubleValues["toolChangePurgeDistance"] = std::stod(m_toolChangePurgeDistance->GetValue().ToStdString());
	sliceValuesOut->doubleValues["toolChangePurgeFeedrate"] = std::stod(m_toolChangePurgeFeedrate->GetValue().ToStdString());


	sliceValuesOut->doubleValues["T0"] = std::stoi(m_T0_Temp->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T1"] = std::stoi(m_T1_TEMP->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["T2"] = std::stoi(m_T2_TEMP->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["bedTemp"] = std::stoi(m_Bed_Temp->GetValue().ToStdWstring());
	sliceValuesOut->boolValues["heatAndWait"] = m_heatAndWait->GetValue();

	//sliceValuesOut->doubleValues["skirtLoops"] = m_SkirtLoops->GetValue();
	//sliceValuesOut->doubleValues["skirtOffset"] = std::stod(m_SkirtOffset->GetValue().ToStdString());

	//autocool
	sliceValuesOut->boolValues["useCoolingFan"] = m_useCoolingFan->GetValue();
	sliceValuesOut->doubleValues["firstLayerFanSpeed"] = m_firstLayerFanSpeed->GetValue();
	sliceValuesOut->boolValues["keepFanAlwaysOn"] = m_fanAlwaysOn->GetValue();
	sliceValuesOut->doubleValues["minFanSpeed"] = m_minFanSpeed->GetValue();
	sliceValuesOut->doubleValues["maxFanSpeed"] = m_fanSpeed->GetValue();
	sliceValuesOut->boolValues["reducePrintSpeedSmallLayers"] = m_reducePrintSpeed->GetValue();
	sliceValuesOut->doubleValues["minLayerTime"] = m_minLayerTime->GetValue();
	sliceValuesOut->doubleValues["minLayerTimeFan"] = m_minLayerTimeFan->GetValue();
	sliceValuesOut->doubleValues["minSlowDownFeedrate"] = readFeedrate(m_minSlowDownFeedrate);

	sliceValuesOut->boolValues["simpleGCodePreview"] = m_simpleGCodeView->GetValue();
	sliceValuesOut->boolValues["colourPreviewByType"] = m_ColourPreviewByType->GetValue();
	sliceValuesOut->doubleValues["maxX"] = std::stod(m_gridWidth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["maxY"] = std::stod(m_gridDepth->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["maxZ"] = std::stod(m_gridHeight->GetValue().ToStdWstring());
	sliceValuesOut->doubleValues["gridSize"] = std::stod(m_gridSpacing->GetValue().ToStdWstring());
	sliceValuesOut->boolValues["showAxis"] = m_showAxis->GetValue();

	//Experimental Values
	sliceValuesOut->doubleValues["minimumLineLength"] = std::stod(m_minLineLength->GetValue().ToStdString());
	sliceValuesOut->doubleValues["endZ"] = std::stod(m_endZ->GetValue().ToStdString());
	sliceValuesOut->doubleValues["infillOutlineOverlapPercentage"] = std::stod(m_infillOverlap->GetValue().ToStdString());
	sliceValuesOut->doubleValues["topSurfaceInfillDensity"] = std::stod(m_topSurfaceInfill->GetValue().ToStdString());
	sliceValuesOut->doubleValues["surfaceInfillDensity"] = std::stod(m_SurfaceInfillDensity->GetValue().ToStdString());
	sliceValuesOut->doubleValues["bottomLayerInfill"] = std::stod(m_BottomLayerInfill->GetValue().ToStdString());
	sliceValuesOut->doubleValues["mmPerPixel"] = std::stod(m_mmPerPixel->GetValue().ToStdString());
	sliceValuesOut->doubleValues["threadCount"] = std::stod(m_ThreadCount->GetValue().ToStdString());

	//sliceValuesOut->boolValues["useCrazyInfill"] = m_CrazyInfillCheck->GetValue();
	sliceValuesOut->boolValues["considerSurfaces"] = m_ConsiderSurfaces->GetValue();
	//sliceValuesOut->boolValues["useInfill"] = m_InfillCheckbox->GetValue();

	//Retraction
	sliceValuesOut->boolValues["useSoftwareRetraction"] = m_fwRetraction->GetValue();
	sliceValuesOut->boolValues["retractOnLayerChange"] = m_retractLayerChange->GetValue();
	sliceValuesOut->boolValues["retactInfillOnlyCrossingPerim"] = m_retractInfillCrossPerimOnly->GetValue();
	sliceValuesOut->boolValues["retractOnlyPerimeterCrossing"] = m_onyRetractCrossingPerimeters->GetValue();
	//sliceValuesOut->boolValues["fanOffDuringRetract"] = m_fanOffRetract->GetValue();
	saveRetractionSettings(sliceValuesOut);

	// Servo
	sliceValuesOut->doubleValues["servoLift"] = std::stod(m_ServoLift->GetValue().ToStdString());
	sliceValuesOut->doubleValues["servoIssue"] = std::stod(m_ServoIssue->GetValue().ToStdString());
	sliceValuesOut->boolValues["useServo"] = m_useServo->GetValue();
	sliceValuesOut->doubleValues["servoLiftTime"] = m_syringeLiftTime->GetValue();
	sliceValuesOut->doubleValues["servoIssueTime"] = m_syringeIssueTime->GetValue();
	sliceValuesOut->doubleValues["syringeServoIndex"] = m_syringeServoIndex->GetValue();

	// Custom GCode Scripts
	sliceValuesOut->stringValues["onStartGCode"] = m_OnStart->GetValue();
	sliceValuesOut->stringValues["onEndGCode"] = m_OnEndGCode->GetValue();
	sliceValuesOut->stringValues["onBeforeZGCode"] = m_BeforeZMove->GetValue();
	sliceValuesOut->stringValues["onAfterZGCode"] = m_OnAfterZMove->GetValue();
	sliceValuesOut->stringValues["onRetractGCode"] = m_OnRetractStrings->GetValue();
	sliceValuesOut->stringValues["onIssueGCode"] = m_OnIssue->GetValue();

	// Purge settings
	sliceValuesOut->boolValues["alwaysPurge"] = m_AlwaysPurge->GetValue();
	sliceValuesOut->boolValues["useSoftwarePurge"] = m_useSoftwarePurge->GetValue();
	sliceValuesOut->boolValues["purgeForceRetract"] = m_purgeForceRetract->GetValue();
	sliceValuesOut->doubleValues["purgeLength"] = std::stod(m_purgeLength->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeMixerSpeed"] = m_purgeMixerSpeed->GetValue();
	sliceValuesOut->doubleValues["purgeWait"] = std::stod(m_purgeWait->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeLocationX"] = std::stod(m_purgeLocationX->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeLocationY"] = std::stod(m_purgeLocationY->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeWidth"] = std::stod(m_purgeWidth->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeHeight"] = std::stod(m_purgeHeight->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeWipeX"] = std::stod(m_purgwWipeX->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeWipeWidth"] = std::stod(m_purgeWipeWidth->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeFeedrate"] = readFeedrate(m_purgeFeedrate);
	sliceValuesOut->boolValues["pauseOnFullPurgeBin"] = m_fullPurgeBinPause->GetValue();
	sliceValuesOut->doubleValues["purgeBinCapacity"] = std::stod(m_purgeBinCapacity->GetValue().ToStdString());
	sliceValuesOut->doubleValues["purgeWipeSpeed"] = readFeedrate(m_purgeWipeSpeed);
	// Colour lookahead
	sliceValuesOut->boolValues["lookAheadPurge"] = m_LookAheadColourTrans->GetValue();
	sliceValuesOut->boolValues["purgeExcess"] = m_purgeExcessColourChange->GetValue();
	sliceValuesOut->doubleValues["distanceBeforeTransition"] = std::stod(m_DistanceToColourChange->GetValue().ToStdString());
	sliceValuesOut->doubleValues["distanceOfTransition"] = std::stod(m_ColourChangeMix->GetValue().ToStdString());

	// Firmware
	//sliceValuesOut->boolValues["useChecksums"] = m_useCheckSums->GetValue();
	sliceValuesOut->stringValues["firmware"] = m_firmwareChoice->GetStringSelection();

	// Speed
	sliceValuesOut->doubleValues["firstLayerSpeed"] = readFeedrate(m_firstLayerPrintSpeed);
	sliceValuesOut->doubleValues["perimeterSpeed"] = readFeedrate(m_perimSpeed);
	sliceValuesOut->doubleValues["infillSpeed"] = readFeedrate(m_infillSpeed);
	sliceValuesOut->doubleValues["travelSpeed"] = readFeedrate(m_travelSpeed);
	//Mixer
	sliceValuesOut->doubleValues["firstLayerMixerSpeed"] = std::stod(m_firstLayerMixerSpeed->GetValue().ToStdString());
	sliceValuesOut->doubleValues["perimeterMixerSpeed"] = std::stod(m_PerimeterMixerSpeed->GetValue().ToStdString());
	sliceValuesOut->doubleValues["infillMixerSpeed"] = std::stod(m_InfillMixerSpeed->GetValue().ToStdString());
	sliceValuesOut->doubleValues["travelMixerSpeed"] = std::stod(m_TravelMixerSpeed->GetValue().ToStdString());


	// Filament
	// Colour
	sliceValuesOut->doubleValues["colourMaterialCost"] = std::stod(m_colourCost->GetValue().ToStdString());
	sliceValuesOut->doubleValues["colourDiameter"] = std::stod(m_colourDiameter->GetValue().ToStdString());
	sliceValuesOut->doubleValues["colourDensity"] = std::stod(m_colourDensity->GetValue().ToStdString());
	// Support
	sliceValuesOut->doubleValues["supportMaterialCost"] = std::stod(m_supportCost->GetValue().ToStdString());
	sliceValuesOut->doubleValues["supportDiameter"] = std::stod(m_supportDiameter->GetValue().ToStdString());
	sliceValuesOut->doubleValues["supportDensity"] = std::stod(m_supportDensity->GetValue().ToStdString());
	// Flex
	sliceValuesOut->doubleValues["flexMaterialCost"] = std::stod(m_flexCost->GetValue().ToStdString());
	sliceValuesOut->doubleValues["flexDiameter"] = std::stod(m_flexDiameter->GetValue().ToStdString());
	sliceValuesOut->doubleValues["flexDensity"] = std::stod(m_flexDensity->GetValue().ToStdString());
	// Suspend
	sliceValuesOut->boolValues["enableGCodeSplitup"] = m_EnableGCodeSplitUp->GetValue();
	sliceValuesOut->boolValues["travelToPurgeOnSuspend"] = m_PuasePurgeBin->GetValue();
	sliceValuesOut->boolValues["purgeOnResume"] = m_purgeResume->GetValue();
	sliceValuesOut->doubleValues["gcodeSuspendTime"] = std::stod(m_GCodeFileTime->GetValue().ToStdString());
	sliceValuesOut->stringValues["onSuspend"] = m_OnSuspendStrings->GetValue();
	sliceValuesOut->stringValues["onResume"] = m_OnResumeStrings->GetValue();
}

void RovaSettings::OverrideBaseInterfaceCheck(wxCommandEvent& event)
{
	m_BaseInterfaceTool->Enable(m_OverrideBaseInterface->GetValue());
	changesNeedingSaving = true;
}

void RovaSettings::UpdateInterfaceCount(wxSpinEvent& event)
{
	m_InterfaceDensity->Enable((m_InterfaceCount->GetValue() > 0));
	changesNeedingSaving = true;
}

void RovaSettings::saveSettings()
{
	boxesToSettingValues(sliceValues);
	changesNeedingSaving = false;
}

void RovaSettings::ShowHideExperimental(wxCommandEvent& event)
{
	toggleExperimental(m_checkExperimental->GetValue());
}

void RovaSettings::toggleExperimental(bool isShow)
{
	m_AdvancedPanel->Show(isShow);
}

void RovaSettings::OnHeadChoice(wxCommandEvent& event)
{
	if (m_headChoice->IsEnabled() && m_headChoice->GetStringSelection() == "RoVa4D - CMYK+WT")
	{
		m_mixerSpeed->Enable(true);
		m_stopMixerNotInUse->Enable(true);
		m_useServo->Enable(true);
		m_CMYWhiteSub->Enable(false);
	}
	else
	{
		if (m_headChoice->GetStringSelection() == "Diamond Hotend - CMY")
		{
			m_CMYWhiteSub->Enable(true);
		}
		else
			m_CMYWhiteSub->Enable(false);
		m_mixerSpeed->Enable(false);
		m_stopMixerNotInUse->Enable(false);
		m_useServo->SetValue(false);
		m_useServo->Enable(false);
	}
	OnUseSyringeLift(event);
}

void RovaSettings::CMYWhiteSub(wxColourPickerEvent& event)
{
	if (m_CMYWhiteSub->GetColour().Red() == 255 &&
		m_CMYWhiteSub->GetColour().Green() == 255 &&
		m_CMYWhiteSub->GetColour().Blue() == 255)
	{
#ifndef CONSOLE
		wxMessageBox("CMY White substition colour must be a colour other than white");
#endif
		m_CMYWhiteSub->SetColour(wxColor(255, 255, 0));
	}
}

void RovaSettings::OnUseSyringeLift(wxCommandEvent& event)
{
	m_syringeServoIndex->Enable(m_useServo->GetValue());
	m_ServoIssue->Enable(m_useServo->GetValue());
	m_ServoLift->Enable(m_useServo->GetValue());
	m_syringeLiftTime->Enable(m_useServo->GetValue());
	m_syringeIssueTime->Enable(m_useServo->GetValue());
}

void RovaSettings::OnCheckPauseFullBin(wxCommandEvent& event)
{
	m_purgeBinCapacity->Enable(m_fullPurgeBinPause->GetValue());
}

void RovaSettings::saveRetractionSettings(SettingValues *sliceValuesOut)
{
	{
		std::string toolSelected = "T" + std::to_string(0);
		sliceValuesOut->doubleValues["zLiftFeed" + toolSelected] = readFeedrate(m_ZLiftFeed0);
		sliceValuesOut->doubleValues["zLift" + toolSelected] = std::stod(m_ZLift0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["travelDistanceWithoutRetract" + toolSelected] = std::stod(m_TravelWithoutRetract0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["coastLength" + toolSelected] = std::stod(m_CoastLength0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractAmount" + toolSelected] = std::stod(m_RetractAmt0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["issueAmount" + toolSelected] = std::stod(m_IssueAmt0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractFeedrate" + toolSelected] = readFeedrate(m_RetractFeed0);
		sliceValuesOut->doubleValues["issueFeedrate" + toolSelected] = readFeedrate(m_Issue0);
		sliceValuesOut->doubleValues["wipeDistance" + toolSelected] = std::stod(m_Wipe0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["wipeFeedrate" + toolSelected] = readFeedrate(m_wipeFeedrate0);
		sliceValuesOut->doubleValues["toolChangeRetractDistance" + toolSelected] = std::stod(m_ToolChangeRetractT0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["toolChangeIssueDistance" + toolSelected] = std::stod(m_toolChangeIssueT0->GetValue().ToStdString());
		sliceValuesOut->doubleValues["PrintDistanceBeforeRetract" + toolSelected] = std::stod(m_PrintDistanceRetractT0->GetValue().ToStdString());
	}
	{
		std::string toolSelected = "T" + std::to_string(1);
		sliceValuesOut->doubleValues["zLiftFeed" + toolSelected] = readFeedrate(m_ZLiftFeed1);
		sliceValuesOut->doubleValues["zLift" + toolSelected] = std::stod(m_ZLift1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["travelDistanceWithoutRetract" + toolSelected] = std::stod(m_TravelWithoutRetract1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["coastLength" + toolSelected] = std::stod(m_CoastLength1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractAmount" + toolSelected] = std::stod(m_RetractAmt1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["issueAmount" + toolSelected] = std::stod(m_IssueAmt1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractFeedrate" + toolSelected] = readFeedrate(m_RetractFeed1);
		sliceValuesOut->doubleValues["issueFeedrate" + toolSelected] = readFeedrate(m_Issue1);
		sliceValuesOut->doubleValues["wipeDistance" + toolSelected] = std::stod(m_Wipe1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["wipeFeedrate" + toolSelected] = readFeedrate(m_wipeFeedrate1);
		sliceValuesOut->doubleValues["toolChangeRetractDistance" + toolSelected] = std::stod(m_ToolChangeRetractT1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["toolChangeIssueDistance" + toolSelected] = std::stod(m_toolChangeIssueT1->GetValue().ToStdString());
		sliceValuesOut->doubleValues["PrintDistanceBeforeRetract" + toolSelected] = std::stod(m_PrintDistanceRetractT1->GetValue().ToStdString());
	}
	{
		std::string toolSelected = "T" + std::to_string(2);
		sliceValuesOut->doubleValues["zLiftFeed" + toolSelected] = readFeedrate(m_ZLiftFeed2);
		sliceValuesOut->doubleValues["zLift" + toolSelected] = std::stod(m_ZLift2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["travelDistanceWithoutRetract" + toolSelected] = std::stod(m_TravelWithoutRetract2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["coastLength" + toolSelected] = std::stod(m_CoastLength2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractAmount" + toolSelected] = std::stod(m_RetractAmt2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["issueAmount" + toolSelected] = std::stod(m_IssueAmt2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["retractFeedrate" + toolSelected] = readFeedrate(m_RetractFeed2);
		sliceValuesOut->doubleValues["issueFeedrate" + toolSelected] = readFeedrate(m_Issue2);
		sliceValuesOut->doubleValues["wipeDistance" + toolSelected] = std::stod(m_Wipe2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["wipeFeedrate" + toolSelected] = readFeedrate(m_wipeFeedrate2);
		sliceValuesOut->doubleValues["toolChangeRetractDistance" + toolSelected] = std::stod(m_ToolChangeRetractT2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["toolChangeIssueDistance" + toolSelected] = std::stod(m_toolChangeIssueT2->GetValue().ToStdString());
		sliceValuesOut->doubleValues["PrintDistanceBeforeRetract" + toolSelected] = std::stod(m_PrintDistanceRetractT2->GetValue().ToStdString());
	}
}

void RovaSettings::fillRetractionSettings(SettingValues *sliceValuesIn)
{
	{
		std::string toolSelected = "T" + std::to_string(0);
		fillNumber(m_ZLift0,				"zLift" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_ZLiftFeed0,			"zLiftFeed" + toolSelected, sliceValuesIn);
		fillNumber(m_TravelWithoutRetract0, "travelDistanceWithoutRetract" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_CoastLength0,			"coastLength" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_RetractAmt0,			"retractAmount" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_IssueAmt0,				"issueAmount" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_RetractFeed0,		"retractFeedrate" + toolSelected, sliceValuesIn);
		fillFeedrate(m_Issue0,				"issueFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_Wipe0,					"wipeDistance" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_wipeFeedrate0,		"wipeFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_ToolChangeRetractT0,	"toolChangeRetractDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_toolChangeIssueT0,		"toolChangeIssueDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_PrintDistanceRetractT0, "PrintDistanceBeforeRetract" + toolSelected, sliceValuesIn, 2);
	}
	{
		std::string toolSelected = "T" + std::to_string(1);
		fillNumber(m_ZLift1,				"zLift" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_ZLiftFeed1,			"zLiftFeed" + toolSelected, sliceValuesIn);
		fillNumber(m_TravelWithoutRetract1, "travelDistanceWithoutRetract" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_CoastLength1,			"coastLength" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_RetractAmt1,			"retractAmount" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_IssueAmt1,				"issueAmount" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_RetractFeed1,		"retractFeedrate" + toolSelected, sliceValuesIn);
		fillFeedrate(m_Issue1,				"issueFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_Wipe1,					"wipeDistance" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_wipeFeedrate1,		"wipeFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_ToolChangeRetractT1,	"toolChangeRetractDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_toolChangeIssueT1,		"toolChangeIssueDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_PrintDistanceRetractT1, "PrintDistanceBeforeRetract" + toolSelected, sliceValuesIn, 2);
	}
	{
		std::string toolSelected = "T" + std::to_string(2);
		fillNumber(m_ZLift2,				"zLift" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_ZLiftFeed2,			"zLiftFeed" + toolSelected, sliceValuesIn);
		fillNumber(m_TravelWithoutRetract2, "travelDistanceWithoutRetract" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_CoastLength2,			"coastLength" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_RetractAmt2,			"retractAmount" + toolSelected, sliceValuesIn, 2);
		fillNumber(m_IssueAmt2,				"issueAmount" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_RetractFeed2,		"retractFeedrate" + toolSelected, sliceValuesIn);
		fillFeedrate(m_Issue2,				"issueFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_Wipe2,					"wipeDistance" + toolSelected, sliceValuesIn, 2);
		fillFeedrate(m_wipeFeedrate2,		"wipeFeedrate" + toolSelected, sliceValuesIn);
		fillNumber(m_ToolChangeRetractT2,	"toolChangeRetractDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_toolChangeIssueT2,		"toolChangeIssueDistance" + toolSelected, sliceValuesIn,2);
		fillNumber(m_PrintDistanceRetractT2, "PrintDistanceBeforeRetract" + toolSelected, sliceValuesIn, 2);
	}
}

