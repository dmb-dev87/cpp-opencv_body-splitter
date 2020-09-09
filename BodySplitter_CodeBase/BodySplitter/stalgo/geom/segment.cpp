#include <iostream>
#include <math.h>
#include <algorithm>

#include "../fpkernel.h"
#include "segment.h"

using namespace std;

Segment::Segment(const Segment &seg) :
	s(seg.s),
	e(seg.e)
{
}


Segment::Segment(const Vector &s, const Vector& e) :
	s(s),
	e(e)
{
}



Segment Segment::swapped() const
{
	return Segment(e,s);
}


double Segment::getDistance(const Segment& seg) const
{
	const double a1 = Vector::det3(s, e, seg.s);
	const double a2 = Vector::det3(s, e, seg.e);
	const double b1 = Vector::det3(seg.s, seg.e, s);
	const double b2 = Vector::det3(seg.s, seg.e, e);


	// They intersect
	if( a1*a2 < 0 && b1*b2 < 0 )
		return 0;


	if( a1*a2 < 0 )
	{	
		const double d2s = seg.getDistance( getStart() );
		const double d2e = seg.getDistance( getEnd() );
		return min(d2s, d2e);
	}

	if( b1*b2 < 0 )
	{
		const double d1s = getDistance( seg.getStart() );
		const double d1e = getDistance( seg.getEnd() );
		return min(d1s, d1e);
	}



	// Get distance by distances of endpoints to segments
	const double d1s = getDistance( seg.getStart() );
	const double d1e = getDistance( seg.getEnd() );
	const double d2s = seg.getDistance( getStart() );
	const double d2e = seg.getDistance( getEnd() );

	return min( min(d1s,d1e), min(d2s,d2e) );
}


Vector Segment::intersect(const Segment& seg) const
{
	//Tactics:
	//  1. Make getStart() the origin. We consider the vectors p, q
	//     that belong to 'seg' and vector v that belongs to getEnd()
	//  2. Let t*v be a vector on the supporting line of *this. We
	//     determine t such that the determinant of p, q, t*v is zero.


	const Vector& p = seg.getStart() - getStart();
	const Vector& q = seg.getEnd() - getStart();
	const Vector& v = getEnd() - getStart();

	const double numerator = Vector::det2(q,p);
	const double denominator = v * (q-p).ccw();

	return getStart() + numerator/denominator * v;
}


double Segment::getDistance(const Vector& v) const
{
	Vector s = getStart();
	Vector e = getEnd();

	//Make v be closer to s than to e
	if( (v-s).lensq() > (v-e).lensq() )
		swap(s,e);

	const Vector dir = e-s;

	if( dir*(v-s) <= 0 )
		return (v-s).len();
	return fabs( dir.norm().cw()*(v-s) );
}



ostream& operator<< (ostream& os, const Segment& s)
{
	return os << "[" << s.getStart() << " --> " << s.getEnd() << "]";
}


