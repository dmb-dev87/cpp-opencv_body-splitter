#pragma once
#include <stdint.h>
#include <numeric>
#include <deque>
#include <memory>

#include "colour.h"
class MovementWriter;
class GCodeLine;

using GCodeLinePath = std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>>;

class ToolColourWriter
{
public:
	static const uint16_t COLOUR_TOOL = 0;
	ToolColourWriter(MovementWriter* movement);
	~ToolColourWriter();

	uint16_t getCurrentTool() const {
		return lastTool;
	}
	Colour getCurrentColour() const {
		return lastColour;
	}

	void writeCurrentTool(GCodeLinePath &result, uint16_t newTool);
	void setCurrentColour(GCodeLinePath &result, Colour &cIn);

	void writeColour(GCodeLinePath &result, const Colour &colour);

private:
	MovementWriter* m_MoveWriter = nullptr;
	uint16_t lastTool = std::numeric_limits<uint16_t>::max();
	Colour lastColour;
};

