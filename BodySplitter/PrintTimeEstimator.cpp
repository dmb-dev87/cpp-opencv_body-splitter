#include "PrintTimeEstimator.h"
#include "Settings.h"

#define COLOUR_PURGE_TIME() Settings::getSingleton().getDoubleValue("purgeTime")

void PrintTimeEstimator::addPrintMove(double length, double feed)
{
	timeEstimate += 
#if USE_INT_COORDS
		to_millimeters(length) / (feed / 60);//mm per minute to mm per second
#else
		(length) / (feed / 60);//mm per minute to mm per second
#endif
	gcodeSplitTime +=
#if USE_INT_COORDS
		to_millimeters(length) / (feed / 60);//mm per minute to mm per second
#else
		(length) / (feed / 60);//mm per minute to mm per second
#endif
}

void PrintTimeEstimator::addColourPurge()
{
	timeEstimate += COLOUR_PURGE_TIME();
	gcodeSplitTime += COLOUR_PURGE_TIME();
}

void PrintTimeEstimator::reset()
{
	timeEstimate = 0;
}

PrintTimeEstimator::PrintTimeEstimator()
{
}

PrintTimeEstimator::~PrintTimeEstimator()
{
}

double PrintTimeEstimator::getTimeEstimate() const
{
	return timeEstimate;
}

std::string PrintTimeEstimator::getTimeEstimateAsString() const
{
	//Seconds
	std::stringstream strStream;
	double timeCopy(timeEstimate);
	unsigned int hours = timeEstimate / 3600;           // Hour component
	unsigned int minutes = fmod(timeEstimate, 3600) / 60;  // Minute component
	unsigned int seconds = fmod(timeEstimate, 60);

	strStream << hours << ":";
	if (minutes < 10)
		strStream << "0";
	strStream << minutes << ":";
	if (seconds < 10)
		strStream << "0";
	strStream << seconds;
	return strStream.str();
}

void PrintTimeEstimator::resetGCodeSplit()
{
	gcodeSplitTime = 0;
}

bool PrintTimeEstimator::needGCodeSplit()
{
	return (gcodeSplitTime / (60 * 60) > GET_DOUBLE("gcodeSuspendTime"));
}