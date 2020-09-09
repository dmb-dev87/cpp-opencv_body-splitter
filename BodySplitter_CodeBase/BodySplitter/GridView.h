#pragma once

/* The build plate/volume grid*/

class GridView
{
private:
	unsigned int gridVertCount;
	unsigned int gridAxisCount;
	float *m_gridAxisCoords = nullptr;
	float *m_gridAxisColours = nullptr;
	float *m_gridCoords;
	float sizeX, sizeY, sizeZ;
public:
	GridView();
	~GridView();
	void draw();
};

