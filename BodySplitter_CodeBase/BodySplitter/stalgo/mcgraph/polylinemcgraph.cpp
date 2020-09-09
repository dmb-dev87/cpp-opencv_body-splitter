#include <assert.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <ostream>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>


#ifdef WITH_OGL
	#include "../glwindows/glplanewindow.h"
	#include "../glwindows/gltextplane.h"
#endif

#include "../util/util.h"
#include "../fpkernel.h"

#include "polylinemcgraph.h"


using namespace std;



const double PolylineMcgraph::MINDET=1e-16;


PolylineMcgraph::PolylineMcgraph(const PolychainVector& pchains, int gridcells,
		bool bothSides, bool nowalls) :
	mcgraph(pchains.getBoundingBox().enlargeFactor(C_2),  gridcells),
	pchains(pchains),
	bothSides(bothSides),
	nowalls(nowalls)
{
}

PolylineMcgraph::~PolylineMcgraph()
{

}


void PolylineMcgraph::initialize()
{
	loggerMcg.log(LL_INFO, "Inserting walls and motorcycles...\n");


	//Vertices where motorcycles emenate
	map< unsigned, list<unsigned> >  sourceVert;
	getSourceVertices(sourceVert);


	for(unsigned c=0; c<pchains.size(); c++)
	{
		//The polygonal chain we talk about
		const Polychain& ch = pchains.at(c);
		//The vertices where motorcycles emanate form 'ch'
		list<unsigned>& vert = sourceVert.at(c);


		//Add the segments...
		const unsigned noedges = edges.size();
		vector<Segment> e = ch.getSegments();
		edges.insert(edges.end(), e.begin(), e.end());

		if( !nowalls )
			for(vector<Segment>::iterator it = e.begin(); it!=e.end(); it++)
				insertWall(*it);


		//Get the reflex vertices and build a set of reflex
		//vertices
		set<unsigned> reflSet;
		list<unsigned> rvert;
		ch.getCwVertices(rvert);
		reflSet.insert( rvert.begin(), rvert.end() );


		//Add motorcycles
		list<unsigned>::iterator it = vert.begin();
		while( it != vert.end() )
		{
			int cwidx = *it;
			int ccwidx = (*it-1+ch.size())%ch.size();
			const Vector base = ch.getVertex(*it);


			//Is it reflex or convex?
			if( ! reflSet.count(*it) )
				swap(cwidx, ccwidx);

			const unsigned eccw = noedges+ccwidx;
			const unsigned ecw = noedges+cwidx;

			assert( eccw < edges.size());
			assert( ecw < edges.size());

			const Vector speed = speedFromEdges(base, eccw, ecw );
			baseedges.push_back( pair<unsigned,unsigned>(eccw,ecw) );

			launchMotorcycle(speed, base, C_0);
			it++;
		}
	}
}


bool PolylineMcgraph::handleHigherDegreeCrash(vector<unsigned> affected, Vector place, Vector& speed)
{
	affected.push_back( affected.front() );

	//Check for "new" reflex vertices
	for(unsigned i=0; i<affected.size()-1; i++)
	{
		//Get two mcs...
		const unsigned pre = affected.at(i);
		const unsigned post = affected.at(i+1);

		const Motorcycle& premc = getMotorcycles().at(pre);
		const Motorcycle& postmc = getMotorcycles().at(post);


		//Check if the angle in cw-direction from premc to postmc is > 180°
		if( premc.getSpeed().getDir().cw() * postmc.getSpeed().getDir() > eps.ZERO )
		{
			const unsigned ecw =  getEdgeCw(pre);
			const unsigned eccw = getEdgeCcw(post);

			const Segment& scw = edges.at(ecw);
			const Segment& sccw = edges.at(eccw);

			const Vector vcw = scw.getEnd() - scw.getStart();
			const Vector vccw = sccw.getStart() - sccw.getEnd();

			//Strictly reflexive...
			if( Vector::det2( vcw, vccw) < -MINDET )
			{
				loggerMcg.log(LL_DEBUG, "  new mc from edges %d, %d.\n", eccw, ecw);
				speed = speedFromEdges(place, eccw, ecw);

				baseedges.push_back( pair<unsigned,unsigned>(eccw,ecw) );
				return true;
			}
		}
	}

	return false;
}



void PolylineMcgraph::getSourceVertices( map< unsigned, list<unsigned> >& sourceVert )
{
	sourceVert.clear();

	for(unsigned c=0; c<pchains.size(); c++)
	{
		const Polychain& ch = pchains.at(c);
		//Indices of vertices of chain 'ch' where a motorcycle
		//emanates
		list<unsigned> vert;

		//Only add reflexive vertices of 'ch'
		if( ! bothSides )
			ch.getCwVertices(vert);
		//Add all vertices...
		else
		{
			const unsigned start = ch.isClosed() ? 0 : 1;
			const unsigned end = ch.isClosed() ? ch.size() : ch.size()-1;
			for(unsigned i=start; i<end; i++)
				vert.push_back(i);
		}

		list<unsigned>::iterator it = vert.begin();
		while( it != vert.end() )
		{
			unsigned mid = *it;
			unsigned pre = (mid-1+ch.size())%ch.size();
			unsigned post = (mid+1)%ch.size();

			//Two segments which are collinear
			const double det = Vector::det3( ch.getVertex(pre), ch.getVertex(mid),
					ch.getVertex(post) );

			//Aha, collinear segments -- remove vertex
			if( fabs(det) < MINDET )
				it = vert.erase(it);
			else
				it++;
		}

		//Add the vertices of 'ch'
		sourceVert.insert( pair<unsigned, list<unsigned> > ( c, vert) );
	}
}


Vector PolylineMcgraph::speedFromEdges(const Vector& base, unsigned eccw, unsigned ecw)
{
	assert( eccw<edges.size() );
	assert( ecw<edges.size() );

	const Segment& sccw = edges.at(eccw);
	const Segment& scw = edges.at(ecw);


	//Let vcw resp. vccw be vectors "on" segments scw resp. sccw such
	//that vcw resp. vccw point to base
	Vector vcw = ( scw.getEnd() - scw.getStart() ).norm();
	Vector vccw = ( sccw.getEnd() - sccw.getStart() ).norm();

	if( (base-scw.getStart()).lensq() < (base-scw.getEnd()).lensq() )
		vcw = -vcw;
	if( (base-sccw.getStart()).lensq() < (base-sccw.getEnd()).lensq() )
		vccw = -vccw;


	//Direction vector of motorcycle
	Vector dir = (vcw+vccw).norm();

	//Stupid situation: eccw and ecw are parallel...
	const double det = Vector::det2(vcw, vccw);
	if( fabs(det) < MINDET )
		dir = vccw.cw();

	//Let alpha be the angle of the triple pre,base,post. The
	//vector dir is the angle-bisecting vector of this triple.
	//Hence, det(dir,next) = det(dir,prev) = sin(alpha/2)

	const double sin1 = fabs(Vector::det2(vcw,dir));
	const double sin2 = fabs(Vector::det2(vccw,dir));
	const double meaninvsin = (C_1/sin1 + C_1/sin2)/C_2;

	//Multiply direction vector of motorcycle with speed
	return meaninvsin*dir;
}


unsigned PolylineMcgraph::getEdgeCcw(unsigned idx) const
{
	assert(idx < baseedges.size() );
	return baseedges.at(idx).first;
}

unsigned PolylineMcgraph::getEdgeCw(unsigned idx) const
{
	assert(idx < baseedges.size() );
	return baseedges.at(idx).second;
}
