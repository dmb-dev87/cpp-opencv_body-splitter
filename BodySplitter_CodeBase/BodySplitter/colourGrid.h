#pragma once

#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include "Geometry.h"
class Line;

/* Used for generating the colour information for crazy infill
Orignally this code was based on straight skeleton but was replaced
When slicing full colour this class requires the greatest CPU time
any attempt to optimze for performance should start here*/

namespace BodySplitter {
  // Type definitions
  using linePtrSet = std::vector<std::shared_ptr<Line>>;
  
  
  /* Represents a colour on a grid coord
   * TODO: merge with existing colour struct */
struct gridColour {
  uint8_t r,g,b;
  bool operator==(const gridColour &rhs)
  {
    return (r == rhs.r && g == rhs.g && b == rhs.b);
  }
  bool operator!=(const gridColour &rhs)
  {
    return !(r == rhs.r && g == rhs.g && b == rhs.b);
  }
  explicit gridColour(uint32_t in) {
    r = in & 0xFF;
    g = (in & 0xFF00) >> 8;
    b = (in & 0xFF0000) >> 16;
  }
  gridColour() : r(255),g(255),b(255) {}
  gridColour(uint8_t rIn, uint8_t gIn, uint8_t bIn) : r(rIn), g(gIn), b(bIn) {}
  explicit operator uint32_t() {
    return (r | g<<8 | b<<16);
  }
};

/* Each grid point is this structure
 * TODO: Reduce memory usage here */
struct gridPix {
  Colour colour;
  uint8_t state = 0;
  bool isTemp = false;
  bool isLineEnd = false;
  
  
  uint8_t border = BORDER_NULL;
  
  const static uint8_t BORDER_EMPTY = 0;
  const static uint8_t BORDER_LEFT = 1;
  const static uint8_t BORDER_RIGHT = 2;
  const static uint8_t BORDER_TOP = 4;
  const static uint8_t BORDER_BOTTOM = 8;
  static const uint8_t BORDER_NULL = 16;	//unprocessed/uncalced
  static const uint8_t LINE_UPON = 32;
  
  const static uint8_t EMPTY = 0;
  const static uint8_t INSIDE_PT = 1;
  const static uint8_t COLOUR_POINT = 2;
  const static uint8_t FILLED = 3;
};

struct listPoint {
  /* 0 1 2
   * 3 X 4
   * 5 6 7
   */
  gridPix* thisOne;
  gridPix *neighbours[8] = {nullptr};
};

class colourGrid 
{
private:
  // SEL removed std::list<listPoint> unfillPts;
  int xCount, yCount;
  int stepCount = 0;
  std::vector<linePtrSet> m_holes;	//Set of holes
  std::vector<Line> borderLines;
  linePtrSet m_lines;
  double epsilon = 1;
  double minX, minY, gridSize, maxX, maxY;
  
  // SEL removed std::set<Line*> discardLines; // These lines were too short to plot(1px)
  
  gridPix** grid = nullptr;
  gridPix** tempGrid = nullptr;
  
  // Methods
  void plotLine(const std::shared_ptr<Line> &line);
  bool gridHasUnfilled() const;
  
  void updatePoint(int xPos, int yPos);
  std::vector<Line> borderToLine(const gridPix &pt, int x, int y);
  std::unordered_map<uint32_t,std::vector<vert2DPath>> insetPathByColour;

  // SEL added
  // This is the colour paths of all exterior and hole perimeters
public:
  std::unordered_map<uint32_t, std::vector<vert2DPath>> outerPerimeterPathByColour;
  Colour m_ColourRemaining;   // when the perimeter has been reduced to one colour it will be stored here for use by 
							  // ExRSPolygon::straightSkeletonColours to set the polygon colour as it may
							  // have changed
private:
  void addSplitLineToColourPaths();

  std::vector < std::pair <double, linePtrSet>> LineGroupsBySpan;

  void plotPath();
  void sortPaths();
  // SEL end


  //SEL March 2020 add methods to qualify vertices on inset lines to prevent colour bleeding along outer perimeter
  void QualifyInsetVertices();
  void RemoveInvalidVerticesFromIslands();
  // log a ruby script for visualization in Sketchup for specified layer, outer perimeters, boundaries and islands
  void RubyOpenLogFile();
  void RubyCloseLogFile();
  void RubyScriptExterior();
  void RubyScriptInsets();
  void RubyScriptIslands(int bRawIsland);
  FILE *m_fRuby = nullptr;
  //SEL end

  mutable bool gridSolved = false;
  void smoothInsetLines(vert2DPath &inpath);
   std::vector<BodySplitter::vert2DPoly> output;
   void removeLonePixels();
   bool isLonePixel(int x, int y);
   bool plottedSinglePixel = false;
public:
  void setMinX(double min) {minX=min;}
  void setMinY(double min) {minY=min;}
  void setMaxX(double max) {maxX=max;}
  void setMaxY(double max) {maxY=max;}
  void setGridSize(double griddy) {gridSize=griddy;}
  
  void addLineSet(linePtrSet &lines);
  void addLineSet(const std::vector<Line> &lines);
  void addHoleSet(linePtrSet &holes);
  void addHoleSet(const std::vector<Line> &holes);
  
  void printGridDebugInfo() const;
  void printGrid() const;
  int initializeGrid();
  void solveGrid();
  bool isSolved() const{return !gridHasUnfilled();}
  void solutionStep();
  void createBorderLines();	// Creates the lines, skips the sauce lines
  void createIslands();
  
  std::vector<BodySplitter::vert2DPoly>& getResult() {return output;}
  
  std::pair<int,int> getPixel(const Vertex &inputPt) const;
  
  //Get 'er done
  double getMinX() const {
    return minX;
  }
  double getMinY() const {
    return minY;
  }
  double getMaxX() const {
    return maxX;
  }
  double getMaxY() const {
    return maxY;
  }
  double getGridSize() const {
    return gridSize;
  }
  
  // Test methods
  bool _TEST_allLinesHaveGridEnd();
  std::size_t _TEST_lineEndCount();
  
  ~colourGrid() {
    if(tempGrid)
    {
      for(int x = 0; x < xCount; x++)
      {
	delete[] tempGrid[x];
      }
      delete[] tempGrid;
    }
    if(grid)
    {
      for(int x = 0; x < xCount; x++)
      {
	delete[] grid[x];
      }
      delete[] grid;
    }
  }
};

} //namespace BodySplitter