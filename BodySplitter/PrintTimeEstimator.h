#pragma once
/* Estimates the time taken to print, assumes infinite acceleration so inaccurate*/
#include <string>

class PrintTimeEstimator
{
	double timeEstimate;
	double gcodeSplitTime;
public:
	void addPrintMove(double length, double feedrate);
	void addColourPurge();
	void addTime(double milliseconds) {
		timeEstimate += milliseconds/1000.0;
		gcodeSplitTime += milliseconds / 1000.0;
	};
	PrintTimeEstimator();
	~PrintTimeEstimator();
	void reset();
	void resetGCodeSplit();
	bool needGCodeSplit();

	double getTimeEstimate() const;
	std::string getTimeEstimateAsString() const;
};

