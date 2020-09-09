#include <limits>
#include <set>
#include <boost/functional/hash.hpp>
#include "MotionPlanner.h"
#include "Settings.h"

#if USE_INT_COORDS
#define SQRDJOIN() pow(to_micrometers(Settings::getSingleton().getDoubleValue("infillJoin")),2)
#else
#define SQRDJOIN() pow((Settings::getSingleton().getDoubleValue("infillJoin")),2)
#endif

MotionPlanner::MotionPlanner()
{
}


MotionPlanner::~MotionPlanner()
{
}

std::vector<Lines> MotionPlanner::arrangeInfill(const std::vector<Lines> &inLineSet, const std::vector<Lines> &perimeters, double density, bool straightJoin)
{
	double spacing = (Settings::getSingleton().getDoubleValue("extrusionWidth"));
	spacing = spacing / (density / 100.0);

	std::vector<Lines> output;
	if (inLineSet.size() < 1)
	{
		output = inLineSet;
	}
	else
	{
		std::vector<Line> workingSet;
		std::set<std::size_t> uniqueLinesHashMap;
		for (auto it : inLineSet)
		{
			for (auto innerIt : it)
			{
				std::size_t temp = 0;
				boost::hash_combine(temp, innerIt.pt1.hash());
				boost::hash_combine(temp, innerIt.pt2.hash());
				if (uniqueLinesHashMap.count(
					temp))
				{
					temp = 0;
					boost::hash_combine(temp, innerIt.pt2.hash());
					boost::hash_combine(temp, innerIt.pt1.hash());
					uniqueLinesHashMap.insert(temp);
				}
				else
				{
					if (innerIt.sqrdLength() > 0.01)
						workingSet.push_back(innerIt);
					uniqueLinesHashMap.insert(temp);
					temp = 0;
					boost::hash_combine(temp, innerIt.pt2.hash());
					boost::hash_combine(temp, innerIt.pt1.hash());
					uniqueLinesHashMap.insert(temp);
				}
			}
		}

		while (workingSet.empty() == false)
		{
			//Start with the shortest line if we have no lines rdy
			Lines::iterator shortestIT;
			if (output.empty())
			{
				shortestIT = Line::getShortestLine(workingSet);
			}
			else
			{
				//Start with the line closest to where we ended
				shortestIT = Line::getClosestLine(workingSet, output.back().back());
			}
			// Start new set with this as the first element
			output.push_back(std::vector<Line>());
			auto &currentSet = output.back();
			currentSet.push_back(*shortestIT);
			workingSet.erase(shortestIT);
			while (1)
			{
				auto nextLine = Line::getClosestLine(workingSet, currentSet.back());
				if (nextLine == workingSet.end())
					break;
				Line newLine(currentSet.back().pt2, nextLine->pt1, 0);
				bool crossesPerim = false;
				if (perimeters.empty() == false)
				{
					for (auto &it : perimeters)
					{
						if (Line::lineCrossesContour(newLine, it))
						{
							crossesPerim = true;
							break;
						}
					}
					// Check if it crosses an existing infill line //Does nothing
					//if (Line::lineCrossesContour(newLine, currentSet))
					//	crossesPerim = true;
				}
				if((!straightJoin || (straightJoin && fabs(newLine.pt2.y-newLine.pt1.y) < 0.001 || fabs(newLine.pt2.x - newLine.pt1.x) < 0.001))
					&& (newLine.sqrdLength() < spacing*5.0 && crossesPerim == false))
				{
					currentSet.push_back(newLine);
					currentSet.push_back(*nextLine);
					workingSet.erase(nextLine);
					
				}
				else
					break;
			}
		}
	}
	//Test output for connectivity
	for (auto LineSet : output)
	{
		for (int i = 0; i < LineSet.size() - 1; i++)
		{
			assert(LineSet[i].pt2.MatchesLocation(LineSet[i + 1].pt1));
		}
	}
	return output;
}
/*
void MotionPlanner::findShortestPath(const std::vector<Lines> &inLineSet, std::vector<Lines> &output, const std::vector<Lines> &perimeters)
{
	if (inLineSet.empty())
	{
		output.clear();
		return;
	}
	std::vector<Line> inLines;
	std::set<std::size_t> uniqueLinesHashMap;
	for (auto it : inLineSet)
	{
		for (auto innerIt : it)
		{
			std::size_t temp = 0;
			boost::hash_combine(temp, innerIt.pt1.hash());
			boost::hash_combine(temp, innerIt.pt2.hash());
			if (uniqueLinesHashMap.count(
				temp))
			{
				temp = 0;
				boost::hash_combine(temp, innerIt.pt2.hash());
				boost::hash_combine(temp, innerIt.pt1.hash());
				uniqueLinesHashMap.insert(temp);
			}
			else
			{
				if(innerIt.sqrdLength() > 0.1)
					inLines.push_back(innerIt);
				uniqueLinesHashMap.insert(temp);
				temp = 0;
				boost::hash_combine(temp, innerIt.pt2.hash());
				boost::hash_combine(temp, innerIt.pt1.hash());
				uniqueLinesHashMap.insert(temp);
			}
		}
	}
	//Rearrange infill lines
	//Find shortestLine
	uint shortestLineIndx = 0;
	double shortestFind = std::numeric_limits<double>::infinity();
	for (uint i = 0; i < inLines.size(); i++)
	{
		if (inLines[i].sqrdLength() < shortestFind)
		{
			shortestLineIndx = i;
			shortestFind = inLines[i].sqrdLength();
		}
	}
	std::vector<Line> sortedSize;
	uint lineCounter = shortestLineIndx;
	if (inLines.empty() || lineCounter >= inLines.size())
	{
		output.clear();
		return;
	}
	do {
		sortedSize.push_back(inLines[lineCounter]);
		lineCounter++;
		if (lineCounter == inLines.size())
			lineCounter = 0;
	} while (lineCounter != shortestLineIndx);

	inLines = sortedSize;

	while (inLines.empty() == false)
	{
		output.push_back(std::vector<Line>());
		// Start with the shortest point
		auto shortestIt = Line::getShortestLine(inLines);
		output.back().push_back((*shortestIt));
		inLines.erase(shortestIt);
		if (inLines.empty())
			break;
		bool foundMatch = true;
		while (foundMatch)
		{
			foundMatch = false;
			std::vector<Line>::iterator closestLine = inLines.begin();
			double closestDistance = output.back().back().pt2.distanceToSquared(closestLine->pt1);
			for (auto it = inLines.begin(); it != inLines.end(); ++it)
			{
				if (output.back().back().pt2.distanceToSquared(it->pt1) < closestDistance)
				{
					closestDistance = output.back().back().pt2.distanceToSquared(it->pt1);
					closestLine = it;
				}
				else if (output.back().back().pt2.distanceToSquared(it->pt2) < closestDistance) //Prefer pt1...
				{
					it->reverse();
					closestDistance = output.back().back().pt2.distanceToSquared(it->pt1);
					closestLine = it;
				}
			}
			if (closestDistance < SQRDJOIN())
			{
				Line tempLine(output.back().back().pt2, closestLine->pt1,0);
				bool crossesPerim = false;
				for (uint perimSet = 0; perimSet < perimeters.size(); perimSet++)
				{
					if (Line::lineCrossesContour(tempLine, perimeters[perimSet]))
					{
						crossesPerim = true;
						break;
					}
					//for (auto outIT : output)
					//{
					//	if(Line::lineCrossesContour(tempLine, outIT))
					//	{
					//		crossesPerim = true;
					//		break;
					//	}
					//}
				}
				if (!crossesPerim)
				{
					foundMatch = true;
					output.back().push_back(Line(output.back().back().pt2, closestLine->pt1,closestLine->getParam()));
					output.back().push_back(*closestLine);
					inLines.erase(closestLine);
				}
			}
			else
			{
				break;
			}
			if (inLines.empty())
				return;
			/*
				for (auto innerIt = inLines.begin(); innerIt != inLines.end(); ++innerIt) {
					if (output.back().empty())
					{
						output.back().push_back(*innerIt);
						innerIt = it->erase(innerIt);
						foundMatch = true;
					}
					else if (output.back().back().pt2.distanceToSquared(innerIt->pt1) <
						SQRDJOIN())
					{
						output.back().push_back(Line(output.back().back().pt2, innerIt->pt1));
						output.back().push_back(*innerIt);
						foundMatch = true;
						innerIt = it->erase(innerIt);
					}

					if (innerIt == it->end())
						break;
				}
			}
			
		}
	}
}
*/