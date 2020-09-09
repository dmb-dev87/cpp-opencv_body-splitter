#include <assert.h>
#include <spdlog/spdlog.h>
#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <deque>
#include <unordered_map>
#include <fstream>

#include "Line.h"
#include "colour.h"
#include "BodySplitterTexture.h"

#ifdef _DEBUG
#include <wx\log.h>
#endif



Line::Line(const Vertex &in1, const Vertex &in2) : pt1(in1), pt2(in2),LineParam(LineType::CREATED_NO_PARAM)
{

}

Line::Line(const Vertex &in1, const Vertex &in2, unsigned int linetype)
	: pt1(in1), pt2(in2), LineParam(linetype)
{

}

bool Line::IntersectsPlane(double z,double height) const
{
	double minZ = std::min(pt1.z, pt2.z);
	double maxZ = std::max(pt1.z, pt2.z);
	return z+height >= minZ && z <= maxZ;
}


Vertex Line::pointAt(double z) const
{
	assert(CrossesPlane(z));
	/*
	point.x         = b->x + (a->x - b->x) * (slice_z - b->z) / (a->z - b->z);
            point.y         = b->y + (a->y - b->y) * (slice_z - b->z) / (a->z - b->z);*/
	Vertex output;
	output.z = z;
	output.x = pt2.x + (pt1.x - pt2.x) * (z - pt2.z) / (pt1.z - pt2.z);
	output.y = pt2.y + (pt1.y - pt2.y)*(z - pt2.z) / (pt1.z - pt2.z);
// 	output.r = pt2.r + (pt1.r - pt2.r)*(z - pt2.z) / (pt1.z - pt2.z);
// 	output.g = pt2.g + (pt1.g - pt2.g)*(z - pt2.z) / (pt1.z - pt2.z);
// 	output.b = pt2.b + (pt1.b - pt2.b)*(z - pt2.z) / (pt1.z - pt2.z);

	output.u = pt2.u + (pt1.u - pt2.u)*(z - pt2.z) / (pt1.z - pt2.z);
	output.v = pt2.v + (pt1.v - pt2.v)*(z - pt2.z) / (pt1.z - pt2.z);

	return output;
}

Vertex Line::pointAtDistance(double distance) const
{
	double ratio = distance / this->length();
	//assert(ratio <= 1.0 && ratio >= 0);
	if (ratio < 0)
		ratio = 0;
	else if (ratio > 1.0)
		ratio = 1.0;
	/*
	point.x         = b->x + (a->x - b->x) * (slice_z - b->z) / (a->z - b->z);
	point.y         = b->y + (a->y - b->y) * (slice_z - b->z) / (a->z - b->z);*/
	Vertex output;
	output.z = pt1.z + (pt2.z - pt1.z)  * ratio;;
	output.x = pt1.x + (pt2.x - pt1.x) * ratio;
	output.y = pt1.y + (pt2.y - pt1.y) * ratio;

	output.u = pt1.u + (pt2.u - pt1.u) * ratio;
	output.v = pt1.v + (pt2.v - pt1.v) * ratio;

	return output;
}

bool Line::InsideLayer(double z, double height) const
{
	return (pt1.z >= z && pt1.z <= z+height &&
		pt2.z >= z && pt2.z <= z+height);
}

bool Line::CrossesPlane(double planeHeight) const
{
	double min = std::min(pt1.z, pt2.z);
	double max = std::max(pt1.z, pt2.z);
	return (min <= planeHeight && max >= planeHeight);
}

bool Line::hasXYLength() const
{
	return (pt1.x != pt2.x || pt1.y != pt2.y);
}

double Line::length() const
{
	return sqrt(pow(pt2.x - pt1.x, 2) +
		pow(pt2.y - pt1.y, 2) +
		pow(pt2.z - pt1.z, 2));
}

double Line::sqrdLength() const
{
	return pow(pt2.x - pt1.x, 2) +
		pow(pt2.y - pt1.y, 2) +
		pow(pt2.z - pt1.z, 2);
}

std::size_t Line::hash() const
{
	std::size_t seed1 = 0;
	boost::hash_combine(seed1, pt1.x);
	boost::hash_combine(seed1, pt1.y);
	boost::hash_combine(seed1, pt1.u);
	boost::hash_combine(seed1, pt1.v);
	std::size_t seed2 = 0;
	boost::hash_combine(seed2, pt2.x);
	boost::hash_combine(seed2, pt2.y);
	boost::hash_combine(seed2, pt2.u);
	boost::hash_combine(seed2, pt2.v);
	return seed1^seed2;
	/*
	if (pt1.x > pt2.x)
	{
		boost::hash_combine(seed, pt1.x);
		boost::hash_combine(seed, pt1.y);
		boost::hash_combine(seed, pt1.u);
		boost::hash_combine(seed, pt1.v);

		boost::hash_combine(seed, pt2.x);
		boost::hash_combine(seed, pt2.y);
		boost::hash_combine(seed, pt2.u);
		boost::hash_combine(seed, pt2.v);
	}
	else// if (pt1.x < pt2.x)
	{
		boost::hash_combine(seed, pt2.x);
		boost::hash_combine(seed, pt2.y);
		boost::hash_combine(seed, pt2.u);
		boost::hash_combine(seed, pt2.v);

		boost::hash_combine(seed, pt1.x);
		boost::hash_combine(seed, pt1.y);
		boost::hash_combine(seed, pt1.u);
		boost::hash_combine(seed, pt1.v);
	}
	/*
	else if(pt1.y > pt2.y)
	{
		boost::hash_combine(seed, pt1.x);
		boost::hash_combine(seed, pt1.y);
		boost::hash_combine(seed, pt1.u);
		boost::hash_combine(seed, pt1.v);

		boost::hash_combine(seed, pt2.x);
		boost::hash_combine(seed, pt2.y);
		boost::hash_combine(seed, pt2.u);
		boost::hash_combine(seed, pt2.v);
	}
	else
	{
		boost::hash_combine(seed, pt2.x);
		boost::hash_combine(seed, pt2.y);
		boost::hash_combine(seed, pt2.u);
		boost::hash_combine(seed, pt2.v);

		boost::hash_combine(seed, pt1.x);
		boost::hash_combine(seed, pt1.y);
		boost::hash_combine(seed, pt1.u);
		boost::hash_combine(seed, pt1.v);
	}*/
	//return seed;
}

std::size_t Line::hash_position() const noexcept
{
	std::size_t pt1Hash = 0;
	std::size_t pt2Hash = 0;
	boost::hash_combine(pt1Hash, pt1.x);
	boost::hash_combine(pt1Hash, pt1.y);

	boost::hash_combine(pt2Hash, pt2.x);
	boost::hash_combine(pt2Hash, pt2.y);

	return pt1Hash^pt2Hash;
}

std::size_t Line::hash_with_colour() const noexcept
{
	std::size_t seed = hash_position();
	boost::hash_combine(seed, colour.hash());
	return seed;
}

void Line::setColour(const Colour &incolour)
{
  colour = incolour;
//   if(incolour.r < 0)
//   {
//     r = maxColour+1;g = maxColour+1;b = maxColour+1;
//   }
//   else
//   {
//   r = incolour.r*(double)Line::maxColour;
//   b = incolour.b*(double)Line::maxColour;
//   g = incolour.g*(double)Line::maxColour;
//   }
// 	pt1.r = incolour.r;
// 	pt1.g = incolour.g;
// 	pt1.b = incolour.b;
// 
// 	pt2.r = incolour.r;
// 	pt2.g = incolour.g;
// 	pt2.b = incolour.b;
}

// Pt1 (32.380000,49.185000) E21003.686828 Pt2 (38.130000,43.435000) E21004.029570Distance: 0.004032
bool Line::splitLine(double distance, Line &lineA, Line &lineB) const
{
	lineA.LineParam = this->LineParam;
	lineB.LineParam = this->LineParam;
	lineA.pt1 = this->pt1;
	double ratio = distance / this->length();
	//if (ratio > 0.999 || ratio < 0.001)
		//return false;
	lineA.pt2.x = pt2.x + (pt1.x - pt2.x) * ratio;
	lineA.pt2.y = pt2.y + (pt1.y - pt2.y)*ratio;
	lineA.pt2.z = this->pt1.z;
// 	lineA.pt2.r = pt2.r + (pt1.r - pt2.r)*ratio;
// 	lineA.pt2.g = pt2.g + (pt1.g - pt2.g)*ratio;
// 	lineA.pt2.b = pt2.b + (pt1.b - pt2.b)*ratio;

	lineA.pt2.u = pt2.u + (pt1.u - pt2.u)*ratio;
	lineA.pt2.v = pt2.v + (pt1.v - pt2.v)*ratio;

	lineB.pt1 = lineA.pt2;
	lineB.pt2 = this->pt2;
	return true;
}

bool Line::splitLine(double u, double v, Line &lineA, Line &lineB) const
{
	lineA.pt1 = this->pt1;
	lineB.pt2 = this->pt2;

	lineA.pt2.x = (pt2.x + pt1.x) / 2.0;
	lineA.pt2.y = (pt2.y + pt1.y) / 2.0;

	lineA.pt2.u = u;
	lineA.pt2.v = v;

	lineB.pt1 = lineA.pt2;
	return true;
}

unsigned int BodySplitter::lineIntersection(double Line1Pt1x, double Line1Pt1y, double Line1Pt2x, double Line1Pt2y,
	double Line2Pt1x, double Line2Pt1y, double Line2Pt2x, double Line2Pt2y,
	double &outx, double &outy)
{
	const unsigned int NO_INTERSECTION = 0;
	const unsigned int SINGLE_INTERSECTION = 1;
	const unsigned int COLINEAR_INPLACE = 2;
	// Store the values for fast access and easy
	// equations-to-code conversion
	//float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
	//float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

	double d = (Line1Pt1x - Line1Pt2x) * (Line2Pt1y - Line2Pt2y) - (Line1Pt1y - Line1Pt2y) * (Line2Pt1x - Line2Pt2x);
	// If d is zero, there is no intersection
	if (d == 0) return NO_INTERSECTION;

	//check for parallel lines, since its a contour line, we say this isnt intersecting
	if (Line1Pt1x != Line1Pt2x &&
		Line2Pt1x != Line2Pt2x)
	{
		if (fabs((Line1Pt2y - Line1Pt1y) / (Line1Pt2x - Line1Pt1x) -
			(Line2Pt2y - Line2Pt1y) / (Line2Pt2x - Line2Pt1x)) < 1)
			return COLINEAR_INPLACE;
	}

	// Get the x and y
	double pre = (Line1Pt1x*Line1Pt2y - Line1Pt1y*Line1Pt2x), post = (Line2Pt1x*Line2Pt2y - Line2Pt1y*Line2Pt2x);
	double x = (pre * (Line2Pt1x - Line2Pt2x) - (Line1Pt1x - Line1Pt2x) * post) / d;
	double y = (pre * (Line2Pt1y - Line2Pt2y) - (Line1Pt1y - Line1Pt2y) * post) / d;
	if (x < 0 || y < 0)
		return NO_INTERSECTION;	//Lines are always above zero
	// Check if the x and y coordinates are within both lines
	if (x < std::min(Line1Pt1x, Line1Pt2x) || x > std::max(Line1Pt1x, Line1Pt2x) ||
		x < std::min(Line2Pt1x, Line2Pt2x) || x > std::max(Line2Pt1x, Line2Pt2x)) return NO_INTERSECTION;
	if (y < std::min(Line1Pt1y, Line1Pt2y) || y > std::max(Line1Pt1y, Line1Pt2y) ||
		y < std::min(Line2Pt1y, Line2Pt2y) || y > std::max(Line2Pt1y, Line2Pt2y)) return NO_INTERSECTION;

	// Return the point of intersection
	outx = x;
	outy = y;

	return SINGLE_INTERSECTION;
	/*
	//Lines left to right, bottom to top
	if (Line1Pt1x > Line1Pt2x)	//Line 1 X
	std::swap(Line1Pt1x, Line1Pt2x);
	if (Line2Pt1x > Line2Pt2x)  //Line 2 x
	std::swap(Line2Pt1x, Line2Pt2x);
	if (Line2Pt1y > Line2Pt2y)  //Line 2 y
	std::swap(Line2Pt1y, Line2Pt2y);
	if (Line1Pt1y > Line1Pt2y)  //Line 1 y
	std::swap(Line1Pt1y, Line1Pt2y);


	//Trivial cases
	if (Line1Pt1x == Line2Pt1x &&
	Line1Pt1y == Line2Pt1y)
	{
	outx = Line1Pt1x; outy = Line1Pt1y;
	if (Line1Pt2x == Line2Pt2x &&
	Line1Pt2y == Line2Pt2y)
	{
	return COLINEAR_INPLACE;
	}
	return SINGLE_INTERSECTION;
	}
	else if (Line1Pt2x == Line2Pt2x &&
	Line1Pt2y == Line2Pt2y)
	{
	outx = Line1Pt2x; outy = Line1Pt2y;
	return SINGLE_INTERSECTION;
	}
	//Deal with vertical Lines
	if (Line1Pt1x == Line1Pt2x)
	{
	if (Line2Pt1x < Line1Pt1x &&
	Line2Pt2x > Line1Pt1x &&
	Line2Pt1y > Line1Pt1y &&
	Line2Pt2y < Line1Pt2y)
	{

	}
	else
	return NO_INTERSECTION;
	}
	*/
}

Vertex Line::UVToVertexCoords(double u, double v) const
{
	double uvLength = sqrt((pt2.u - pt1.u)*(pt2.u - pt1.u) +
		(pt2.v - pt1.v)*(pt2.v - pt1.v));

	double distanceTo = sqrt((u - pt1.u)*(u - pt1.u) +
		(v - pt1.v)*(v - pt1.v));

	return pointAtDistance((distanceTo/uvLength)*length());
}

Line Line::getColinearLine(double u1, double v1, double u2, double v2) const
{
	return Line(UVToVertexCoords(u1,v1), UVToVertexCoords(u2,v2));
}

// void Line::setColourAverage()
// {
// 	double r = (pt1.r + pt2.r) / 2.0;
// 	double g = (pt1.g + pt2.g) / 2.0;
// 	double b = (pt1.b + pt2.b) / 2.0;
// 	pt1.r = r; pt1.g = g; pt1.b = b;
// 	pt2.r = r; pt2.g = g; pt2.b = b;
// }

// void Line::divideIntoColourSegments(const Line &input, double res, std::vector<Line> &out)
// {
// 	//Number of line segments
// 	int segments = std::max(fabs(input.pt1.r - input.pt2.r) / res,
// 		std::max(fabs(input.pt1.g - input.pt2.g) / res,
// 			fabs(input.pt1.b - input.pt2.b) / res));
// 	double distanceForEach = input.length() / (double)segments;
// 	Vertex lastPoint = input.pt1;
// 	for (int i = 1; i < segments; i++)
// 	{
// 		Line newLine(lastPoint, input.pointAtDistance(distanceForEach*(double)i),input.getParam());
// 		lastPoint = newLine.pt2;
// 		newLine.setColourAverage();
// 		out.push_back(newLine);
// 	}
// 	Line newLine(lastPoint, input.pt2, input.getParam());
// 	newLine.setColourAverage();
// 	out.push_back(newLine);
// }

double Line::distancetoPointR2(const Vertex &pt) const
{
	return fabs((pt2.y - pt1.y)*pt.x - (pt2.x - pt1.x)*pt.y + pt2.x*pt1.y - pt2.y*pt1.x) /
		sqrt((pt2.y - pt1.y)*(pt2.y - pt1.y) + (pt2.x - pt1.x)*(pt2.x - pt1.x));
}


/*
Vector GetClosetPoint(Vector A, Vector B, Vector P, bool segmentClamp)
{
	Vector AP = P - A:
	Vector AB = B - A;
	float ab2 = AB.x*AB.x + AB.y*AB.y;
	float ap_ab = AP.x*AB.x + AP.y*AB.y;
	float t = ap_ab / ab2;
	if (segmentClamp)
	{
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;
	}
	Vector Closest = A + AB * t;
	return Closest;
}
*/
Vertex Line::pointClostestTo(const Vertex &inPt) const
{
	//if (this->pt1.y == this->pt2.y)
	//{
	//	return Vertex(0, 0);
	//}
	double APx = inPt.x - pt1.x;
	double APy = inPt.y - pt1.y;

	double ABx = pt2.x - pt1.x;
	double ABy = pt2.y - pt1.y;
	double ab2 = ABx*ABx + ABy*ABy;
	double ap_ab = APx*ABx + APy*ABy;

	double t = ap_ab / ab2;

	if (t < 0.0) t = 0.0;
	else if (t>1.0) t = 1.0;

	Vertex output(pt1.x + ABx*t, pt1.y + ABy*t);
	double distance = sqrt((output.x - pt1.x)*(output.x - pt1.x) + (output.y - pt1.y)*(output.y - pt1.y));

	return pointAtDistance(distance);
}

std::vector<Line> Line::breakLineToPixels(const BodySplitterTexture *ptrTexture) const
{
	std::vector<Line> output;
	
	std::vector<Colour> cols = ptrTexture->pixelColours(this->pt1.u, this->pt1.v, this->pt2.u, this->pt2.v);
//	double colourLineSize = this->length() / (double)cols.size();
	double deltaX = (pt2.x - pt1.x) / (double)cols.size();
	double deltaY = (pt2.y - pt1.y) / (double)cols.size();
	std::unordered_map<Colour, int> colourMap;
	int colourCounter = 0;
	for(int i = 0; i < cols.size(); i++)
	{
	  if(colourMap.count(cols[i]) == 0)
	    colourMap[cols[i]] = colourCounter++;
#ifndef _WIN32
	  std::cout <<"\033[1;"<<colourMap[cols[i]]+41<<"mX";
#endif
	}
#ifndef _WIN32
	std::cout << "\033[0m";
	std::cout << "\n";
#endif
	Vertex pt1(this->pt1);
	Vertex pt2(pt1.x + deltaX, pt1.y + deltaY);
	Colour lastColour = cols.front();
	for (std::size_t i = 1; i < cols.size(); i++)
	{
	  if(cols[i] != lastColour)
	  {
	    output.push_back(Line(pt1,pt2));
	    output.back().setColour(lastColour);
	    pt1 = pt2;
	    lastColour = cols[i];
	  }
	  pt2.x += deltaX;
	  pt2.y += deltaY;
	}
	if(output.empty()) //This means its all one colour
	{
	  output.push_back(Line(pt1,pt2));
	  output.back().setColour(lastColour);
	}
#ifndef _WIN32
	std::cout << "Line size: "<<output.size()<<"\n";
#endif
	return output;
}

void Line::makePathContinous(std::vector<Line> &out)
{
	if (out.empty())
		return;
	for (std::size_t i = 0; i < out.size()-1; i++)
	{
		out[i].pt2 = out[i + 1].pt1;
	}
	out.back().pt2 = out.front().pt1;
}

bool Line::interesctsLineR2(const Line &targ) const
{
	
	double den = ((targ.pt2.y - targ.pt1.y)*(this->pt2.x - this->pt1.x) - (targ.pt2.x - targ.pt1.x)*(this->pt2.y - this->pt1.y));
	double num1 = ((targ.pt2.x - targ.pt1.x)*(this->pt1.y - targ.pt1.y) - (targ.pt2.y - targ.pt1.y)*(this->pt1.x - targ.pt1.x));
	double num2 = ((this->pt2.x - this->pt1.x)*(this->pt1.y - targ.pt1.y) - (this->pt2.y - this->pt1.y)*(this->pt1.x - targ.pt1.x));
	double u1 = num1 / den;
	double u2 = num2 / den;
	//std::cout << u1 << ":" << u2 << std::endl;
	if (den == 0 && num1 == 0 && num2 == 0) /* The two lines are coincidents */
		return true;
	if (den == 0)/* The two lines are parallel */
		return false;
	if (u1 <0 || u1 > 1 || u2 < 0 || u2 > 1)/* Lines do not collide */
		return false;
	 /* Lines DO collide */
	return true;
}

#if USE_INT_COORDS
bool Line::IsColinear(const Line &in) const
{
	if ((this->pt1.x == this->pt2.x) &&(in.pt1.x == in.pt2.x))
		return true;
	if ((this->pt1.x == this->pt2.x) ^ (in.pt1.x == in.pt2.x))
		return false;
	double slopeA = fabs(((double)pt2.y - (double)pt1.y) / ((double)pt2.x - (double)pt1.x));
	double slopeB = fabs(((double)in.pt2.y - (double)in.pt1.y) / ((double)in.pt2.x - (double)in.pt1.x));
	return fabs(slopeA - slopeB) < 0.01;
}
#else
bool Line::IsColinear(const Line &in) const
{
	if (fabs(this->pt1.x - this->pt2.x) < 0.01 &&fabs(in.pt1.x - in.pt2.x) < 0.01)
		return true;
	double slopeA = fabs((pt2.y - pt1.y) / (pt2.x - pt1.x));
	double slopeB = fabs((in.pt2.y - in.pt1.y) / (in.pt2.x - in.pt1.x));
	return fabs(slopeA - slopeB) < 0.01;
}
#endif

bool Line::touchesLine(const Line &input) const
{
	return input.touchesPoint(pt1) || input.touchesPoint(pt2);
	//return pt1 == input.pt1 || pt1 == input.pt2 || pt2 == input.pt1 || pt2 == input.pt2;
}

bool Line::touchesPoint(const Vertex &point) const
{
	return point.MatchesLocation(pt1) || point.MatchesLocation(pt2);
}

Colour Line::getColour() const
{
	return colour;
}

void Line::mergeWith(const Line &input)
{
	if (pt2 == input.pt1)
	{
		pt2 = input.pt2;
	}
	else if (pt1 == input.pt1)
	{
		pt1 = pt2;
		pt2 = input.pt2;
	}
	else if (pt1 == input.pt2)
	{
		pt1 = input.pt1;
	}
	else if (pt2 == input.pt2)
	{
		pt2 = input.pt1;
	}
	//else
	//{
	//	wxLogDebug("Error on merging lines");

	//}
}

void Line::addParameter(unsigned int newType)
{
	if (LineParam == LineType::CREATED_NO_PARAM)
		LineParam = newType;
	else
		LineParam |= newType;
}

bool Line::isLineOfType(unsigned int type) const
{
	return (LineParam & type) > 0;
}

bool Line::matchesLocation(const Line &in) const
{
	if (pt1.MatchesLocation(in.pt1) && pt2.MatchesLocation(in.pt2))
		return true;
	else if (pt1.MatchesLocation(in.pt2) && pt2.MatchesLocation(in.pt1))
		return true;
	return false;
}

void Line::removeDuplicateLines(std::vector<Line> &inputLines)
{
	std::vector<Line> output;
	std::unordered_set<std::size_t> usedLines;
//	return;
	/*
	
	
	for (uint i = 0; i < inputLines.size(); i++)
	{
		if (usedLines.count(inputLines[i].hash()) == 0)
		{
			output.push_back(inputLines[i]);
			usedLines.insert(inputLines[i].hash());
		}

	}
	*/
	
	
	for (int i = 0; i < inputLines.size(); i++)
	{
		bool foundMatch = false;
		if (inputLines[i].isLineOfType(Line::LineType::OUTERPERIM))
		{
			output.push_back(inputLines[i]);
			continue;
		}
		for (int k = 0; k < inputLines.size(); k++)
		{
			if (i == k)
				continue;

			if (inputLines[i].IsColinear(inputLines[k]))
			{
				//Keep the biggest
				if (inputLines[i].sqrdLength() > inputLines[k].sqrdLength())
				{
					output.push_back(inputLines[i]);
				}
				else
					output.push_back(inputLines[k]);
				foundMatch = true;
				break;
			}
		}
		if (foundMatch == false)
			output.push_back(inputLines[i]);
	}
	
	if(inputLines.size() != output.size())
		inputLines = output;
}

void Line::removeExtraInsets(std::vector<Line> &inputlines)
{
	for (auto outerIT = inputlines.begin(); outerIT != inputlines.end(); ++outerIT)
	{
		if (outerIT->isLineOfType(Line::LineType::OUTERPERIM))
			continue;
		else if (outerIT->isLineOfType(Line::LineType::TO_INSET))
		{
			//Find if a matching line exists, in such a case, delete them both
			for (auto innerIT = outerIT+1; innerIT != inputlines.end(); ++innerIT)
			{
				if (innerIT->isLineOfType(Line::LineType::TO_INSET))
				{
					if (outerIT->getColour() == innerIT->getColour())
					{
						if (outerIT->touchesPoint(innerIT->pt1) && outerIT->touchesPoint(innerIT->pt2))
						{
							//Lines are probably the same
							inputlines.erase(innerIT); //Is ahead of outerIT
							outerIT = inputlines.erase(outerIT);
							break;
						}
					}
				}
			}
			if (outerIT == inputlines.end())//avoid going past end
				break;
		}
	}
	removeUnconnectedLines(inputlines);
	return;
	//return;
	//removeZeroLengthLines(inputlines);
	std::vector<Line> output;
	for (int i = 0; i < inputlines.size(); i++)
	{
		if (!inputlines[i].isLineOfType(LineType::OUTERPERIM))
		{
			/*
			uint8_t lineTouches = 0;
			for (int j = 0; j < inputlines.size(); j++)
			{
				if (j == i)
					continue;
				if (inputlines[j].isLineOfType(LineType::OUTERPERIM) & (inputlines[j].touchesPoint(inputlines[i].pt1) | inputlines[j].touchesPoint(inputlines[i].pt2)))
					lineTouches++;
				if (lineTouches > 2)
				{
					break;
				}
			}
			if (lineTouches == 1)
				output.push_back(inputlines[i]);
				*/
				
		}
		else
			output.push_back(inputlines[i]);
	}
	
	if(inputlines.size() != output.size())
		inputlines = output;
		
}

void Line::removeUnconnectedLines(std::vector<Line> &input)
{
	for (auto outerIT = input.begin(); outerIT != input.end(); ++outerIT)
	{
		int touchCount = 0;
		for (auto innerIT = input.begin(); innerIT != input.end(); ++innerIT)
		{
			if (outerIT == innerIT)
				continue;
			if (innerIT->touchesPoint(outerIT->pt1))
				touchCount++;
			else if (innerIT->touchesPoint(outerIT->pt2))
				touchCount++;
			if (touchCount == 2)
				break;
		}
		if (touchCount != 2)
		{
			outerIT = input.erase(outerIT);
			if (outerIT == input.end())
				break;
		}
	}
}

//SEL
//    remove ShortLines will combine line segments until the distance from pt1 to pt2 is greater than the
//    given tolerance sqrdLength
void Line::removeShortLines(std::vector<Line> &inputLines, double sqrdLength)
{
#if 0 //SEL I don't like this implementation
	bool skippedLast = false;
	std::vector<Line> output;
	for (uint i = 0; i < inputLines.size(); i++)
	{
		if (skippedLast)
		{
			inputLines[i].pt1 = output.back().pt2;
			output.push_back(inputLines[i]);
			skippedLast = false;
		}
		else
		{
			if (inputLines[i].sqrdLength() < sqrdLength && output.empty() == false)
			{
				skippedLast = true;
			}
			else
				output.push_back(inputLines[i]);
		}
	}
	inputLines = output;
#else // So I made my own
	std::vector<Line> output;
	uint i = 1;

	// push the first candidate line segment into the output vector
	output.push_back(inputLines.front());
	

	while (i < inputLines.size())
	{
		// If candidate line segment is to small
		if (output.back().sqrdLength() < sqrdLength)
		{
			// Then extend the candidate line segment to the termination point (pt2 ) of the next line segment.
			output.back().pt2 = inputLines[i].pt2;
		}
		else
		{
			// else let's push a new candidate line segment onto the output vector
			output.push_back(inputLines[i]);
		}

		// advance the index to the next line segment
		i++;
	}

	if (output.size() != inputLines.size())
		inputLines = output;


#endif
}

void Line::removeZeroLengthLines(std::vector<Line> &inputLines)
{
	std::vector<Line> output;
	for (int i = 0; i < inputLines.size(); i++)
	{
		if (inputLines[i].sqrdLength() > 0.00000)
			output.push_back(inputLines[i]);
	}
	if (output.size() != inputLines.size())
		inputLines = output;
}

// void Line::setRandomColour(std::vector<Line> &inputLines)
// {
// 	Colour newColour;
// 	newColour.r = (double)(rand() % 100) / 100.0;
// 	newColour.g = (double)(rand() % 100) / 100.0;
// 	newColour.b = (double)(rand() % 100) / 100.0;
// 
// 	for (auto it = inputLines.begin(); it != inputLines.end(); ++it)
// 	{
// 		it->setColour(newColour);
// 	}
// }

void Line::setLineSetToColour(std::vector<Line> &input, const Colour &col)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i].setColour(col);
	}
}

void Line::removePointlessPoints(std::vector<Line> &input)
{
	std::vector<Line> output;
	for (int i = 0; i < input.size(); i++)
	{
		Line *nextLine;
		if (i + 1 == input.size())
		{
			nextLine = &input.front();
		}
		else
			nextLine = &input[i + 1];
		if (input[i].getColour() == nextLine->getColour() && input[i].IsColinear(*nextLine))
		{
			Line newLine(input[i].pt1, nextLine->pt2);
			newLine.setColour(input[i].getColour());
			output.push_back(newLine);
			i++;
			if (i == input.size())
				break;
		}
		else
			output.push_back(input[i]);
	}
	input = output;
}

void Line::rotateLine(double angle)
{
	glm::mat4 rotateMat;
	glm::vec2 rotationVec; 
	rotationVec.x = glm::radians(angle);
	rotateMat = glm::rotate(rotateMat, rotationVec.x, glm::vec3(0, 0, 1.0));
	glm::vec4 glmPt1 = pt1.toglm4();
	glm::vec4 glmPt2 = pt2.toglm4();
	glmPt1 = rotateMat*glmPt1;
	glmPt2 = rotateMat*glmPt2;
	pt1.x = glmPt1.x;
	pt1.y = glmPt1.y;

	pt2.x = glmPt2.x;
	pt2.y = glmPt2.y;
}

size_t Line::color_hash() const
{
	return colour.hash();
}


void Line::rotateLines(std::vector<Line> &input, double degrees)
{
	for (auto it = input.begin(); it != input.end(); ++it)
		it->rotateLine(degrees);
}

void Line::generateLines(std::vector<Line> &output, double lineSpacing, double x1, double x2, double y1, double y2, unsigned int linetype)
{
	bool alternator = true;
	for (double x = x1; x < x2; x += lineSpacing)
	{
		if(alternator)
			output.push_back(Line(Vertex(x, y1), Vertex(x, y2),linetype));
		else
			output.push_back(Line(Vertex(x, y2), Vertex(x, y1),linetype));
		alternator = !alternator;
	}
}

void Line::generateLinesB(std::vector<Line> &output, double lineSpacing, double x1, double x2, double y1, double y2, unsigned int linetype)
{
	bool alternator = true;
	for (double y = y1; y < y2; y += lineSpacing)
	{
		if (alternator)
			output.push_back(Line(Vertex(x1, y), Vertex(x2, y), linetype));
		else
			output.push_back(Line(Vertex(x2, y), Vertex(x1, y), linetype));
		alternator = !alternator;
	}
}

void Line::translateLines(std::vector<Line> &output, double x, double y)
{
	for (auto it = output.begin(); it != output.end(); ++it)
	{
		it->pt1.x += x; it->pt1.y += y;
		it->pt2.x += x; it->pt2.y += y;
	}
}
void Line::translateLines(std::vector<Vertex2D>& output, double x, double y)
{
	for (auto it = output.begin(); it != output.end(); ++it)
	{
		it->x += x; it->y += y;
	}
}

void Line::addParamToSet(std::vector<Line> &output, unsigned int param)
{
// 	for (auto it : output)
// 		it.addParameter(param);
}

bool Line::lineCrossesContour(const Line &testLine, const std::vector<Line> &contour)
{
	if (contour.empty())
		return false;
	for (auto it : contour)
	{
		if (it.interesctsLineR2(testLine))
			return true;
	}
	return false;
}

double Line::getSetLength(const std::vector<Line> &lineSet)
{
	double output = 0.0;
	for (auto it : lineSet)
		output += it.length();
	return output;
}

void Line::reverseSet(std::vector<Line> &in)
{
	for (auto it : in)
		it.reverse();
	std::reverse(in.begin(), in.end());
}

#ifndef NDEBUG

void Line::writeLinesToPolyFile(const std::string& filename, const std::vector< Line >& lines)
{
    std::ofstream foutput;
    foutput.open(filename);
    foutput << lines.size() << "\n";
    for (uint i = 0; i < lines.size(); i++)
    {
	    foutput << lines[i].pt1.x << " " << lines[i].pt1.y << "\n";
    }
    foutput.close();
}
#endif

void Line::removeEqualOpposites(std::vector< Line >& inputLines)
{
  if(inputLines.size() <= 1)
    return;
  bool foundMatch = true;
  while(foundMatch)
  {
    foundMatch = false;
    
    for(int source = 0; source < inputLines.size()-1; ++source)
    {
      for(int compare = source+1; compare < inputLines.size(); ++compare)
      {
	if(inputLines[source].pt1.x == inputLines[compare].pt2.x &&
	  inputLines[source].pt1.y == inputLines[compare].pt2.y &&
	  inputLines[source].pt2.x == inputLines[compare].pt1.x &&
	  inputLines[source].pt2.y == inputLines[compare].pt1.y)
	{
	  foundMatch = true;
	  inputLines.erase(inputLines.begin()+compare);
	  break;
	}
	if(inputLines[source].pt1.x == inputLines[compare].pt1.x &&
	  inputLines[source].pt1.y == inputLines[compare].pt1.y &&
	  inputLines[source].pt2.x == inputLines[compare].pt2.x &&
	  inputLines[source].pt2.y == inputLines[compare].pt2.y)
	{
	  foundMatch = true;
	  inputLines.erase(inputLines.begin()+compare);
	  break;
	}
      }
      if(foundMatch)
	break;
    }
  }
}

std::vector<Line>::iterator Line::getShortestLine(std::vector<Line> &inputSet)
{
	assert(inputSet.empty() == false);
	if (inputSet.size() == 1)
		return inputSet.begin();
	std::vector<Line>::iterator shortestLine = inputSet.begin();
	double shortestDistance = shortestLine->sqrdLength();
	for (std::vector<Line>::iterator lineIT = inputSet.begin()+1; lineIT != inputSet.end(); ++lineIT)
	{
		if (lineIT->sqrdLength() < shortestDistance)
		{
			shortestDistance = lineIT->sqrdLength();
			shortestLine = lineIT;
		}
	}
	return shortestLine;
}

std::vector<Line>::iterator Line::getClosestLine(std::vector<Line> &lineSet, const Line &targetLine)
{
	if (lineSet.empty())
		return lineSet.end();
	double shortestDistance = Vertex::distanceR2Sqrd(lineSet.front().pt1, targetLine.pt2);
	std::vector<Line>::iterator closestLine = lineSet.begin();
	for (auto it = lineSet.begin(); it != lineSet.end(); ++it)
	{
		if (Vertex::distanceR2Sqrd(targetLine.pt2,it->pt1) < shortestDistance)
		{
			shortestDistance = Vertex::distanceR2Sqrd(targetLine.pt2, it->pt1);
			closestLine = it;
		}
		if (Vertex::distanceR2Sqrd(targetLine.pt2, it->pt2) < shortestDistance)
		{
			shortestDistance = Vertex::distanceR2Sqrd(targetLine.pt2, it->pt2);
			it->reverse();
			closestLine = it;
		}
	}
	return closestLine;
}

bool Line::isSelfIntersecting(const std::vector< Line >& in)
{
  for(auto it = in.begin(); it != in.end(); ++it)
  {
    for(auto innerIT = it+1; innerIT != in.end(); ++innerIT)
    {
      if(!it->touchesLine(*innerIT) && it->interesctsLineR2(*innerIT))
	return true;
    }
  }
  return false;
}

void Line::writeLinesWithColourToFile(const std::string &fileName, const std::vector<Line> &lines)
{
	std::ofstream outfile;
	outfile.open(fileName);
	if (outfile.is_open() == false)
		return;
	writeLinesWithColourToFile(outfile,lines);

	outfile.close();
}

void Line::writeLinesWithColourToFile(std::ofstream& fileStream, const std::vector< Line >& lines)
{
  	fileStream << lines.size() << "\n";
	for (auto it : lines)
	{
		fileStream << (int)it.colour.getR<uint8_t>() << " " << (int)it.colour.getG<uint8_t>() << " " << (int)it.colour.getB<uint8_t>() << "\n";
		fileStream << it.pt1.x << " " << it.pt1.y << " " << it.pt1.z << "\n";
		fileStream << it.pt2.x << " " << it.pt2.y << " " << it.pt2.z << "\n";
	}
}

double Line::getAngleBetween(const Line &lineA, const Line &lineB)
{
	glm::vec3 lineAVec(lineA.pt2.x - lineA.pt1.x, lineA.pt2.y - lineA.pt1.y,0);
	glm::vec3 lineBVec(lineB.pt2.x - lineB.pt1.x, lineB.pt2.y - lineB.pt1.y,0);

	return acos(glm::dot(lineAVec, lineBVec) / (glm::length(lineAVec)*glm::length(lineBVec)));
}

void Line::removeLineWithMultipleConnections(std::vector<Line> &inputLines)
{
	for(std::vector<Line>::iterator lineIT = inputLines.begin(); lineIT != inputLines.end();)
	{
		int matchingPoints = 0;

		for (std::vector<Line>::iterator compareLine = inputLines.begin(); compareLine != inputLines.end(); ++compareLine)
		{
			if (lineIT == compareLine)
				continue;

			if ((*lineIT).pt1.MatchesLocation((*compareLine).pt1))
				matchingPoints++;
			if ((*lineIT).pt2.MatchesLocation((*compareLine).pt1))
				matchingPoints++;
			if ((*lineIT).pt1.MatchesLocation((*compareLine).pt2))
				matchingPoints++;
			if ((*lineIT).pt2.MatchesLocation((*compareLine).pt2))
				matchingPoints++;
			if (matchingPoints > 3)
				break;
		}

		if (matchingPoints > 3)
		{
			lineIT = inputLines.erase(lineIT);
		}
		else
			++lineIT;
	}
}