#include <iostream>
#include <math.h>
#include <assert.h>

#include "../fpkernel.h"
#include "vector.h"

using namespace std;


Vector & Vector::operator+=(const Vector& v)
{
	x += v.x;
	y += v.y;
	return *this;
}


Vector & Vector::operator-=(const Vector& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}


Vector & Vector::operator*=(const double& scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

Vector & Vector::operator/=(const double& scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}

Vector Vector::operator-() const
{
	return Vector(-x,-y);
}

double Vector::len() const
{
	return sqrt(x*x+y*y);
}


double Vector::pseudophi() const
{
	if( fabs(x) >= fabs(y) )
	{
		// Sector to the right
		if( x > 0 )
		{
			const double phi = y/x;
			return phi<0 ? phi+8.0 : phi;
		}
		// Sector to the left
		else if( x < 0 )
		{
			const double phi = y/x;
			return 4.0 + phi;
		}
		// Hence, x and y are zero
		else
		{
			return 0.0;
		}
	}
	else if( fabs(y) >= fabs(x) )
	{
		// Sector to the top
		if( y > 0 )
		{
			const double phi = x/y;
			return 2.0-phi;
		}
		// Sector to the bottom
		if( y < 0 )
		{
			const double phi = x/y;
			return 6.0-phi;
		}
		// Actually, the impossible case, x=y=0. Should be catched above.
		else
		{
			assert(false);
			return 0.0;
		}

	}
	// This can only happen, if x or y is NaN.
	else
	{
		return 0.0;
	}
}

double Vector::lensq() const
{
	return x*x+y*y;
}

Vector Vector::norm() const
{
	return Vector(*this)/=len();
}

Vector Vector::ccw() const
{ 
	return Vector(-y,x);
}

Vector Vector::cw() const
{
	return Vector(y,-x);
}




double Vector::det3 (const Vector& a, const Vector& b, const Vector& c)
{
#ifdef WITH_COREBACKEND
	if((a.x == c.x && a.y == c.y) || (b.x == c.x && b.y == c.y) || (a.x == b.x && a.y == b.y))
		return 0;
#endif
	return ((a.x - b.x)*(b.y-c.y) + (b.y-a.y)*(b.x-c.x));
}

double Vector::det2(const Vector& a, const Vector& b)
{
#ifdef WITH_COREBACKEND
	if(a.x == b.x && a.y == b.y)
		return 0;
#endif
	return a.x*b.y - a.y*b.x;
}

double Vector::sin3(const Vector& a, const Vector& o, const Vector& b)
{
	return det3(a, o, b)/( (a-o).len() * (b-o).len() );
}

double Vector::sin2(const Vector& a, const Vector& b)
{
	return det2(a, b)/( a.len() * b.len() );
}

double Vector::cos2(const Vector& a, const Vector& b)
{
	return (a*b)/(a.len() * b.len() );
}


Vector Vector::rot(const double phi) const
{
	const double c = cos(phi);
	const double s = sin(phi);
	return Vector( x*c - y*s, x*s + y*c);
}


Vector operator-(const Vector& l, const Vector& r)
{
	return Vector(l.x-r.x, l.y-r.y);
}

Vector operator+(const Vector& l, const Vector& r)
{
	return Vector(l.x+r.x, l.y+r.y);
}

Vector operator*(double l, const Vector& r)
{
	return Vector(l*r.x, l*r.y);
}

Vector operator/(const Vector& r, double l )
{
	return Vector(r.x/l, r.y/l);
}


double operator*(const Vector& l, const Vector& r)
{
	return l.x*r.x + l.y*r.y;
}



ostream& operator<< (ostream& os, const Vector& v)
{
	return os << "(" << v.x << ", " << v.y << ")";
}

