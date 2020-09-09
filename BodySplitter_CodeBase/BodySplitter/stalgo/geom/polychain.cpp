#include <assert.h>
#include <iostream>
#include <list>
#include <math.h>
#include <vector>
#include <algorithm>

#include "../fpkernel.h"
#include "polychain.h"
#include "../constants.h"

using namespace std;


Polychain::Polychain() :
	vertices( vector<Vector>() ),
	closed( false )
{
}

	
Polychain::Polychain(const Polychain& p):
	vertices(p.vertices),
	closed(p.closed)
{
}


Polychain::~Polychain()
{
}


void Polychain::addVertex(Vector p)
{
	vertices.push_back(p);
}


Vector Polychain::getVertex(unsigned i) const
{
	assert( i<size() );
	return vertices.at(i);
}

unsigned Polychain::getNoSegs() const
{
	if( size() <= 1 )
		return 0;

	return isClosed() ? size() : size()-1;
}


Segment Polychain::getSegment(unsigned i) const
{
	assert( i<getNoSegs() );
	assert( size() > 0 );

	return Segment( getVertex(i), getVertex( (i+1)%size() ));

}


vector<Segment> Polychain::getSegments() const
{
	vector<Segment> l;

	for(unsigned i=0; i<getNoSegs(); i++)
		l.push_back(getSegment(i));

	return l;
}


void Polychain::getCwVertices( list<unsigned>& reflx  ) const
{
	reflx.clear();

	//A triangle is convex
	if( size() <= 2 )
		return;

	const unsigned start = isClosed() ? 0 : 1;
	for( unsigned i=start; i<getNoSegs(); i++ )
	{
		const double det = Vector::det3(
				getVertex( (i+size()-1)%size() ),
				getVertex( (i)%size() ),
				getVertex( (i+1)%size() ) );
	
		if( det<0 )
			reflx.push_back(i);
	}

	assert( reflx.size() == getNoCwVertices() );
}


unsigned  Polychain::getNoCwVertices() const
{
	if( size() <= 2 )
		return 0;

	unsigned no=0;
	const unsigned start = isClosed() ? 0 : 1;

	for( unsigned i=start; i<getNoSegs(); i++ )
	{
		const double det = Vector::det3(
				getVertex( (i+size()-1)%size() ),
				getVertex( (i)%size() ),
				getVertex( (i+1)%size() ) );
	
		if( det<0 )
			no++;
	}
	

	return no;
}


Rect Polychain::getBoundingBox() const
{
	if( size()==0 )
		return Rect(HUGE_DBL,-HUGE_DBL,HUGE_DBL,-HUGE_DBL);

	double l, r, t, b;
	l = r = getVertex(0).x;
	b = t = getVertex(0).y;

	for(unsigned i=1; i<size(); i++)
	{
		const Vector v = getVertex(i);
		l = min(l, v.x);
		r = max(r, v.x);
		b = min(b, v.y);
		t = max(t, v.y);
	}

	return Rect(l,r,b,t);
}


void Polychain::reverse()
{
	vector<Vector> newpnts;

	for(int i=size()-1; i>=0; i--)
		newpnts.push_back( getVertex(i) );

	vertices = newpnts;
}

bool Polychain::doesContain(const Vector &vec) const
{
	int nvert = vertices.size();
		int i, j, c = 0;
		for (i = 0, j = nvert - 1; i < nvert; j = i++) {
			if (((vertices[i].y>=vec.y) != (vertices[j].y>=vec.y)) &&
				(vec.x <= (vertices[j].x - vertices[i].x) * (vec.y - vertices[i].y) / (vertices[j].y - vertices[i].y) + vertices[i].x))
				c = !c;
		}
		return c;
		/*
		int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
	 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
  */
}
