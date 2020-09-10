#include <assert.h>
#include <iostream>
#include <math.h>
#include <algorithm>

#include "../fpkernel.h"
#include "rect.h"
#include "../constants.h"

using namespace std;


Rect::Rect(double left, double right, double bottom, double top) :
	left(left),
	right(right),
	bottom(bottom),
	top(top)
{
}


Rect Rect::enlarge(double size) const
{
	Rect r(left-size, right+size, bottom-size, top+size);

	if( r.getWidth() < 0 )
		r.left = r.right = (left+right)/C_2;
	if( r.getHeight() < 0 )
		r.top = r.bottom = (top+bottom)/C_2;

	return r;
}


Rect Rect::enlargeFactor(double factor) const
{
	assert( factor > 0 );

	const double cx = (left+right)/C_2;
	const double cy = (bottom+top)/C_2;

	const double w = getWidth()*factor;
	const double h = getHeight()*factor;

	return Rect(cx-w/C_2, cx+w/C_2, cy-h/C_2, cy+h/C_2);
}


Rect Rect::intersect(const Rect& r) const
{
	return Rect(
			max(left, r.left),
			min(right, r.right),
			max(bottom, r.bottom),
			min(top, r.top) );
}


bool Rect::doesContain(const Vector &v) const
{
	return left<=v.x && v.x<=right &&
		bottom<=v.y && v.y<=top;
}


double Rect::getDistance(const Vector& v) const
{
	double distHorSegs;
	double distVerSegs;

	if( v.x <= (left+right)/C_2 )
		distVerSegs = getLeftSeg().getDistance(v);
	else
		distVerSegs = getRightSeg().getDistance(v);

	if( v.y <= (bottom+top)/C_2 )
		distHorSegs = getBottomSeg().getDistance(v);
	else
		distHorSegs = getTopSeg().getDistance(v);

	return min(distVerSegs, distHorSegs);
}


ostream& operator<< (ostream& os, const Rect& r)
{
	return os << "[" << r.left << ", " << r.right <<
		", " << r.bottom << ", " << r.top << "]";
}


Rect Rect::fromCenter(const Vector&v, double width, double height)
{
	return Rect( v.x-width/C_2, v.x+width/C_2, v.y-height/C_2, v.y+height/C_2);
}


Segment Rect::getTopSeg() const
{
	return Segment( Vector(left,top), Vector(right,top) );
}


Segment  Rect::getBottomSeg() const
{
	return Segment( Vector(left,bottom), Vector(right,bottom) );
}


Segment  Rect::getLeftSeg() const
{
	return Segment( Vector(left,bottom), Vector(left, top) );
}

Segment  Rect::getRightSeg() const
{
	return Segment( Vector(right,bottom), Vector(right, top) );
}

