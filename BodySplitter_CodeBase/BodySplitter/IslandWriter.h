#pragma once
#include <deque>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>

class PolyWithOffset;
class GCodeLine;
class Colour;
class MovementWriter;
class ToolColourWriter;

using GCodeLinePath = std::deque<std::pair<std::string, std::shared_ptr<GCodeLine>>>;

class IslandWriter
{
public:
	~IslandWriter();

	IslandWriter(
		std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColour,
		std::unordered_map<uint16_t, std::vector<PolyWithOffset>> &islandsByTool,
		unsigned int layerID,
		MovementWriter* mWriter,
		double feedRateMultiplier = 1.0);

	void writeAllIslands(GCodeLinePath &result);

private:
	

	unsigned int layerID = 0;
	double feedRateMultiplier = 1.0;
	MovementWriter* m_MovementWriter;
	ToolColourWriter* m_ToolWriter;
	std::unordered_map<Colour, std::vector<PolyWithOffset>> *islandsByColour;
	std::unordered_map<uint16_t, std::vector<PolyWithOffset>> *islandsByTool;

	void writeIslandsByColour(GCodeLinePath &result);
	void writeIslandsByTool(GCodeLinePath &result);
};

