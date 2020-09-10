#include <algorithm>
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
#include <stdio.h>

#ifdef WITH_OGL
	#include "../glwindows/glplanewindow.h"
	#include "../glwindows/gltextplane.h"
#endif

#include "../util/util.h"
#include "../fpkernel.h"

#include "skmcgraph.h"

#include "../straightskeleton/straightskeleton.h"


using namespace std;




skmcgraph::skmcgraph(const StraightSkeleton& sk,
		int gridcells) :
	mcgraph( sk.getInput().getBoundingBox(0).enlargeFactor(2),  gridcells),
	sk(sk)
{
}

skmcgraph::~skmcgraph()
{
}



void skmcgraph::initialize()
{
	loggerMcg.log(LL_INFO, "Inserting walls...\n");

	// Add all input edges as walls
	set<unsigned> eset = sk.getInput().getEdgeSet();
	for( set<unsigned>::iterator it = eset.begin(); it != eset.end(); it++ )
		wallToEdge[insertWall( sk.getInput().getEdgeSegment(*it))] = *it;




	loggerMcg.log(LL_INFO, "Determine motorcycles...\n");

	const KineticSLGraph& wf = sk.getWavefront();

	// Run through all vertices of wf and eventually add motorcycles
	set<unsigned> vset = wf.getVertexSet();
	for( set<unsigned>::iterator it = vset.begin();
			it != vset.end(); it++ )
	{
		const unsigned v = *it;

		if( sk.isWfVertexReflex(v) )
		{
			assert( wf.getVertexDegree(v) == 2);

			const Ray speed = wf.getVertex(*it).vel;
			mcStartVertex.push_back(v);
			launchMotorcycle( speed.getDir(), speed.getStart(), C_0);

			// Determine the ccw and cw base edge of the motorcycle
			const unsigned e1 = wf.getAnIncidentEdge(v);
			const unsigned e2 = wf.getCWEdge(e1, v);
			const unsigned v1 = wf.getOppositeVertex(e1, v);
			const unsigned v2 = wf.getOppositeVertex(e2, v);

			if( Vector::det3(
						wf.getVertexPos(v1, sk.getEpsilons().ZERO),
						wf.getVertexPos(v, sk.getEpsilons().ZERO),
						wf.getVertexPos(v2, sk.getEpsilons().ZERO)) <= 0 )
			{
				mcBaseEdges.push_back( pair<unsigned,unsigned>( e1, e2 ));
			}
			else
			{
				mcBaseEdges.push_back( pair<unsigned,unsigned>( e2, e1 ));
			}
		}
	}
}


unsigned skmcgraph::getStartVertexOfMc(unsigned mcidx)  const
{
	assert(mcStartedAtVertex(mcidx));
	return mcStartVertex.at(mcidx);
}


bool skmcgraph::handleHigherDegreeCrash(vector<unsigned> affected, Vector place, Vector& speed)
{
	(void) place;

	// Sort motorcycles by direction angle
	vector< pair<double,unsigned> > sorted;
	for(unsigned i=0; i<affected.size(); i++)
	{
		const unsigned mcidx = affected.at(i);
		const Motorcycle& mc = getMotorcycles().at(mcidx);
		const Vector speed = mc.getSpeed().getDir();

		sorted.push_back( pair<double,unsigned>(
			speed.pseudophi(),
			mcidx ));
	}
	sort(sorted.begin(), sorted.end());


	//Check for "new" reflex vertices
	for(unsigned i=0; i<sorted.size(); i++)
	{
		const unsigned mcidx1 = sorted.at(i).second;
		const unsigned mcidx2 = sorted.at((i+1)%sorted.size()).second;

		const Motorcycle& mc1 = getMotorcycles().at(mcidx1);
		const Motorcycle& mc2 = getMotorcycles().at(mcidx2);

		const Vector speed1 = mc1.getSpeed().getDir();
		const Vector speed2 = mc2.getSpeed().getDir();

		// Got two motorcycles spanning more than 180°
		if( Vector::sin2(speed1,speed2) < - sk.getEpsilons().ZERO_ANGLE )
		{
			const Ray mcCWvel = getMotorcycles().at(mcidx1).getSpeed();

			const unsigned eccw = mcBaseEdges.at(mcidx2).first;
			const unsigned ecw = mcBaseEdges.at(mcidx1).second;

			loggerMcg.log(LL_DEBUG, "Shoot new mc due to mcs %d and %d defined by wf-edges %d and %d.\n",
					mcidx1, mcidx2, eccw, ecw);

			// The speed vector of the refl motorcycle
			const Vector vccw = sk.getWfEdgeSpeed(eccw);
			const Vector vcw = sk.getWfEdgeSpeed(ecw);

			if( (vcw+vccw).lensq() <= sk.getEpsilons().ZERO_ANGLE )
			{
				loggerMcg.log(LL_DEBUG, "  parallel case.\n");
				speed = mcCWvel.getDir();
				mcBaseEdges.push_back( mcBaseEdges.at(mcidx1) );
				return true;
			}

			const Vector reflVel = sk.getVertexSpeed(vccw, vcw);

			// This is a reflex motorcycle, shoot the new motorcycle as usual
			if( (Vector::sin2(vccw, reflVel) + Vector::sin2(reflVel, vcw) ) <= sk.getEpsilons().ZERO_ANGLE )
			{
				speed = reflVel;
				mcBaseEdges.push_back( pair<unsigned,unsigned>( eccw, ecw) );
				return true;
			}
			// The new motorcycle would be convex. Continue the cw motorcycle's movement
			else
			{
				speed = mcCWvel.getDir();
				mcBaseEdges.push_back( mcBaseEdges.at(mcidx1) );
				return true;
			}
			return true;
		}
	}

	return false;
}


