#include "FilamentUsageCounter.h"
#include "Settings.h"
#include "colour.h"

const char* COLOUR_NAME[] = {
	"Cyan", "Magenta", "Yellow", "Black","White"
};

FilamentUsageCounter::FilamentUsageCounter()
{
	for (uint16_t i = 0; i < 6; i++)
		ColourFilament[i] = 0;
	//if (GET_BOOL("rova5Compat"))
	//	otherToolUsage.resize(std::floor(GET_DOUBLE("compatExtruderCount")));
	//else
	//	otherToolUsage.resize(2); // flex and support only
}

FilamentUsageCounter::~FilamentUsageCounter()
{

}

void FilamentUsageCounter::addColourUsage(const Colour &in, double eDistance)
{
	auto cmykw = in.getCMYKW();
	for (int i = 0; i < 5; i++)
		ColourFilament[i] += cmykw[i] / 100.0*eDistance;
}

void FilamentUsageCounter::addOtherTool(uint16_t tool, double eDistance)
{
	otherToolUsage[tool] += eDistance;
}

void FilamentUsageCounter::reset()
{
	otherToolUsage.clear();
	for (uint16_t i = 0; i < 6; i++)
		ColourFilament[i] = 0;
	otherToolUsage.clear();
	//if (GET_BOOL("rova5Compat"))
	//	otherToolUsage.resize(std::floor(GET_DOUBLE("compatExtruderCount")));
	//else
	//	otherToolUsage.resize(2); // flex and support only
}

double FilamentUsageCounter::getToolUsage(uint16_t toolID) const
{
	if (otherToolUsage.count(toolID))
		return otherToolUsage.at(toolID);
	else
		return 0;
}

double FilamentUsageCounter::getColourUsage(uint16_t colourID) const
{
	assert(colourID < 6);
	return ColourFilament[colourID];
}

double* FilamentUsageCounter::getColourUsage()
{
	return &ColourFilament[0];
}

double FilamentUsageCounter::distanceToKG(double distance, double density) const
{
	// M = V*p
	// V = pi * r^2 * d
	return ((distance / 10.0) * (PI * 0.00765625)*density)/1000.0;
}

std::string FilamentUsageCounter::getFilamentUsageAsString() const
{
	std::stringstream output;
	std::fixed(output);
	output.precision(3);
	if (!GET_BOOL("rova5Compat"))
	{
		for (int i = 0; i < 5; i++)
		{
			if (ColourFilament[i] > 0.001)
			{
				output << COLOUR_NAME[i] << ":\n"
					<< "    Length: " << ColourFilament[i] << "mm\n    Mass: " << distanceToKG(ColourFilament[i], GET_DOUBLE("colourDensity"))*1000.0 << "g\n";
			}
		}
	}
	for (auto it : otherToolUsage)
	{
		if (GET_BOOL("rova5Compat"))
		{
			output << "Tool " << it.first << ":\n";
			output << "    Length: " << it.second << "mm\n    Mass: " << distanceToKG(it.second)*1000.0 << "g\n";
		}
		else
		{
			if (it.first == 1)
			{
				output << "Flexible:\n";
				output << "    Length: " << it.second << "mm\n    Mass: " << distanceToKG(it.second, GET_DOUBLE("flexDensity"))*1000.0 << "g\n";
			}
			else if (it.first == 2)
			{
				output << "Support:\n";
				output << "    Length: " << it.second << "mm\n    Mass: " << distanceToKG(it.second, GET_DOUBLE("supportDensity"))*1000.0 << "g\n";
			}
		}
		
	}
	return output.str();
}

double FilamentUsageCounter::getTotalCost() const
{
	double totalCost = 0;
	if (!GET_BOOL("rova5Compat"))
	{
		for (int i = 0; i < 5; i++)
		{
			if (ColourFilament[i] != ColourFilament[i]) // isNan
			{

			}
			else
				totalCost += distanceToKG(ColourFilament[i], GET_DOUBLE("colourDensity"))*GET_DOUBLE("colourMaterialCost");
		}
	}
	for (auto it : otherToolUsage)
	{
		if (GET_BOOL("rova5Compat"))
		{
			totalCost += distanceToKG(it.second)*GET_DOUBLE("colourMaterialCost");
		}
		else
		{
			if (it.first == 1)
			{
				totalCost += distanceToKG(it.second, GET_DOUBLE("flexDensity"))*GET_DOUBLE("flexMaterialCost");
			}
			else if (it.first == 2)
			{
				totalCost += distanceToKG(it.second, GET_DOUBLE("supportDensity"))*GET_DOUBLE("supportMaterialCost");
			}
			else
			{
				totalCost += distanceToKG(it.second, GET_DOUBLE("colourDensity"))*GET_DOUBLE("colourMaterialCost");
			}
		}
	}
	return totalCost;
}
