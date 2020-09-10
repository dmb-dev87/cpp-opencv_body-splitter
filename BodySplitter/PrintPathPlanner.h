#pragma once

class PrintPathElement;

/* Print Path Elements */
/*
1. Travel Line - No colour information
2. Ext Line - Perim, Infil - Perim has colour, infill doesnt really
3. Colour Change Line
4. Fan
5. Servo
6. Pause?
7. Colour Purge
8. Layer Change
*/

class PrintPathPlanner
{
private:
	void addLayerDetails();
public:
	PrintPathPlanner();
	~PrintPathPlanner();
};

