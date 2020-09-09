#pragma once
#include <unordered_map>
class Colour;

/* Contains a running total of the extrusion distance for
each tool and for each colour */

class FilamentUsageCounter
{
private:
	double ColourFilament[6];//CMY KW_T
	std::unordered_map<uint16_t, double> otherToolUsage;
	double distanceToKG(double distance, double density = 1.25) const;//1.25 g/cm3
public:
	void reset();
	FilamentUsageCounter();
	~FilamentUsageCounter();
	void addColourUsage(const Colour &in, double extrusionLength);
	void addOtherTool(uint16_t toolID, double length);

	double getToolUsage(uint16_t toolID) const;
	double getColourUsage(uint16_t colourID) const;
	double* getColourUsage();

	std::string getFilamentUsageAsString() const;
	double getTotalCost() const;
	std::string getFilamentUsageAsStringSimple() const;
};