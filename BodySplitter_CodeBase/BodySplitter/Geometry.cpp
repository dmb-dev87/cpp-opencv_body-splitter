#include "Geometry.h"
#include <math.h>

#include "Triangle.h"
#include "Vertex.h"
#include "Line.h"
#include "Perimeter.h"
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <unordered_set>
#include "Settings.h"

extern int gl_nSEL;   //SEL debug - verbose output if true;


const std::pair<int, double> findMaximumDistance(const std::vector<Vertex2D>& Points) {
	Vertex2D firstpoint = Points.front();
	Vertex2D lastpoint = Points.back();
	int index = 0;  //index to be returned
	double Mdist = -1; //the Maximum distance to be returned

	//distance calculation
	Vertex2D p = lastpoint - firstpoint;
	for (int i = 1; i < Points.size() - 1; i++) { //traverse through second point to second last point
		Vertex2D pp = Points[i] - firstpoint;
		double Dist = fabs(pp * p) / p.Norm(); //formula for point-to-line distance
		if (Dist > Mdist) {
			Mdist = Dist;
			index = i;
		}
	}
	return std::make_pair(index, Mdist);
}
// 
namespace BodySplitter
{
	// 
	// Used for ipe file output
	const double shift = 400;


	//SEL March 2020   add some helper geometry functions
	//
	// I know how to derive this from first principles even for 3D but this time I was lazy
	// and just googled it.
	//https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
	//
	double dMinimumDistancePtToLineSegment(const Vertex & vert, std::shared_ptr<Line> line) noexcept
	{
		Vertex a, b, projection;

		// Return minimum distance between line segment line and point vert
		

		// check the length of the line segment
		double l2 = SQR(line->pt1.x - line->pt2.x) +
					SQR(line->pt1.y - line->pt2.y);		// length squared i.e. |pt1-pt2|^2 -  avoid a sqrt

		if (l2 > 0.00001)    // pt1 == pt2 but I won't use an == for floating point numbers
		{
			return DIST2D(vert, line->pt1);  // The line segment is zeroish length so the distance is simply
											// from the point (vert) to either line end 
		}
												
		// Consider the line extending the segment, parameterized as pt1 + t (pt2 - pt1).
		// We find the projection of point vert onto the line-> 
		// It falls where t = [(vert-pt1) . (pt2-pt1)] / |pt2-pt1|^2
		// We clamp t from [0,1] to handle points outside the segment pt1 pt2. (got it!  This simple and clever)
		a.x = vert.x - line->pt1.x;
		a.y = vert.y - line->pt1.y;
		b.x = line->pt2.x - line->pt1.x;
		b.y = line->pt2.y - line->pt1.y;
		double t = MAX(0, MIN(1, DOT2D(a, b) / l2));
		projection.x = line->pt1.x + t * (b.x);  // Projection falls on the segment
		projection.y = line->pt1.y + t * (b.y);  // Projection falls on the segment
		return DIST2D(vert, projection);
	}

	// This one is simple.  Just go through all the line segments of the poly a keep the minimum
	double dMinimumDistancePtToPolygon(const Vertex & vert, const std::vector<std::shared_ptr<Line>> &poly) noexcept
	{
		std::size_t   i;
		double dMinDistance;
		
		dMinDistance = dMinimumDistancePtToLineSegment(vert, poly[0]);

		for (i = 1; i < poly.size(); i++)
		{
			dMinDistance = MIN(dMinDistance,dMinimumDistancePtToLineSegment(vert, (poly[i])));

		}
		return dMinDistance;
	}
	//SEL end


	//SEL I dind't have a clue what was happening here but a quick google search helped
	//
	//  https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
	//
	bool isPointInPoly(const Vertex &vert, const Lines &poly) noexcept
	{
		std::size_t   i, j = poly.size() - 1;
		bool  oddNodes = false;

		for (i = 0; i < poly.size(); i++) {
			if ((poly[i].pt2.y < vert.y && poly[j].pt2.y >= vert.y
				|| poly[j].pt2.y < vert.y && poly[i].pt2.y >= vert.y)
				&& (poly[i].pt2.x <= vert.x || poly[j].pt2.x <= vert.x)) {
				oddNodes ^= (poly[i].pt2.x + (vert.y - poly[i].pt2.y) / (poly[j].pt2.y - poly[i].pt2.y)*(poly[j].pt2.x - poly[i].pt2.x) < vert.x);
			}
			j = i;
		}

		return oddNodes;
	}

	vert2DPath linePathToVertPath(const std::vector< Line >& sourceLines)
	{
		vert2DPath output;
		output.poly.push_back(sourceLines.front().pt1);
		output.poly.push_back(sourceLines.front().pt2);
		for (int i = 1; i < sourceLines.size(); ++i)
		{
			output.poly.push_back(sourceLines[i].pt2);
		}
		output.colour = sourceLines.front().colour;
		return output;
	}


	bool isPointInPoly(const Vertex &vert, const std::vector<std::shared_ptr<Line>> &poly) noexcept
	{
		std::size_t   i, j = poly.size() - 1;
		bool  oddNodes = false;

		for (i = 0; i < poly.size(); i++) {
			if ((poly[i]->pt2.y < vert.y && poly[j]->pt2.y >= vert.y
				|| poly[j]->pt2.y < vert.y && poly[i]->pt2.y >= vert.y)
				&& (poly[i]->pt2.x <= vert.x || poly[j]->pt2.x <= vert.x)) {
				oddNodes ^= (poly[i]->pt2.x + (vert.y - poly[i]->pt2.y) / (poly[j]->pt2.y - poly[i]->pt2.y)*(poly[j]->pt2.x - poly[i]->pt2.x) < vert.x);
			}
			j = i;
		}

		return oddNodes;
	}
	// 	bool pointTouchesPolyCorner(const Vertex &vert1, const Vertex &vert2, const Lines &poly) noexcept
	// 	{
	// 		for (auto &it : poly)
	// 		{
	// 			if (matchesR2Location(it.pt2, vert1,1e-1) && matchesR2Location(it.pt2, vert2,1e-1))
	// 				return true;
	// 		}
	// 		return false;
	// 	}
	// 	bool matchesR2Location(const Vertex &pt1, const Vertex &pt2, double threshold) noexcept
	// 	{
	// 		return (fabs(pt1.x - pt2.x) > threshold && fabs(pt1.y - pt2.y) > threshold);
	// 	}
	// 	
	std::vector< Vertex2D > simplifyPolygon(const std::vector< Vertex2D >& inPoly, double ep)
	{
		if (inPoly.size() <= 3)
			return inPoly;

		std::pair<int, double> maxDistance = findMaximumDistance(inPoly);
		if (maxDistance.second >= ep) {
			int index = maxDistance.first;
			std::vector<Vertex2D>::const_iterator it = inPoly.begin();
			std::vector<Vertex2D> path1(inPoly.begin(), it + index + 1); //new path l1 from 0 to index
			std::vector<Vertex2D> path2(it + index, inPoly.end()); // new path l2 from index to last

			std::vector<Vertex2D> r1 = simplifyPolygon(path1, ep);
			std::vector<Vertex2D> r2 = simplifyPolygon(path2, ep);

			//Concat simplified path1 and path2 together
			std::vector<Vertex2D> rs(r1);
			rs.pop_back();
			rs.insert(rs.end(), r2.begin(), r2.end());
			return rs;
		}
		else { //base case 2, all points between are to be removed.
			std::vector<Vertex2D> r(1, inPoly.front());
			r.push_back(inPoly.back());
			return r;
		}
	}

	//
	// brokenLinesToVert2DPath   broken lines to vertex2D lines with colour
	//
	// Purpose:   This will take the unordered broken lines and create a new vector of 2D vertices suitable for making
	//            a polygon.  It will arrange the vertices in order in a continous path joining the ends of lines
	//            
	// This is quite clever as it works towards both ends of the path at the same time using a deque container Class.  How
	// does Robert know all these things?!?!?  Unfortunately 
	// it has a problem that only occurs when the colour grid forms a cross.  This will have 3 matched segments
	// and we should choose not to cross the paths (Don't cross the streams,  Bill Murray, Ghost Busters)
	// crossing the paths can cause a polygon that crosses itself and may result in a section of the polygon
	// being dropped when the offset polygon is clipped.
	//
	vert2DPath brokenLinesToVert2DPath(std::vector<Line>& inlines, uint8_t r, uint8_t g, uint8_t b)
	{
		vert2DPath output;

		std::deque<Vertex2D> workingDeck;
		if (inlines.empty())
			return output;
		workingDeck.push_back(Vertex2D(inlines.front().pt1));
		workingDeck.push_back(Vertex2D(inlines.front().pt2));
		inlines.erase(inlines.begin());

#if 1 // new SEL way that will look for more matches
		// create a handy data struct to hold the candidate matches
		// There can only be 0, 1, or 3 matches on a grid point
		struct
		{
			int nIndex;
			int nPt1;
			int nDirection;
		}dtMatches[16];

		int foundMatch = 1;
		int matches;
		int nDir,nCandidateDir;

		while (foundMatch)
		{
			foundMatch = 0;

			for (int nFront = 0; nFront < 2; nFront++)
			{
				matches = 0;
				// go through all the lines to process and find a match of either point 1 or point 2 continuing the last segment of the path
				// we can stop searching if there are 3 matches
				for (int n = 0; n < inlines.size(); n++)
				{
					for (int nPt1 = 1; nPt1 >= 0; nPt1--)
					{
						if ((nFront && (workingDeck.front().MatchesLocation(nPt1 ? inlines[n].pt1 : inlines[n].pt2))) ||
							(!nFront && (workingDeck.back().MatchesLocation(nPt1 ? inlines[n].pt1 : inlines[n].pt2))))
						{
							if (matches < 16)
							{
								dtMatches[matches].nIndex = n;
								dtMatches[matches].nPt1 = nPt1;
							}
							matches++;
							foundMatch = 1;
						}
						//if (foundMatch >= 16)  // We should never have more than 3 matches
						//	break;
					}
					//if (foundMatch >= 16)  // We should never have more than 3 matches
					//	break;
				}// for all the lines

				 // This condition should never occur
				if (matches && (matches != 1) && (matches !=3 ))  // found a match that is not 1 or 3 
				{
					std::cerr << "found more matches creating a border line " << matches << "\n";
					matches = 1;
				}

				if (matches == 1)  // Simple case, we only have a single line segment to join
				{
					if (nFront)
					{
						workingDeck.push_front(Vertex2D(dtMatches[0].nPt1 ? inlines[dtMatches[0].nIndex].pt2 : inlines[dtMatches[0].nIndex].pt1));
					}
					else
					{
						workingDeck.push_back(Vertex2D(dtMatches[0].nPt1 ? inlines[dtMatches[0].nIndex].pt2 : inlines[dtMatches[0].nIndex].pt1));
					}
					inlines.erase(inlines.begin() + dtMatches[0].nIndex);
				}

				else if (matches == 3)  // This is the more complicated case where the lines cross
				{
					Vertex ptFrom, ptTo, ptNew;
					int nLen;

					// find the direction we are coming from by getting the last two points of the path at the end we are joining
					if (nFront)
					{
						ptFrom = workingDeck[1];
						ptTo = workingDeck[0];
					}
					else
					{
						nLen = workingDeck.size();
						ptFrom = workingDeck[nLen-2];
						ptTo = workingDeck[nLen-1];
					}

					// determine direction of travel for the last segment added to the path
					if (fabs(ptTo.x - ptFrom.x) > fabs(ptTo.y - ptFrom.y))
					{
						if (ptFrom.x < ptTo.x)
						{
							nDir = 3;  // Going to the Right
						}
						else
						{
							nDir = 2;  // Going to the left
						}
					}
					else
					{
						if (ptFrom.y < ptTo.y)
						{
							nDir = 1;  // Going up
						}
						else
						{
							nDir = 0;  // Going down
						}
					}
					switch (nDir)   // determine which candidate direction we would like (these choices could be different, as long
									// as we don't continue in the same direction crossing the streams)
					{
					case 0: nCandidateDir = 2; break;  // IF we come from top down we will next go to the left
					case 1: nCandidateDir = 3; break;  // If we come from the bottom up we will next go right
					case 2: nCandidateDir = 0; break;  // If we from from the right to the left then we will next go down
					case 3: nCandidateDir = 1; break;  // If we come from the left to the right then we will next go up
					}

					std::cerr << nDir << " " << nCandidateDir << " " << ptFrom.x << ", " << ptFrom.y << "  " << ptTo.x << ", " << ptTo.y << "\n";

					// find the direction of the 3 matches and select the appropriate direction so the lines don't cross.
					for (int m = 0; m < 3; m++)  // for all the matches
					{

						ptNew = Vertex2D(dtMatches[m].nPt1 ? inlines[dtMatches[m].nIndex].pt2 : inlines[dtMatches[m].nIndex].pt1);
						std::cerr <<m << " " << ptNew.x << ", " << ptNew.y;

						// determine direction of travel for this candidate path
						if (fabs(ptTo.x - ptNew.x) > fabs(ptTo.y - ptNew.y))
						{
							if (ptTo.x < ptNew.x)
							{
								nDir = 3;  // Going to the Right
							}
							else
							{
								nDir = 2;  // Going to the left
							}
						}
						else
						{
							if (ptTo.y < ptNew.y)
							{
								nDir = 1;  // Going up
							}
							else
							{
								nDir = 0;  // Going down
							}
						}

						std::cerr << "  " << nDir << "\n";
						if (nDir == nCandidateDir) // if this is the direction we want
						{
							// !!! what if we never reach this condition.  We will have an infinite loop
							if (nFront)
							{
								workingDeck.push_front(ptNew);
							}
							else
							{
								workingDeck.push_back(ptNew);
							}
							inlines.erase(inlines.begin() + dtMatches[m].nIndex);
							break;
						}

					}// for each candidate segment

				}// else there are 3 matches

			}// for both the front and the back of the path

#else   // old way that stops on the very first match
		// This apparently will work when fighting the staypuff marshmellow man when we have to 
		// again cross the streams.
		bool foundMatch = true;
		while (foundMatch)
		{
			foundMatch = false;
			for (auto it = inlines.begin(); it != inlines.end(); ++it)
			{
				if (workingDeck.back().MatchesLocation(it->pt1))
				{
					foundMatch = true;
					workingDeck.push_back(Vertex2D(it->pt2));
					inlines.erase(it);
					break;
				}
				else if (workingDeck.back().MatchesLocation(it->pt2))
				{
					foundMatch = true;
					workingDeck.push_back(Vertex2D(it->pt1));
					inlines.erase(it);
					break;
				}
				else if (workingDeck.front().MatchesLocation(it->pt1))
				{
					foundMatch = true;
					workingDeck.push_front(Vertex2D(it->pt2));
					inlines.erase(it);
					break;
				}
				else if (workingDeck.front().MatchesLocation(it->pt2))
				{
					foundMatch = true;
					workingDeck.push_front(Vertex2D(it->pt1));
					inlines.erase(it);
					break;
				}
			}
#endif
		}
		output.poly.insert(output.poly.begin(), workingDeck.begin(), workingDeck.end());
		output.colour = Colour(r, g, b);
		return output;
	}

	// mergeLinesWithPaths
	//
	// input:
	//   outer   - this is a vector of 2D verticespaths sections of the outer perimeter and inner holes, all of the same colour
	//   inset   - this is a vector of 2D vertices path that will be the inset lines created colourGrid create borders.  They are 
	//			   the same colour as lines
	//   tolerance - how far away to look for a matching line
	//
	//  return  -  a vector of 2D vertices paths in a loop / (polygons)
	//
	//  Purpose:  This method should create a polygon for each valid combination of outer perimeters and inset boundaries 
	//
	//
#if 1 //SEL Method
	std::vector< vert2DPoly > mergeLinesWithPaths(std::vector< vert2DPath >& outer, std::vector< vert2DPath >& inset, double tolerance)
	{

		// This function assumes they are all the same colour, caller responsible

		std::vector<vert2DPath> WorkingOuterLines = outer;		// a copy of outer perimeter lines
		std::vector<vert2DPath> &WorkingInsetLines = inset;		// a copy of inset lines

		std::vector<vert2DPoly> outputPolys;					// a vector of combined polygons to be returned

		int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logMergePaths");

		if (bVerbose)
		{
			std::cout << "mergeLinesWithPath " << outer.size() << " Outer Perimeter Lines and " << inset.size() << " inset lines.  Tolerance = " << tolerance << "\n";
			std::cout << "Colour " << WorkingOuterLines.front().colour << "\n";
		}

		// Function should eventually remove all the working outer lines (outer perimeters and inset lines)
		// loop while there are still perimeter lines left
		while (WorkingOuterLines.empty() == false)
		{
			if (bVerbose)
			{
				std::cout << "Creating a new Polygon starting with Outer Perimeter path beginning at point ( ";
				std::cout << WorkingOuterLines.begin()->poly.begin()->x << ", " << WorkingOuterLines.begin()->poly.begin()->y << " ) ending at point (";
				std::cout << WorkingOuterLines.begin()->poly.back().x << ", " << WorkingOuterLines.begin()->poly.back().y << " )\n";

			}
			outputPolys.push_back(vert2DPoly());							//create a new polygon
			outputPolys.back().colour = WorkingOuterLines.front().colour;	//assign it the correct colour
			std::deque<Vertex2D> workingDeck;								//create a new workingDeck of 2D vertices that will contain the matched up outer paths with inset paths

			// we will initialize the working deck with the very first Outer Line path
			workingDeck.insert(workingDeck.begin(), WorkingOuterLines.begin()->poly.begin(), WorkingOuterLines.begin()->poly.end());
			WorkingOuterLines.erase(WorkingOuterLines.begin());				//erase the  first outter line


			bool foundMatch = true;
			while (foundMatch)		// loop until we find no more matches
			{
				foundMatch = false;
				if (bVerbose)
				{
					std::cout << "Now look for another outer line that will join the current polygon beginning at point ( ";
					std::cout << workingDeck.front().x << ", " << workingDeck.front().y << " ) ending at point (";
					std::cout << workingDeck.back().x << ", " << workingDeck.back().y << " )\n";

				}

				// Let's scan all the outer lines to see if we have another match to the working deck
				for (auto lineIT = WorkingOuterLines.begin(); lineIT != WorkingOuterLines.end(); ++lineIT) // for all the outer and hole lines
				{
					// If the start of the outer line matches the back of the working deck then insert it at the back
					if (workingDeck.back().MatchesLocation(lineIT->poly.front(), tolerance))
					{
						foundMatch = true;
						workingDeck.insert(workingDeck.end(), lineIT->poly.begin(), lineIT->poly.end());
					}
					else if (workingDeck.back().MatchesLocation(lineIT->poly.back(), tolerance))
					{
						foundMatch = true;
						std::reverse(lineIT->poly.begin(), lineIT->poly.end());
						workingDeck.insert(workingDeck.end(), lineIT->poly.begin(), lineIT->poly.end());
					}
					else if (workingDeck.front().MatchesLocation(lineIT->poly.back(), tolerance))
					{
						workingDeck.insert(workingDeck.begin(), lineIT->poly.begin(), lineIT->poly.end());
						foundMatch = true;
					}
					else if (workingDeck.front().MatchesLocation(lineIT->poly.front(), tolerance))
					{
						foundMatch = true;
						std::reverse(lineIT->poly.begin(), lineIT->poly.end());
						workingDeck.insert(workingDeck.begin(), lineIT->poly.begin(), lineIT->poly.end());
					}

					if (foundMatch)
					{
						if (bVerbose)
						{
							std::cout << "outer line found beginning at point ( ";
							std::cout << lineIT->poly.front().x << ", " << lineIT->poly.front().y << " ) ending at point (";
							std::cout << lineIT->poly.back().x << ", " << lineIT->poly.back().y << " )\n\n";

						}
						WorkingOuterLines.erase(lineIT);
						break;
					}
				}// for all the outer and hole lines


				// end of lines
				if (foundMatch) // dont start the paths yet until all the broken outer lines are combined   // I don't think there should ever be a 
					// broken outer line left because of how they are now created 
				{
					//Let's test if we have completed a loop and then break out of while
					if (workingDeck.front().MatchesLocation(workingDeck.back(), tolerance))
					{
						if (bVerbose)
						{
							std::cout << "Polygon loop completed with this outer line so stop checking for more lines\n";
						}
						break;
					}
					else
					{
						continue;
					}
				}

				// we can log this unbroken outer line
				if (bVerbose)
				{
					std::cout << "No more outer lines will join.   Let's see if we can join an inset line\n";
					std::cout << "to the current polygon beginning at point ( ";
					std::cout << workingDeck.front().x << ", " << workingDeck.front().y << " ) ending at point (";
					std::cout << workingDeck.back().x << ", " << workingDeck.back().y << " )\n";
				}


				// if we are here then there are no more matches and all the outer perimeters and holes broken lines have been joined up
				// and are stored in WorkingInsetLines

				// so now let's loop though all the inset lines (WorkingInsetLines) and see if either end matches
				// either end of the outer lines
				for (auto pathIT = WorkingInsetLines.begin(); pathIT != WorkingInsetLines.end(); ++pathIT)
				{
					if (workingDeck.back().MatchesLocation(pathIT->poly.front(), tolerance))
					{
						foundMatch = true;
						//+1 skips the identical point
						workingDeck.insert(workingDeck.end(), pathIT->poly.begin(), pathIT->poly.end());
					}
					else if (workingDeck.back().MatchesLocation(pathIT->poly.back(), tolerance))
					{
						foundMatch = true;
						std::reverse(pathIT->poly.begin(), pathIT->poly.end());
						workingDeck.insert(workingDeck.end(), pathIT->poly.begin(), pathIT->poly.end());
					}
					else if (workingDeck.front().MatchesLocation(pathIT->poly.back(), tolerance))
					{
						foundMatch = true;
						workingDeck.insert(workingDeck.begin(), pathIT->poly.begin(), pathIT->poly.end());
					}
					else if (workingDeck.front().MatchesLocation(pathIT->poly.front(), tolerance))
					{
						foundMatch = true;
						std::reverse(pathIT->poly.begin(), pathIT->poly.end());
						workingDeck.insert(workingDeck.begin(), pathIT->poly.begin(), pathIT->poly.end());
					}

					if (foundMatch)   //if found a match we can delete the inset path from the vector (WorkingInsetLines) and break out of the loop
					{

						// log the combined outer lines and inner lines
						if (bVerbose)
						{
							std::cout << "Inset line found beginning at point ( ";
							std::cout << pathIT->poly.front().x << ", " << pathIT->poly.front().y << " ) ending at point (";
							std::cout << pathIT->poly.back().x << ", " << pathIT->poly.back().y << " )\n\n";
						}

						WorkingInsetLines.erase(pathIT);

						break;
					}
				}// for all inset lines

				if (foundMatch)
				{
					//Let's test if we have completed a loop and then break out of while
					if (workingDeck.front().MatchesLocation(workingDeck.back(), tolerance))
					{
						if (bVerbose)
						{
							std::cout << "Polygon loop completed with this inset so stop checking for more lines\n";
						}
						break;
					}
				}
			}// while matches found
			if (bVerbose)
			{
				std::cout << "All available outer and inset lines have been joined to current polygon beginning at point ( ";
				std::cout << workingDeck.front().x << ", " << workingDeck.front().y << " ) ending at point (";
				std::cout << workingDeck.back().x << ", " << workingDeck.back().y << " )\n";
			}

			outputPolys.back().poly.insert(outputPolys.back().poly.end(), workingDeck.begin(), workingDeck.end());
		}// while outer perimeters or hole lines left
		if (bVerbose)
		{
			std::cout << "Completed " << outputPolys.size() << " Polygons\n---------------------------------------------------------------\n\n";
		}
		return outputPolys;

#else // Robert's method
std::vector< vert2DPoly > mergeLinesWtihPaths(const std::vector< Line >& lines, std::vector< vert2DPath >& poly, double tolerance)
{
	// This function assumes they are all the same colour, caller responsible

	std::vector<Line> workingLines = lines;				// an alias for outer perimeter lines
	std::vector<vert2DPath> &workingPaths = poly;		// an alias for the inset lines

	std::vector<vert2DPoly> outputPolys;				// a vector of combined polygons to be returned

	int bVerbose = gl_nSEL && Settings::getSingleton().getBoolValue("logMergePaths");

	// Function should eventually remove all the working lines (outer perimeters)
	// loop while there are still perimeter lines left
	while (workingLines.empty() == false)
	{
		outputPolys.push_back(vert2DPoly());						//create a new polygon
		outputPolys.back().colour = workingLines.front().colour;	//assign it the correct colour
		std::deque<Vertex2D> workingDeck;							//create a new workingDeck of 2D vertices
		workingDeck.push_back(Vertex2D(workingLines.front().pt1));  //initialize the working deck to point 1 and point 2 of the first outer line
		workingDeck.push_back(Vertex2D(workingLines.front().pt2));
		workingLines.erase(workingLines.begin());					//erase the  first out line


																	// I think this section will combine any broken outer perimeter lines into a complete set.  Is this wasteful.   They should have been complete before except 
																	// for the first and last regions along an outer perimeter or hole may not have been linked.  
																	// This code will accept the first match found but I think this is acceptable because there should not be any branches on the enclosing perimeters or inner holes
																	// This section gives me some concern.  The tolerance is quit large when I believe that the lines themselves should be contiguous within floating point tolerances.
																	// This section will loop until a single unbroken line is placed in the working deck and removed from the workingLines

																	// Got it. !!! This shouldn't stop at the first match.   It should find the closest point that is no further than tolerance away.   This probably worked with a very fine grid
																	// but even then it could screw up.   Because of the large tolerance it is picking fragments of the exterior perimeter before it get's to the end of the lines.
																	//  Maybe the closest isn't the approach we want.  we want to check only the ends of the regions alone the outer perimeters and we don't care about all the points in-between
																	// the working deck is mixed up with outer perimeter points (fine resolution) and inset lines (I hope to have very course resolution).  I have to study this further.   
																	// Logging the actual results I see that there are fragments of perimeter lines left.   I do think I should treat them as regions.   Hmmm.   tricky to implement.
		bool foundMatch = true;
		while (foundMatch)		// loop until we find no more matches
		{
			foundMatch = false;
			// Check lines for matches first
			for (auto lineIT = workingLines.begin(); lineIT != workingLines.end(); ++lineIT) // for all the outer and hole lines
			{
				if (workingDeck.back().MatchesLocation(lineIT->pt1, tolerance))
				{
					foundMatch = true;
					workingDeck.back() = Vertex2D(lineIT->pt1);
					workingDeck.push_back(Vertex2D(lineIT->pt2));
				}
				else if (workingDeck.back().MatchesLocation(lineIT->pt2, tolerance))
				{
					foundMatch = true;
					workingDeck.back() = Vertex2D(lineIT->pt2);
					workingDeck.push_back(Vertex2D(lineIT->pt1));
				}
				else if (workingDeck.front().MatchesLocation(lineIT->pt1, tolerance))
				{
					foundMatch = true;
					workingDeck.front() = Vertex2D(lineIT->pt1);
					workingDeck.push_front(Vertex2D(lineIT->pt2));
				}
				else if (workingDeck.front().MatchesLocation(lineIT->pt2, tolerance))
				{
					foundMatch = true;
					workingDeck.front() = Vertex2D(lineIT->pt2);
					workingDeck.push_front(Vertex2D(lineIT->pt1));
				}
				if (foundMatch)
				{
					workingLines.erase(lineIT);
					break;
				}
			}// for all the outer and hole lines


			 // end of lines
			if (foundMatch) // dont start the paths yet until all the broken lines are combined
				continue;

			// we can log this unbroken outer line
			if (bVerbose)
			{
				int n = 0;
				std::cout << "Found unbroken outer or hole perimeter of " << workingDeck.size() << " points\n";
				for (auto it = workingDeck.begin(); it != workingDeck.end(); it++) // for all points in the working deck
				{
					std::cout << n++ << ", " << it->x << ", " << it->y << "\n";
				}
				std::cout << "\n";
			}


			// if we are here then there are no more matches and all the outer perimeters and holes broken lines have been joined up
			// and are stored in workingPaths

			// so now let's loop though all the inset lines (workingPaths) and see if either end matches
			// either end of the outer lines
			for (auto pathIT = workingPaths.begin(); pathIT != workingPaths.end(); ++pathIT)
			{
				if (workingDeck.back().MatchesLocation(pathIT->poly.front(), tolerance))
				{
					foundMatch = true;
					//+1 skips the identical point
					workingDeck.insert(workingDeck.end(), pathIT->poly.begin(), pathIT->poly.end());
				}
				else if (workingDeck.back().MatchesLocation(pathIT->poly.back(), tolerance))
				{
					foundMatch = true;
					std::reverse(pathIT->poly.begin(), pathIT->poly.end());
					workingDeck.insert(workingDeck.end(), pathIT->poly.begin(), pathIT->poly.end());
				}
				else if (workingDeck.front().MatchesLocation(pathIT->poly.back(), tolerance))
				{
					foundMatch = true;
					workingDeck.insert(workingDeck.begin(), pathIT->poly.begin(), pathIT->poly.end());
				}
				else if (workingDeck.front().MatchesLocation(pathIT->poly.front(), tolerance))
				{
					foundMatch = true;
					std::reverse(pathIT->poly.begin(), pathIT->poly.end());
					workingDeck.insert(workingDeck.begin(), pathIT->poly.begin(), pathIT->poly.end());
				}

				if (foundMatch)   //if found a match we can delete the inset path from the vector (workingPaths) and break out of the loop
				{
					workingPaths.erase(pathIT);

					// log the combined outer lines and inner lines
					if (bVerbose)
					{
						int n = 0;
						std::cout << "Found a matching inset line to outer perimeter. " << workingDeck.size() << "  combined points shown \n";
						for (auto it = workingDeck.begin(); it != workingDeck.end(); it++) // for all points in the working deck
						{
							std::cout << n++ << ", " << it->x << ", " << it->y << "\n";
						}
						std::cout << "\n";
					}

					break;
				}
			}// for all inset lines

		}// while matches found
		outputPolys.back().poly.insert(outputPolys.back().poly.end(), workingDeck.begin(), workingDeck.end());
	}// while outer perimeters or hole lines left
	return outputPolys;
#endif
	}

	// 
	// vert2DPoly makePath(const std::vector< std::shared_ptr< Line > >& line, const std::vector< vert2DPath >& paths, double tolerance)
	// {
	//   std::vector<vert2DPath> inputPaths = paths;
	//   vert2DPoly out;
	//   std::deque<Vertex2D> workingSet;
	//   out.r = line.front()->r;out.g = line.front()->g;out.b = line.front()->b;
	//   workingSet.push_back(Vertex2D(line.front()->pt1));
	//   workingSet.push_back(Vertex2D(line.front()->pt2));
	//   for(int i =1; i < line.size(); i++)
	//     workingSet.push_back(Vertex2D(line[i]->pt2));
	//   bool foundMatch = true;
	//   while(foundMatch)
	//   {
	//     foundMatch = false;
	//     for(auto pathIT = inputPaths.begin(); pathIT != inputPaths.end(); ++pathIT)
	//     {
	//       if(pathIT->poly.front().MatchesLocation(workingSet.back(),tolerance))
	//       {
	// 	foundMatch = true;
	// 	workingSet.insert(workingSet.end(),pathIT->poly.begin(),pathIT->poly.end());
	//       }
	//       else if(pathIT->poly.back().MatchesLocation(workingSet.back(),tolerance))
	//       {
	// 	foundMatch = true;
	// 	std::reverse(pathIT->poly.begin(),pathIT->poly.end());
	// 	workingSet.insert(workingSet.end(),pathIT->poly.begin(),pathIT->poly.end());
	//       }
	//       else if(pathIT->poly.back().MatchesLocation(workingSet.front(),tolerance))
	//       {
	// 	foundMatch = true;
	// 	workingSet.insert(workingSet.begin(),pathIT->poly.begin(),pathIT->poly.end());
	//       }
	//       else if(pathIT->poly.front().MatchesLocation(workingSet.front(),tolerance))
	//       {
	// 	foundMatch = true;
	// 	std::reverse(pathIT->poly.begin(),pathIT->poly.end());
	// 	workingSet.insert(workingSet.begin(),pathIT->poly.begin(),pathIT->poly.end());
	//       }
	//       
	//       if(foundMatch)
	//       {
	// 	inputPaths.erase(pathIT);
	// 	break;
	//       }
	//     }
	//     if(workingSet.front().MatchesLocation(workingSet.back(),tolerance))
	//       break;
	//   }
	//   if(workingSet.front().MatchesLocation(workingSet.back(),tolerance) == false)
	//   {
	//     std::cout << "Polypath not closed!\n";
	//   }
	//   else
	//   {
	//     std::cout << "Polypath closed!\n";
	//   }
	//   out.poly.insert(out.poly.end(),workingSet.begin(),workingSet.end());
	//   return out;
	// }
	// 
	// 
	void writePolygonToIPE(const vert2DPoly& poly, const std::string& filename)
	{
		std::ofstream outfile;
		outfile.open(filename);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";
		outfile << "<path layer=\"polychains\" stroke=\"black\" pen=\"heavier\">\n";

		outfile << poly.poly.front().x + shift << " " << poly.poly.front().y + shift << " m\n";
		for (int i = 1; i < poly.poly.size(); i++)
		{
			outfile << poly.poly[i].x + shift << " " << poly.poly[i].y + shift << " l\n";
		}

		outfile << "</path>\n";
		outfile << "</group>\n";
		outfile << "</page>\n";
		outfile << "</ipe>\n";

		outfile.close();
	}
	// 
	void writePathToIPE(const std::vector< Vertex2D >& poly, const std::string& filename, bool close)
	{
		std::ofstream outfile;
		outfile.open(filename);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";
		outfile << "<path layer=\"polychains\" stroke=\"black\" pen=\"heavier\">\n";

		outfile << poly.front().x + shift << " " << poly.front().y + shift << " m\n";
		for (int i = 1; i < poly.size(); i++)
		{
			outfile << poly[i].x + shift << " " << poly[i].y + shift << " l\n";
		}
		outfile << "</path>\n";
		outfile << "</group>\n";
		// -----------------------------------
		outfile << "<group layer=\"crosses\">\n";
		outfile << "<path layer=\"crosses\" stroke=\"red\" pen=\"heavier\">\n";

		for (int i = 0; i < poly.size(); i++)
		{
			outfile << "<path>\n";
			outfile << poly[i].x + shift - 0.5 << " " << poly[i].y + shift << " m\n";
			outfile << poly[i].x + shift + 0.5 << " " << poly[i].y + shift << " l\n";
			outfile << "</path>" << "\n";
			outfile << "<path>\n";
			outfile << poly[i].x + shift << " " << poly[i].y - 0.5 + shift << " m\n";
			outfile << poly[i].x + shift << " " << poly[i].y + 0.5 + shift << " l\n";
			outfile << "</path>" << "\n";
		}
		//   outfile << poly.front().x+shift << " " << poly.front().y+shift << " m\n";
		//   for(int i =1; i < poly.size(); i++)
		//   {
		//     outfile << poly[i].x+shift << " "<<poly[i].y+shift<<" l\n";
		//   }
		//   outfile << "</path>\n";
		outfile << "</group>\n";

		outfile << "</page>\n";
		outfile << "</ipe>\n";

		outfile.close();
	}
	// 
	// 
	void writePathToIPE(const BodySplitter::vert2DPath& poly, const std::string& filename, bool close)
	{
		std::ofstream outfile;
		outfile.open(filename);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";
		outfile << "<path layer=\"polychains\" stroke=\"black\" pen=\"heavier\">\n";

		outfile << poly.poly.front().x + shift << " " << poly.poly.front().y + shift << " m\n";
		for (int i = 1; i < poly.poly.size(); i++)
		{
			outfile << poly.poly[i].x + shift << " " << poly.poly[i].y + shift << " l\n";
		}
		outfile << "</path>\n";
		//   outfile << "</group>\n";

		//       outfile << "<group layer=\"crosses\">\n";

		//   outfile << "<group layer=\"crosses\">\n";
		const double crossSize = 0.3;
		for (int i = 0; i < poly.poly.size(); i++)
		{
			outfile << "<path layer=\"polychains\" stroke=\"red\" pen=\"heavier\">\n";
			outfile << poly.poly[i].x + shift - crossSize << " " << poly.poly[i].y + shift << " m\n";
			outfile << poly.poly[i].x + shift + crossSize << " " << poly.poly[i].y + shift << " l\n";
			outfile << "</path>" << "\n";
			outfile << "<path layer=\"polychains\" stroke=\"red\" pen=\"heavier\">\n";
			outfile << poly.poly[i].x + shift << " " << poly.poly[i].y - crossSize + shift << " m\n";
			outfile << poly.poly[i].x + shift << " " << poly.poly[i].y + crossSize + shift << " l\n";
			outfile << "</path>" << "\n";
		}
		outfile << "</group>\n";
		outfile << "</page>\n";
		outfile << "</ipe>\n";

		outfile.close();
	}
	// 
	// void writePathsToIPE(const std::vector<std::vector< Vertex2D >>& poly, const std::__cxx11::string& filename, bool close)
	// {
	//     std::ofstream outfile;
	//   outfile.open(filename);
	//   // Write header
	//   outfile << "<?xml version=\"1.0\"?>\n";
	//   outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
	//   outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
	//   outfile << "<info bbox=\"cropbox\"/>\n";
	//   outfile << "<ipestyle name=\"rova\">\n";
	//   outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
	//   outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
	//   outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
	//   outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
	//   int colourCounter = 0;
	//   for(auto polyIT : poly)
	//   {
	//     outfile << "<color name=\"this"<<colourCounter++<<"\" value=\""<<(rand()%128)/128.0<<" "<<(rand()%128)/128.0<< " "<<(rand()%128)/128.0<<"\"/>\n";
	//   }
	//   colourCounter = 0;
	//   outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
	//   outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
	//   outfile <<"</ipestyle>\n";
	//   
	//   outfile << "<page gridsize=\"16\">\n";
	//   outfile << "<layer name=\"polychains\"/>\n";
	//   outfile << "<layer name=\"sk\"/>\n";
	// 
	//   outfile << "<group layer=\"polychains\">\n";
	//   
	//   for(auto polyIT : poly)
	//   {
	//     outfile << "<path layer=\"polychains\" stroke=\"this"<<colourCounter++<<"\" pen=\"heavier\">\n";
	// 
	//   outfile << polyIT.front().x+shift << " " << polyIT.front().y+shift << " m\n";
	//   for(int i =1; i < polyIT.size(); i++)
	//   {
	//     outfile << polyIT[i].x+shift << " "<<polyIT[i].y+shift<<" l\n";
	//   }
	//   outfile << "</path>\n";
	//   }
	//   outfile << "</group>\n";
	//   outfile << "</page>\n";
	//   outfile << "</ipe>\n";
	//   
	//   outfile.close();
	// }
	// 
	void writeLinesToIPE(const Lines &lines, const std::string &fileName)
	{
		std::ofstream outfile;
		outfile.open(fileName);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		outfile << "<pen name=\"heavier\" value=\"0.01\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";
		for(auto it : lines)
		{
		  outfile << "<path layer=\"polychains\" stroke=\"black\" pen=\"heavier\">\n";
		  
		  outfile << it.pt1.x + (double)shift  << " " << it.pt1.y + (double)shift << " m\n";
		  outfile << it.pt2.x + (double)shift  << " " << it.pt2.y + (double)shift << " l\n";
		  
		  outfile << "</path>" << "\n";
		}
	      outfile << "</group>\n";
	      outfile << "</page>\n";
	      outfile << "</ipe>\n";

	      outfile.close();
	}
	void writePathsToIPE(const std::vector< BodySplitter::vert2DPath >& poly, const std::string& filename, bool close)
	{
		std::ofstream outfile;
		outfile.open(filename);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		int colourCounter = 0;
		for (auto polyIT : poly)
		{
			outfile << "<color name=\"this" << colourCounter++ << "\" value=\"" << polyIT.colour.getR<double>() << " " << polyIT.colour.getG<double>() << " " << polyIT.colour.getB<double>() << "\"/>\n";
		}
		colourCounter = 0;
		outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";

		for (auto polyIT : poly)
		{
			// outfile << "<color name=\"this"<<colourCounter<<"\" value=\""<<polyIT.r/128.0<<" "<<polyIT.g/128.0<< " "<<polyIT.b/128.0<<"\"/>\n";
			outfile << "<path layer=\"polychains\" stroke=\"this" << colourCounter++ << "\" pen=\"heavier\">\n";

			outfile << polyIT.poly.front().x + shift << " " << polyIT.poly.front().y + shift << " m\n";
			for (int i = 1; i < polyIT.poly.size(); i++)
			{
				outfile << polyIT.poly[i].x + shift << " " << polyIT.poly[i].y + shift << " l\n";
			}
			outfile << "</path>\n";

			const double crossSize = 0.3;
			for (int i = 0; i < polyIT.poly.size(); i++)
			{
				outfile << "<path layer=\"polychains\" stroke=\"red\" pen=\"heavier\">\n";
				outfile << polyIT.poly[i].x + shift - crossSize << " " << polyIT.poly[i].y + shift << " m\n";
				outfile << polyIT.poly[i].x + shift + crossSize << " " << polyIT.poly[i].y + shift << " l\n";
				outfile << "</path>" << "\n";
				outfile << "<path layer=\"polychains\" stroke=\"red\" pen=\"heavier\">\n";
				outfile << polyIT.poly[i].x + shift << " " << polyIT.poly[i].y - crossSize + shift << " m\n";
				outfile << polyIT.poly[i].x + shift << " " << polyIT.poly[i].y + crossSize + shift << " l\n";
				outfile << "</path>" << "\n";
			}
		}
		outfile << "</group>\n";
		outfile << "</page>\n";
		outfile << "</ipe>\n";

		outfile.close();
	}
	// 
	// 
	void writePolygonsToIPE(const std::vector< BodySplitter::vert2DPoly >& poly, const std::string& filename)
	{
		std::ofstream outfile;
		outfile.open(filename);
		// Write header
		outfile << "<?xml version=\"1.0\"?>\n";
		outfile << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">\n";
		outfile << "<ipe version=\"70000\" creator=\"InsetTestORD\">\n";
		outfile << "<info bbox=\"cropbox\"/>\n";
		outfile << "<ipestyle name=\"rova\">\n";
		outfile << "<color name=\"red\" value=\"1 0 0\"/>\n";
		outfile << "<color name=\"green\" value=\"0 1 0\"/>\n";
		outfile << "<color name=\"blue\" value=\"0 0 1\"/>\n";
		outfile << "<color name=\"gray4\" value=\"0.5 0.5 0.5\"/>\n";
		int colourCounter = 0;
		for (auto polyIT : poly)
		{
			outfile << "<color name=\"this" << colourCounter++ << "\" value=\"" << polyIT.colour.getR<double>() << " " << polyIT.colour.getG<double>() << " " << polyIT.colour.getB<double>() << "\"/>\n";
		}
		colourCounter = 0;
		outfile << "<pen name=\"heavier\" value=\"0.1\"/>\n";
		outfile << "<symbolsize name=\"tiny\" value=\"1.1\"/>\n";
		outfile << "</ipestyle>\n";

		outfile << "<page gridsize=\"16\">\n";
		outfile << "<layer name=\"polychains\"/>\n";
		outfile << "<layer name=\"sk\"/>\n";

		outfile << "<group layer=\"polychains\">\n";

		for (auto polyIT : poly)
		{
			if (polyIT.poly.empty())
			{
				colourCounter++;
				continue;
			}
			// outfile << "<color name=\"this"<<colourCounter<<"\" value=\""<<polyIT.r/128.0<<" "<<polyIT.g/128.0<< " "<<polyIT.b/128.0<<"\"/>\n";
			outfile << "<path layer=\"polychains\" stroke=\"this" << colourCounter++ << "\" pen=\"heavier\">\n";
			outfile << polyIT.poly.front().x + shift << " " << polyIT.poly.front().y + shift << " m\n";
			for (int i = 1; i < polyIT.poly.size(); i++)
			{
				outfile << polyIT.poly[i].x + shift << " " << polyIT.poly[i].y + shift << " l\n";
			}

			outfile << "</path>\n";
		}
		outfile << "</group>\n";
		outfile << "</page>\n";
		outfile << "</ipe>\n";

		outfile.close();
	}

	bool isColinear(const Vertex2D &pt1, const Vertex2D &pt2, const Vertex2D &pt3)
	{
		if (abs((pt2.y - pt1.y)*(pt3.x - pt2.x) - (pt3.y - pt2.y)*(pt2.x - pt1.x)) < 0.1)
			return true;
		return false;
	}

	vert2DPath smoothLines(const vert2DPath& inpath)
	{
		//Find lines at 45'
		vert2DPath result = inpath;
		bool foundMatch = true;
		if (inpath.poly.size() < 5)
			return inpath;
		std::size_t startPoint = 0;
		while (foundMatch)
		{
			foundMatch = false;
			for (int i = startPoint; i < result.poly.size() - 4; i++)
			{
				if (isColinear(result.poly[i], result.poly[i + 2], result.poly[i + 4]))
				{
					auto it = result.poly.erase(result.poly.begin() + i + 1);
					// Note to future - below seems like a reasonable optimzation, but without it, non-45 degree lines can be simplified
					//startPoint = it - result.poly.begin();
					foundMatch = true;
					break;
				}
			}
		}
		//foundMatch = true;
		//startPoint = 0;
		//while (foundMatch)
		//{
		//	foundMatch = false;
		//	for (int i = startPoint; i < result.poly.size() - 2; i++)
		//	{
		//		if (isColinear(result.poly[i], result.poly[i+1], result.poly[i+2]))
		//		{
		//			auto it = result.poly.erase(result.poly.begin() + i + 1);
		//			foundMatch = true;
		//			break;
		//		}
		//		else
		//		{
		//			startPoint = i;
		//		}
		//	}
		//}
		return result;
	}

	// 
	// void convertPixelToWorld(std::vector< Vertex2D >& inPath, double minX, double minY, double gridSize)
	// {
	//   for(auto &pointIT : inPath)
	//   {
	//     // (minX+gridSize*x+gridSize/2.0,minY+gridSize*y+gridSize/2.0)
	//     pointIT.x = minX+gridSize*pointIT.x;
	//     pointIT.y = minY+gridSize*pointIT.y;
	//   }
	// }
	// 
	Vertex convertPixelToWorld(const Vertex& in, double minX, double minY, double gridSize)
	{
		return Vertex(minX + gridSize*in.x, minY + gridSize*in.y);
	}
	// 
	inline constexpr uint32_t colourMash(uint8_t r, uint8_t g, uint8_t b)
	{
		return r | g << 8 | b << 16;
	}
	// 
	std::vector<std::vector<std::shared_ptr<Line>>> createIslandsByColour(const std::vector< std::shared_ptr< Line > >& lines)
	{
		std::vector<std::vector<std::shared_ptr<Line>>> output;
		if (lines.empty())
			return output;
		std::unordered_map<uint32_t, std::vector<std::shared_ptr<Line>>> linesByColour;
		for (auto it : lines)
		{
			uint32_t cHash = it->getColour().hash();
			if (linesByColour.count(cHash) == 0)
			{
				linesByColour[cHash] = std::vector<std::shared_ptr<Line>>();
			}
			linesByColour[cHash].push_back(it);
		}
		std::cerr << "lines by colour: " << linesByColour.size() << "\n";
		while (linesByColour.empty() == false)
		{
			std::deque<std::shared_ptr<Line>> thisPiece;
			uint32_t cHash = linesByColour.begin()->first;
			if (linesByColour[cHash].empty())
			{
				linesByColour.erase(cHash);
				continue;
			}
			else
			{
				thisPiece.push_back(linesByColour[cHash].front());
				linesByColour[cHash].erase(linesByColour[cHash].begin());
				bool foundMatch = true;
				while (foundMatch)
				{
					foundMatch = false;
					for (auto it = linesByColour[cHash].begin(); it != linesByColour[cHash].end(); ++it)
					{
						if (thisPiece.back()->pt2.MatchesLocation((*it)->pt1))
						{
							thisPiece.push_back(*it);
							foundMatch = true;
							linesByColour[cHash].erase(it);
							break;
						}
						else if (thisPiece.back()->pt2.MatchesLocation((*it)->pt2))
						{
							(*it)->reverse();
							thisPiece.push_back(*it);
							foundMatch = true;
							linesByColour[cHash].erase(it);
							break;
						}
						else if (thisPiece.front()->pt1.MatchesLocation((*it)->pt2))
						{
							thisPiece.push_front(*it);
							foundMatch = true;
							linesByColour[cHash].erase(it);
							break;
						}
						else if (thisPiece.front()->pt1.MatchesLocation((*it)->pt1))
						{
							(*it)->reverse();
							thisPiece.push_front(*it);
							foundMatch = true;
							linesByColour[cHash].erase(it);
							break;
						}
					}
				}
			}
			output.push_back(std::vector<std::shared_ptr<Line>>());
			output.back().insert(output.back().end(), thisPiece.begin(), thisPiece.end());
		}
		return output;
	}

	Lines vertPathToLinePath(const std::vector<Vertex2D> &path)
	{
		Lines output;

		Vertex pt1 = path.front();
		for (int i = 1; i < path.size(); i++)
		{
			Vertex pt2 = path[i];
			output.push_back(Line(pt1, pt2, 0));
			pt1 = pt2;
		}
		output.push_back(Line(output.back().pt2, output.front().pt1,0));
		return output;
	}
	// 
	// std::pair< double, double > convertPixelToWorld(const std::pair< double, double >& pixel, double minX, double minY, double gridSize)
	// {
	//     // (minX+gridSize*x+gridSize/2.0,minY+gridSize*y+gridSize/2.0)
	//   return std::pair<double,double>(minX+gridSize*pixel.first,minY+gridSize*pixel.second);
	// }
	// 
	// Vertex convertWorldtoPixel(const Vertex& in, double minX, double minY, double gridSize)
	// {
	//   /*
	//    *     float y1 = (line->pt1.y-minY)/gridSize;
	//     float y2 = (line->pt2.y-minY)/gridSize;
	//     */
	//   return Vertex(floor((in.x-minX)/gridSize),
	// 		floor((in.y-minY)/gridSize));
	// }
	// 
	// 
	// std::vector<std::vector<Vertex2D>> brokenLinesToVert2DPath(const std::vector< Line >& lines)
	// {
	//   std::vector<std::vector<Vertex2D>> output;
	//   std::vector<Line> inputSet = lines;
	//   Line::removeDuplicateLines(inputSet);
	//   
	//   while(inputSet.empty() == false)
	//   {
	//     bool foundMatch = true;
	//     std::deque<Vertex2D> workingSet;
	//     output.push_back(std::vector<Vertex2D>());
	//     workingSet.push_back(Vertex2D(inputSet.front().pt1));
	//     workingSet.push_back(Vertex2D(inputSet.front().pt2));
	//     inputSet.erase(inputSet.begin());
	//     while(foundMatch)
	//     {
	//       foundMatch = false;
	//       for(auto lineIT = inputSet.begin(); lineIT != inputSet.end(); ++lineIT)
	//       {
	// 	if(workingSet.back().MatchesLocation(lineIT->pt1,1))
	// 	{
	// 	  workingSet.push_back(Vertex2D(lineIT->pt2));
	// 	  foundMatch = true;
	// 	}
	// 	else if(workingSet.front().MatchesLocation(lineIT->pt2,1))
	// 	{
	// 	  workingSet.push_front(Vertex2D(lineIT->pt1));
	// 	  foundMatch = true;
	// 	}
	// 	else if(workingSet.back().MatchesLocation(lineIT->pt2,1))
	// 	{
	// 	  workingSet.push_back(Vertex2D(lineIT->pt1));
	// 	  foundMatch = true;
	// 	}
	// 	else if(workingSet.front().MatchesLocation(lineIT->pt1,1))
	// 	{
	// 	  workingSet.push_front(Vertex2D(lineIT->pt2));
	// 	  foundMatch = true;
	// 	}
	// 	if(foundMatch)
	// 	{
	// 	  inputSet.erase(lineIT);
	// 	  break;
	// 	}
	//       }
	//     }
	//     output.back().insert(output.back().begin(),workingSet.begin(),workingSet.end());
	//   }
	//   return output;
	// }
	// 
	// std::vector<vert2DPoly> mergeLinesWtihPaths(const std::vector< std::shared_ptr<Line> >& lines, std::vector< vert2DPath >& poly)
	// {
	//   std::vector<vert2DPoly> outpolys;
	//   std::unordered_map<uint32_t,std::vector<std::shared_ptr<Line>>> linesByColour;
	//   for(auto it : lines)
	//   {
	//     uint32_t cHash = colourMash(it->r,it->g,it->b);
	//     if(linesByColour.count(cHash) == 0)
	//     {
	//       linesByColour[cHash] = std::vector<std::shared_ptr<Line>>();
	//     }
	//     linesByColour[cHash].push_back(it);
	//   }
	//   Now build loops with this info...
	//   Start with largest outside line piece
	//   while(linesByColour.empty() == false)
	//   {
	//     std::deque<Vertex2D> thisPiece;
	//     uint32_t cHash = linesByColour.begin()->first;
	//     if(linesByColour[cHash].empty())
	//     {
	//       linesByColour.erase(cHash);
	//     }
	//     else
	//     {
	//       thisPiece.push_back(linesByColour[cHash].front()->pt1);
	//       thisPiece.push_back(linesByColour[cHash].front()->pt2);
	//       linesByColour[cHash].erase(linesByColour[cHash].begin());
	//       bool foundMatch = true;
	//       while(foundMatch)
	//       {
	// 	foundMatch = false;
	// 	for(auto lineIT = linesByColour[cHash].begin(); lineIT != linesByColour[cHash].end(); ++lineIT)
	// 	{
	// 	  if((*lineIT)->touchesPoint(thisPiece.back()))
	// 	  {
	// 	    foundMatch = true;
	// 	    if((*lineIT)->pt2.MatchesLocation(thisPiece.back()))
	// 	      (*lineIT)->reverse();
	// 	    
	// 	    thisPiece.push_back((*lineIT)->pt2);
	// 	  }
	// 	  else if((*lineIT)->touchesPoint(thisPiece.front()))
	// 	  {
	// 	    foundMatch = true;
	// 	    if((*lineIT)->pt1.MatchesLocation(thisPiece.front()))
	// 	      (*lineIT)->reverse();
	// 	    thisPiece.push_front((*lineIT)->pt1);
	// 	  }
	// 	  if(foundMatch)
	// 	  {
	// 	    linesByColour[cHash].erase(lineIT);
	// 	    break;
	// 	  }
	// 	}
	//       }
	//       Now match it up with the inset lines
	//       foundMatch = true;
	//       while(foundMatch)
	//       {
	// 	foundMatch = false;
	// 	for(auto polyIT = poly.begin(); polyIT != poly.end(); ++polyIT)
	// 	{
	// 	  if(cHash == colourMash(polyIT->r,polyIT->g,polyIT->b))
	// 	  {
	// 	    if(polyIT->poly.front().MatchesLocation(thisPiece.back(),2))
	// 	    {
	// 	      polyIT->poly.front().x = thisPiece.back().x;
	// 	      polyIT->poly.front().y = thisPiece.back().y;
	// 	      thisPiece.insert(thisPiece.end(),polyIT->poly.begin(), polyIT->poly.end());
	// 	      foundMatch = true;
	// 	      poly.erase(polyIT);
	// 	      break;
	// 	    }
	// 	    if(polyIT->poly.back().MatchesLocation(thisPiece.front(),2))
	// 	    {
	// 	      polyIT->poly.back().x = thisPiece.front().x;
	// 	      polyIT->poly.back().y = thisPiece.front().y;
	// 	      thisPiece.insert(thisPiece.begin(),polyIT->poly.begin(),polyIT->poly.end());
	// 	      foundMatch = true;
	// 	      poly.erase(polyIT);
	// 	      break;
	// 	    }
	// 	    if(polyIT->poly.back().MatchesLocation(thisPiece.back(),2))
	// 	    {
	// 	      polyIT->poly.back().x = thisPiece.back().x;
	// 	      polyIT->poly.back().y = thisPiece.back().y;
	// 	      std::reverse(polyIT->poly.begin(),polyIT->poly.end());
	// 	      thisPiece.insert(thisPiece.end(),polyIT->poly.begin(), polyIT->poly.end());
	// 	      foundMatch = true;
	// 	      poly.erase(polyIT);
	// 	      break;
	// 	    }
	// 	    if(polyIT->poly.front().MatchesLocation(thisPiece.front(),2))
	// 	    {
	// 	      polyIT->poly.front().x = thisPiece.front().x;
	// 	      polyIT->poly.front().y = thisPiece.front().y;
	// 	      std::reverse(polyIT->poly.begin(),polyIT->poly.end());
	// 	      thisPiece.insert(thisPiece.begin(),polyIT->poly.begin(),polyIT->poly.end());
	// 	      foundMatch = true;
	// 	      poly.erase(polyIT);
	// 	      break;
	// 	    }
	// 	  }
	// 	}
	//       }
	//     }
	//     if(thisPiece.empty() == false)
	//     {
	//       outpolys.push_back(vert2DPoly());
	//       outpolys.back().r = cHash & 0xFF;
	//       outpolys.back().g = (cHash & 0xFF00)>>8;
	//       outpolys.back().b = (cHash & 0xFF0000)>>16;
	//       outpolys.back().poly.insert(outpolys.back().poly.end(),
	// 				  thisPiece.begin(),thisPiece.end());
	//     }
	//   }
	//   see if these output polys connect to each other
	//   bool foundMatch = true;
	//   while(foundMatch)
	//   {
	//     foundMatch = false;
	//     for(auto outerLoop = outpolys.begin(); outerLoop != outpolys.end(); ++outerLoop)
	//     {
	//       for(auto innerLoop = outpolys.begin(); innerLoop != outpolys.end(); ++innerLoop)
	//       {
	// 	if(outerLoop == innerLoop)
	// 	{
	// 	  continue;
	// 	}
	// 	if(outerLoop->r == innerLoop->r &&
	// 	  outerLoop->g == innerLoop->g &&
	// 	  outerLoop->b == innerLoop->b)
	// 	{
	// 	  if(outerLoop->poly.back().MatchesLocation(innerLoop->poly.front(),2))
	// 	  {
	// 	    outerLoop->poly.insert(outerLoop->poly.end(),innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    std::cout << "Merged poly\n";
	// 	    foundMatch = true;
	// 	    outpolys.erase(innerLoop);
	// 	    break;
	// 	  }
	// 	  if(outerLoop->poly.back().MatchesLocation(innerLoop->poly.back(),2))
	// 	  {
	// 	    std::reverse(innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    outerLoop->poly.insert(outerLoop->poly.end(),innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    std::cout << "Merged poly\n";
	// 	    foundMatch = true;
	// 	    outpolys.erase(innerLoop);
	// 	    break;
	// 	  }
	// 	  if(outerLoop->poly.front().MatchesLocation(innerLoop->poly.back(),2))
	// 	  {
	// 	   std::reverse(innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    outerLoop->poly.insert(outerLoop->poly.begin(),innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    std::cout << "Merged poly\n";
	// 	    foundMatch = true;
	// 	    outpolys.erase(innerLoop);
	// 	    break;
	// 	  }
	// 	  if(outerLoop->poly.front().MatchesLocation(innerLoop->poly.front(),2))
	// 	  {
	// 	    std::reverse(innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    outerLoop->poly.insert(outerLoop->poly.begin(),innerLoop->poly.begin(),innerLoop->poly.end());
	// 	    std::cout << "Merged poly\n";
	// 	    foundMatch = true;
	// 	    outpolys.erase(innerLoop);
	// 	    break;
	// 	  }
	// 	}
	//       }
	//       if(foundMatch)
	// 	break;
	//     }
	//   }
	//   Check if the poly is a loops
	//   for(auto &it : outpolys)
	//   {
	//     if(it.poly.front().MatchesLocation(it.poly.back(),2))
	//     {
	//       it.poly.back().x = it.poly.front().x;
	//       it.poly.back().y = it.poly.front().y;
	//       std::cout << "Polygon loops, is closed\n";
	//     }
	//   }
	//   return outpolys;
	//   
	//   std::vector<std::shared_ptr<Line>> linesToProcess  = lines;
	//   for(auto &it : poly)
	//   {
	//     std::deque<Vertex2D> thisPoly;
	//     thisPoly.insert(thisPoly.end(),it.poly.begin(),it.poly.end());
	//     bool matchFound = true;
	//     while(matchFound)
	//     {
	//       matchFound = false;
	//       for(auto lineIT = linesToProcess.begin(); lineIT != linesToProcess.end(); ++lineIT)
	//       {
	// 	if(matchFound)
	// 	  break;
	// 	if((*lineIT)->r == it.r && (*lineIT)->g == it.g && (*lineIT)->b == it.b)
	// 	{
	// 	  
	// 	  if(thisPoly.front().MatchesLocation((*lineIT)->pt1,1))
	// 	  {
	// 	    thisPoly.front().x = (*lineIT)->pt1.x;
	// 	    thisPoly.front().y = (*lineIT)->pt1.y;
	// 	    thisPoly.push_front(Vertex2D((*lineIT)->pt2));
	// 	    matchFound = true;
	// 	    
	// 	    thisPoly.back().x = lineIT->pt2.x;
	// 	    thisPoly.back().y = lineIT->pt2.y;
	// 	    
	// 	    linesToProcess.erase(lineIT);
	// 	    break;
	// 	  }
	// 	  
	// 	  if(thisPoly.front().MatchesLocation((*lineIT)->pt2,1))
	// 	  {
	// 	    (*lineIT)->reverse();
	// 	    thisPoly.front().x = (*lineIT)->pt1.x;
	// 	    thisPoly.front().y = (*lineIT)->pt1.y;
	// 	    thisPoly.push_front(Vertex2D((*lineIT)->pt2));
	// 	    
	// 	    thisPoly.back().x = lineIT->pt2.x;
	// 	    thisPoly.back().y = lineIT->pt2.y;
	// 	    
	// 	    matchFound = true;
	// 	    linesToProcess.erase(lineIT);
	// 	    break;
	// 	  }
	// 	  
	// 	  if(thisPoly.back().MatchesLocation((*lineIT)->pt1,1))
	// 	  {
	// 	    (*lineIT)->reverse();
	// 	    thisPoly.back().x = (*lineIT)->pt2.x;
	// 	    thisPoly.back().y = (*lineIT)->pt2.y;
	// 	    thisPoly.push_back(Vertex2D((*lineIT)->pt1));
	// 
	// 	    thisPoly.front().x = lineIT->pt1.x;
	// 	    thisPoly.front().y = lineIT->pt1.y;
	// 	    
	// 	    matchFound = true;
	// 	    linesToProcess.erase(lineIT);
	// 	    break;
	// 	  }
	// 	  if(thisPoly.back().MatchesLocation((*lineIT)->pt2,1))
	// 	  {
	// 	    thisPoly.back().x = (*lineIT)->pt2.x;
	// 	    thisPoly.back().y = (*lineIT)->pt2.y;
	// 	    thisPoly.push_back(Vertex2D((*lineIT)->pt1));
	// 	    
	// 	    thisPoly.front().x = lineIT->pt1.x;
	// 	    thisPoly.front().y = lineIT->pt1.y;
	// 	    
	// 	    matchFound = true;
	// 	    linesToProcess.erase(lineIT);
	// 	    break;
	// 	  }
	// 	}
	//       }
	//     }
	//     std::cout << "Lines to process size: "<<linesToProcess.size()<<"\n";
	//     it.poly.clear();
	//     it.poly.insert(it.poly.end(),thisPoly.begin(),thisPoly.end());
	//   }
	// }
	// 
	// std::vector< vert2DPoly > merge(std::vector< std::vector< std::shared_ptr< Line > > >& source, std::vector< vert2DPath >& insets)
	// {
	//   std::vector<vert2DPoly> output;
	//   for(auto inIsland : source)
	//   {
	//     std::vector<Vertex2D> thisSet;
	//     thisSet.push_back(inIsland.front()->pt1);
	//     thisSet.push_back(inIsland.front()->pt2);
	//     for(int i = 1; i < inIsland.size(); ++i)
	//     {
	//       thisSet.push_back(inIsland[i]->pt2);
	//     }
	// //    auto thisSet = workingSourceLines.front();
	//     // Find matching inset
	//     std::cout << "pt1: ("<<thisSet.front().x<<","<<thisSet.front().y<<") Pt2: ("<<
	// 			    thisSet.back().x<<","<<thisSet.back().y<<") \n";
	//     for(auto insetIT = insets.begin(); insetIT != insets.end(); ++insetIT)
	//     {
	//       std::cout << "Inset Pt1 ("<<insetIT->poly.front().x<<","<<insetIT->poly.front().y<<") Pt2: ("
	// 		    <<insetIT->poly.front().x<<","<<insetIT->poly.front().y<<")\n";
	//       if(insetIT->poly.front().MatchesLocation(thisSet.back()) && insetIT->poly.back().MatchesLocation(thisSet.front()))
	//       {
	// 	thisSet.insert(thisSet.end(),insetIT->poly.begin(),insetIT->poly.end());
	// 	insets.erase(insetIT);
	// 	break;
	//       }
	//       else if(insetIT->poly.back().MatchesLocation(thisSet.back()) && insetIT->poly.front().MatchesLocation(thisSet.front()))
	//       {
	// 	std::reverse(insetIT->poly.begin(),insetIT->poly.end());
	// 	thisSet.insert(thisSet.end(),insetIT->poly.begin(),insetIT->poly.end());
	// 	insets.erase(insetIT);
	// 	break;
	//       }
	//     }
	//     output.push_back(vert2DPoly());
	//     output.back().r = inIsland.front()->r;
	//     output.back().g = inIsland.front()->g;
	//     output.back().b = inIsland.front()->b;
	//     
	//     
	//     output.back().poly = thisSet;
	//   }
	//   return output;
	// }
	// 
/**/
	bool rayIntersectsTriangle(const Triangle &tri, const glm::vec3 &origin, const glm::vec3 &dir, glm::vec3 &intPt)
	{
		glm::vec3 edge1 = glm::vec3(tri.pt2) - glm::vec3(tri.pt1);
		glm::vec3 edge2 = glm::vec3(tri.pt3) - glm::vec3(tri.pt1);

		const glm::vec3 n = glm::cross(edge1,edge2);
		const glm::vec3 q = glm::cross(glm::vec3(dir),edge2);
		const float a = glm::dot(edge1, q);

		if (glm::dot(n, glm::vec3(dir)) >= 0 || fabs(a) < 0.000001)
			return false;

		const glm::vec3 s = (glm::vec3(origin) - glm::vec3(tri.pt1)) / a;
		const glm::vec3 r = glm::cross(s, edge1);
		double b[3];
		b[0] = glm::dot(s,q);
		b[1] = glm::dot(r,glm::vec3(dir));
		b[2] = 1.0f - b[0] - b[1];

		if ((b[0] < 0.0f) || (b[1] < 0.0f) || (b[2] <
			0.0f)) return false;
		
		float t = glm::dot(edge2, r);
		if (t <= 0)
			return false;
		intPt = glm::vec3(origin) + glm::vec3(dir)*t;
		return true;
	}
}	//namespace BodySplitter
// 
// #if USE_INT_COORDS
// namespace BodySplitter
// {
// 	micrometers to_micrometers(millimeters in)
// 	{
// 		return lround(in*1e6);
// 	}
// 	millimeters to_millimeters(micrometers in)
// 	{
// 		return (double)in/1e6;
// 	}
// 
// };//Namespace BodySplitter
// 
// #endif