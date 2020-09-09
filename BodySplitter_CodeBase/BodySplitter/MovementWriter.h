#pragma once
#include <deque>
#include <string>
#include <memory>
#include <vector>

#include "colour.h"

class GCodeLine;
class Layer;

using GCodeLinePath = std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>>;

class MovementWriter
{
public:
	MovementWriter(std::shared_ptr<Layer> layer);
	~MovementWriter();

	void writeExtrudeTo(GCodeLinePath &result,double x,double y, double z, double e_multiplier, double extWidth, double f = -1);
	// Get The E-length of an extrude move
	double getExtrudeLengthTo(double x1, double y1, double x2, double y2, double z1, double z2, double e_multipler, double extWidth) const;
	void writeTravelTo(GCodeLinePath &result, double x,double y, double z, double f = -1, bool dontRetract = false, unsigned int lineType = 0);
	void writeTravelXY(GCodeLinePath &result, double x,double y, double f = -1, bool dontRetract = false, unsigned int lineType = 0);
	void writeZTravelTo(GCodeLinePath &result, double z,double f = -1);
	// Used to move fore and aft in the purge bucket
	void writeManualMove(GCodeLinePath &result, double x,	double y, double z, double e, double f = -1);

	void writeSyringeIssue(GCodeLinePath &result);
private:
	enum class RETRACTION_TYPE { TRAVEL_MOVE = 0, TOOL_CHANGE, PAUSE, DONTWIPE, NO_RETRACTION };
	enum class AXIS { X = 0, Y, Z, E };

	const double MIN_MOVE = 0.001;

	double quantizeMove(double in, const AXIS axis) const;
	void writeRetract(GCodeLinePath &result, RETRACTION_TYPE retractType);
	void writeIssue(GCodeLinePath &result);

	bool lineCrossesPerim(double x1, double y1, double x2, double y2);
	
	std::shared_ptr<Layer> currentLayer;
	std::vector<RETRACTION_TYPE> toolRetractionState;
	uint16_t lastTool = std::numeric_limits<uint16_t>::max();
	Colour lastColour;
	double lastX, lastY, lastZ, lastE;
	bool retracted = false;
	bool lastRetractWasWipe = false;
	double lastFeedrate;
	double printDistanceSinceRetract = 0;

	double xOffset = 0; double yOffset = 0;
};

