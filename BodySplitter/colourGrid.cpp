#include "colourGrid.h"
#include <cmath>
#include <iostream>
#include "Geometry.h"
#include "Line.h"
#include <unordered_map>
#include "Settings.h"


extern int gl_nSEL;   //SEL debug - verbose output if true;
					  //SEL added this

// This structure is used with a vector to map out the group of lines of 
// similar colours goings around the outer perimeter or an internal hole 
typedef struct dtColourRegionOfSplitLines
{
	Colour RegionColour;
	int bJoinFirstAndLast;
	int nStart;
	int nEnd;
	double dXMin, dYMin, dXMax, dYMax;
}dtColourRegionOfSplitLinesTag;


//
// initializeGrid
//
// Purpose:
//
// Create a 2 dimension colour grid that will be used to create islands of colour corresponding to the colour along the surface perimeter.  
// The initialization will create the grid in the correct size based on the bounding rectangle about the perimeter polygon divided by the
//  setting mmPerPixel.
// Each pixel in the grid will be set as empty or inside the polygon but not inside a hole (state = INSIDE_PT)
// Then each line of the polygon and holes will be applied to the grid setting the pixel's colour attribute to be the same as the line.  The border
// will be flagged as LINE_UPON and the state will be set to COLOUR_POINT.
//
// We will now return 0 if we have reduced this polygon to one colour !!!
//  and we will return 1 if we are good to continue
int BodySplitter::colourGrid::initializeGrid()
{

	xCount = ceil((maxX - minX) / gridSize) + 1;
	yCount = ceil((maxY - minY) / gridSize) + 1;

	std::cerr << "Grid Size: (" << xCount << "," << yCount << ")\n";

	// create a two dimension array of gridpixels
	grid = new gridPix*[xCount];
	for (int x = 0; x < xCount; x++)
		grid[x] = new gridPix[yCount];

	// for every pixel in the grid, determine if they are inside the overall polygon BUT not inside any hole
	// set the pixel state to INSIDE_PT
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (BodySplitter::isPointInPoly(Vertex(minX + gridSize*x + gridSize / 2.0, minY + gridSize*y + gridSize / 2.0), m_lines))
			{
				bool isInHole = false;
				for (int j = 0; j < m_holes.size(); j++)
				{
					if (BodySplitter::isPointInPoly(Vertex(minX + gridSize*x + gridSize / 2.0, minY + gridSize*y + gridSize / 2.0), m_holes[j]))
					{
						isInHole = true;
						break;
					}
				}
				if (isInHole == false)
					grid[x][y].state = gridPix::INSIDE_PT;
			}
		}
	}


	//   
	// For the exterior perimeter
	// and for each hole
	// call helper function to convert regions of the same colour into an unbroken path, while ensuring that each path will span enough x or y directions
	//
	if (gl_nSEL && Settings::getSingleton().getBoolValue("logInitializedGrid"))
	{
		std::cout << "********  Convert outer perimeter and inside holes to coloured region of paths\n";
	}//SEL
	addSplitLineToColourPaths();   // This does it all.  Converts the colour split outer perimeter and holes into complete paths

	//
	// SEL   we can test if the number of colours is reduced to one here.
	//
	if (outerPerimeterPathByColour.size() < 2)
	{
		return 0;
	}

	//
	// SEL we have to do a little more testing.   There can be multiple regions of the same colour when processing holes
	//
	//  really ???  Steve, why do you think that???
	//
	Colour testColour;
	int b = 1;
	int bSingleColour = 1;
	for (auto it : outerPerimeterPathByColour)
	{
		if (b)
		{
			testColour = it.second.front().colour;
			b = 0;
		}
		else
		{
			if (testColour != it.second.front().colour)
			{
				bSingleColour = 0;
			}
		}
	}
	if (bSingleColour)
	{
		if (gl_nSEL  && Settings::getSingleton().getBoolValue("logInitializedGrid"))
		{
			std::cout << "More than one region but all the same colour???\n\n";
		}//SEL
		return 0;
	}


	// 
	// SEL  we need an additional test here.   If there are holes but they are the same colour, then the 
	//      outerPerimeterPathByColour.size() will still be larger than 1
	//

	//
	// sort the paths in descending order of span length
	//
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logInitializedGrid"))
	{
		std::cout << "Sort the paths based on maximum span, longest to shortest\n\n";
	}//SEL
	sortPaths();

	//
	// Now plot paths for each colour into the grid
	//
	plotPath();

	// Now our grid should have pixel status of INSIDE_PT and LINE_UPON/COLOUR_POINT with the appropriate colour hash value all ready 
	// to be solved
	//  Yea Haw!!!
	return 1;

#if 0 //old way.   I now want to work with complete and validated paths rather than broken lines
	// Create Matching points
	// the plotLine method will draw all the lines into the grid
	// for each pixel drawn on, the state will be set to COLOUR_POINT and the border will be set to LINE_UPON
	// the pixel colour will be set to the line colour
	int nLine = 0; //SEL
	for (auto it : m_lines)
	{
		//if(it.r == 0)
		if (gl_nSEL && Settings::getSingleton().getBoolValue("logInitializedGrid"))
		{
			std::cout << "********  Plot Line " << nLine++ << "  ********\n";
		}//SEL
		plotLine(it);
	}
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logInitializedGrid"))
	{
		std::cout << "*****************************\n\n";
	}//SEL

	// plot all the holes !!! SEL must test this !!!
	for (auto it : m_holes)
	{
		for (auto holeIT : it)
			plotLine(holeIT);
	}
#endif

}

//
// solveGrid
//
// Purpose:   Take the initialized grid and fill all internal pixels with the same colour as the nearest perimeter line working from the outside and travelling 
//            towards the center, building one pixel layer at a time until all inside points are filled.  The colours will build
//            in blocks towards the center of the polygon.   This will give the 3D printed model the best appearance as the colour will build into the model 
//            rather than just on the surface perimeter.
//
void BodySplitter::colourGrid::solveGrid()
{
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logSolvedGrid");
	int nIter = 0;

#if 0 // SEL - no longer remove lone pixels.  Just make the pixels large enough to warrant printing them, even if a single pixel
	  // I have added a new setting for this ColourGridFactorOfExtrusionWidth
	// first it will remove any single coloured pixels that have no adjacent pixel of the same colour
	if (plottedSinglePixel)
	{
		if (bVerbose)
		{
			std::cout << "Remove Lone Pixels from Grid\n";
		}
		removeLonePixels();
	}
#endif

	// iterate through solution step until there are no Unfilled pixels left
	// unfilled pixels have the state INSIDE_PT.  When completed all inside points will be colour points.
	// Each solutionStep will traverse around the grid in a clockwise spiral copying colour pixels towards the center
	// in a single layer for each pass.   The Spiral will move one column and one row smaller each pass until
	// it has converged.
	while (gridHasUnfilled())
	{
		nIter++;
		if (bVerbose)
		{
			std::cout << "******************************\nUnfilled Pixels Remain.  Solution Step " << nIter << "\n";
		}
		//
		solutionStep();
	}
	//removeLonePixels();
	gridSolved = true;
	if (bVerbose)
	{
		std::cout << "Grid is Solved\n";
	}
}

// Tests the grid and returns true if any pixel has the state INSIDE_PT
bool BodySplitter::colourGrid::gridHasUnfilled() const
{
	if (gridSolved)
		return false;
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (grid[x][y].state == gridPix::INSIDE_PT)
				return true;
		}
	}
	gridSolved = true;
	return false;
}

//
// if the pixel is alone in colour the colour will be set to the same as the pixel to the left (-x)
// unless it is the first pixel in the line and then the colour to the right will be used.
//
// tests are made to all 8 pixels surrounding this test pixel
void BodySplitter::colourGrid::removeLonePixels()
{
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logSolvedGrid");

	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (isLonePixel(x, y))
			{
				if (bVerbose)
				{
					std::cout << "      Remove Lone Pixels ( " << x << ", " << y << " )\n";
				}
				if (xCount > 1)
				{
					if (x > 0)
						grid[x][y].colour = grid[x - 1][y].colour;
					else
						grid[x][y].colour = grid[x + 1][y].colour;
				}
			}
		}
	}
}

bool BodySplitter::colourGrid::isLonePixel(int x, int y)
{

	Colour colourToCheck = grid[x][y].colour;

	// check the colour to the left
	if (x > 0 && grid[x-1][y].colour == colourToCheck)   
		return false;

	// check the colour to the right
	if (x < xCount-1 && grid[x + 1][y].colour == colourToCheck)
		return false;

	// check the colour above
	if (y > 0 && grid[x][y-1].colour == colourToCheck)
		return false;

	// check the colour below
	if (y < yCount-1 && grid[x][y + 1].colour == colourToCheck)
		return false;

	// check the colour left and above
	// * 0 0
	// 0 . 0
	// 0 0 0
	if (x > 0 && y > 0 && grid[x - 1][y - 1].colour == colourToCheck)
		return false;

	// check the colour to the right and above
	// 0 0 *
	// 0 . 0
	// 0 0 0
	if (y > 0 && x < xCount - 1 && grid[x + 1][y - 1].colour == colourToCheck)
		return false;

	// check the colour to the left and below
	// 0 0 0
	// 0 . 0
	// * 0 0
	if (y < yCount - 1 && x>0 && grid[x - 1][y + 1].colour == colourToCheck)
		return false;

	// check the colour to the right and below
	// 0 0 0
	// 0 . 0
	// 0 0 *
	if (y < yCount - 1 && x < xCount - 1 && grid[x + 1][y + 1].colour == colourToCheck)
		return false;

	return true;
}

void BodySplitter::colourGrid::printGrid() const
{
	//SEL debug.  I changed the implementation so I could understand the representation of each pixel
	static int colourCounter = 0;
	static int colourCounter2 = 0;
	static std::unordered_map<uint32_t, std::pair<int, int> > colourMap;
	std::pair<int, int> colourPair;
	std::cout << "\n";
	int nLine;
	for (int y = yCount -1 ; y >= 0; y--)
	{
		for (nLine = 0; nLine < 2; nLine++) // go through the x list twice to display each pixel state on two separate lines, two characters on each line for each pixel
		{
			for (int x = 0; x < xCount; x++)
			{
				if (!nLine && grid[x][y].state == gridPix::COLOUR_POINT)  // test if this colour has been mapped before and add it if not
				{
					if (colourMap.count(static_cast<uint32_t>(grid[x][y].colour)) == 0)
					{
						colourMap[static_cast<uint32_t>(grid[x][y].colour)] = std::pair<int, int>(colourCounter++, colourCounter2);
					}
					if (colourCounter > 7)
					{
						colourCounter2++;
						colourCounter = 0;
					}
					colourPair = colourMap[static_cast<uint32_t>(grid[x][y].colour)];
				}

				// on the first line display state and colour if colour is defined  
				// mutually exclusive state will be displayed 
				//
				// colour will be a single digit code if it is a colour point
				//
				// isLineEnd is not currently implemented
				//
				if (!nLine)
				{
					switch (grid[x][y].state)
					{
					case gridPix::EMPTY: std::cout << ' '; break;
					case gridPix::INSIDE_PT: std::cout << '.'; break;
					case gridPix::COLOUR_POINT: std::cout << '+'; break;
					case gridPix::FILLED: std::cout << '@'; break;
					}

					// only display the colour if this pixel is not empty
					if (grid[x][y].state == gridPix::EMPTY)
					{
						std::cout << ' ';
					}
					else
					{
						std::cout << colourPair.first;
					}
				}// first line

				else //This is the second line
				// on the second line we will display the border status
				// first an asterik or a white space will indicate if a line upon
				// next the border will display with ascii characters for left right top and bottom or empty and they can be or'd together
				{
					std::cout << ((grid[x][y].border & gridPix::LINE_UPON) ? '*' : ' ');

					if ((grid[x][y].border & gridPix::BORDER_NULL) || (grid[x][y].border == gridPix::BORDER_EMPTY))
					{
						std::cout << ' ';
					}
					else
					{
						switch (grid[x][y].border &  0xF)
						{
						case 0: std::cout << ' '; break; //line upon and otherwise no border
						case 1: std::cout << '('; break; //left line
						case 2: std::cout << ')'; break; //right line
						case 3: std::cout << 'N'; break; //left and right
						case 4: std::cout << '-'; break; //top
						case 5: std::cout << 'r'; break; //top and left
						case 6: std::cout << '>'; break; //top and right
						case 7: std::cout << 'n'; break; //top, left and right
						case 8: std::cout << '_'; break; //bottom
						case 9: std::cout << '<'; break; //bottom and left
						case 10: std::cout << 'V'; break;//bottom and right
						case 11: std::cout << 'U'; break;//bottom, left and right
						case 12: std::cout << '='; break;//bottom and top
						case 13: std::cout << '['; break;//bottom, top and left
						case 14: std::cout << ']'; break;//bottom, top and right
						case 15: std::cout << '#'; break;//bottom, top, left and right
						}
					}

				}// second line
			}// for yCount
			std::cout << "\n";

		}// for alternate lines
	}//for x count
	std::cout << "\n";

#ifdef NOPE // Robert's implementation
	static int colourCounter = 0;
	static int colourCounter2 = 0;
	static std::unordered_map<uint32_t, std::pair<int, int> > colourMap;
	std::cout << "\n";
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (grid[x][y].state == gridPix::COLOUR_POINT)
			{
				if (colourMap.count(static_cast<uint32_t>(grid[x][y].colour)) == 0)
				{
					colourMap[static_cast<uint32_t>(grid[x][y].colour)] = std::pair<int, int>(colourCounter++, colourCounter2);
				}
				if (colourCounter > 7)
				{
					colourCounter2++;
					colourCounter = 0;
				}
				std::pair<int, int> colourPair = colourMap[static_cast<uint32_t>(grid[x][y].colour)];
				if (grid[x][y].isLineEnd)
					std::wcout << "\033[1;" << colourPair.first + 40 << "m\033[1;" << colourPair.second + 30 << "m+";
				else if (grid[x][y].border & gridPix::LINE_UPON)//else if(grid[x][y].liesUpon != nullptr)
					std::wcout << "\033[1;" << colourPair.first + 40 << "m\033[1;" << colourPair.second + 30 << "m-";
				else if (grid[x][y].border == gridPix::BORDER_NULL)
					std::wcout << "\033[1;" << colourPair.first + 40 << "m\033[1;" << colourPair.second + 30 << "mX";
				else if (grid[x][y].border == gridPix::BORDER_EMPTY)
					std::wcout << "\033[1;" << colourPair.first + 40 << "m\033[1;" << colourPair.second + 30 << "mY";
				else
					std::wcout << "\033[1;" << colourPair.first + 40 << "m\033[1;" << colourPair.second + 30 << "mZ";
			}
			else if (grid[x][y].isLineEnd == false)
				std::cout << " ";//(int)grid[x][y].state;
			else
				std::cout << "+";
			std::cout << "\033[0m";
		}
		std::cout << "\n";
	}
#endif
}

void BodySplitter::colourGrid::addLineSet(const std::vector< Line >& lines)
{
	for (auto it : lines)
	{
		m_lines.push_back(std::make_shared<Line>(it));
	}
}

//
// solutionStep
//
// This will perform a single step towards solving the colourGrid
//
// This method will traverse through the entire grid in a clockwise spiral fashion starting at X=0, Y=0
// travelling towards +X, -Y, -X, +Y, each spiral moving one row or column closer towards the center of the grid.  
// It will stop at the center of the grid.
//
// a temporary grid is created to contain the changes for this single step of the solution.  This allows the step to only 
// add a single layer of COLOUR_POINTS to the inside perimeter of the polygon.  Otherwise it will fill the entire grid 
// in one pass and I'm guessing that is not the desired response.
//
// as it traverses it will examine each pixel to determine if it is a COLOUR_POINT.  If found it will perform updatePoint
// which I don't think is a very descriptive name.   UpdatePoint will test the pixel above, below, to the left and to the right.  
// If the state of that pixel is INSIDE_PT it will copy the current COLOUR_POINT to that pixel in the temporary grid.
// After the spiral has been completed, the temporary grid will be copied to the final grid.
// 
// After completing each pass of solutionStep, the variable stepCount will be incremented.   The next pass of solutionStep
// will start the spiral at X=1, Y=1  (the value of stepCount)  and the spiral will get smaller with each pass until it is completed.
//
void BodySplitter::colourGrid::solutionStep()
{
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logSolvedGrid");

	////////////////////////////////////////////////////////////////
	//
	// let's create an entire copy of the grid and call it tempGrid
	// This is only done the first time solutionStep is called
	//  
	//
	if (tempGrid == nullptr)
	{
		if (bVerbose)
		{
			std::cout << "Create temporary Grid copy of primary Grid\n";
		}
		tempGrid = new gridPix*[xCount];//[yCount]; // Create the next copy of the grid
		for (int x = 0; x < xCount; x++)
			tempGrid[x] = new gridPix[yCount];
	}

	for (int x = 0; x < xCount; x++)
		memcpy(tempGrid[x], grid[x], sizeof(gridPix)*yCount);
	//
	/////////////////////////////////////////////////////////////////

	//
	// stepCount represents something groovy I'm sure and I'm looking forward to finding out what.
	//
	int y = stepCount;
	int x = stepCount;
	int lowY = stepCount + 1;
	int lowX = stepCount;
	int currentWidth = std::max(0, xCount - stepCount);
	int currentHeight = std::max(0, yCount - stepCount);


	if (y > currentHeight || x > currentWidth)
	{
		gridSolved = true;
		if (bVerbose)
		{
			std::cout << "y > currentHeight || x > currentWidth so solution is done\n";
		}
		return;
	}

	while (y <= currentHeight && x <= currentWidth)
	{
			//X First
		// shimmy to the right
		if (bVerbose)
		{
			std::cout << "Shimy to the Right: x= " << x << " y= " << y << " lowX = stepCount = " << lowX << " lowY = " << lowY << " currentWidth= " << currentWidth << " currentHeight= " << currentHeight << "\n";
		}
		for (; x < currentWidth; x++)
		{
			if (tempGrid[x][y].state == gridPix::COLOUR_POINT && tempGrid[x][y].isTemp == false)
			{
				if (bVerbose)
				{
					std::cout << "Moving X + direction.  Update around colour Point ( " << x << ", " << y << " )\n";
				}
				updatePoint(x, y);
			}
		}
		x--;
		currentWidth--;
		y++;
		// shimmy down
		if (bVerbose)
		{
			std::cout << "Shimy down: x= " << x << " y= " << y << " lowX = stepCount = " << lowX << " lowY = " << lowY << " currentWidth= " << currentWidth << " currentHeight= " << currentHeight << "\n";
		}
		for (; y < currentHeight; y++)
		{
			if (tempGrid[x][y].state == gridPix::COLOUR_POINT && tempGrid[x][y].isTemp == false)
			{
				if (bVerbose)
				{
					std::cout << "Moving Y + direction.  Update around colour Point ( " << x << ", " << y << " )\n";
				}
				updatePoint(x, y);
			}
		}
		y--;
		currentHeight--;
		x--;
		// shimmy to the left
		if (bVerbose)
		{
			std::cout << "Shimy to the left: x= " << x << " y= " << y << " lowX = stepCount = " << lowX << " lowY = " << lowY << " currentWidth= " << currentWidth << " currentHeight= " << currentHeight << "\n";
		}
		for (; x > lowX - 1; x--)
		{
			if (tempGrid[x][y].state == gridPix::COLOUR_POINT && tempGrid[x][y].isTemp == false)
			{
				if (bVerbose)
				{
					std::cout << "Moving X - direction.  Update around colour Point ( " << x << ", " << y << " )\n";
				}
				updatePoint(x, y);
			}
		}
		x++;
		lowX++;
		y--;
		// shimy up
		if (bVerbose)
		{
			std::cout << "Shimy up:  x= " << x << " y= " << y << " lowX = stepCount = " << lowX << " lowY = " << lowY << " currentWidth= " << currentWidth << " currentHeight= " << currentHeight << "\n";
		}
		for (; y > lowY - 1; y--)
		{
			if (tempGrid[x][y].state == gridPix::COLOUR_POINT && tempGrid[x][y].isTemp == false)
			{
				if (bVerbose)
				{
					std::cout << "Moving Y - direction.  Update around colour Point ( " << x << ", " << y << " )\n";
				}
				updatePoint(x, y);
			}
		}
		lowY++;
		y++;
		x++;
	}//while
	

	stepCount++;

	//
	// copy all the tempGrid pixels that have been modified into the primary grid
	//
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (tempGrid[x][y].isTemp)
			{
				grid[x][y] = tempGrid[x][y];
				grid[x][y].isTemp = false;
				grid[x][y].isLineEnd = false;
			}
		}
	}

}

void BodySplitter::colourGrid::printGridDebugInfo() const
{
	std::cout << "Grid size: (" << xCount << "," << yCount << ")\n";
	std::cout << "Memory required: " << sizeof(gridPix)*xCount*yCount / 1024 << " kilobytes\n";
}

void BodySplitter::colourGrid::addLineSet(BodySplitter::linePtrSet& lines)
{
	assert(m_lines.empty());	//Only should have one outter perimeter
	m_lines = lines;
}

void BodySplitter::colourGrid::addHoleSet(BodySplitter::linePtrSet& holes)
{
	m_holes.push_back(holes);
}

void BodySplitter::colourGrid::addHoleSet(const std::vector< Line >& holes)
{
	m_holes.push_back(linePtrSet());
	for (auto lineIT : holes)
	{
		m_holes.back().push_back(std::make_shared<Line>(lineIT));
	}
}

void BodySplitter::colourGrid::plotLine(const std::shared_ptr< Line >& line)
{
	float y1 = (line->pt1.y - minY) / gridSize;
	float y2 = (line->pt2.y - minY) / gridSize;

	float x1 = (line->pt1.x - minX) / gridSize;
	float x2 = (line->pt2.x - minX) / gridSize;
	assert(y1 >= 0);
	assert(x1 >= 0);
	assert(y2 >= 0);
	assert(x2 >= 0);

	assert((int)y1 < yCount);
	assert((int)x1 < xCount);
	assert((int)y2 < yCount);
	assert((int)x2 < xCount);

	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logInitializedGrid");

	gridPix newPix;
	newPix.state = gridPix::COLOUR_POINT;
	newPix.colour = line->colour;//gridColour(line->colour.getR<uint8_t>(),line->colour.getG<uint8_t>(),line->colour.getB<uint8_t>());

		// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;
	int pixelsPlotted = 0;
	int bFirst = 1; //SEL added this for logging
	int nLastX, nLastY=-1; //SEL debug

	if (bVerbose)
	{
		std::cout << "Pt 1 (" << line->pt1.x << ", " << line->pt1.y << ") Pt 2 (" << line->pt2.x << ", " << line->pt2.y << ") dx = " << dx << ", dy = " << dy << "\n";
		std::cout << " Line colour = " << line->colour << " x1 = " << x1 << " y1 = " << y1 << " x2 = " << x2 << " y2 = " << y2 << "  steep = " << steep << "\n";
	} //SEL debug

	for (int x = (int)x1; x < maxX; x++)
	{
		if (steep)
		{
			if (y < xCount && x < yCount && y >= 0 && x >= 0)
			{
				grid[y][x].colour = newPix.colour;
				grid[y][x].state = newPix.state;
				// 	grid[y][x].liesUpon = line.get();
				grid[y][x].border |= gridPix::LINE_UPON;
				pixelsPlotted++;

				nLastX = y; //SEL
				nLastY = x; //SEL
			}
			//SetPixel(y,x, color);
		}
		else
		{
			if (y < yCount && x < xCount && y >= 0 && x >= 0)
			{
				grid[x][y].colour = newPix.colour;
				grid[x][y].state = newPix.state;
				grid[x][y].border |= gridPix::LINE_UPON;
				pixelsPlotted++;
				// 	grid[x][y].liesUpon = line.get();

				nLastX = x; //SEL
				nLastY = y; //SEL
			}
			//         SetPixel(x,y, color);
		}

		if (bVerbose && (pixelsPlotted == 1) && bFirst)
		{
			std::cout << "Grid start of line (" << nLastX << ", " << nLastY << ")  ";
			bFirst = 0;
		}


		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}

#if 1
	//SEL how about we draw a single pixel in the event that the line is too short to be plotted
	if (pixelsPlotted == 0)
	{
		double x, y;

		y1 = (line->pt1.y - minY) / gridSize;
		y2 = (line->pt2.y - minY) / gridSize;

		x1 = (line->pt1.x - minX) / gridSize;
		x2 = (line->pt2.x - minX) / gridSize;

		x = (x1 + x2) / 2.0;
		y = (y1 + y2) / 2.0;

		nLastX = (int)x;
		nLastY = (int)y;

		grid[nLastX][nLastY].colour = newPix.colour;
		grid[nLastX][nLastY].state = newPix.state;
		grid[nLastX][nLastY].border |= gridPix::LINE_UPON;

		plottedSinglePixel = true;

	}
#endif

	if(bVerbose)
	{
		if (pixelsPlotted)
		{
			std::cout << " end of line (" << nLastX << ", " << nLastY << ")\n\n";
		}
		else
		{
			std::cout << "Single Pixel forced to be Ploted at (" << nLastX << ", " << nLastY << ")\n\n";
		}
	}// SEL debug



	if (pixelsPlotted == 1)
	{
		plottedSinglePixel = true; 
		if (bVerbose)
		{
			std::cout << "Only single pixel plotted\n";
		}// SEL debug
	}
}

//
// update point will copy the specified tempGrid pixel
//  to unfilled pixels to the left, right, top or bottom (not diagonal)
//       copied pixels will have the LINE_UPON border bit cleared, isLineEnd set to false
//       and isTemp set to true
//  LINE_UPON should only be set for the external perimeter.  isTemp is used to ensure that 
//  only one layer of pixels is added per pass of solutionStep  
//
void BodySplitter::colourGrid::updatePoint(int xPos, int yPos)
{
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logSolvedGrid");

	//
	//copy the specified pixel of tempGrid into gridValue
	//   clear the LINE_UPON bit and isLineEnd and 
	//   set the isTemp bit to true.
	gridPix gridValue = tempGrid[xPos][yPos];
	gridValue.border &= ~gridPix::LINE_UPON;
	gridValue.isLineEnd = false;
	gridValue.isTemp = true;


	// if the tempGrid pixel to the left, the right, above or below is an INSIDE_PT
	//    then overwrite them with gridValue
	if (xPos > 0 && tempGrid[xPos - 1][yPos].state == gridPix::INSIDE_PT)
	{
		tempGrid[xPos - 1][yPos] = gridValue;
		if (bVerbose)
		{
			std::cout << "      filling left pixel ( " << (xPos - 1) << ", " << yPos << " )\n";
		}

	}
	if (yPos > 0 && tempGrid[xPos][yPos - 1].state == gridPix::INSIDE_PT)
	{
		tempGrid[xPos][yPos - 1] = gridValue;
		if (bVerbose)
		{
			std::cout << "      filling above pixel ( " << (xPos) << ", " << (yPos-1) << " )\n";
		}
	}
	if (xPos < xCount - 1 && tempGrid[xPos + 1][yPos].state == gridPix::INSIDE_PT)
	{
		tempGrid[xPos + 1][yPos] = gridValue;
		if (bVerbose)
		{
			std::cout << "      filling right pixel ( " << (xPos + 1) << ", " << yPos << " )\n";
		}
	}
	if (yPos < yCount - 1 && tempGrid[xPos][yPos + 1].state == gridPix::INSIDE_PT)
	{
		tempGrid[xPos][yPos + 1] = gridValue;
		if (bVerbose)
		{
			std::cout << "      filling below pixel ( " << (xPos) << ", " << (yPos+1) << " )\n";
		}
	}
	return;
	
}

// 
// createBorderLines
//
// Purpose:   This method will create lines along to borders of the blocks of like colour found in grid
//
void BodySplitter::colourGrid::createBorderLines()
{
	//
	// This section will examine each pixel in the grid looking for colour points that do not have a line
	// upon them.   It will set the border state bits as appropriate and the states can be or'ed together as they are not 
	// mutually exclusive
	//
	//   If pixel to the left is a different colour    then OR BORDER_LEFT
	//   If pixel to the bottom is a different colour  then OR BORDER_BOTTOM
	//   If pixel to the right is a different colour   then OR BORDER_RIGHT
	//   If pixel to the top is a different colour     then OR BORDER_TOP
	//
	//  Then it will execute borderToLine to create single pixel long lines on the appropriate boundaries of the pixel and 
	//  insert these in the line vector borderLines
	//
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (grid[x][y].border & gridPix::LINE_UPON)
				continue;
			if (grid[x][y].state == gridPix::COLOUR_POINT)
			{
				Colour startPointColour = grid[x][y].colour;
				grid[x][y].border = gridPix::BORDER_EMPTY;
				if (x != 0 && grid[x - 1][y].colour != startPointColour && grid[x - 1][y].state == gridPix::COLOUR_POINT)
					grid[x][y].border |= gridPix::BORDER_LEFT;
				if (y != 0 && grid[x][y - 1].colour != startPointColour && grid[x][y - 1].state == gridPix::COLOUR_POINT)
					grid[x][y].border |= gridPix::BORDER_BOTTOM;
				if (x != xCount - 1 && grid[x + 1][y].colour != startPointColour && grid[x + 1][y].state == gridPix::COLOUR_POINT)
					grid[x][y].border |= gridPix::BORDER_RIGHT;
				if (y != yCount - 1 && grid[x][y + 1].colour != startPointColour && grid[x][y + 1].state == gridPix::COLOUR_POINT)
					grid[x][y].border |= gridPix::BORDER_TOP;

				//
				// now create 0 to 4 lines for each border of the pixel and assign the line the same colour as the pixel
				// Insert the lines into borderLines
				auto linez = borderToLine(grid[x][y], x, y);
				if (linez.empty() == false)
					borderLines.insert(borderLines.end(), linez.begin(), linez.end());
			}
		}
	}

	//
	// This section will sort out the lines into separate line vectors based on their colour (colour being indexed by a hash value)
	//
	//  insetlinesByColour is an unordered map containing the colour hash and a line vector of all lines of that colour
	//
	std::unordered_map<uint32_t, std::vector<Line>> insetlinesByColour;

	for (auto it : borderLines) // for all borderLines
	{
		uint32_t cHash = it.colour.hash();  //creat the hash value for this colour
		// std::cout << "Colour hash: "<<(int)cHash<<"\n";

		// if this colour is already found in the map
		if (insetlinesByColour.count(cHash))
		{
			// 
			insetlinesByColour[cHash].push_back(it);
		}
		else
		{
			// else create a new line vector
			insetlinesByColour[cHash] = std::vector<Line>();
			// and then insert this borderLine into the line vector
			insetlinesByColour[cHash].push_back(it);
		}
	}// for all borderLines

	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logBoundaryGrid");

	//
	// This section will go through the map of insetLinesByColour and reduce all the broken lines
	// into sections of 2D vertex arrays.  Because it iteratively call brokenLinesToVert2DPath
	// I'm thinking that each island internal boundary lines will be connected even if it isn't in a loop.  
	// These will be segments of lines and there will be one for each separate island of the same colour.  
	//
	static int i = 0;
	if (bVerbose) //SEL debug
	{
		std::cout << "Combine broken Lines to Vertex Paths for each colour\n";
	}

	for (auto it : insetlinesByColour)
	{
		uint32_t cHash = it.first;
		//  insetPathByColour.insert(it.first);
		while (it.second.size() > 2)
			insetPathByColour[cHash].push_back(BodySplitter::brokenLinesToVert2DPath(it.second,
				it.first & 0xFF, (it.first & 0xFF00) >> 8, (it.first & 0xFF0000) >> 16));
		if (bVerbose) //SEL debug
		{
			std::cout << "Inset paths " << insetPathByColour[cHash].size() << " for Colour Hash = " << it.first << "\n";
			for (int j = 0; j < insetPathByColour[cHash].size(); j++)
			{
				std::cout << "Path " << j << " has " << insetPathByColour[cHash][j].poly.size() << " points\n";
			}
		}

	}

	// smooth out the inset lines for each  path
	for (auto &colourIT : insetPathByColour)
	{
		for (auto &linesetIT : colourIT.second)
		{
 			smoothInsetLines(linesetIT);
		}
	}

	int j,k,n = 0;

	// convert all the vertices in pixel index to world coordiantes
	for (auto &it : insetPathByColour)
	{
		if (bVerbose) //SEL debug
		{
			std::cout << "Smoothed InsetPathByColour " << n++ << " Colour Hash = " << it.first << "\n";
		}
		k = 0;
		for (auto &lineIT : it.second)
		{
			if (bVerbose) //SEL debug
			{
				std::cout << "Line number " << k++ << "\n";
			}
			j = 0;
			for (auto &pt : lineIT.poly)
			{
				pt = BodySplitter::convertPixelToWorld(pt, minX, minY, gridSize);  //does this overwrite pt????
				if (bVerbose) //SEL debug
				{
					std::cout << j++ << ", " << pt.x << ", " << pt.y <<  "\n";
				}
			}
		}
	}

	//SEL March 2020   Log border fragments to ruby script
	int bRubyScript = gl_nSEL && Settings::getSingleton().getBoolValue("logPerimeterAndBordersToRubyScript");
	if (bRubyScript)
	{
		RubyOpenLogFile();

		//create line objects for all coloured exterior line fragment
		RubyScriptExterior();

		//create line objects for all coloured inset line fragments
		RubyScriptInsets();
	}
	

	//SEL  March 2020   ColourBleeding fix
	//
	// This is the time to go through all the insets and validate the vertices.   We will flag any vertex that is outside
	// the body or is too close to the outer perimeter.   I will store the value in the ini file and call it
	// BleedThreshold=1.0 mm
	//
	double dBleedThreshold = Settings::getSingleton().getDoubleValue("BleedThreshold");
	if (dBleedThreshold > 0.0)
	{
		//call the vertex qualification method
		QualifyInsetVertices();
	}

	if (bRubyScript)
	{
		//create line objects for all coloured inset line fragments
		RubyScriptInsets();
	}

	createIslands();  // Now create the islands including external perimeter and merge this with the internal insets

	//SEL March 2020   Log raw islands to ruby script for SketchUp visualization
	if (bRubyScript)
	{

		//create single group of for all coloured islands and call them RawIslands
		RubyScriptIslands(1);

	}

	//  March 2020   ColourBleeding fix
	//    Now have all our islands (polygons) completed.  Let's go through these polygons and remove
	//    all vertices that are invalid.  
	if (dBleedThreshold > 0.0)
	{
		//int i = 0;
		//for (auto itIsland : output)  // for each island
		//{
		//	std::cerr << "Outside: island[" << i << "] pts before = " << output[i++].poly.size() << "\n";
		//}
		//call the polygon remove invalid vertex method
		RemoveInvalidVerticesFromIslands();
		//i = 0;
		//for (auto itIsland : output)  // for each island
		//{
		//	std::cerr << "Outside: island[" << i << "] pts after = " << output[i++].poly.size() << "\n";
		//}

		if (bRubyScript)
		{

			//create single group of for all coloured islands and call them QualifiedIslands
			RubyScriptIslands(0);

		}
	}

	if (bRubyScript)
	{
		RubyCloseLogFile();
	}
}

//
// createIslands
// 
//   This is probably the most important method to BodySplitter to make it a great product
//
//  Purpose:   Create individual islands (poygons) for each colour region in this grid.   The external perimeter (along the surface of the model)
//             will retain the same coordinates, therefore keeping the same resolution as the monochrome complete polygon for the unsplit island.  These split chunks 
//             of coloured perimeter will be matched at each end to inset lines that will form a closed loop needed for a new polygon.
//
// BUG!   I think that this will fail when there is no inset lines for the split segment of colour, which occurs when plotting a single pixel or no pixel.  This
//        becomes common as the resolution decreases.   Look inside the helper method  mergeLinesWtihPaths
//
//
void BodySplitter::colourGrid::createIslands()
{
#if 1 //SEL Steve's method
	
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logMergePaths");

	for (auto it : insetPathByColour)  // For each set of paths by colour - these are the inset lines that were formed by the boundary lines in the colour grid
	{
		uint32_t cHash = it.first;	// get the colour for this set of inset paths

		// now test that we have outerPerimeterPathByColour for the same colour
		if (!outerPerimeterPathByColour.count(cHash) )
		{
			if (bVerbose)
			{
				std::cout << "There is no outer perimeter or hole lines to match this inset line's colour " << cHash << "\n";
			}
			std::cerr << "There is no outer perimeter or hole lines to match this inset line's colour " << cHash << "\n";
			continue;
		}

		if (bVerbose)
		{
			std::cout << "Merging lines for colour " << cHash << "\n";
		}
		//merge the perimeters and holes (outerPerimeterPathByColour[cHash]) with the inset lines (it.second) of the same colour.  Tolerance is twice the gridsize
		auto result = mergeLinesWithPaths(outerPerimeterPathByColour[cHash], it.second, gridSize*2.8284);  //Factor of 2 was not sufficient.  I changed it to 2*sqrt(2)

		// return the new merged lines (type of vert2DPoly) in result and insert these in output
		output.insert(output.end(), result.begin(), result.end());

		// Now I am not quit clear what is going on next.   Why are there inset lines left over and why are we making them into polygons??
		if (it.second.empty() == false)
		{
			if (bVerbose)
			{
				std::cout << "Just why am I here.  There is a stray inset line left over.\n";
			}
#if 0 // I don't want to make a perimeter.  This never works out well
			// There are more inset paths, try and make a perimeter
			for (auto polyIT : it.second)
			{
				BodySplitter::vert2DPoly newPoly;
				newPoly.colour = it.second.front().colour;
				newPoly.poly = polyIT.poly;
				output.push_back(newPoly);
			}
#endif
		}
	}// for each inset group by colour


#else //Robert's Method
	static int mergedFile = 0;
	// Match each produced islands
	std::unordered_map<uint32_t, std::vector<Line>> lineByColour;
	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logMergePaths");

	// 
	// for all the original split lines on the outside perimeter of the model.  Note: Some of these might not have matching inset lines if my fix 
	// didn't work.  
	// create a map of lines for each colour (lineByColour)
	//
	// I am going to enhance this to create ExteriorPathsByColour.  The paths will not have any broken lines and
	// can be treated as a unit.   This is important for 
	for (auto it : m_lines)
	{
		uint32_t cHash = it->colour.hash();
		if (lineByColour.count(cHash) > 0)
			lineByColour[cHash].push_back(*it);
		else
		{
			lineByColour[cHash] = std::vector<Line>();
			lineByColour[cHash].push_back(*it);
		}
	}
	//
	// do the same thing for holes and add them to the lineByColour vector 
	//
	for (auto lineSetIT : m_holes)
	{
		for (auto lineIT : lineSetIT)
		{
			uint32_t cHash = lineIT->colour.hash();
			if (lineByColour.count(cHash) > 0)
				lineByColour[cHash].push_back(*lineIT);
			else
			{
				lineByColour[cHash] = std::vector<Line>();
				lineByColour[cHash].push_back(*lineIT);
			}
		}
	}

	//   
	//  LOOKY HERE
	//
	// Here is the problem.   This takes all the inset paths and matches them with the coloured lines along the perimeter.   
	// What happens if there isn't a insetPathByColour for a corresponding external line.   It will not create an island and it will leave big chunks missing, 
	// up to and including the entire polygon
	//
	// I have decided to fix this up stream and not allow any garbage in.   I will filter the splitlines
	// and ensure that each colour region will span enough distance to provide VALID inset paths
	//
	for (auto it : insetPathByColour)  // For each set of paths by colour - these are the inset lines that were formed by the boundary lines in the colour grid
	{
		uint32_t cHash = it.first;	// get the colour for this set of inset paths

		if (bVerbose)
		{
			std::cout << "Merging lines for colour " << cHash << "\n";
		}
		//merge the perimeters and holes (lineByColour[cHash]) with the inset lines (it.second) of the same colour.  Tolerance is twice the gridsize
		auto result = mergeLinesWtihPaths(lineByColour[cHash], it.second, gridSize*2.0);

		// return the new merged lines (type of vert2DPoly) in result and insert these in output
		output.insert(output.end(), result.begin(), result.end());

		// Now I am not quit clear what is going on next.   Why are there inset lines left over and why are we making them into polygons??
		if (it.second.empty() == false)
		{
			if (bVerbose)
			{
				std::cout << "Just why am I here\n";
			}
			// There are more inset paths, try and make a perimeter
			for (auto polyIT : it.second)
			{
				BodySplitter::vert2DPoly newPoly;
				newPoly.colour = it.second.front().colour;
				newPoly.poly = polyIT.poly;
				output.push_back(newPoly);
			}
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// borderToLine
//
//    Input:   gridPix  pt    Pixel to examine
//             int x,y        Pixel index in grid
//
//    Return   line vector
//
// Purpose:  This method will examine the given point to determine if it is a border.  If it is 
//           it will generate a single line one pixel lone along the outside of the pixel
//           in which the border occurs.   (Left right top or bottom).  It may generate up to 
//           4 lines if there is a border on each side.
//           The lines are assigned the same colour as the pixel
//           The coordiinate system for the lines is in grid space and must be converted to world coordinates 
//           later
///////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Line> BodySplitter::colourGrid::borderToLine(const gridPix &pt, int x, int y)
{
	std::vector<Line> out;
	if (pt.border == gridPix::BORDER_EMPTY || pt.border == gridPix::BORDER_NULL)
		return out;

	if (pt.border & gridPix::BORDER_LEFT)
	{
		Vertex pt1, pt2;
		pt1 = Vertex(x - 0.5, y - 0.5);
		pt2 = Vertex(x - 0.5, y + 0.5);
		out.push_back(Line(pt1, pt2, 0));
		out.back().colour = pt.colour;
	}
	if (pt.border & gridPix::BORDER_RIGHT)
	{
		Vertex pt1, pt2;
		pt1 = Vertex(x + 0.5, y - 0.5);
		pt2 = Vertex(x + 0.5, y + 0.5);
		out.push_back(Line(pt1, pt2, 0));
		out.back().colour = pt.colour;
	}
	if (pt.border & gridPix::BORDER_BOTTOM)
	{
		Vertex pt1, pt2;
		pt1 = Vertex(x - 0.5, y - 0.5);
		pt2 = Vertex(x + 0.5, y - 0.5);
		out.push_back(Line(pt1, pt2, 0));
		out.back().colour = pt.colour;
	}
	if (pt.border & gridPix::BORDER_TOP)
	{
		Vertex pt1, pt2;
		pt1 = Vertex(x - 0.5, y + 0.5);
		pt2 = Vertex(x + 0.5, y + 0.5);
		out.push_back(Line(pt1, pt2, 0));
		out.back().colour = pt.colour;
	}
	return out;
}

// SEL I have moved this from Mesh.cpp void Mesh::splitByPerim(Perimeter* perim)
//  because I have much more of need for this combination of split contours here
//
// Check for short line segments of isolated colour
// SEL I think this section needs some work
// I want to look at the span all line fragments of the same colour to see
// if these are long enough
// minimumLineLength is not sufficient.  It must span X or Y by enough distance to actually show up 
// in the colour grid and produce an island.   The test criteria should be based on nozzle size 
// as should the colourGrid pixel size.   Any group of coloured lines that are too short will be added to the last valid region 
// and be set to that colour.  If we are at the end of the list then I guess we will have to join this to the previous
// group.
//
//
//  addSplitLineToColourPaths
//
// Purpose::   This method will go through the outer perimeter and all the holes that have already been split by colour region
//             and ensure they cover enough distance to be physically printed with the extrusion width.  Regions that are too
//  small will be combined with a different colour that is long enough. 
//
//  Then two vectors will be produced.  The first "LineGroupsBySpan"  will contain the paths (vector of lines) and Span of that
//  This will be later sorted and used to plot the lines to the pixels.   The second vector "outerPerimeterPathByColour" will
//  also contain the path in Vertex form for that colour region but in groups of colour.  This will be used to join these outer lines
//  with the inset lines created by createBorder:Lines
//
void BodySplitter::colourGrid::addSplitLineToColourPaths()
{
	int bDone = 0;
	int nStage = 0;    // 0 work on outer perimeter
					   // 1 to n convert hole perimeters

	BodySplitter::linePtrSet *linesToProcess;
	std::vector<dtColourRegionOfSplitLines> ColourRegionsOfLinesToProcess;
	dtColourRegionOfSplitLines thisRegion;
	int nShortCircuit;
	int nHoles = m_holes.size();

	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
	{
		std::cout << "----------------------- addSplitLineToColourPaths ---------------------------------------n";
	}

	while (!bDone)
	{

		// We will go through this process for the outer perimeter line set and then for every hole. 
		ColourRegionsOfLinesToProcess.clear();
		nShortCircuit = 0;

		if (nStage == 0)
		{
			linesToProcess = &m_lines;
			if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
			{
				std::cout << "Processing Outer Perimeter\n";
			}
		}
		else
		{
			if (nStage <= nHoles)
			{
				linesToProcess = &(m_holes[nStage - 1]);
				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
				{
					std::cout << "Processing Inner Hole Perimeter " << nStage <<"\n";
				}
			}
			else
			{
				bDone = 1;
				break;
			}
		}
		nStage++;

		// The only broken path possible may be the first region since the loop can start anywhere.  We will treat this as a special case
		// if the first and the last line has the same colour.   Work backwards until a colour transition is found.   Remember, that there must be 
		// more than one colour or we would not be here.   
		// After we have a vector of paths we can then qualify them for span size.
		//


		//I'm sure this isn't the most elegant way of solving this problem, but I already wrote this
		//colour region vector stuff earlier in Mesh.cpp.   When it is done I will have everything
		//I need to use this as a guide to add paths from the region.


		//
		// This section prepares the vector of line regions of the same colour and accumulates the spanning distance in X and Y
		//
		if (linesToProcess->size() > 2)  // The smallest perimeter is a triangle
		{
			if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
			{
				std::cout << "Traverse Perimeter for different colour regions and validate the length of each region.\n";
				std::cout << "For " << linesToProcess->size() << " Lines\n";
			}

			// initialize the very first colour region to the values of the first line
			thisRegion.RegionColour = (*linesToProcess)[0]->colour;
			thisRegion.nStart = 0;
			thisRegion.bJoinFirstAndLast = 0;
			thisRegion.dXMin = MIN(linesToProcess->at(0)->pt1.x, linesToProcess->at(0)->pt2.x);
			thisRegion.dYMin = MIN(linesToProcess->at(0)->pt1.y, linesToProcess->at(0)->pt2.y);
			thisRegion.dXMax = MAX(linesToProcess->at(0)->pt1.x, linesToProcess->at(0)->pt2.x);
			thisRegion.dYMax = MAX(linesToProcess->at(0)->pt1.y, linesToProcess->at(0)->pt2.y);

			// go through all the remaining lines and look for changes in the colour
			for (uint i = 1; i < linesToProcess->size(); i++)
			{
				// if the colour has changed
				if ((*linesToProcess)[i]->colour != thisRegion.RegionColour)
				{
					if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
					{
						std::cout << "Colour change Detected at Line " << i << " and Region Completed\n";
					}


					// complete last region and add it to the vector
					thisRegion.nEnd = i - 1;
					ColourRegionsOfLinesToProcess.push_back(thisRegion);

					// prepare for the next region
					thisRegion.RegionColour = linesToProcess->at(i)->colour;
					thisRegion.nStart = i;
					thisRegion.dXMin = MIN(linesToProcess->at(i)->pt1.x, linesToProcess->at(i)->pt2.x);
					thisRegion.dYMin = MIN(linesToProcess->at(i)->pt1.y, linesToProcess->at(i)->pt2.y);
					thisRegion.dXMax = MAX(linesToProcess->at(i)->pt1.x, linesToProcess->at(i)->pt2.x);
					thisRegion.dYMax = MAX(linesToProcess->at(i)->pt1.y, linesToProcess->at(i)->pt2.y);

				}

				// else the colour hasn't changed 
				else
				{
					// update the min and max values
					thisRegion.dXMin = MIN(MIN(linesToProcess->at(i)->pt1.x, linesToProcess->at(i)->pt2.x), thisRegion.dXMin);
					thisRegion.dYMin = MIN(MIN(linesToProcess->at(i)->pt1.y, linesToProcess->at(i)->pt2.y), thisRegion.dYMin);
					thisRegion.dXMax = MAX(MAX(linesToProcess->at(i)->pt1.x, linesToProcess->at(i)->pt2.x), thisRegion.dXMax);
					thisRegion.dYMax = MAX(MAX(linesToProcess->at(i)->pt1.y, linesToProcess->at(i)->pt2.y), thisRegion.dYMax);
				}

				// if this the very last line
				if (i == (linesToProcess->size() - 1))
				{
					// if we are still on the first region then we only have one colour so we can short circuit out of here ... baby!
					if (ColourRegionsOfLinesToProcess.empty())
					{
						nShortCircuit = 1;
						//break;
					}

					// else we have more than one colour region
					// and we will complete this region now
					thisRegion.nEnd = i;

					// Let's see if this region joins to first region because these lines should always form a complete loop
					if (!nShortCircuit && (thisRegion.RegionColour == ColourRegionsOfLinesToProcess[0].RegionColour))
					{
						// then we will join the first and last region by setting a flag
						thisRegion.bJoinFirstAndLast = ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast = 1;
						if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
						{
							std::cout << "Last Line and Region Joined to first Region (Both same colour)\n";
						}
					}
					else
					{
						if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
						{
							std::cout << "Last Line so Region completed\n";
						}
					}
					// add the very last region to the vector
					ColourRegionsOfLinesToProcess.push_back(thisRegion);
				}
			}// for remaining lines

				// we can stop here if we only have one colour
			if (nShortCircuit)
			{
				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
				{
					std::cout << "This perimeter only has one colour.\n";
				}
			}
			else
			{
				// log the region vector
				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
				{
					std::cout << "This perimeter has " << ColourRegionsOfLinesToProcess.size() << " Regions ";
					if (ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast)
					{
						std::cout << " and the first and last region are joined\n";
					}
					else
					{
						std::cout << " and the first and last region are not joined\n";
					}
					for (uint m = 0; m < ColourRegionsOfLinesToProcess.size(); m++)
					{

						std::cout << "Region " << m << " Colour " << ColourRegionsOfLinesToProcess[m].RegionColour << " Start= " << ColourRegionsOfLinesToProcess[m].nStart;
						std::cout << " End= " << ColourRegionsOfLinesToProcess[m].nEnd << " XMax,Xmin= " << ColourRegionsOfLinesToProcess[m].dXMax << ColourRegionsOfLinesToProcess[m].dXMin;
						std::cout << " YMax,YMin= " << ColourRegionsOfLinesToProcess[m].dYMax << ColourRegionsOfLinesToProcess[m].dYMin << "\n";
						std::cout << "XSpan, YSpan " << ColourRegionsOfLinesToProcess[m].dXMax - ColourRegionsOfLinesToProcess[m].dXMin << ", ";
						std::cout << ColourRegionsOfLinesToProcess[m].dYMax - ColourRegionsOfLinesToProcess[m].dYMin << "\n";
					}
				}// if logging

				//
				// in this section we will go through all the regions to ensure they span enough x or y pixels 
				// in the colourGrid (in the future) that they will form a printable island.  If the region is too short
				// it will be combined in the next region and take its colour

				// set the minimum span based on the extruder width * the scale factor 
				double dMinSpan = Settings::getSingleton().getDoubleValue("SplitLineMinimumSpanFactorOfExtrusionWidth") *
					Settings::getSingleton().getDoubleValue("T0extrusionWidth");

				int nIndex = 0;
				int i;
				double d, dX, dY;

				do
				{
					// Let's check if this region is same colour as the previous region and combine them.  
					//  This will occur when a little bit of colour is removed from between two regions of the same colour
					if ((nIndex > 0) && (ColourRegionsOfLinesToProcess[nIndex - 1].RegionColour == ColourRegionsOfLinesToProcess[nIndex].RegionColour))
					{
						// set the end of the lines of the last region to this regions and combine the min and max values
						ColourRegionsOfLinesToProcess[nIndex - 1].nEnd = ColourRegionsOfLinesToProcess[nIndex].nEnd;
						ColourRegionsOfLinesToProcess[nIndex - 1].dXMax = MAX(ColourRegionsOfLinesToProcess[nIndex].dXMax, ColourRegionsOfLinesToProcess[nIndex - 1].dXMax);
						ColourRegionsOfLinesToProcess[nIndex - 1].dYMax = MAX(ColourRegionsOfLinesToProcess[nIndex].dYMax, ColourRegionsOfLinesToProcess[nIndex - 1].dYMax);
						ColourRegionsOfLinesToProcess[nIndex - 1].dXMin = MIN(ColourRegionsOfLinesToProcess[nIndex].dXMin, ColourRegionsOfLinesToProcess[nIndex - 1].dXMin);
						ColourRegionsOfLinesToProcess[nIndex - 1].dYMin = MIN(ColourRegionsOfLinesToProcess[nIndex].dYMin, ColourRegionsOfLinesToProcess[nIndex - 1].dYMin);

						// I think that is everything so let's erase this colour region and advance to the next
						ColourRegionsOfLinesToProcess.erase(ColourRegionsOfLinesToProcess.begin() + nIndex);

						// we don't have to increment nIndex because we just deleted the current region
						continue;

					}

					// special case when first region is joined to last region
					// then combine the span to include both regions.
					if ((nIndex == 0) && ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast)
					{
						dX = (MAX(ColourRegionsOfLinesToProcess.begin()->dXMax, ColourRegionsOfLinesToProcess.back().dXMax) -
							MIN(ColourRegionsOfLinesToProcess.begin()->dXMin, ColourRegionsOfLinesToProcess.back().dXMin));
						dY = (MAX(ColourRegionsOfLinesToProcess.begin()->dYMax, ColourRegionsOfLinesToProcess.back().dYMax) -
							MIN(ColourRegionsOfLinesToProcess.begin()->dYMin, ColourRegionsOfLinesToProcess.back().dYMin));
					}
					else
					{
						dX = ColourRegionsOfLinesToProcess[nIndex].dXMax - ColourRegionsOfLinesToProcess[nIndex].dXMin;
						dY = ColourRegionsOfLinesToProcess[nIndex].dYMax - ColourRegionsOfLinesToProcess[nIndex].dYMin;
					}

					//let's check the span
					if ((dX < dMinSpan) && (dY < dMinSpan))
					{
						// oh oh, this colour span is too short

						// if this is not the first region, then combine it with the last as we know this one is already
						// sufficiently large and then we can go on from here.
						if (nIndex > 0)
						{
							// First change the colour of all the lines in this region to that of the last region
							for (i = ColourRegionsOfLinesToProcess[nIndex].nStart; i <= ColourRegionsOfLinesToProcess[nIndex].nEnd; i++)
							{
								linesToProcess->at(i)->colour = ColourRegionsOfLinesToProcess[nIndex - 1].RegionColour;
							}

							m_ColourRemaining = ColourRegionsOfLinesToProcess[nIndex - 1].RegionColour; // save this colour in case we are reducing down to a single colour

							// set the end of the lines of the last region to this regions and combine the min and max values
							ColourRegionsOfLinesToProcess[nIndex - 1].nEnd = ColourRegionsOfLinesToProcess[nIndex].nEnd;
							ColourRegionsOfLinesToProcess[nIndex - 1].dXMax = MAX(ColourRegionsOfLinesToProcess[nIndex].dXMax, ColourRegionsOfLinesToProcess[nIndex - 1].dXMax);
							ColourRegionsOfLinesToProcess[nIndex - 1].dYMax = MAX(ColourRegionsOfLinesToProcess[nIndex].dYMax, ColourRegionsOfLinesToProcess[nIndex - 1].dYMax);
							ColourRegionsOfLinesToProcess[nIndex - 1].dXMin = MIN(ColourRegionsOfLinesToProcess[nIndex].dXMin, ColourRegionsOfLinesToProcess[nIndex - 1].dXMin);
							ColourRegionsOfLinesToProcess[nIndex - 1].dYMin = MIN(ColourRegionsOfLinesToProcess[nIndex].dYMin, ColourRegionsOfLinesToProcess[nIndex - 1].dYMin);

							// I think that is everything so let's erase this colour region and advance to the next
							ColourRegionsOfLinesToProcess.erase(ColourRegionsOfLinesToProcess.begin() + nIndex);

							// we don't have to increment nIndex because we just deleted the current region
						}
						else // else we are still on the first region
						{
							// since we are still in the first region, then combine the next region into this and test again
							// only if the next region exists
							if ((ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast && (ColourRegionsOfLinesToProcess.size() > 2)) ||
								(!ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast && (ColourRegionsOfLinesToProcess.size() > 1)))
							{
								Colour col;
								if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
								{
									std::cout << "The very first region is too short\n";
								}

								// let's choose the colour from the region with the largest span
								d = MAX(ColourRegionsOfLinesToProcess[1].dXMax - ColourRegionsOfLinesToProcess[1].dXMin,
									ColourRegionsOfLinesToProcess[1].dYMax - ColourRegionsOfLinesToProcess[1].dYMin);

								if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
								{
									std::cout << "region 1 span " << d << ", Region 0 span " << MAX(dX, dY) << "\n";
								}

								if (d < MAX(dX, dY))
								{
									col = ColourRegionsOfLinesToProcess[0].RegionColour;
									if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
									{
										std::cout << "Region 0 is longer and it's colour is  " << col << "\n";
										std::cout << "Copy Region 0 Colour over all the lines in region 1\n";
									}
									for (i = ColourRegionsOfLinesToProcess[1].nStart; i <= ColourRegionsOfLinesToProcess[1].nEnd; i++)
									{
										linesToProcess->at(i)->colour = col;
									}
								}
								else
								{
									col = ColourRegionsOfLinesToProcess[1].RegionColour;
									ColourRegionsOfLinesToProcess[0].RegionColour = col;

									if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
									{
										std::cout << "Region 1 is longer and it's colour is  " << col << "\n";
										std::cout << "Copy Region 1 Colour over all the lines in region 0\n";
									}
									for (i = ColourRegionsOfLinesToProcess[0].nStart; i <= ColourRegionsOfLinesToProcess[0].nEnd; i++)
									{
										linesToProcess->at(i)->colour = col;
									}
								}

								m_ColourRemaining = col;   // let's save the colour to pass upwards to ExRSPolygon::straightSkeletonColours(bool isUVMap)


								// set the end of the lines of the last region to this regions and combine the min and max values
								ColourRegionsOfLinesToProcess[0].nEnd = ColourRegionsOfLinesToProcess[1].nEnd;
								ColourRegionsOfLinesToProcess[0].dXMax = MAX(ColourRegionsOfLinesToProcess[1].dXMax, ColourRegionsOfLinesToProcess[0].dXMax);
								ColourRegionsOfLinesToProcess[0].dYMax = MAX(ColourRegionsOfLinesToProcess[1].dYMax, ColourRegionsOfLinesToProcess[0].dYMax);
								ColourRegionsOfLinesToProcess[0].dXMin = MIN(ColourRegionsOfLinesToProcess[1].dXMin, ColourRegionsOfLinesToProcess[0].dXMin);
								ColourRegionsOfLinesToProcess[0].dYMin = MIN(ColourRegionsOfLinesToProcess[1].dYMin, ColourRegionsOfLinesToProcess[0].dYMin);

								// I think that is everything so let's erase this colour region and check again on the combined region

								ColourRegionsOfLinesToProcess.erase(ColourRegionsOfLinesToProcess.begin() + 1);
								// again, we don't have to increment nIndex because we just deleted the next region and combined it into this first region

							}
							else  // we only have one (or two joined) regions of one colour
							{
								// if we have combined all our regions into one colour we can short circuit out and see if the printer can print such a small island in one colour
								nShortCircuit = 1;
								break;
							}
						}
					}// if span is too small
					else
					{
						nIndex++;
					}

				} // while we have another region to check (if the first and last are joined then we have already tested the last region
				while (nIndex < ((ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast ? ColourRegionsOfLinesToProcess.size() - 1 : ColourRegionsOfLinesToProcess.size())));

				//how about some logging 
				if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
				{
					std::cout << "After testing each colour region for sufficient span and combining those too small into a neigbouring colour\n";
					std::cout << "we now have " << ColourRegionsOfLinesToProcess.size() << " regions ";
					if ((ColourRegionsOfLinesToProcess.size() > 1) && ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast)
					{
						std::cout << " and the first and last region are joined\n";
					}
					else if ((ColourRegionsOfLinesToProcess.size() > 1))
					{
						std::cout << " and the first and last region are not joined\n";
					}
					else
					{
						std::cout << "\n";
					}
					for (uint m = 0; m < ColourRegionsOfLinesToProcess.size(); m++)
					{

						std::cout << "Region " << m << " Colour " << ColourRegionsOfLinesToProcess[m].RegionColour << " Start= " << ColourRegionsOfLinesToProcess[m].nStart;
						std::cout << " End= " << ColourRegionsOfLinesToProcess[m].nEnd << " XMax,Xmin= " << ColourRegionsOfLinesToProcess[m].dXMax << ColourRegionsOfLinesToProcess[m].dXMin;
						std::cout << " YMax,YMin= " << ColourRegionsOfLinesToProcess[m].dYMax << ColourRegionsOfLinesToProcess[m].dYMin << "\n";
						std::cout << "XSpan, YSpan " << ColourRegionsOfLinesToProcess[m].dXMax - ColourRegionsOfLinesToProcess[m].dXMin << ", ";
						std::cout << ColourRegionsOfLinesToProcess[m].dYMax - ColourRegionsOfLinesToProcess[m].dYMin << "\n";
					}
					std::cout << "Lines To Process " << linesToProcess->size() << "\n";
					std::cout << "Line, Pt1X, Pt1Y, Pt2X, Pt2Y, Colour\n";

					for (uint m = 0; m < linesToProcess->size(); m++)
					{
						std::cout << m << ", " << (*linesToProcess)[m]->pt1.x << ", " << (*linesToProcess)[m]->pt1.y << ", " << (*linesToProcess)[m]->pt2.x << ", " << (*linesToProcess)[m]->pt2.y << ", " << (*linesToProcess)[m]->colour << "\n";
					}


				}// if logging
			}// else more than one colour region

			// We have all the lines by region so maybe !!! this is the time to sort them based only on span length, not by colour
			// this will be used to plotLine on the colour grid and can then be discarded.



			// Now let's create paths for our properly qualified line regions
			// We will store this in the object variable outerPerimeterPathByColour and it may already have information inside it.
			//
			// We also want to sort all the regions of lines by Span size, not by colour so the vector LineGroupsBySpan will also
			// be created.  This will be later sorted and all the lines will be plotted from this vector.
			//
			int nJoined = ColourRegionsOfLinesToProcess[0].bJoinFirstAndLast;
			double dSpan = 0.;
			int nLastRegion = ColourRegionsOfLinesToProcess.size() - 1;
			for (uint m = 0; m < (ColourRegionsOfLinesToProcess.size() - nJoined); m++)
			{

				uint32_t cHash = ColourRegionsOfLinesToProcess[m].RegionColour.hash();  //create the hash value for this colour for this region


				// check if this colour is in the map
				if (!outerPerimeterPathByColour.count(cHash))
				{
					// else create a new map entry pair of spans and paths
					outerPerimeterPathByColour[cHash] = std::vector<BodySplitter::vert2DPath>();
				}

				// push back a new path onto the vector.  This will hold the path for this colour region
				outerPerimeterPathByColour[cHash].push_back(BodySplitter::vert2DPath());

				// push back a new group of lines onto LineGroupsBySpan vector
				LineGroupsBySpan.push_back(std::pair <double, linePtrSet>());

				// special case when first region is joined to the last region
				// this is the only time we will have a broken line so
				// we should start the path with the last region
				// and combine the span
				if (m == 0 && nJoined)
				{
					//combine max span
					dSpan = MAX((MAX(ColourRegionsOfLinesToProcess.begin()->dXMax, ColourRegionsOfLinesToProcess.back().dXMax) -
						MIN(ColourRegionsOfLinesToProcess.begin()->dXMin, ColourRegionsOfLinesToProcess.back().dXMin)),
						(MAX(ColourRegionsOfLinesToProcess.begin()->dYMax, ColourRegionsOfLinesToProcess.back().dYMax) -
							MIN(ColourRegionsOfLinesToProcess.begin()->dYMin, ColourRegionsOfLinesToProcess.back().dYMin)));


					// get point 1 from last region
					//Holy redirection Batman
					// The following will add the first point from the first line in the very last coloured region as a vertex to the m'th path, just to get us started
					outerPerimeterPathByColour[cHash].back().poly.push_back(Vertex2D(linesToProcess->at(ColourRegionsOfLinesToProcess[nLastRegion].nStart)->pt1));

					// for each line in the last region we will add point 2 to the path as we don't need to repeat point 1
					for (int i = ColourRegionsOfLinesToProcess[nLastRegion].nStart; i <= ColourRegionsOfLinesToProcess[nLastRegion].nEnd; i++)
					{
						outerPerimeterPathByColour[cHash].back().poly.push_back(Vertex2D(linesToProcess->at(i)->pt2));

						// we will also add these lines to the LineGroupsBySpan vector
						LineGroupsBySpan.back().second.push_back(linesToProcess->at(i));
					}

					// now we will simply drop through to finish adding the first region

				}
				else
				{
					//get the max span
					dSpan = MAX(ColourRegionsOfLinesToProcess[m].dXMax - ColourRegionsOfLinesToProcess[m].dXMin,
						ColourRegionsOfLinesToProcess[m].dYMax - ColourRegionsOfLinesToProcess[m].dYMin);

					// start the path with point 1 from the first line
					outerPerimeterPathByColour[cHash].back().poly.push_back(Vertex2D(linesToProcess->at(ColourRegionsOfLinesToProcess[m].nStart)->pt1));
				}

				// and now insert this region into the map
				outerPerimeterPathByColour[cHash].back().colour = ColourRegionsOfLinesToProcess[m].RegionColour;  // the colour to assign to this path

				LineGroupsBySpan.back().first = dSpan;

				// for each line in the region we will add point 2 to the path as we don't need to repeat point 1
				for (int i = ColourRegionsOfLinesToProcess[m].nStart; i <= ColourRegionsOfLinesToProcess[m].nEnd; i++)
				{
					outerPerimeterPathByColour[cHash].back().poly.push_back(Vertex2D(linesToProcess->at(i)->pt2));

					// we will also add these lines to the LineGroupsBySpan vector
					LineGroupsBySpan.back().second.push_back(linesToProcess->at(i));
				}

			}// for all colour regions

		}// if there lines to process
	}// while not done processing line and holes
	if (gl_nSEL  && Settings::getSingleton().getBoolValue("logLineSplitting")) //SEL debug
	{
		std::cout << "Number of Outer Perimeter Paths formed for this polygon and holes " << outerPerimeterPathByColour.size() << "\n";
		int n = 0;
		for (auto it : outerPerimeterPathByColour)
		{
			std::cout << n++ << " Colour " << it.first << " Path quantity = " << it.second.size() << "\n";
		}
		std::cout << "-------- end of addSplitLineToColourPaths ----------------------\n\n";
	}

	// If we have holes, then it is possible to have multiple paths, but they might still all be the same colour.   
	// We must test for that and then abort the crazy infill step
}

//SEL
//
// plotPath
//
// Purpose:   This method will iterate through all the different colours of the outer and hole perimeter paths (split on colour regions)
//            and plot the lines in the colour grid
//
//			These paths should have been sorted from longest to shortest span independant of colour (by  sortPath)
//
void BodySplitter::colourGrid::plotPath()
{

	for (auto it : LineGroupsBySpan)  //each region of lines
	{

		for (auto itPathLine : it.second)  // for all the lines in this path of colour region
		{
			plotLine(itPathLine);

		}
	}
}

// SEL
//The following two functions will perform the path sorting
//I'm sure that there is a better way of doing this, but I don't know what that is.
//
void MySwap(std::vector<std::pair <double, BodySplitter::linePtrSet>> &path, int x, int y)
{
	std::pair <double, BodySplitter::linePtrSet> temp = path[x];
	path[x] = path[y];
	path[y] = temp;

}

void MyQuickSortPath(std::vector<std::pair<double, BodySplitter::linePtrSet>> &path, int L, int R )
{
	int i, j, mid;
	i = L;
	j = R;
	mid = L + (R - L) / 2;
	double piv = path[mid].first;

	while (i<R || j>L) {
		while (path[i].first > piv)  //while (path[i].first < piv)
			i++;
		while (path[j].first < piv)  //while (path[j].first > piv)
			j--;

		if (i <= j) {
			MySwap(path, i, j); 
			i++;
			j--;
		}
		else {
			if (i < R)
				MyQuickSortPath(path, i, R);
			if (j > L)
				MyQuickSortPath(path, L, j);
			return;
		}
	}
}

//
// SEL   sortPaths
//
// Purpose:   This method will sort all the outer perimeter and holes paths (in line form), split by colour, from the
// largest span to the smallest 
//
void BodySplitter::colourGrid::sortPaths()
{
	// This was easier than I anticipated -  Duh, a single line.   Does that really take a method?  
	MyQuickSortPath(LineGroupsBySpan,0, LineGroupsBySpan.size()-1);
}


//SEL March 2020
//
// QualifyInsetVertices
//
// Purpose:   This method is the first step in eliminating the colour bleeding artefacts that are
//            caused when an inset line starts or ends very close to the outside perimeter, or even in
//            some cases even outside the body entirely.  This occurs because we desire the grid size to be course and 
//            aliasing can occur.  In this method, we will go through all the insets by colour, path and then vertex.
//            We will test each vertex against the overall polygon and holes and if it is outside or within BleedThreshold
//            Of the exterior we will flag it as invalid.   We will not remove it yet because that would 
//			  cause problems with createIslands.
//
void BodySplitter::colourGrid::QualifyInsetVertices()
{
	double dBleedThreshold = Settings::getSingleton().getDoubleValue("BleedThreshold");
	double dMinDistance=0.;

	// qualify all the vertices 
	for (auto &itColour : insetPathByColour)  // for each colour
	{
		for (auto &lineIT : itColour.second)   // for each path
		{
			for (auto &pt : lineIT.poly)  // for each Vertex2D
			{
				if (BodySplitter::isPointInPoly(pt, m_lines))// of the point is inside the poly
				{
					for (int j = 0; j < m_holes.size(); j++)//test all holes
					{
						if (BodySplitter::isPointInPoly(pt, m_holes[j]))
						{
							pt.bValid = false; // the point is inside the polygon but also inside a hole
							break;
						}
						else // The point is not in a hole but lets check how close to the surface of the hole
						{
							if (BodySplitter::dMinimumDistancePtToPolygon(pt, m_holes[j]) <= dBleedThreshold)
							{
								pt.bValid = false; //too close so out you go!
								break; //can stop checking the rest of the holes
							}
						}
					}

					// The point is inside the polygon so let's check how close it is to the surface
					if(BodySplitter::dMinimumDistancePtToPolygon(pt, m_lines) <= dBleedThreshold)
					{
						pt.bValid = false;//too close so out you go!
					}
				}
				else // point is outside polygon and is invalid
				{
					pt.bValid = false;
				}
			}//for each Vertex2D
		}// for each path
	}// for each colour
}

//SEL March 2020
//
//  RemoveInvalidVerticesFromIslands
//
// Purpose:		This method is fairly obvious by its name.  It is the second part of eliminating colour bleeding
//				as described for QualifyInsetVertices.   We will traverse through all the islands and remove any
//				vertex that is flagged invalid.
//
void BodySplitter::colourGrid::RemoveInvalidVerticesFromIslands()
{
	// The islands are stored in the member vector "output"
	int nIslandIndex, nPointIndex;

	for (nIslandIndex = 0; nIslandIndex < output.size(); nIslandIndex++)  // for each island
	{
		for (nPointIndex = 0; nPointIndex < output[nIslandIndex].poly.size();  )// for each vertex
		{
			if (!output[nIslandIndex].poly[nPointIndex].bValid)  // how do I erase this?
			{
				output[nIslandIndex].poly.erase(output[nIslandIndex].poly.begin() + nPointIndex);  //Will this work???
			}
			else
			{
				nPointIndex++;  // only advance the index if we are not deleting a vertex.
								// if we delete a vertex, then the index is already pointing at the next vertex
			}
		}
	}
}

//SEL March 2020
//    Open the log file for the ruby script that will visualize the outer line fragments, inset and the
//    created islands.  I have now set the global logging variable gl_nSEL to equal the layer number to log
//	  so I will take advantage of that here.  (oh I as so clever ... and good looking too!)
//
void BodySplitter::colourGrid::RubyOpenLogFile()
{
	char sz[256];

	if (m_fRuby != nullptr)
	{
		fclose(m_fRuby);
	}

	sprintf(sz, "RubyScriptLayer%d.rb", gl_nSEL);

	m_fRuby = fopen(sz, "wt");

	if (m_fRuby != nullptr)
	{
		fprintf(m_fRuby,	"Sketchup.active_model.rendering_options[\"EdgeColorMode\"]=0\n"
							"model = Sketchup.active_model\n"
							"ents = model.entities\n"
							"view = model.active_view\n"
							"pts = []\nline = []\n\n");
	}
}

void BodySplitter::colourGrid::RubyCloseLogFile()
{
	if (m_fRuby != nullptr)
	{
		fprintf(m_fRuby, "view.refresh\n");
		fclose(m_fRuby);
	}
}

void BodySplitter::colourGrid::RubyScriptExterior()
{

	// everything we need is in here:  outerPerimeterPathByColour
	if (m_fRuby == nullptr)
		return;

	int i, j, k;

	double dZ = Settings::GetSingleton().getDoubleValue("firstLayerThickness") +
		((double)gl_nSEL) * Settings::GetSingleton().getDoubleValue("layerThickness");

	i = 0;
	for (auto &it : outerPerimeterPathByColour)
	{
		j = 0;
		for (auto &lineIT : it.second)
		{
			fprintf(m_fRuby, "pts.clear\n");

			k = 0;
			for (auto &pt : lineIT.poly)
			{
				fprintf(m_fRuby, "pts[%d] = [%f, %f, %f]\n",k, pt.x, pt.y, dZ);
//				if (k > 0)
//				{
//					fprintf(m_fRuby, "line[%d] = ents.add_line(pts[%d],pts[%d])\n", k - 1, k - 1, k);
//					fprintf(m_fRuby, "line[%d].material = 0x%0x\n", k - 1, it.first);
//				}
				k++;
			}
			fprintf(m_fRuby, "mycurve = ents.add_curve pts\n");
			fprintf(m_fRuby, "mycurve.each {|e| e.material = 0x%0x}\n", it.first);
			fprintf(m_fRuby, "group = ents.add_group mycurve\n");
			fprintf(m_fRuby, "group.name = \"Exterior%d-%d-%d\"\n", i, j, k);
			j++;
		}
		i++;
	}
}

void BodySplitter::colourGrid::RubyScriptInsets()
{
	if (m_fRuby == nullptr)
		return;

	int i,j,k;

	double dZ = Settings::GetSingleton().getDoubleValue("firstLayerThickness") +
		((double)gl_nSEL) * Settings::GetSingleton().getDoubleValue("layerThickness");

	i = 0;
	for (auto &it : insetPathByColour)
	{
		j = 0;
		for (auto &lineIT : it.second)
		{
			fprintf(m_fRuby, "pts.clear\n");

			k = 0;
			for (auto &pt : lineIT.poly)
			{
				fprintf(m_fRuby, "pts[%d] = [%f, %f, %f]\n", k, pt.x, pt.y, dZ);
				if (!pt.bValid)  // place a construction point at invalid points
				{
					fprintf(m_fRuby,"ents.add_cpoint [%f, %f, %f]\n", pt.x, pt.y, dZ);
				}
				k++;
			}
			fprintf(m_fRuby, "mycurve = ents.add_curve pts\n");
			fprintf(m_fRuby, "mycurve.each {|e| e.material = 0x%0x}\n", it.first);
			fprintf(m_fRuby, "group = ents.add_group mycurve\n");
			fprintf(m_fRuby, "group.name = \"Inset%d-%d-%d\"\n",i,j,k );
			j++;
		}
		i++;
	}
}

void BodySplitter::colourGrid::RubyScriptIslands(int bRawIsland)
{
	if (m_fRuby == nullptr)
		return;

	double dZ = Settings::GetSingleton().getDoubleValue("firstLayerThickness") + 
		((double)gl_nSEL ) * Settings::GetSingleton().getDoubleValue("layerThickness");

	// The islands are stored in the member vector "output"
	//Look to void Mesh::LogPerimeters() for guidance on exporting to Ruby script
	int nIsland = 0;
	for (auto itIsland : output)  // for each island
	{
		fprintf(m_fRuby, "pts.clear\n");
		int i = 0;
		for (auto itPoint : itIsland.poly)// for each vertex
		{
			fprintf(m_fRuby, "pts[%d] = [%f, %f, %f]\n",i, itPoint.x, itPoint.y, dZ);
			i++;
		}
		// loop back to start point to close the curve so sketchup will create a face
		fprintf(m_fRuby, "pts[%d] = [%f, %f, %f]\n", i, itIsland.poly[0].x, itIsland.poly[0].y, dZ);
		fprintf(m_fRuby, "face = ents.add_face ents.add_curve pts\n");
		fprintf(m_fRuby, "face.material = [%d,%d,%d]\n", (int)itIsland.colour.getR<uint8_t>(), 
														 (int)itIsland.colour.getG<uint8_t>(),
														 (int)itIsland.colour.getB<uint8_t>());
		fprintf(m_fRuby, "face.back_material = [%d,%d,%d]\n", (int)itIsland.colour.getR<uint8_t>(),
														 (int)itIsland.colour.getG<uint8_t>(),
														 (int)itIsland.colour.getB<uint8_t>());
		fprintf(m_fRuby, "group = ents.add_group face\n");
		fprintf(m_fRuby, "group.name = \"%s-%d\"\n", bRawIsland ? "RawIsland" : "ModifiedIsland", nIsland);
		nIsland++;
	}

}

void BodySplitter::colourGrid::smoothInsetLines(vert2DPath &inpath)
{
	inpath = BodySplitter::smoothLines(inpath);
}


/* This test no longer tests anything */
// bool BodySplitter::colourGrid::_TEST_allLinesHaveGridEnd()
// {
//   std::set<Line*> startPts;
//   std::set<Line*> endPts;
//   
//   for(auto it : m_lines)
//   {
//     startPts.insert(it.get());
//     endPts.insert(it.get());
//   }
//   
// //   for(int x = 0; x < xCount; x++)
// //   {
// //     for(int y = 0; y < yCount; y++)
// //     {
// //       if(grid[x][y].isLineEnd)
// //       {
// // 	if(startPts.count(grid[x][y].leftLine) > 0)
// // 	  startPts.erase(grid[x][y].leftLine);
// // 	/*else*/ if(startPts.count(grid[x][y].rightLine) > 0)
// // 	  startPts.erase(grid[x][y].rightLine);
// // 	/*else*/ if(endPts.count(grid[x][y].leftLine) > 0)
// // 	  endPts.erase(grid[x][y].leftLine);
// // 	/*else*/ if(endPts.count(grid[x][y].rightLine) > 0)
// // 	  endPts.erase(grid[x][y].rightLine);
// // // 	else
// // // 	{
// // // 	  std::cout << "Line end doesnt still exist! Test failed\n";
// // // 	  return false;
// // // 	}
// //       }
// //     }
// //   }
//   // Remove discardLines
//   for(auto it : discardLines)
//   {
//     if(startPts.count(it) > 0)
//       startPts.erase(it);
//     if(endPts.count(it) > 0)
//       endPts.erase(it);
//   }
//   if(discardLines.empty() == false)
//     std::cout << "Discard: "<<discardLines.size()<<" lines discarded due to length < 1px\n";
//   if(startPts.size() > 0 || endPts.size() > 0)
//   {
//     std::cout << "Not all end points exist on the grid\n";
//     std::cout << "StartPts.size()="<<startPts.size()<<"\n";//<<" EndPts.size()="<<endPts.size()<<"\n";
//     for(auto it : startPts)
//       std::cout << "startLine:"<<it<<"\n";
//     std::cout << "EndPts.size()="<<endPts.size()<<"\n";
//     for(auto it : endPts)
//       std::cout << "endLine:"<<it<<"\n";
//     
//     return false;
//   }
//   return true;
// }

std::size_t BodySplitter::colourGrid::_TEST_lineEndCount()
{
	std::size_t count = 0;
	for (int x = 0; x < xCount; x++)
	{
		for (int y = 0; y < yCount; y++)
		{
			if (grid[x][y].isLineEnd)
			{
				//std::cout << "LeftLine:"<<grid[x][y].leftLine<<" RightLine:"<<grid[x][y].rightLine<<"\n";
				count++;
			}
		}
	}
	return count;
}