#include "IslandWriter.h"

#include "iDrawable.h"
#include "Settings.h"
#include "GCodeWriter.h"

#include "MovementWriter.h"
#include "ToolColourWriter.h"

#include "Colour.h"

/*
IslandWriter::IslandWriter(std::unordered_map<Colour, std::vector<PolyWithOffset>> &islandsByColourIn,
	std::unordered_map<uint16_t, std::vector<PolyWithOffset>> &islandsByToolIn,
	unsigned int layerIDIn, MovementWriter* mWriter,
	double feedRateMultiplierIn) : islandsByColour(&islandsByColourIn), islandsByTool(&islandsByToolIn),
	layerID(layerIDIn), m_MovementWriter(mWriter),feedRateMultiplier(feedRateMultiplierIn)
{
}


IslandWriter::~IslandWriter()
{
}

void IslandWriter::writeAllIslands(GCodeLinePath &result)
{
	if (m_ToolWriter->getCurrentTool() == (uint16_t)iDrawable::ToolType::FULL_COLOUR && !GET_BOOL("rova5Compat"))
	{
		writeIslandsByColour(result);
		writeIslandsByTool(result);
	}
	else
	{
		 writeIslandsByTool(result);
		writeIslandsByColour(result);
	}
}

void IslandWriter::writeIslandsByColour(GCodeLinePath &result)
{
	if (islandsByColour->empty())
		return;
	if (m_ToolWriter->getCurrentColour != ToolColourWriter::COLOUR_TOOL)
	{
		m_ToolWriter->writeCurrentTool(result,ToolColourWriter::COLOUR_TOOL);
	}
	if (m_ToolWriter->getCurrentColour().isSet() == false)
	{
		m_ToolWriter->writeColour(result,islandsByColour->begin()->first);
	}
	if (GET_BOOL("lookAheadPurge")) {
		writeIslandsByColourLookahead(result, islandsByColour, layer, feedratemultiplier);
		return;
	}
	const Colour *nextColour = nullptr;
	while (islandsByColour->empty() == false)
	{
		nextColour = nullptr;
		if (islandsByColour->count(m_ToolWriter->getCurrentColour()) > 0)
		{
			//Last colour used, is used again
			if (islandsByColour[m_ToolWriter->getCurrentColour()].empty())
			{
				islandsByColour->erase(m_ToolWriter->getCurrentColour());
			}
			else
			{
				if (islandsByColour->size() > 1 && GET_BOOL("lookAheadPurge"))
				{
					for (auto &it : *islandsByColour)
					{
						if (it.first == m_ToolWriter->getCurrentColour())
							continue;
						else if (!it.second.empty())
						{
							nextColour = &it.first;
							break;
						}
					}
				}
				else
					nextColour = nullptr;
				Colour colourChache = m_ToolWriter->getCurrentColour();
				writeIslands((islandsByColour[m_ToolWriter->getCurrentColour()]), layer, feedratemultiplier, nextColour);
				islandsByColour[colourChache].clear();
				islandsByColour->erase(colourChache);
				if (nextColour != nullptr)
					lastColour = *nextColour;
			}
		}
		else
		{
			if (!islandsByColour->empty())
				m_ToolWriter->writeColour(result,islandsByColour->begin()->first);
		}
	}
}

void IslandWriter::writeIslandsByTool(GCodeLinePath &result)
{

}
*/