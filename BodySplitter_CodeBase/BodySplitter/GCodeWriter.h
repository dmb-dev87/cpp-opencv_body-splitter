#pragma once
#include <fstream>
#include <memory>
#include <vector>

#include "Vertex.h"
#include "colour.h"
#include "PrintTimeEstimator.h"
#include "FilamentUsageCounter.h"
#include "iSlicable.h"
#include "iDrawable.h"
#include <boost/circular_buffer.hpp>

/* This class existing to turn the set of lines from a slice into a gcode file*/

#define GCODE_PRECISION_XY 3
#define GCODE_PRECISION_E 5
#define GCODE_PRECISION_F 0

class machineControl;
class iSlicable;
class GCodeLine;
class Line;
class ExRSPolygon;
class Infill;
class InnerPerims;
class Layer;
class SupportManager;

typedef std::pair<std::string, std::shared_ptr<GCodeLine>> linePair;

class GCodeWriter
{
public:
	using PolyWithOffset = std::pair<Vertex2D, std::shared_ptr<ExRSPolygon>>;
private:
	const uint16_t COLOUR_TOOL = 0;
	enum class RETRACTION_TYPE { TRAVEL_MOVE = 0, TOOL_CHANGE, PAUSE, DONTWIPE, NO_RETRACTION};
	enum class AXIS { X = 0, Y, Z, E };
	PrintTimeEstimator timeEstimate;
	FilamentUsageCounter filamentUsageEstimate;
	unsigned int m_currentLayer;

	std::set<uint16_t> toolsUsed;

	static const double MAX_E_RESET;

	std::list<std::shared_ptr<iSlicable>> slicables;
	unsigned int maxLayers = 0;
	//For coasting
	//std::vector<std::pair<std::string,Line>> linesToWrite;
	//Write the lines that will not need to be coasted
	void writeLinesWaiting();
	void writeAllBufferedLines();
	void writeFormattedLine(const std::shared_ptr< GCodeLine >& line);
	//void mergeColinearLines();
	//Coast command issued, modify the lines and write the coast

	//For wiping
	//boost::circular_buffer<std::pair<bool,Line>> lastLines;
	//void bufferCommand(const std::string &command);
	std::ofstream outputStream;
	void writeStartGCode();
	void writeEndGCode();
	void writeLayerCode(double z);
	void writeStartTemperatures();

	void writeMixerSpeed(uint16_t mixerSpeed, bool isPurge = false, std::string comment = "");
	void writeFanOff();
	void writeFanSpeed(double fanSpeed); //in percentage

	void writeExtrudeTo(double x, double y, double z, double e_multiplier, double extWidth, std::shared_ptr<Layer> layer, double f=-1);
	// Get The E-length of an extrude move
	double getExtrudeLengthTo(double x1, double y1, double x2, double y2, double z1, double z2, double e_multipler, double extWidth, std::shared_ptr<Layer> layer) const;
	void writeTravelTo(double x, double y, double z, double f = -1, bool dontRetract = false, unsigned int lineType = 0);
	void writeTravelXY(double x, double y, double f = -1, bool dontRetract = false, unsigned int lineType = 0);
	void writeZTravelTo(double z, double f = -1);
	// Used to move fore and aft in the purge bucket
	void writeManualMove(double x, double y, double z, double e, double f = -1);

	void writePause(uint32_t milliseconds, std::string comment = "");

	//Retraction
	void writeRetract(RETRACTION_TYPE retractType);
	void writeManualIssue(double IssueAmt, double FeedRate); //change by CG on Aug 5th
	void writeIssue();
	void writeResetE();
	std::deque<GCodeLine> getWipePath();
	void writeCoast();

	void writeColour(const Colour &in);
	void writeSoftwareColourPurge(const Colour &in);
	// Used for lookahead purge - purgeAmt is remaining filament to purge
	void writePurge(double purgeAmt, double targetX, double targetY);
	//Used for retracting all mixing extruders equally
	void writeColourEven();
	void writeColourReturn();
	/* Lookahead purge - This sets the colour change back in already processed GCode, returns the amt it cant...*/
	double writeInlineColourChange(const Colour &in, double distanceBack);
	double writeInlineColourChange(uint16_t toolID, double distanceBack);
	double writeInlineColourChange(const std::string &colourCmd, double distanceBack);
	/* Find the amount of infill upcoming that we can use for colour trans zone*/
	double getInfillLength(std::vector<PolyWithOffset> islands, std::shared_ptr<Layer> layer, bool includeFullInfill) const;
	double getInfillLength(std::shared_ptr<ExRSPolygon> poly, std::shared_ptr<Layer> layer, bool includeFullInfill) const;
	void writeComment(const std::string &comment);

	void scaleBufferedLayer(double scaleAmt);


	void writePoly(std::shared_ptr<ExRSPolygon> &in, std::shared_ptr<Layer> layer, bool infillAndPerims, bool infillPass, bool fullInfill);
	// Write the poly but either only infill or only perimeters
	void writeBasicLine(const std::vector<std::vector<Line>> &in, std::shared_ptr<Layer> layer, double extWidth, double f = -1);

	// Return the extra purge distance required
	double writeInfillPurge(std::vector<std::vector<Vertex2D>> &purges, double amt, double extWidth, std::shared_ptr<Layer> layer);
	void writeRemainingInfill(std::vector<std::vector<Vertex2D>> &purges, double extWidth, std::shared_ptr<Layer> layer);
	double writeNonTransition(std::vector<PolyWithOffset> &islands, unsigned int layer, Colour const &nextColour, double feedratemultiplier = 1.0);
	void writeIslandsByColourLookahead(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour, unsigned int layer, double feedratemultiplier = 1.0);

	void writeIslandsByColour(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour, unsigned int layer,double feedratemultiplier = 1.0);
	void writeIslandsByTool(std::unordered_map<uint16_t, std::vector<PolyWithOffset>> &islandsByTool, unsigned int layer, double feedratemultiplier = 1.0);
	void writeIslands(std::vector<PolyWithOffset> &islands, unsigned int layer, double feedrateMultiplier, const Colour const* nextColour, int nextTool = -1);

	void writePerimeterTree(const InnerPerims* perimTree, std::shared_ptr<Layer> layer, double extWidth, double fullInfillExt, bool infillAndPerims, bool infillPass, bool fullInfill);
	void writePerimeterTreeReversed(const InnerPerims* perimTree, std::shared_ptr<Layer> layer, double extWidth, double fullInfillExt, bool infillAndPerims, bool infillPass, bool fullInfill);
	void writePath(const std::vector<Vertex2D> &path, std::shared_ptr<Layer> layer, double extWidth, double f = -1);
	void writePaths(const std::vector<std::vector<Vertex2D>> &path, std::shared_ptr<Layer> layer, double extWidth, double f = -1);
	void writeInfillPath(const std::vector<std::vector<Vertex2D>> &paths, std::shared_ptr<Layer> layer, double extWidth, bool isFullInfill, double f = -1);

	double getInnerPerimTreeInfillLength(const InnerPerims *parent, bool includeFullInfil, double extWidth, std::shared_ptr<Layer> layer) const;

	void writeSyringeLift();
	void writeSyringeIssue();
	//void writeSyringePos(int servoPos);

	void bufferLine(const std::string &text, const std::string &comment = std::string());
	void bufferLine(const GCodeLine &line);
	//Quantization
	double quantizeMove(double in, const AXIS axis) const;

	std::vector<Line> arrangeLineLoop(const std::vector<Line> &loop, const Vertex &closestPt) const;
	std::vector<Line> arrangeInfill(const std::vector<Line> &fillLines, const Vertex &closestPt) const;

	// Gets the string for this colour change, if we are using rovacompat its a tool change for example
	std::string getColourString(const Colour &in, bool hardwarePurge = false) const;

	void writeSuspend();
	void writeResume();

	std::vector<RETRACTION_TYPE> toolRetractionState;

	bool firstTCommand = true; // by freelancer latino
	void writeTool(uint16_t tool);
	std::string outputFileNameSplit;
	uint8_t lastFanSpeed = 0;
	double xOffset;
	double yOffset;
	double lastX, lastY, lastZ;
	double retractFanSpeedCache;
	double lastE;
	double lastFeedrate;
	double lastR, lastG, lastB;
	double printDistanceSinceRetract = 0;
	bool retracted = false;
	std::vector<Line> currentLayerLines;
	Vertex lastToolPosition;
	unsigned int currentLayer = 0;
	Colour lastColour;
	int filePartCounter = 0;
	uint16_t lastTool = std::numeric_limits<uint16_t>::max();
	int lastMixerSpeed = -1;
	double purgeBinAmount = 0;
	bool recommendPurgeExcess;
	bool lastRetractWasWipe = false;
	// Set to primed, if we have printed or purged this tool before
	std::unordered_set<uint16_t> m_primedToolSet;

	std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>> lineBuffer;
	bool isUsingLayerLineBuffer = false;
	std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>> layerLineBuffer;
	machineControl *m_ptrMachine;
	std::shared_ptr<SupportManager> m_ptrSupportManager;

	std::string hax_buffer;	// by freelancer latino

	bool lineCrossesPerim(double x1, double y1, double x2, double y2);
public:
	// Write to file
	GCodeWriter(const std::string &outputFileName);
	// Send to printer
	GCodeWriter(machineControl *ptrToMachineContr);
	void writeLayer(unsigned int layer);
	~GCodeWriter();
	void startWriter();
	void endWriter()
	{
		writeEndGCode();
	}
	std::string getTimeToPrint() const {
		return timeEstimate.getTimeEstimateAsString();
	}
	double getTimeDouble() const {
		return timeEstimate.getTimeEstimate();
	}
	std::string getFilamentUsageEstimatePrint() const {
		return filamentUsageEstimate.getFilamentUsageAsString();
	}
	double getPrintCost() const {
		return filamentUsageEstimate.getTotalCost();
	}


	void addSlicable(std::shared_ptr<iSlicable> newSlicable)
	{
		slicables.push_back(newSlicable);
		maxLayers = std::max(maxLayers, newSlicable->getLayerCount());
	}

	std::size_t getLayerCount() const;
	void registerSupportManager(std::shared_ptr<SupportManager> sman);
	std::set<uint16_t> &getToolsUsed() {
		return toolsUsed;
	}
};

