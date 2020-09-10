#include <iostream>
#include <list>
#include <math.h>

#include "../fpkernel.h"
#include "ray.h"
#include "../constants.h"

using namespace std;



Ray::Ray()
{
}

Ray::Ray(Vector start, Vector dir) :
	start(start),
	dir(dir)
{
}

Ray::Ray(const Ray& r) :
	start(r.start),
	dir(r.dir)
{
}


Ray & Ray::operator*=(const double& scalar)
{
	dir *= scalar;
	return *this;
}


Ray Ray::operator-() const
{
	return Ray(getStart(), -getDir());
}


Ray& Ray::operator+=(const Vector& v)
{
	start += v;
	return *this;
}

Ray& Ray::operator-=(const Vector& v)
{
	start -= v;
	return *this;
}

Vector Ray::intersect(const Rect& rect) const
{
	list<double> times;


	if( fabs(dir.x) > 0 )
	{
		times.push_back( (rect.left-start.x)/dir.x );
		times.push_back( (rect.right-start.x)/dir.x );
	}
	if( fabs(dir.y) > 0 )
	{
		times.push_back( (rect.bottom-start.y)/dir.y );
		times.push_back( (rect.top-start.y)/dir.y );
	}


	double mintime = HUGE_DBL;
	while(times.size() > 0)
	{
		const double t = times.front();
		times.pop_front();

		if( t >= 0 && t<mintime )
			mintime = t;			
	}

	return getStart() + mintime*getDir();
}


double Ray::projDistance(const Vector& pnt) const
{
	return (pnt-start)*dir/( dir.lensq());
}


double Ray::orthoDistance(const Vector& pnt) const
{
	return Vector::det2( pnt-start, dir.norm() );
}


double Ray::minDistScalar(const Ray& r) const
{
	const double denom = (getDir() - r.getDir()).lensq();
	const double num = (getDir() - r.getDir())*(r.getStart() - getStart());

	return num/denom;
}


pair<double,double> Ray::intersect(const Ray& r) const
{
	// Solving the linear equation system...
	const double denom = Vector::det2(getDir(), r.getDir());
	const Vector p = (r.getStart() - getStart()) / denom;

	const double a = Vector(r.getDir().y, -r.getDir().x) * p;
	const double b = - Vector(-getDir().y, getDir().x) * p;

	return pair<double,double>(a,b);
}


Ray operator*(double l, const Ray& r)
{
	return Ray(r)*=l;
}


Ray operator+(Ray r, const Vector& v)
{
	r += v;
	return r;
}


Ray operator-(Ray r, const Vector& v)
{
	r -= v;
	return r;
}


ostream& operator<< (ostream& os, const Ray& r)
{
	return os << "[" << r.getStart() << " --" << r.getDir() << "--> " << r.getEnd() << "]";
}



