#include <algorithm>
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <ostream>
#include <queue>
#include <set>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

#ifdef WITH_OGL
	#include "../glwindows/glplane.h"
	#include "../glwindows/glline.h"
	#include "../glwindows/glrect.h"
	#include "../glwindows/gltext.h"
	#include "../glwindows/gltextplane.h"
	#include "../glwindows/glpoint.h"
	#include "../glwindows/glcircle.h"
	#include "../glwindows/glwindow.h"
	#include "../glwindows/glplanewindow.h"
#endif

#include "../util/util.h"
#include "../fpkernel.h"

#include "straightskeleton.h"

#include "../mcgraph/skmcgraph.h"

using namespace std;


#ifdef WITH_ALGOOUTPUT
	Logger loggerSk;
#else
	NullLogger loggerSk;
#endif



bool operator>(const StraightSkeleton::event& e1, const StraightSkeleton::event& e2)
{
	return e1.time > e2.time;
}


StraightSkeleton::event::event(unsigned wfedge, double time, const Vector &place) :
	wfedge(wfedge),
	time(time),
	place(place)
{
}


StraightSkeleton::StraightSkeleton(const KineticSLGraph& input) :
	lastEventTime(0.0),
	input(input),
	computed(false),
	noEventsProcessed(0),
	initialized(false),
	initializedWf(false),
	eps(getDefaultEpsilons())
{
}

StraightSkeleton::~StraightSkeleton()
{
	for(vector<wfvtxproperties*>::iterator it=wfvtxprop.begin(); it!=wfvtxprop.end(); ++it)
		if( *it )
			delete *it;
}


void StraightSkeleton::initialize()
{
	if( initialized )
		return;
	initialized=true;

	loggerSk.log(LL_GOODNEWS, "Computing straight skeleton...\n");
	timings.startTiming("sk");
	timings.startTiming("sk init");

	loggerSk.log(LL_INFO, "Input size: %d vertices, %d edges.\n", input.getNoVertices(), input.getNoEdges());

	// Initialize the wavefront
	initializeWavefront();

	// Create the initial sk
	set<unsigned> vset = wf.getVertexSet();
	for( set<unsigned>::iterator vit = vset.begin(); vit != vset.end(); vit++)
	{
		const unsigned v = *vit;
		addSkVertex(v, C_0);
	}

	// Remember the sk vertices for the input edge. That is, fill
	// skfaceEndpoints
	for( map<upair, unsigned>::iterator it = wfedges.begin();
		it != wfedges.end(); it++ )
	{
		// The directed input edge
		const upair ie = (*it).first;
		// The directed wavefront edge
		const unsigned wfe = (*it).second;

		// Get the sk vertices from the wavefront vertices
		const unsigned skStart = getWfVertexInfo( wf.getEdge(wfe).s ).skstart;
		const unsigned skEnd = getWfVertexInfo( wf.getEdge(wfe).d ).skstart;

		loggerSk.log(LL_DEBUG, "  Face left of directed input edge (%d, %d) starts at %d and ends at %d.\n",
				ie.first, ie.second, skStart, skEnd );

		skfaceEndpoints[ie] = upair(skStart, skEnd );
	}


	// Create the motorcycle graph
	loggerSk.log(LL_INFO, "Computing motorcyclegraph\n");
	auto_ptr<skmcgraph> mcg = auto_ptr<skmcgraph>(
			new skmcgraph(*this, input.getNoVertices()) );
	mcg->initialize();
	mcg->compute();


	//Insert the resulting mcg into the wavefront
	loggerSk.log(LL_INFO, "Inserting motorcycle graph into wavefront\n");
	insertMcgIntoWavefront(*mcg);


	loggerSk.log(LL_INFO, "Initial filling of priority queue\n");
	set<unsigned> eset = wf.getEdgeSet();
	for( set<unsigned>::iterator it = eset.begin(); it != eset.end(); it++ )
	{
		insertEvent(*it, 2*eps.ZERO_EVENT);
	}

	timings.stopTiming("sk init");

	noInitialWfEdges = wf.getNoEdges();
	loggerSk.log(LL_INFO, "Initialization finished (wf: %d vertices, %d edges).\n",
			wf.getNoVertices(), noInitialWfEdges);
}


void StraightSkeleton::initializeWavefront()
{
	if( initializedWf )
		return;
	initializedWf=true;

	// Create wavefront of input
	loggerSk.log(LL_INFO, "Creating initial wavefront and straight skeleton from input\n");
	createWavefront();
}


void StraightSkeleton::computeStep()
{
	if( isComputed() )
		return;

    try
    {
        // Initialize algorithm, if not already done
        initialize();

        // Get next valid event...
        while( !events.empty() )
        {
            // Fetch the top event
            const event curr = events.top();
            events.pop();

            // Edge is already dead
            if( ! wf.isEdge(curr.wfedge) )
                continue;

            // ... and process it
            processEvent(curr);
            lastEventTime = curr.time;
            break;
        }


        // Finish the computation
        if( events.empty() )
            finishComputation();

        noEventsProcessed++;
        if(noEventsProcessed%10000==0 || isComputed())
        {
            const unsigned noEdges = wf.getNoEdges();
            loggerMcg.relog(LL_INFO, "Remaining no. of wf edges: %8d of %d  (%.1f%% collapsed)",
                    noEdges, noInitialWfEdges, 100.0-100.0*noEdges/noInitialWfEdges);
        }
    }
    catch(...)
    {
        throw StraightSkeletonError("StraightSkeleton::computeStep()");
    }
}

void StraightSkeleton::finishComputation()
{
	loggerSk.log(LL_INFO, "Add infinite edges to straight skeleton.\n");

	// Create the initial sk
	set<unsigned> vset = wf.getVertexSet();
	vector< pair<double,unsigned> > infVertices;
	infVertices.reserve( vset.size() );

	for( set<unsigned>::iterator vit = vset.begin(); vit != vset.end(); vit++)
	{
		const unsigned v = *vit;
		const wfvtxtype vt = getWfVertexInfo(v).type;

		if( vt==STEINER_RESTING )
		{
#ifndef NDEBUG
			const Vector lastpos =  wf.getVertexPos(v, getLastEventTime());
			if( lastpos.lensq() < sq(eps.INFDIST) )
			{
				loggerSk.log(LL_ERROR, "Resting steiner %d is too near to be at infinity! pos=(%f,%f)\n",
					v, TO_MDOUBLE(lastpos.x), TO_MDOUBLE(lastpos.y));
			}
#endif
			continue;
		}

		const Vector pos = getVertexEscapePos(v);
		loggerSk.log(LL_DEBUG, "  wf-vertex %d of type %d at pos (%f, %f) is left.\n",
				v, vt, TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y));

		if( vt!=STEINER_MOVING && getWfVertexInfo(v).skstart!=0 )
		{
			const unsigned v1 = getWfVertexInfo(v).skstart;
			const unsigned v2 = sk.addVertex(pos, 0);
			loggerSk.log(LL_DEBUG, "    connect sk-vtx %d with new sk-vtx %d.\n", v1, v2);
			sk.addEdge( v1, v2, 0, eps.ZERO_EVENT);

			// Remember the infinite vertex
			double phi = atan2( pos.y, pos.x);
			infVertices.push_back( pair<double,unsigned>(phi,v2) );
		}
	}


	// Connect them
	assert( infVertices.size() >= 3);
	sort(infVertices.begin(), infVertices.end() );
	for( unsigned i=0; i<infVertices.size(); i++ )
	{
		const unsigned i2 = (i+1)%infVertices.size();
		const unsigned v = infVertices.at(i).second;
		const unsigned v2 = infVertices.at(i2).second;
		sk.addEdge(v, v2, 0, eps.ZERO_EVENT);
	}


	computed = true;
	timings.stopTiming("sk");


	loggerSk.log(LL_INFO, "Generating arc to face mapping.\n");

	// Check for integrity of faces
	for( map<upair, upair>::iterator it = skfaceEndpoints.begin();
			it != skfaceEndpoints.end(); it++ )
	{
		const upair ei = (*it).first;

		const unsigned faceStart = (*it).second.first;
		const unsigned faceEnd = (*it).second.second;

		loggerSk.log(LL_DEBUG, "  Face left of input edge (%d -> %d): start %d, end %d.\n",
				ei.first, ei.second, faceStart, faceEnd);

		// Get the face vertices
		list<unsigned> vertices;
		bool good = getSkfaceVertices(ei, vertices);

		// Could not traverse face
		if( ! good )
		{
			loggerSk.log(LL_ERROR, "The sk face left of input edge (%d -> %d) is not connected!\n",
					ei.first, ei.second);
		}


		loggerSk.log(LL_DEBUG, "  Vertex sequence: ");
		list<unsigned>::iterator vit = vertices.begin();
		loggerSk.log(LL_DEBUG, "%d ", *vit);

		unsigned curr = *vit;
		for(vit++ ; vit!=vertices.end(); vit++)
		{
			const unsigned pre = curr;
			curr = *vit;
			loggerSk.log(LL_DEBUG, "%d ", curr);

			assert(sk.isAdjacent(pre, curr));
			const unsigned arc = sk.getCommonEdge(pre,curr);
			const upair darc(pre, curr);

			if( skarcFaces.count( darc ) > 0)
				loggerSk.log(LL_ERROR, "Warning, two faces right to sk arc %d (%d, %d)!\n",
						arc, pre, curr);
			skarcFaces[darc] = ei;
		}
		loggerSk.log(LL_DEBUG, "\n");
	}


	loggerSk.log(LL_GOODNEWS, "Finished computing straight skeleton.\n");
}


void StraightSkeleton::compute()
{
    while(!isComputed())
        computeStep();
}


void StraightSkeleton::processEvent(const event& ev)
{
	// Get dates of the event
	const double time = ev.time;
	const unsigned eve = ev.wfedge;
	assert( wf.isEdge(eve) );

	const KineticSLGraph::Edge eved = wf.getEdge(eve);

	loggerSk.log(LL_DEBUG, "Process %d. event at time %e on edge %d (%d, %d)\n",
			noEventsProcessed, TO_MDOUBLE(time), eve, eved.s, eved.d);

	if( wfeorigin.count(eve) > 0 )
	{
		upair inputedge = wfeorigin.at(eve);
		loggerSk.log(LL_DEBUG, "  original input edge: (%d, %d)\n", inputedge.first, inputedge.second);
	}

	// Get the two endpoints of eve
	unsigned u = eved.s;
	unsigned v = eved.d;

	// Force a standardized situation. Note, we rely on the ordering of enum wfvtxtype!
	if( getWfVertexInfo(u).type > getWfVertexInfo(v).type )
		swap(u,v);


	const wfvtxtype& ut = getWfVertexInfo(u).type;
	const wfvtxtype& vt = getWfVertexInfo(v).type;

	const Ray& uvel = wf.getVertex(u).vel;
	const Ray& vvel = wf.getVertex(v).vel;

	// If a vertex is already too far away, do not process it
	if( (time*uvel).getEnd().lensq() >= getVertexEscapePos(u).lensq()  ||
		(time*vvel).getEnd().lensq() >= getVertexEscapePos(v).lensq() )
	{
		loggerSk.log(LL_DEBUG, "  event is taking place at infinity. skipping.\n");
		return;
	}




	// Check whether u and v are driving on a line
	bool drivingOnALine = false;
	if( ut<=STEINER_RESTING && uvel.getDir().lensq() > sq(eps.ZERO) )
	{
		drivingOnALine = fabs(Vector::sin3(
					uvel.getEnd(), uvel.getStart(), vvel.getStart())) < eps.ZERO_ANGLE;
	}



	if( ut==CONVEX && vt==CONVEX )
	{
		processEdgeEvent(ev);
	}
	else if( drivingOnALine && (
			(ut==REFLEX && vt==STEINER_MOVING) ||
			(ut==REFLEX && vt==CONVEX_MULTI) ||
			(ut==STEINER_MOVING && vt==STEINER_MOVING) ||
			(ut==CONVEX_MULTI && vt==STEINER_MOVING) ||
			(ut==CONVEX_MULTI && vt==CONVEX_MULTI) ))
	{
		processSplitEvent(ev);
	}
	else if( (ut==CONVEX && vt==STEINER_MOVING ) ||
			(ut==CONVEX && vt==REFLEX ) ||
			(ut==CONVEX && vt==CONVEX_MULTI ) )
	{
		processSwitchEvent(ev);
	}
	else if( (ut==REFLEX && vt==STEINER_RESTING) ||
			(ut==STEINER_MOVING && vt==STEINER_RESTING) ||
			(ut==CONVEX_MULTI && vt==STEINER_RESTING) )
	{
		processStartEvent(ev);
	}
	else if( vt==CONVEX_MULTI &&
			(ut==REFLEX || ut==CONVEX_MULTI) &&
			!drivingOnALine )
	{
		loggerSk.log(LL_DEBUG, "  ignore event. Multi split event should be handled by another edge collapse.\n");
	}
	else if( (ut==REFLEX || ut==CONVEX_MULTI || ut==STEINER_MOVING) &&
			vt==STEINER_MOVING && !drivingOnALine )
	{
		loggerSk.log(LL_DEBUG, "  ignore event. Event should be handled by another edge collapse.\n");
	}
	else if( vt==STEINER_MULTI &&
			(ut==REFLEX || ut==CONVEX_MULTI || ut==STEINER_MOVING) )
	{
		processMultiSplitEvent(ev);
	}
	else
	{
		loggerSk.log(LL_ERROR, "Processing invalid event of vertices of type %d and %d\n",
				getWfVertexInfo(eved.s).type, getWfVertexInfo(eved.d).type);
		assert(false);
	}
}


void StraightSkeleton::processEdgeEvent(const event& ev)
{
	loggerSk.log(LL_DEBUG, "  Processing edge event...\n");

	const unsigned eve = ev.wfedge;
	const double time = ev.time;
	const Vector place = ev.place;
	unsigned u = wf.getEdge(eve).s;
	unsigned v = wf.getEdge(eve).d;

	assert( getWfVertexInfo(u).type == CONVEX );
	assert( getWfVertexInfo(v).type == CONVEX );

	assert( wf.getVertexDegree(u) == 2);
	assert( wf.getVertexDegree(v) == 2);

	// Make u the left and v the right vertex w.r.t to propgation direction of eve
	if( isWfEdgeExpanding(u, v) > 0 )
		swap(u, v);

	// Get incident edges and opposite vertices
	unsigned edu = wf.getCCWEdge(eve, u);
	unsigned edv = wf.getCWEdge(eve, v);


	const unsigned skvtx = sk.addVertex(place);

	vector<unsigned> vert;
	vert.push_back( getWfVertexInfo(u).skstart );
	vert.push_back( getWfVertexInfo(v).skstart );
	sk.addEdgeStar(skvtx, vert, 0, eps.ZERO_EVENT);

	loggerSk.log(LL_DEBUG, "  Add sk vertex %d and edges to vertices %d and %d.\n",
			skvtx, getWfVertexInfo(u).skstart, getWfVertexInfo(v).skstart);

	// patch the wavefront accordingly
	patchWavefront(edu, u, edv, v, place, time, skvtx);

	removeWfVertex(u);
	removeWfVertex(v);
}


void StraightSkeleton::processSwitchEvent(const event& ev)
{
	loggerSk.log(LL_DEBUG, "  Processing switch event...\n");

	const double time = ev.time;
	const Vector place = ev.place;
	const unsigned eve = ev.wfedge;
	unsigned u = wf.getEdge(eve).s;
	unsigned v = wf.getEdge(eve).d;

	//Vertex u reaches the moving steiner point v
	if( getWfVertexInfo(u).type > getWfVertexInfo(v).type )
		swap(u,v);


#ifndef NDEBUG
	const wfvtxtype& ut = getWfVertexInfo(u).type;
#endif
	const wfvtxtype& vt = getWfVertexInfo(v).type;
	assert( ut==CONVEX && ( vt==STEINER_MOVING || vt==REFLEX || vt==CONVEX_MULTI ));
	assert( wf.getVertexDegree(u) == 2);

	const unsigned eu2 = wf.getCWEdge(eve, u);
	const unsigned u2 = wf.getOppositeVertex(eu2,u);
	const wfvtxtype u2t = getWfVertexInfo(u2).type;

	unsigned ev2 = wf.getCCWEdge(eve,v);
	unsigned ev3 = wf.getCWEdge(eve,v);
	unsigned v2 = wf.getOppositeVertex(ev2, v);
	unsigned v3 = wf.getOppositeVertex(ev3, v);
	wfvtxtype v2t = getWfVertexInfo(v2).type;
	wfvtxtype v3t = getWfVertexInfo(v3).type;

	// ev2 and eu2 will be the two defining edges of convex u
	if( fabs(wf.getVertex(v).vel.orthoDistance( wf.getVertexPos(v2, -eps.ZERO_ANGLE))) <
		fabs(wf.getVertex(v).vel.orthoDistance( wf.getVertexPos(v3, -eps.ZERO_ANGLE))) )
	{
		swap(v2,v3);
		swap(ev2,ev3);
		swap(v2t,v3t);
	}

	// Some sanity checks, topology based
	if( v2t==STEINER_RESTING || v2t==STEINER_MULTI )
	{
		loggerSk.log(LL_WARNING, "Switch event: classification of neighbors of %d wrong?\n", v);
		swap(v2,v3);
		swap(ev2,ev3);
		swap(v2t,v3t);
	}


	// Is eve CW from ev3?
	const bool evecw = (wf.getCWEdge(ev3,v) == eve );

	// Vertex u and v should drive in opposite directions, if v is moving
	if( v3t==STEINER_MOVING && (wf.getVertex(v).vel.getDir().norm() +
				wf.getVertex(v3).vel.getDir().norm()).lensq() > sq(eps.ZERO_ANGLE) )
	{
		loggerSk.log(LL_ERROR, "Switch event: vertex %d and %d are not driving in opposite direction.\n",
				u, v);
	}



	loggerSk.log(LL_DEBUG, "  convex %d met vertex %d of type %d.\n", u, v, vt);
	loggerSk.log(LL_DEBUG, "  vertex %d drives to %d. convex %d will be adjacent to %d\n", v, v3, u, v2);


	if( u2t==CONVEX && fabs(getCollapseTime(eu2)-time) < eps.ZERO )
	{
		loggerSk.log(LL_DEBUG, "  process edge event of neighboring edge %e first...\n", eu2);
		processEdgeEvent( getCollapseEvent(eu2) );
		return;
	}


	// Check if another event happened...
	if( fabs(getCollapseTime(ev3)-time) < eps.ZERO_EVENT )
	{
		// The actual event
		event actEv = getCollapseEvent(ev3);

		// Multi split event
		if( v3t==STEINER_MULTI )
		{
			loggerSk.log(LL_DEBUG, "  Actually, a multi split event took place for edge %d.\n", ev3);
			processMultiSplitEvent(actEv);
			return;
		}
		else if( v3t==STEINER_MOVING || v3t==CONVEX_MULTI || v3t==REFLEX )
		{
			loggerSk.log(LL_DEBUG, "  Actually, a split event took place for edge %d.\n", ev3);
			processSplitEvent(actEv);
			return;
		}
	}



	// Check for the parallel case first
	if( fabs(Vector::sin2( getWfEdgeDir(eu2,u,time-eps.ZERO_ANGLE),
			wf.getVertex(v).vel.getDir() )) < eps.ZERO_ANGLE )
	{
		loggerSk.log(LL_DEBUG, "  The edge %d moves parallel to vertex %d. Merge the Steiner edges.\n", eu2, v);


		vector<unsigned> nedges;
		unsigned eu2_ = eu2;
		mergeParallelSteinerEdges(eu2_, u, ev3, v, time, nedges);
		insertEvents(nedges, time);
		return;
	}


	if( wf.isAdjacent(v3,u2) && wf.getVertexSegment(v,u2,time).lensq() < sq(eps.ZERO) )
	{
		loggerSk.log(LL_DEBUG, "  The vertices %d, %d and %d meet at the same time.\n", v, u, u2);
		const wfvtxtype u2t = getWfVertexInfo(u2).type;

		unsigned skvtx = sk.addVertex(place);
		loggerSk.log(LL_DEBUG, "  Add sk vertex %d and edges.\n", skvtx);
		vector<unsigned> vert;
		vert.push_back( getWfVertexInfo(u).skstart );
		if( u2t==REFLEX || u2t==CONVEX || u2t==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(u2).skstart );
		if( vt==REFLEX || vt==CONVEX || vt==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(v).skstart );
		sk.addEdgeStar(skvtx, vert, 0, eps.ZERO_EVENT);

		removeWfVertex(u);
		const unsigned euv3 = wf.getCommonEdge(u2, v3);
		assert( wf.getVertexDegree(u2) == 2 );
		const unsigned eu3 = wf.getCWEdge( euv3, u2);

		// Get the steiner edge incident to v3
		assert( wf.getVertexDegree(v3) == 3 );
		const unsigned ev4 = wf.getCCWEdge(ev3,v3)==euv3 ? wf.getCWEdge(ev3,v3) : wf.getCCWEdge(ev3,v3);

		vector<upair> steineredges;
		steineredges.push_back( upair(ev4, v3) );
		loggerSk.log(LL_DEBUG, "  Recall edge %d as Steiner edge.\n", ev4);

		if( evecw )
			patchWavefront(ev2, v, eu3, u2, place, time, skvtx, steineredges);
		else
			patchWavefront(eu3, u2, ev2, v, place, time, skvtx, steineredges);

		if( wf.isVertex(v3) )
			removeWfVertex(v3);
		removeWfVertex(u2);
		removeWfVertex(v);
		return;
	}



	// Determine all steiner edges which are to be saved and all vertices which
	// are to be removed afterwards.
	vector<upair> steineredges;
	// Shall we remove v3 afterwards?
	bool rmv3 = false;

	// If ev3 collapses at the same time --> consider its incident edges as steiner edges
	if( fabs(getCollapseTime(ev3)-time) < eps.ZERO )
	{
		loggerSk.log(LL_DEBUG, "  Edge %d collapsed at the same time. Recall Steiner edges.\n", ev3);
		assert( v3t == STEINER_RESTING );

		steineredges.push_back( upair( wf.getCCWEdge(ev3,v3), v3) );
		steineredges.push_back( upair( wf.getCWEdge(ev3,v3), v3) );

		rmv3 = true;
		wf.removeEdge(ev3);
	}
	// The default case: ev3 is a steiner edge which is to be saved
	else
	{
		loggerSk.log(LL_DEBUG, "  Recall edge %d as Steiner edge.\n", ev3);
		steineredges.push_back( upair(ev3, v) );
	}




	// ev2 collapsed at the same time --> handle this event too
	if( v2t==CONVEX && fabs(getCollapseTime(ev2)-time) < eps.ZERO_EVENT )
	{
		const unsigned ev4 = wf.getCWEdge(ev2, v2);
		const unsigned v4 = wf.getOppositeVertex(ev4,v2);
		loggerSk.log(LL_DEBUG, "  Convex %d, convex %d and %d met.\n", u, v2, v4);


		if( getWfVertexInfo(v4).type==CONVEX && fabs(getCollapseTime(ev4)-time) < eps.ZERO_EVENT )
		{
			loggerSk.log(LL_DEBUG, "  process edge event of neighboring edge %e first...\n", ev4);
			processEdgeEvent( getCollapseEvent(ev4) );
			return;
		}


		unsigned skvtx = sk.addVertex(place);
		loggerSk.log(LL_DEBUG, "  Add sk vertex %d and edges.\n", skvtx);
		vector<unsigned> vert;
		vert.push_back( getWfVertexInfo(u).skstart );
		vert.push_back( getWfVertexInfo(v2).skstart );
		if( vt==REFLEX || vt==CONVEX || vt==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(v).skstart );
		sk.addEdgeStar(skvtx, vert, 0, eps.ZERO_EVENT);

		if( evecw )
			patchWavefront(ev4, v2, eu2, u, place, time, skvtx, steineredges);
		else
			patchWavefront(eu2, u, ev4, v2, place, time, skvtx, steineredges);

		removeWfVertex(v2);
	}
	else
	{
		unsigned skvtx = sk.addVertex(place);
		loggerSk.log(LL_DEBUG, "  Add sk vertex %d and edges.\n", skvtx);
		vector<unsigned> vert;
		vert.push_back( getWfVertexInfo(u).skstart );
		if( vt==REFLEX || vt==CONVEX || vt==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(v).skstart );
		sk.addEdgeStar(skvtx, vert, 0, eps.ZERO_EVENT);

		unsigned newv;
		if( evecw )
			newv = patchWavefront(ev2, v, eu2, u, place, time, skvtx, steineredges);
		else
			newv = patchWavefront(eu2, u, ev2, v, place, time, skvtx, steineredges);


		// Actually, skvtx is pointless
		// If we would not delete this sk vertex then we may end up with up to
		// n^2 sk vertices if there are n^2 switch events.
		if( newv!=0 && !( vt==REFLEX || vt==CONVEX || vt==CONVEX_MULTI ) )
		{
			loggerSk.log(LL_DEBUG, "  Remove sk vertex %d.\n", skvtx);
			sk.removeVertex(skvtx);
			getWfVertexInfo(newv).skstart = getWfVertexInfo(u).skstart;
		}
	}


	removeWfVertex(v);
	removeWfVertex(u);
	if( rmv3 )
		removeWfVertex(v3);
}



void StraightSkeleton::mergeParallelSteinerEdges_fixEndVertex(unsigned lastvtx,
		unsigned e1, double time, vector<unsigned> &nedges, unsigned &eu, unsigned &u)
{
	loggerSk.log(LL_DEBUG, "      fix vertex %d incident to edge %d\n", lastvtx, e1);

	assert( wf.isVertex(lastvtx) );
	assert( wf.isEdgeIncident(e1, lastvtx) );
	const unsigned v1 = wf.getOppositeVertex(e1, lastvtx);

	bool collapseEdgeE1 = false;
	const wfvtxtype lastvtxt = getWfVertexInfo(lastvtx).type;


	if( lastvtxt == STEINER_MULTI )
	{
		loggerSk.log(LL_DEBUG, "        nothing to do.\n", lastvtx);
	}
	if( lastvtxt==STEINER_MOVING || lastvtxt==STEINER_RESTING )
	{
		loggerSk.log(LL_DEBUG, "        remove moving/resting Steiner %d vertex.\n", lastvtx);

		assert( wf.getVertexDegree(lastvtx) == 2 );
		const unsigned e2 = wf.getCWEdge(e1, lastvtx);
		const unsigned ne2 = wf.mergeEdges(lastvtx, e1, e2);
		cpyWfeorign(ne2, e1);
		insertEvent(ne2, time);

		wf.removeVertex(lastvtx);
	}
	else if( lastvtxt==REFLEX || lastvtxt==CONVEX_MULTI )
	{
		if( lastvtx==REFLEX )
		{
			loggerSk.log(LL_DEBUG, "        %d was reflex. Hidden switch event with vertex %d.\n", lastvtx, v1);
			collapseEdgeE1=true;
		}
		else
		{
			getWfVertexInfo(lastvtx).type = CONVEX;
			loggerSk.log(LL_DEBUG, "        %d was multi convex, change to convex.\n", lastvtx);

			// The edge e1 collapsed, too
			if( wf.getEdgeSegment(e1, time).lensq() < sq(eps.ZERO_EVENT) )
			{
				loggerSk.log(LL_DEBUG, "        hidden edge event with vertex %d.\n", v1);
				collapseEdgeE1=true;
			}
		}
	}


	// We need to remove edge e1, which collapsed right now
	if( collapseEdgeE1 )
	{
#ifndef NDEBUG
		const wfvtxtype v1t = getWfVertexInfo(v1).type;
#endif

		assert( wf.getVertexDegree(lastvtx) == 2 );
		assert( v1t == CONVEX );
		const unsigned e2 = wf.getCWEdge(e1,v1);
		const unsigned v2 = wf.getOppositeVertex(e2, v1);

		const unsigned e3 = wf.getCWEdge(e1,lastvtx);
		const unsigned v3 = wf.getOppositeVertex(e3, lastvtx);

		Vector place;
		double time;
		getCollapseTimePlace(e1, time, place);


		// Create straight skeleton vertex
		const unsigned skvtx = sk.addVertex(place);
		loggerSk.log(LL_DEBUG, "        Created new sk-vertex %d.\n", skvtx);

		// Connect straight skeleton vertex
		vector<unsigned> vert;
		vert.push_back(getWfVertexInfo(lastvtx).skstart);
		vert.push_back(getWfVertexInfo(v1).skstart);
		sk.addEdgeStar( skvtx, vert, 0, eps.ZERO_EVENT );

		// Shoot a new convex vertex
		const Ray vel = getVertexSpeed(e2, v1, e3, lastvtx, place, time);
		const unsigned v4 = addWfVertex(vel, CONVEX );
		getWfVertexInfo(v4).skstart = skvtx;
		loggerSk.log(LL_DEBUG, "        Shot new convex vertex %d with speed (%f, %f)\n",
				v4, TO_MDOUBLE(vel.getDir().x), TO_MDOUBLE(vel.getDir().y));


		loggerSk.log(LL_DEBUG, "        Reconnect vertex %d with %d and %d\n", v4, v2, v3);

		wf.removeEdge(e1);
		const unsigned ne2 = wf.repotEdge(e2, v1, v4, time-eps.ZERO);
		const unsigned ne3 = wf.repotEdge(e3, lastvtx, v4, time-eps.ZERO);
		cpyWfeorign(ne2, e2);
		cpyWfeorign(ne3, e3);
		nedges.push_back(ne2);
		nedges.push_back(ne3);

		// Reconnect v4
		wf.removeVertex(v1);
		wf.removeVertex(lastvtx);

		// Update u and eu
		if( v1==u )
			u = v4;
		if( eu == e2 )
			eu = ne2;
	}
}


void StraightSkeleton::mergeParallelSteinerEdges(unsigned &eu, unsigned &u, unsigned ev,
				unsigned v, double time, vector<unsigned> &nedges)
{
	assert( wf.isEdge(eu) );
	assert( wf.isEdge(ev) );
	assert( wf.isVertex(v) );
	assert( wf.isVertex(u) );
	assert( wf.isEdgeIncident(eu, u) );
	assert( wf.isEdgeIncident(ev, v) );
	assert( wf.isAdjacent(u, v) );


	// Get the direction of the parallel sequences.
	const Vector dir = getWfEdgeDir(eu, u, time-eps.ZERO_ANGLE);
	const Vector vel = getWfEdgeSpeed(eu);

	// cw is true: we walk from u along eu, finally along ev to v in clockwise
	// direction.
	assert( fabs(Vector::cos2(vel, dir)) < eps.ZERO );
	const bool cw = (Vector::sin2(vel, dir) > 0);


	loggerSk.log(LL_DEBUG, "    mergeParallelSteinerEdges: eu=%d, u=%d, ev=%d, v=%d, cw=%d\n",
			eu, u, ev, v, cw);

	// contains all edge-vertex pairs if we walk in (cw ? clockwise :
	// counter-clockwise) direction starting at (eu,u) and finally reaching ev
	list<upair> face;
	// Start with (eu,u)
	face.push_back( upair(eu,u) );


	// Fill face with the edge-vertex pairs that build the face we are going to
	// collapse
	unsigned u2 = wf.getOppositeVertex(eu,u);
	while( u2 != v )
	{
#ifndef NDEBUG
		// We did not visit it yet
		for( list<upair>::iterator it = face.begin(); it!=face.end(); it++)
			assert( u2 != it->second );
#endif


		// It is a multi convex vertex. Check if we should convert it to a
		// convex vertex by removing an opposite resting Steiner Vertex
		const wfvtxtype u2t = getWfVertexInfo(u2).type;
		if( u2t == CONVEX_MULTI )
		{
			const unsigned e3 = cw ? wf.getCCWEdge(eu,u2) : wf.getCWEdge(eu,u2);
			const unsigned u3 = wf.getOppositeVertex(e3,u2);

			if(  getWfVertexInfo(u3).type == STEINER_RESTING )
			{
				const unsigned ecw = wf.getCWEdge(e3,u3);
				const unsigned eccw = wf.getCCWEdge(e3,u3);

				const Vector dcw = getWfEdgeDir(ecw, u3, time-eps.ZERO_ANGLE);
				const Vector dccw = getWfEdgeDir(eccw, u3, time-eps.ZERO_ANGLE);
				const Vector d3 = getWfEdgeDir(e3, u3, time-eps.ZERO_ANGLE);

				// The edges ecw and eccw are on a line, but e3 is not. Note,
				// we also need to check that ecw and eccw are on the same line
				// as the collapsed face.
				if( fabs(Vector::sin2(dcw,dccw)) < fabs(Vector::sin2(dcw,d3)) &&
					fabs(Vector::cos2(vel, dcw)) < eps.ZERO_ANGLE )
				{
					loggerSk.log(LL_DEBUG, "      vertex %d is multi convex and becomes convex.\n", u2);

					wf.removeEdge(e3);
					getWfVertexInfo(u2).type = CONVEX;

					// Merge incident edges to u3
					const unsigned newe = wf.mergeEdges(u3, eccw, ecw);
					insertEvent(newe, time);
					removeWfVertex(u3);
				}
			}
		}


		// Get next edge
		eu = cw ? wf.getCCWEdge(eu,u2) : wf.getCWEdge(eu,u2);
		u = u2;
		u2 = wf.getOppositeVertex(eu,u);

		// Insert into list
		face.push_back( upair(eu,u) );
		loggerSk.log(LL_DEBUG, "      remember edge %d and vertex %d.\n", eu, u );
	}
	assert( face.size() > 1 );


	// Restore eu and u
	eu = face.begin()->first;
	u = face.begin()->second;



	// All edges between v and lastit are Steiner edges and are removed. All
	// vertices on that path need to be merged with wavefront vertices
	list<upair>::iterator lastit = face.begin();
	for(list<upair>::iterator it = ++face.begin() ; it!=face.end(); ++it)
	{
		const unsigned vtx = it->second;
		const unsigned ed = it->first;
		if( wf.getVertexDegree(vtx) > 2 )
		{
			// The next edge is not in correct direction -- go one step further
			if( fabs(Vector::cos2(vel, getWfEdgeDir(ed, vtx, time-eps.ZERO_ANGLE)))
					> eps.ZERO_ANGLE )
			{
				++it;
			}
			lastit = it;
			break;
		}
	}
	unsigned lastvtx;
	try {
		lastvtx = lastit->second;
	}
	catch (...)
	{
		throw;
		return;
	}
	loggerSk.log(LL_DEBUG, "      last vertex: %d.\n", lastvtx);


	// Remove all Steiner edges
	for( list<upair>::iterator it=lastit; it!=face.end(); it++)
		wf.removeEdge( it->first );


	assert( wf.isVertex(u) );
	assert( wf.isEdge(eu) );

	// Fix the vertex lastvtx
	list<upair>::iterator it = lastit;
	it--;
	mergeParallelSteinerEdges_fixEndVertex(lastvtx, it->first, time, nedges, eu, u);
	assert( wf.isVertex(u) );
	assert( wf.isEdge(eu) );

	// Fix the vertex v
	if( wf.isAdjacent(u, v) )
		mergeParallelSteinerEdges_fixEndVertex(v, wf.getCommonEdge(u,v), time, nedges, eu, u);
	assert( wf.isVertex(u) );
	assert( wf.isEdge(eu) );



	// All resting Steiner vertices which are to be merged with wfe
	vector<unsigned> todo;

	unsigned wfvtx = u;
	unsigned wfe = eu;
	// Walking along the resting Steiner vertices and merge them into the
	// wavefront edge wfe
	for( list<upair>::iterator it=--face.end(); it!=lastit; it--)
	{
		unsigned vtx = it->second;
		wfvtxtype vtxt = getWfVertexInfo(vtx).type;
		const Vector pos = wf.getVertexPos(vtx,time);
		assert( vtxt==STEINER_RESTING || vtxt==STEINER_MULTI );

		if( vtxt==STEINER_MULTI )
		{
			// It was a multi steiner vertex and can be deleted now
			if( wf.isVertexIsolated(vtx) )
			{
				loggerSk.log(LL_DEBUG, "      vertex %d is obsolete, removing it.\n", vtx);
				removeWfVertex(vtx);
				continue;
			}
			else
			{
				vtxt = STEINER_MOVING;
				const unsigned vtx_tmp = addWfVertex( Ray(pos-time*vel, vel), vtxt );
				nedges.push_back( wf.addEdge(vtx, vtx_tmp, time-eps.ZERO) );

				loggerSk.log(LL_DEBUG, "      created a proxy moving steiner vertex %d incident to %d.\n",
						vtx_tmp, vtx);
				vtx = vtx_tmp;
			}
		}

		assert( wf.isVertexTerminal(vtx) );
		const unsigned vtxinedge = wf.getAnIncidentEdge(vtx);
		const unsigned vtx2 = wf.getOppositeVertex(vtxinedge, vtx);
		const Vector vtxdir = getWfEdgeDir(vtxinedge, vtx, time-eps.ZERO);

		loggerSk.log(LL_DEBUG, "      handling resting/multi Steiner vertex %d adjacent to %d.\n", vtx, vtx2);

		// Advance to the correct wfe where vtx can be projected onto and do the splitting
		bool done=false;
		while( !done )
		{
			assert( wf.isEdge(wfe) );
			assert( wf.isVertex(wfvtx) );
			const unsigned wfvtx2 = wf.getOppositeVertex(wfe,wfvtx);
			unsigned wfenext = cw ? wf.getCCWEdge(wfe,wfvtx2) : wf.getCWEdge(wfe,wfvtx2);
			const Vector wfenextdir = getWfEdgeDir(wfenext, wfvtx2, time-eps.ZERO);

			const Ray wferay( wf.getVertexPos(wfvtx,time),
					wf.getVertexSegment(wfvtx, wfvtx2, time).getDir() );


			// Compute distance to wfvtx2 and the projection distance on wfe.
			// Note, if wfe has zero length then wferay.getDir() is a zero
			// vector. Hence, we do a more complex computation to get a same
			// value for projdist
			const double wfvtx2dist = wf.getVertexSegment(wfvtx2,vtx,time).lensq();
			double projdist = -HUGE_DBL;
			if( wferay.getDir().lensq() < sq(eps.ZERO) )
				projdist = wfvtx2dist<sq(eps.ZERO) ? 1.0 : HUGE_DBL;
			else
				projdist = wferay.projDistance(pos);

			loggerSk.log(LL_DEBUG, "      wfe %d: projdist %e in interval [0,1]?\n", wfe, TO_MDOUBLE(projdist) );
			assert( projdist > -eps.ZERO_EVENT);
			assert( dir * wferay.getDir() >= 0 || wferay.getDir().lensq() < sq(eps.ZERO) );

			const Vector wfvtxdir = wf.getVertex(wfvtx).vel.getDir();
			const Vector wfvtx2dir = wf.getVertex(wfvtx2).vel.getDir();

			// If the vtx coincides with wfvtx2 and if they drive in the same direction
			if( 	getWfVertexInfo(wfvtx2).type == CONVEX &&
					fabs(projdist-1.0) < eps.ZERO_EVENT &&
					fabs(Vector::sin2(wfvtx2dir, vtxdir)) < eps.ZERO_ANGLE )
			{
				loggerSk.log(LL_DEBUG, "        is to be merged with convex %d.\n", wfvtx2);
				getWfVertexInfo(wfvtx2).type = CONVEX_MULTI;
				const unsigned e = wf.repotEdge(vtxinedge, vtx, wfvtx2, time-eps.ZERO);
				cpyWfeorign(e, vtxinedge);
				nedges.push_back(e);

				wf.removeVertex(vtx);
				done = true;
			}
			// If the vtx coincides with wfvtx and if they drive in the same direction
			else if( getWfVertexInfo(wfvtx).type == CONVEX &&
					fabs(projdist) < eps.ZERO_EVENT &&
					fabs(Vector::sin2(wfvtxdir, vtxdir)) < eps.ZERO_ANGLE )
			{
				loggerSk.log(LL_DEBUG, "        is to be merged with convex %d.\n", wfvtx);
				getWfVertexInfo(wfvtx).type = CONVEX_MULTI;
				const unsigned e = wf.repotEdge(vtxinedge, vtx, wfvtx, time-eps.ZERO);
				cpyWfeorign(e, vtxinedge);
				nedges.push_back(e);
				wf.removeVertex(vtx);
				break;
			}

			// If vtx is in the interior of wfe or if vtx points to the interior of wfe
			// or if wfe is the last possible wfe
			else if ( projdist < 1.0 - eps.ZERO ||
					Vector::sin2(wfvtx2dir, vtxdir) <= 0 ||
					wfenextdir * dir <= eps.ZERO )
			{
				loggerSk.log(LL_DEBUG, "        recall for wfe %d.\n", wfe);

				// Remember vtx for this wfe, but do not proceed to the next wfe
				todo.push_back(vtx);

				// Convert the resting steiner vertex to a moving steiner vertex
				getWfVertexInfo(vtx).type = STEINER_MOVING;
				const Vector vel = getWfEdgeSpeedInDir(wfe, vtxdir);
				const Vector pos = (projdist*wferay).getEnd();
				wf.getVertex(vtx).vel = Ray( pos - time*vel, vel);

				// Remove old edge and re-insert a new edge
				nedges.push_back( wf.renewEdge(vtxinedge ));

				// Do not proceed to the next wfe...
				break;
			}


			loggerSk.log(LL_DEBUG, "        split wfe %d by vertices: ", wfe);
			for( vector<unsigned>::iterator it = todo.begin(); it != todo.end(); it++)
				loggerSk.log(LL_DEBUG, "%d ", *it);
			loggerSk.log(LL_DEBUG, "\n");

			// Do not forget to split wfe by all vertices in todo.
			// We do this a bit in the future: if a Steiner vertex coincides
			// with a wfvtx then we see in the future to which direction the
			// Steiner vertex is moving to
			vector<unsigned> edges = wf.splitEdgeByVertices(wfe, todo, time-eps.ZERO);
			for( vector<unsigned>::iterator it = edges.begin(); it != edges.end(); it++)
			{
				const unsigned e = *it;
				nedges.push_back(e);
				cpyWfeorign(e, wfe);

				if( wf.isEdgeIncident(e, u) )
				{
					assert(eu == wfe);
					eu = e;
				}
			}

			// Clear the todo list
			todo.clear();

			// We need to go to the next wavefront edge.
			wfvtx = wfvtx2;
			wfe = wfenext;
		}
	}

	// Split the last wfe
	loggerSk.log(LL_DEBUG, "      finally, split wfe %d by vertices: ", wfe);
	for( vector<unsigned>::iterator it = todo.begin(); it != todo.end(); it++)
		loggerSk.log(LL_DEBUG, "%d ", *it);
	loggerSk.log(LL_DEBUG, "\n");

	// Split the final wfe
	vector<unsigned> edges = wf.splitEdgeByVertices(wfe, todo, time-eps.ZERO);
	for( vector<unsigned>::iterator it = edges.begin(); it != edges.end(); it++)
	{
		const unsigned e = *it;
		nedges.push_back(e);
		cpyWfeorign(e, wfe);

		if( wf.isEdgeIncident(e, u) )
		{
			assert(eu == wfe);
			eu = e;
		}
	}

	assert( wf.isVertex(u) );
	assert( wf.isEdge(eu) );
}


unsigned StraightSkeleton::patchWavefront(unsigned eccw, unsigned vccw, unsigned ecw,
				unsigned vcw, Vector place, double time, unsigned skvtx)
{
	return patchWavefront(eccw, vccw, ecw, vcw, place, time, skvtx, vector<upair>());
}


unsigned StraightSkeleton::patchWavefront(unsigned eccw, unsigned vccw, unsigned ecw,
				unsigned vcw, Vector place, double time, unsigned skvtx,
				std::vector<std::pair<unsigned,unsigned> > steineredges )
{
	loggerSk.log(LL_DEBUG, "  Patch wavefront eccw=%d, vccw=%d, ecw=%d, vcw=%d; skvtx=%d.\n",
			eccw, vccw, ecw, vcw, skvtx);

	vector<unsigned> nedges;
	vector<unsigned> nedges_late;

	assert( wf.isVertex(vccw) );
	assert( wf.isVertex(vcw) );
	assert( wf.isEdge(eccw) );
	assert( wf.isEdge(ecw) );
	assert( wf.isEdgeIncident(eccw, vccw) );
	assert( wf.isEdgeIncident(ecw, vcw) );
	assert( wfeorigin.count(eccw) );
	assert( wfeorigin.count(ecw) );

	unsigned vccw2 = wf.getOppositeVertex(eccw, vccw);
	unsigned vcw2 = wf.getOppositeVertex(ecw, vcw);


	// This wavefront collapsed
	if( vcw2 == vccw2  &&  skvtx!=0  &&  steineredges.empty() )
	{
		loggerSk.log(LL_DEBUG, "    Wavefront collapsed.\n");

		loggerSk.log(LL_DEBUG, "    Add sk edge to wf-vtx %d\n", vcw2);
		assert( getWfVertexInfo(vcw2).skstart );
		sk.addEdge(skvtx, getWfVertexInfo(vcw2).skstart, 0, eps.ZERO_EVENT );

		removeWfVertex(vcw2);
		assert( steineredges.empty() );

		// No new vertex created
		return 0;
	}


	const Vector eccwvel = getWfEdgeSpeed(eccw);
	const Vector ecwvel = getWfEdgeSpeed(ecw);


	// Shoot a new vertex which replaces vcw and vccw
	const wfvtxtype newvt = skvtx!=0  ?  CONVEX : STEINER_MOVING;
	const Ray vel = getVertexSpeed(eccw, vccw, ecw, vcw, place, time);
	const unsigned newv = addWfVertex(vel, newvt);
	loggerSk.log(LL_DEBUG, "    Shot new vertex %d with speed (%f, %f)\n",
			newv, TO_MDOUBLE(vel.getDir().x), TO_MDOUBLE(vel.getDir().y));

	if( newvt==CONVEX )
	{
		assert( skvtx != 0 );
		getWfVertexInfo(newv).skstart = skvtx;
	}
	else
	{
		assert( skvtx == 0 );
		loggerSk.log(LL_DEBUG, "    No sk vertex given. Vertex %d is a moving steiner vertex.\n", newv);
		if( (eccwvel - ecwvel).lensq() > sq(eps.ZERO_ANGLE) )
			loggerSk.log(LL_ERROR, "Incident edges of moving Steiner vertex %d are not collinear.\n", newv);
	}



	loggerSk.log(LL_DEBUG, "    Assign the Steiner edges to eccw=%d or ecw=%d.\n", eccw, ecw);

	vector<unsigned> splitCW;
	vector<unsigned> splitCCW;

	for( std::vector<upair>::iterator it=steineredges.begin();
			it != steineredges.end(); it++)
	{
		const unsigned v = it->second;
		const unsigned e = it->first;
		const unsigned v2 = wf.getOppositeVertex(e, v);
		const wfvtxtype v2t = getWfVertexInfo(v2).type;
		const double side = Vector::sin2( vel.getDir(),
				wf.getVertexPos(v2,-eps.ZERO_ANGLE)-place );

		const unsigned ea = wf.getCWEdge(e,v2);
		const unsigned eb = wf.getCCWEdge(e,v2);
		const Vector eadir = getWfEdgeDir(ea, v2, time-eps.ZERO_ANGLE);
		const Vector ebdir = getWfEdgeDir(eb, v2, time-eps.ZERO_ANGLE);
		const Vector edir = getWfEdgeDir(e, v2, time-eps.ZERO_ANGLE);

		// this edge is on the propagation line of newv
		if( fabs(side) < eps.ZERO_ANGLE )
		{
			loggerSk.log(LL_DEBUG, "    edge %d is on the propagation direction.\n", e);

			// If the new vertex was convex, it becomes reflex or multi convex now.
			if( newvt==CONVEX )
			{
				// Is newv multi-convex or reflex?
				if( (Vector::sin2(eccwvel, vel.getDir()) + Vector::sin2(vel.getDir(), ecwvel) ) <=
						eps.ZERO_ANGLE )
				{
					loggerSk.log(LL_DEBUG, "      new vertex %d got reflex.\n", newv);
					getWfVertexInfo(newv).type = REFLEX;
				}
				else
				{
					loggerSk.log(LL_DEBUG, "      new vertex %d got multi-convex.\n", newv);
					getWfVertexInfo(newv).type = CONVEX_MULTI;
				}
			}

			nedges.push_back( wf.repotEdge(e, v, newv, time-eps.ZERO) );
		}
		// This edge is obsolete now...
		else if( v2t==STEINER_MOVING || (v2t==STEINER_RESTING &&
					fabs(Vector::sin2(eadir,ebdir)) < min( fabs(Vector::sin2(edir,eadir)),
							fabs(Vector::sin2(edir,ebdir))) ) )
		{
			loggerSk.log(LL_DEBUG, "    steineredge %d is obsolete.\n", e);

			const unsigned ea = wf.getCWEdge(e,v2);
			const unsigned eb = wf.getCCWEdge(e,v2);

			wf.removeEdge(e);
			const unsigned neab = wf.mergeEdges(v2, ea, eb);
			cpyWfeorign(neab, ea);
			nedges.push_back(neab);
			removeWfVertex(v2);

			// Attention: v2 could have been v(c)cw2!

			if( v2==vcw2 )
			{
				assert( ecw==eb );
				ecw = neab;
				vcw2 = wf.getOppositeVertex(ecw, vcw);
			}
			if( v2==vcw )
			{
				assert( eccw==ea );
				eccw = neab;
				vccw2 = wf.getOppositeVertex(eccw, vccw);
			}
		}
		// This edge is obsolete now...
		else if( v2t==CONVEX_MULTI )
		{
			wf.removeEdge(e);
			getWfVertexInfo(v2).type = CONVEX;
		}
		// We shoot a new steiner point splitting either ecw or eccw
		else
		{
			// Get split edge and speed of new moving steiner
			unsigned esplit = side>0 ? eccw : ecw;
			unsigned vsplit = side>0 ? vccw : vcw;

			// TODO: Use ZERO_ANGLE here?
			const Vector edir = getWfEdgeDir(e, v, time-eps.ZERO);
			const Vector esplitvel = getWfEdgeSpeed(esplit);

			if( fabs(Vector::cos2(edir, esplitvel)) < eps.ZERO_ANGLE )
			{
				mergeParallelSteinerEdges(esplit, vsplit, e, v, time, nedges);

				// Restore eccw resp. ecw
				if( side > 0 )
				{
					eccw = esplit;
					vccw = vsplit;
				}
				else
				{
					ecw = esplit;
					vcw = vsplit;
				}

				assert( wf.isEdge(eccw) );
				assert( wf.isEdge(ecw) );
				assert( wf.isEdgeIncident(eccw, vccw) );
				assert( wf.isEdgeIncident(ecw, vcw) );

				vccw2 = wf.getOppositeVertex(eccw, vccw);
				vcw2 = wf.getOppositeVertex(ecw, vcw);
			}
			else
			{
				const Ray vel = getWfEdgeSpeedInDir(esplit, e, v, time);
				const unsigned newst = addWfVertex(vel, STEINER_MOVING);

				// Create a new moving steiner vertex
				loggerSk.log(LL_DEBUG, "    new moving steiner %d on split-edge %d with speed (%f,%f).\n",
						newst, esplit, TO_MDOUBLE(vel.getDir().x), TO_MDOUBLE(vel.getDir().y));

				// Recreate edge
				nedges.push_back( wf.repotEdge(e, v, newst, time-eps.ZERO));

				// Remember which edge is split
				if( side>0 )
					splitCCW.push_back(newst);
				else
					splitCW.push_back(newst);
			}
		}
	}

	assert( wf.isEdge(eccw) );
	assert( wf.isEdge(ecw) );

	// Remove the old edges and reconnect them with the new emanated vertex
	const unsigned neccw = wf.repotEdge(eccw, vccw, newv, time-eps.ZERO);
	const unsigned necw = wf.repotEdge(ecw, vcw, newv, time-eps.ZERO);
	cpyWfeorign(neccw, eccw);
	cpyWfeorign(necw, ecw);

	loggerSk.log(LL_DEBUG, "    reconnected %d by edges %d (ccw) and %d (cw).\n", newv, neccw, necw);


	if( splitCCW.empty())
	{
		loggerSk.log(LL_DEBUG, "    ccw %d not split.\n", neccw);

		if( (eccwvel+ecwvel).lensq() > sq(eps.ZERO) )
		{
			if( isWfEdgeExpanding(vccw2, newv) > 0 )
			{
				loggerSk.log(LL_DEBUG, "      add (late) event of %d\n", neccw);
				nedges_late.push_back( neccw );
			}
			else
			{
				loggerSk.log(LL_DEBUG, "      add event of %d\n", neccw);
				nedges.push_back( neccw );
			}
		}
	}
	else
	{
		loggerSk.log(LL_DEBUG, "    split ccw edge %d by %d vertices\n", neccw, splitCCW.size());

		vector<unsigned> newedges = wf.splitEdgeByVertices(neccw, splitCCW, time+eps.ZERO, eps.ZERO);
		assert( wfeorigin.count(eccw) );

		for( vector<unsigned>::iterator it = newedges.begin(); it != newedges.end(); it++)
		{
			nedges_late.push_back( *it );
			cpyWfeorign(*it, eccw);
		}
	}

	if( splitCW.empty())
	{
		loggerSk.log(LL_DEBUG, "    cw %d not split.\n", necw);

		if( (eccwvel+ecwvel).lensq() > sq(eps.ZERO) )
		{

			if( isWfEdgeExpanding(newv, vcw2) > 0 )
			{
				loggerSk.log(LL_DEBUG, "      add (late) event of %d\n", necw);
				nedges_late.push_back( necw );
			}
			else
			{
				loggerSk.log(LL_DEBUG, "      add event of %d\n", necw);
				nedges.push_back( necw );
			}
		}
	}
	else
	{
		loggerSk.log(LL_DEBUG, "    split cw edge %d by %d vertices\n", necw, splitCW.size());

		vector<unsigned> newedges = wf.splitEdgeByVertices(necw, splitCW, time+eps.ZERO, eps.ZERO);
		assert( wfeorigin.count(ecw) );

		for( vector<unsigned>::iterator it = newedges.begin(); it != newedges.end(); it++)
		{
			nedges_late.push_back( *it );
			cpyWfeorign(*it, ecw);
		}
	}


	insertEvents(nedges, time);
	insertEvents(nedges_late, time+eps.ZERO_EVENT);

	return newv;
}


void StraightSkeleton::processSplitEvent(const event& ev)
{
	loggerSk.log(LL_DEBUG, "  Processing split event...\n");

	const double time = ev.time;
	const unsigned eve = ev.wfedge;
	const Vector place = ev.place;
	unsigned u = wf.getEdge(eve).s;
	unsigned v = wf.getEdge(eve).d;

	if( getWfVertexInfo(v).type < getWfVertexInfo(u).type  )
		swap(u,v);


	const wfvtxtype& ut = getWfVertexInfo(u).type;
	const wfvtxtype& vt = getWfVertexInfo(v).type;
	assert ( (ut==REFLEX && vt==STEINER_MOVING ) ||
			(ut==REFLEX && vt==CONVEX_MULTI ) ||
			(ut==CONVEX_MULTI && vt==CONVEX_MULTI) ||
			(ut==CONVEX_MULTI && vt==STEINER_MOVING) ||
			(ut==STEINER_MOVING && vt==STEINER_MOVING));

	unsigned eul = wf.getCCWEdge(eve, u);
	unsigned eur = wf.getCWEdge(eve, u);
	unsigned evl = wf.getCWEdge(eve, v);
	unsigned evr = wf.getCCWEdge(eve, v);

	const unsigned ul2 = wf.getOppositeVertex(eul, u);
	const unsigned ur2 = wf.getOppositeVertex(eur, u);
	const unsigned vl2 = wf.getOppositeVertex(evl, v);
	const unsigned vr2 = wf.getOppositeVertex(evr, v);


	// Vertex u and v should drive in opposite directions
	if( (wf.getVertex(u).vel.getDir().norm() +
				wf.getVertex(v).vel.getDir().norm()).lensq() > sq(eps.ZERO_ANGLE) )
	{
		loggerSk.log(LL_ERROR, "Split event: vertex %d and %d are not driving in opposite direction.\n",
				u, v);
	}

	loggerSk.log(LL_DEBUG, "  u-left=e%d/v%d, v-left=e%d/v%d, u-right=e%d/v%d, "
			"v-right=e%d/v%d\n", eul, ul2, evl, vl2, eur, ur2, evr, vr2);


	// Left and right side collapsed
	if( ul2==vl2 && ur2==vr2 )
	{
		// The straight skeleton vertex
		const unsigned skvtx = sk.addVertex(place,0);
		loggerSk.log(LL_DEBUG, "  Both sides collapsed. Created new sk-vertex %d.\n", skvtx);

		vector<unsigned> vert;
		vert.push_back( getWfVertexInfo(ul2).skstart );
		vert.push_back( getWfVertexInfo(ur2).skstart );

		if( ut==REFLEX || ut==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(u).skstart );
		if( vt==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(v).skstart );

		sk.addEdgeStar( skvtx, vert, 0, eps.ZERO_EVENT );

		// Remove the old wavefront vertices
		removeWfVertex(u);
		removeWfVertex(v);
		removeWfVertex(ul2);
		removeWfVertex(ur2);

		return;
	}

	// If two moving steiner vertices met then nothing happens
	if( ut==STEINER_MOVING && vt==STEINER_MOVING )
	{
		loggerSk.log(LL_DEBUG, "  Both are moving steiner vertices. Just remove edge.\n");

		wf.removeEdge(eve);

		// Merge incident edges of u and v
		const unsigned neu = wf.mergeEdges(u, eul, eur);
		const unsigned nev = wf.mergeEdges(v, evl, evr);
		cpyWfeorign(neu, eul);
		cpyWfeorign(nev, evl);
		insertEvent(neu, time);
		insertEvent(nev, time);

		// Remove the vertices u, v
		removeWfVertex(u);
		removeWfVertex(v);

		return;
	}


	// The straight skeleton vertex
	const unsigned skvtx = sk.addVertex(place,0);
	loggerSk.log(LL_DEBUG, "  Created new sk-vertex %d for reflex %d.\n", skvtx, u);


	vector<unsigned> vert;

	if( ut==REFLEX || ut==CONVEX_MULTI )
		vert.push_back( getWfVertexInfo(u).skstart );
	if( vt==CONVEX_MULTI )
		vert.push_back( getWfVertexInfo(v).skstart );

	assert( vert.size() > 0 );
	sk.addEdgeStar( skvtx, vert, 0, eps.ZERO_EVENT );

	// Patch the left side
	patchWavefront(eul, u, evl, v, place, time, skvtx);
	// Patch the right side
	patchWavefront(evr, v, eur, u, place, time, skvtx);

	// Remove the old wavefront vertices
	removeWfVertex(u);
	removeWfVertex(v);
}


void StraightSkeleton::processStartEvent(const event& ev)
{
	loggerSk.log(LL_DEBUG, "  Processing start event...\n");

	const Vector place = ev.place;
	const double time = ev.time;
	const unsigned eve = ev.wfedge;
	unsigned u = wf.getEdge(eve).s;
	unsigned v = wf.getEdge(eve).d;

	// Vertex u reaches the resting steiner point v
	if( getWfVertexInfo(u).type>getWfVertexInfo(v).type )
		swap(u,v);

	const wfvtxtype ut = getWfVertexInfo(u).type;
#ifndef NDEBUG
	const wfvtxtype vt = getWfVertexInfo(v).type;
#endif
	assert( ut==REFLEX || ut==STEINER_MOVING || ut==CONVEX_MULTI );
	assert( vt==STEINER_RESTING );
	assert( wf.getVertexDegree(u)==3 );
	assert( wf.getVertexDegree(v)==3 );


	//Velocity vector of u
	const Ray uvel = wf.getVertex(u).vel;
	// Get the wf-edges defining reflex vertex u
	const unsigned ecw = wf.getCWEdge(eve, u);
	const unsigned eccw = wf.getCCWEdge(eve, u);


	// Consider the two remaining edges on incident to v
	// e1 will be the edge where u will drive along
	// e2 will drive sideways
	unsigned e1 = wf.getCCWEdge(eve, v);
	unsigned e2 = wf.getCWEdge(eve, v);
	unsigned v1 = wf.getOppositeVertex(e1, v);
	unsigned v2 = wf.getOppositeVertex(e2, v);


	// Let e1 be the segment that is most collinear with eve
	if( fabs( Vector::sin2(wf.getVertex(u).vel.getDir(),
							getWfEdgeDir(e2, v, time-eps.ZERO_ANGLE) )) <
		fabs( Vector::sin2(wf.getVertex(u).vel.getDir(),
							getWfEdgeDir(e1, v, time-eps.ZERO_ANGLE) ))
		)
	{
		swap(e1, e2);
		swap(v1, v2);
	}

	// u is a moving Steiner vertex and the edges e1 and e2 are collinear (and eve and e1 (or e2) are not).
	if( ut==STEINER_MOVING  &&
			fabs( Vector::sin2(getWfEdgeDir(e2, v, time-eps.ZERO_ANGLE),
					getWfEdgeDir(e1, v, time-eps.ZERO_ANGLE) )) <
			fabs( Vector::sin2(wf.getVertex(u).vel.getDir(),
					getWfEdgeDir(e1, v, time-eps.ZERO_ANGLE) ))
		)
	{
		loggerSk.log(LL_DEBUG, "  origin of %d is moving Steiner vertex %d, simply remove the edge.\n", v, u);

		const unsigned eu1 = wf.getCWEdge(eve,u);
		const unsigned eu2 = wf.getCCWEdge(eve,u);
		const unsigned ev1 = wf.getCWEdge(eve,v);
		const unsigned ev2 = wf.getCCWEdge(eve,v);

		wf.removeEdge(eve);

		// Merge incident edges of u and v
		const unsigned neu = wf.mergeEdges(u, eu1, eu2);
		const unsigned nev = wf.mergeEdges(v, ev1, ev2);
		cpyWfeorign(neu, eu1);
		cpyWfeorign(nev, ev1);
		insertEvent(neu, time);
		insertEvent(nev, time);

		removeWfVertex(u);
		removeWfVertex(v);

		return;
	}

	if( ut==CONVEX_MULTI  &&
		fabs(Vector::sin2( getWfEdgeDir(e1, v, time-eps.ZERO_ANGLE),
				getWfEdgeDir(e2, v, time-eps.ZERO_ANGLE))) <
		fabs(Vector::sin2( getWfEdgeDir(e1, v, time-eps.ZERO_ANGLE),
				getWfEdgeDir(eve, v, time-eps.ZERO_ANGLE)))
		)
	{
		loggerSk.log(LL_DEBUG, "  origin of %d is convex multi vertex %d, which becomes convex.\n", v, u);

		const unsigned eu1 = wf.getCWEdge(eve,u);
		const unsigned eu2 = wf.getCCWEdge(eve,u);
		const unsigned ev1 = wf.getCWEdge(eve,v);
		const unsigned ev2 = wf.getCCWEdge(eve,v);

		wf.removeEdge(eve);

		// Merge incident edges of u and v
		const unsigned nev = wf.mergeEdges(v, ev1, ev2);
		cpyWfeorign(nev, ev1);
		insertEvent(nev, time);
		removeWfVertex(v);

		getWfVertexInfo(u).type = CONVEX;

		if( fabs(getCollapseTime(eu1)-time) < fabs(getCollapseTime(eu2)-time) )
			processSwitchEvent(getCollapseEvent(eu1));
		else
			processSwitchEvent(getCollapseEvent(eu2));
		return;
	}


	const wfvtxtype v1t = getWfVertexInfo(v1).type;
	// This will be the edge where v is now contained in
	unsigned splite;

	// Try a topological decision first
	if( wf.isAdjacent(u,v2) )
		splite = wf.getCommonEdge(u,v2);
	else
	{
		// p2 is right of moving vertex u
		if( Vector::sin3(wf.getVertexPos(v1, time-eps.ZERO_ANGLE ),
						wf.getVertexPos(u, time-eps.ZERO_ANGLE ),
						wf.getVertexPos(v2, time-eps.ZERO_ANGLE ))  > 0 )
			splite = ecw;
		else
			splite = eccw;
	}


	loggerSk.log(LL_DEBUG, "  Kicking vertex %d will drive to vertex %d.\n", u, v1);
	loggerSk.log(LL_DEBUG, "  Resting %d will drive to %d, splitting edge %d.\n", v, v2, splite);


	// Check for the parallel case first
	if( fabs(Vector::cos2( getWfEdgeSpeed(splite),
			getWfEdgeDir(e2, v, time-eps.ZERO) )) < eps.ZERO_ANGLE )
	{
		loggerSk.log(LL_DEBUG, "  The edge %d moves parallel to edge %d. Merge the Steiner edges.\n", splite, e2);

		vector<unsigned> nedges;
		mergeParallelSteinerEdges(splite, u, e2, v, time, nedges);
		insertEvents(nedges, time);
		return;
	}

	// Get opposite vertex on splite
	const unsigned v3 = wf.getOppositeVertex(splite, u);

	const unsigned notsplite = splite==ecw ? eccw : ecw;
	const unsigned v4 = wf.getOppositeVertex(notsplite, u);

	if( getWfVertexInfo(v3).type==CONVEX && fabs( getCollapseTime(splite)-time) <= eps.ZERO )
	{
		processSwitchEvent( getCollapseEvent(splite) );
		return;
	}
	if( getWfVertexInfo(v4).type==CONVEX && fabs( getCollapseTime(notsplite)-time) <= eps.ZERO )
	{
		processSwitchEvent( getCollapseEvent(notsplite) );
		return;
	}


	// We have a split event happening at the same time
	if(	(v1t==STEINER_MOVING || v1t==REFLEX || v1t==CONVEX_MULTI ) &&
		(wf.getVertexPos(u,time)-wf.getVertexPos(v1,time)).lensq() < sq(eps.ZERO) )
	{
		loggerSk.log(LL_DEBUG, "  Split event for vertices %d and %d.\n", u, v1);

		// The straight skeleton vertex
		const unsigned skvtx = sk.addVertex(place,0);
		loggerSk.log(LL_DEBUG, "    Created new sk-vertex %d.\n", skvtx);

		vector<unsigned> vert;
		if( ut==REFLEX || ut==CONVEX_MULTI )
			vert.push_back( getWfVertexInfo(u).skstart );
		if( v1t==REFLEX || v1t==CONVEX_MULTI  )
			vert.push_back( getWfVertexInfo(v1).skstart );
		assert( vert.size() > 0 );
		sk.addEdgeStar( skvtx, vert, 0, eps.ZERO_EVENT );


		const unsigned evccw = wf.getCCWEdge(e1,v1);
		const unsigned evcw = wf.getCWEdge(e1,v1);

		// One side has a steiner edge incident...
		vector<upair> steineredges;
		steineredges.push_back( upair(e2,v) );
		// .. the other has not
		vector<upair> nosteineredges;

		// Patch both sides
		patchWavefront(eccw, u, evcw, v1, place, time, skvtx, splite==eccw ? steineredges : nosteineredges);
		patchWavefront(evccw, v1, ecw, u, place, time, skvtx, splite==ecw ? steineredges : nosteineredges);

		removeWfVertex(u);
		removeWfVertex(v1);

		// The vertex v could have been remved by mergeParallelSteinerEdges_fixEndVertex
		if( wf.isVertex(v) )
			removeWfVertex(v);
		return;
	}




	// Lets start moving!
	wf.getVertex(v).vel = getWfEdgeSpeedInDir(splite, e2, v, time);
	const Vector vspeed = wf.getVertex(v).vel.getDir();
	getWfVertexInfo(v).type = STEINER_MOVING;

	// Create new vertex v
	loggerSk.log(LL_DEBUG, "  Set speed of steiner vertex %d to speed (%f, %f)\n",
			v, TO_MDOUBLE(vspeed.x), TO_MDOUBLE(vspeed.y));



	loggerSk.log(LL_DEBUG, "  Removed old edges. Relinking...\n");

	if( v2==v3 )
	{
		loggerSk.log(LL_DEBUG, "  Collapsing triangle.\n");
		const wfvtxtype v3t = getWfVertexInfo(v3).type;

		// Remove the old edges
		wf.removeEdge(e2);
		assert( wf.getVertexDegree(v) == 2 );
		assert( wf.getVertexDegree(v3) == 2 );

		// Merge the incident edges of v and v3

		const unsigned ne1 = wf.mergeEdges(v, e1, eve);
		removeWfVertex(v);

		if( v3t == CONVEX_MULTI )
		{
			loggerSk.log(LL_DEBUG, "    %d is a convex multi vertex and becomes convex.\n", v3);

			getWfVertexInfo(v3).type = CONVEX;
			processSwitchEvent(getCollapseEvent(splite));
		}
		else
		{
			loggerSk.log(LL_DEBUG, "    %d is a moving steiner vertex, remove edge %d.\n", v3, e2);
			assert( v3t == STEINER_MOVING );

			const unsigned e4 = wf.getCWEdge(splite, v3);
			const unsigned ne4 = wf.mergeEdges(v3, splite, e4);
			cpyWfeorign(ne4, e4);

			removeWfVertex(v3);

			vector<unsigned> nedges;
			nedges.push_back(ne1);
			nedges.push_back(ne4);
			insertEvents(nedges, time);
		}
		return;
	}


	// Remove the old edges
	wf.removeEdge(eve);
	vector<unsigned> nedges;

	const Ray vvel = wf.getVertex(v).vel;
	Vector mid = wf.getVertexSegment(u,v1,time).getMid();
	if( uvel.projDistance(mid) > eps.ZERO_EVENT )
		mid = wf.getVertexPos(u,time+eps.ZERO_EVENT);
	wf.getVertex(u).vel = Ray(mid, Vector(0,0));
	wf.getVertex(v).vel = Ray( (mid+wf.getVertexPos(v3,time))/2 , Vector(0,0));

	nedges.push_back( wf.repotEdge(e1, v, u, time, eps.ZERO ) );
	nedges.push_back( wf.renewEdge(e2) );
	upair nsplite = wf.splitEdge(splite, v, time, eps.ZERO);

	assert( wfeorigin.count(splite) );
	cpyWfeorign(nsplite.first, splite);
	cpyWfeorign(nsplite.second, splite);
	nedges.push_back(nsplite.first);
	nedges.push_back(nsplite.second);

	wf.getVertex(u).vel = uvel;
	wf.getVertex(v).vel = vvel;

	// Move vertex v back
	insertEvents(nedges, time);
}


void StraightSkeleton::processMultiSplitEvent(const event& ev)
{
	loggerSk.log(LL_DEBUG, "  Processing multi split event...\n");

	const Vector place = ev.place;
	const double time = ev.time;
	const unsigned eve = ev.wfedge;
	unsigned u = wf.getEdge(eve).s;
	unsigned v = wf.getEdge(eve).d;

	// Vertex u reaches the resting steiner point v
	if( getWfVertexInfo(u).type>getWfVertexInfo(v).type )
		swap(u,v);

#ifndef NDEBUG
	const wfvtxtype& ut = getWfVertexInfo(u).type;
	const wfvtxtype& vt = getWfVertexInfo(v).type;
#endif
	assert( ut==REFLEX || ut==CONVEX_MULTI || ut==STEINER_MOVING );
	assert( vt==STEINER_MULTI );


	const unsigned skvtx = sk.addVertex(place);
	loggerSk.log(LL_DEBUG, "  add sk-vertex %d.\n", skvtx);
	vector<unsigned> skvert;



	// Go around the multi-steiner vertex and consider the cones between collapsed edges
	unsigned eccw = eve;
	do
	{
		// Get the next wedge
		const unsigned ecw = eccw;
		eccw = wf.getCCWEdge(eccw,v);
		const unsigned vcw = wf.getOppositeVertex(ecw,v);
		unsigned vccw = wf.getOppositeVertex(eccw,v);
		const wfvtxtype vcwt = getWfVertexInfo(vcw).type;

		loggerSk.log(LL_DEBUG, "  consider incident cw edge %d adj to vertex %d\n", ecw, vcw);

		assert( vcwt==REFLEX || vcwt==CONVEX_MULTI || vcwt==STEINER_MOVING );

		if( getWfVertexInfo(vcw).type != STEINER_MOVING )
		{
			loggerSk.log(LL_DEBUG, "  add a new sk-edge.\n");
			assert( sk.isVertex(skvtx));
			skvert.push_back( getWfVertexInfo(vcw).skstart );
		}

		assert( fabs(getCollapseTime(ecw)-time) < eps.ZERO_EVENT );

		// Skip incident edges which did not reach v
		list<unsigned> emane;
		while( fabs(getCollapseTime(eccw)-time) > eps.ZERO_EVENT )
		{
			loggerSk.log(LL_DEBUG, "    remember emanating edge %d\n", eccw);
			emane.push_back(eccw);
			// Goto next edge
			eccw = wf.getCCWEdge(eccw,v);
			vccw = wf.getOppositeVertex(eccw,v);

			if( eccw == ecw )
				break;
		}


		loggerSk.log(LL_DEBUG, "    found incident ccw edge %d adj to vertex %d\n", eccw, vccw);

		const wfvtxtype vccwt = getWfVertexInfo(vccw).type;
		assert( vccwt==REFLEX || vccwt==CONVEX_MULTI || vccwt==STEINER_MOVING );

		unsigned ecw2 = wf.getCWEdge(ecw,vcw);
		unsigned eccw2 = wf.getCCWEdge(eccw,vccw);
		unsigned vcw2 = wf.getOppositeVertex(ecw2, vcw);
		unsigned vccw2 = wf.getOppositeVertex(eccw2, vccw);

		if( ecw2 == eccw2 )
		{
			loggerSk.log(LL_DEBUG, "    Triangle collapsed to a point, containing edge %d.\n", ecw2 );
			assert( emane.empty());
			continue;
		}

		if( vcw2 == vccw2 )
		{
			loggerSk.log(LL_DEBUG, "    4-gon collapsed to a line, incident to vertex %d.\n", vcw2 );
			assert( emane.empty() );

			loggerSk.log(LL_DEBUG, "    add a new sk-edge.\n");
			assert( sk.isVertex(skvtx));
			skvert.push_back( getWfVertexInfo(vcw2).skstart );

			removeWfVertex(vcw2);
			continue;
		}


		// Get the propagation normals of the edges which are connected to the
		// new vertex newv, which is created below.
		const Vector vecw2 = getWfEdgeSpeed(ecw2);
		const Vector veccw2 = getWfEdgeSpeed(eccw2);


		// vcw and vccw are steiner and the splitting lines move parallel in the same direction
		if( vcwt==STEINER_MOVING && vccwt==STEINER_MOVING && (vecw2-veccw2).lensq()<sq(eps.ZERO_ANGLE) )
		{
			loggerSk.log(LL_DEBUG, "    a wf-line supported by moving steiner %d (e%d) and %d (e%d) sweeps %d.\n",
					vcw, ecw2, vccw, eccw2, v);


			assert( sk.isVertexIsolated(skvtx));
			sk.removeVertex(skvtx);
			loggerSk.log(LL_DEBUG, "    new sk vertex %d removed.\n", skvtx);


			// Get the steiner edges
			vector<upair> steineredges;
			for( list<unsigned>::iterator it = emane.begin(); it != emane.end(); it++)
				steineredges.push_back( upair(*it, v) );

			// Patch the cone
			const unsigned u = patchWavefront(eccw2, vccw, ecw2, vcw, place, time, 0, steineredges);

			// Vertex u is obsolete, and could be a moving Steiner vertex!
			if( wf.getVertexDegree(u) == 2 )
			{
				// TODO: do we remove reflex or convex vertices here?
				loggerSk.log(LL_DEBUG, "    remove convex vertex %d.\n", u);

				const unsigned e1 = wf.getVertex(u).inEdge;
				const unsigned e2 = wf.getCWEdge(e1,u);
				const unsigned e12 = wf.mergeEdges(u, e1, e2);
				cpyWfeorign( e12, eccw2 );
				insertEvent( e12, time);

				removeWfVertex(u);
			}

			// This cone is done, and its the only one
			break;
		}


		// Get the steiner edges
		vector<upair> steineredges;
		for( list<unsigned>::iterator it = emane.begin(); it != emane.end(); it++)
			steineredges.push_back( upair(*it, v) );

		// Patch the cone
		patchWavefront(eccw2, vccw, ecw2, vcw, place, time, skvtx, steineredges);

	}while(eccw != eve);


	if( sk.isVertex(skvtx) )
	{
		if( skvert.size() > 0 )
			sk.addEdgeStar( skvtx, skvert, 0, eps.ZERO_EVENT );
		else
			loggerSk.log(LL_WARNING, "Created an isolated new sk vertex %d.\n", skvtx);
	}

	// Remove all adjacent vertices to v, in particular u.
	vector<unsigned> adj = wf.getAdjacentVertices(v);
	for( unsigned i=0; i<adj.size(); i++)
		removeWfVertex(adj.at(i));
	// Remove v
	removeWfVertex(v);
}


unsigned StraightSkeleton::skipRestingSteiner(unsigned e, unsigned v, double time) const
{
	assert( wf.isVertex(v) );
	assert( wf.isEdge(e) );
	assert( wf.isEdgeIncident(e,v) );
	assert( getWfVertexInfo(v).type == STEINER_MOVING ||
			getWfVertexInfo(v).type == STEINER_RESTING ||
			getWfVertexInfo(v).type == STEINER_MULTI ||
			getWfVertexInfo(v).type == CONVEX_MULTI ||
			getWfVertexInfo(v).type == REFLEX );

	const Vector s = wf.getVertexPos(v, time);
	v = wf.getOppositeVertex(e,v);
	const Vector d = wf.getVertexPos(v, time);

	while( getWfVertexInfo(v).type==STEINER_RESTING )
	{
		const unsigned e1 = wf.getCCWEdge(e, v);
		const unsigned e2 = wf.getCWEdge(e, v);
		const unsigned v1 = wf.getOppositeVertex(e1, v);
		const unsigned v2 = wf.getOppositeVertex(e2, v);

		const Vector p1 = wf.getVertexPos(v1,time);
		const Vector p2 = wf.getVertexPos(v2,time);

		const double sin1 = fabs(Vector::sin3(s,d,p1));
		const double sin2 = fabs(Vector::sin3(s,d,p2));

		if( sin1<eps.ZERO || sin2<eps.ZERO )
		{
			if( sin1 < sin2 )
			{
				e = e1;
				v = v1;
			}
			else
			{
				e = e2;
				v = v2;
			}
		}
		else
		{
			break;
		}
	}

	return v;
}


void StraightSkeleton::removeWfVertex(unsigned v)
{
	assert( wf.isVertex(v));
	wf.removeVertex(v);
}


unsigned StraightSkeleton::addWfVertex(const Ray& vel, wfvtxtype t)
{
	wfvtxproperties* prop = new wfvtxproperties(t);
	wfvtxprop.push_back(prop);
	const unsigned v = wf.addVertex(vel, (void*) prop );
	return v;
}


StraightSkeleton::wfvtxproperties& StraightSkeleton::getWfVertexInfo(unsigned v)
{
	assert( wf.isVertex(v) );
	wfvtxproperties* prop = (wfvtxproperties*) wf.getVertex(v).data;
	assert( prop );
	return *prop;
}


const StraightSkeleton::wfvtxproperties& StraightSkeleton::getWfVertexInfo(unsigned v) const
{
	assert( wf.isVertex(v) );
	wfvtxproperties* prop = (wfvtxproperties*) wf.getVertex(v).data;
	assert( prop );
	return *prop;
}


void StraightSkeleton::cpyWfeorign(unsigned e, unsigned esrc)
{
	assert( wf.isEdge(e) );


	if( esrc>0 && wfeorigin.count(esrc)>0 )
	{
		wfeorigin[e] = wfeorigin.at(esrc);
	}
	else
	{
#ifndef NDEBUG
		const KineticSLGraph::Edge& ed = wf.getEdge(e);
		unsigned v1 = ed.s;
		unsigned v2 = ed.d;

		if( getWfVertexInfo(v2).type > getWfVertexInfo(v1).type )
			swap(v1, v2);

		// The new edge must correspond to a motorcycle trace. Hence, one of
		// both involved wf-vertices must be a resting Steiner vertex of a
		// Steiner multi vertex.
		assert( getWfVertexInfo(v1).type==STEINER_MULTI || getWfVertexInfo(v1).type==STEINER_RESTING ||
			getWfVertexInfo(v1).type==STEINER_MOVING || getWfVertexInfo(v1).type==CONVEX_MULTI ||
			getWfVertexInfo(v2).type==STEINER_MULTI || getWfVertexInfo(v2).type==STEINER_RESTING ||
			getWfVertexInfo(v2).type==STEINER_MOVING || getWfVertexInfo(v2).type==CONVEX_MULTI );

		if( getWfVertexInfo(v1).type==REFLEX &&
				(getWfVertexInfo(v2).type==STEINER_MOVING || getWfVertexInfo(v2).type==CONVEX_MULTI ) )
		{
			const Ray &vel1 = wf.getVertex(v1).vel;
			const Ray &vel2 = wf.getVertex(v2).vel;
			assert( fabs(Vector::det2(vel1.getDir(), vel2.getDir())) < eps.ZERO );
		}
#endif
	}
}


unsigned StraightSkeleton::addSkVertex(unsigned wfvtx, double time)
{
	return addSkVertex(wfvtx, wf.getVertexPos(wfvtx,time));
}


unsigned StraightSkeleton::addSkVertex(unsigned wfvtx, Vector pos)
{
	assert( wf.isVertex(wfvtx) );
	const unsigned skvtx = sk.addVertex(pos);

	getWfVertexInfo(wfvtx).skstart = skvtx;
	loggerSk.log(LL_DEBUG, "  sk-vertex %d given by wf-vertex %d\n", skvtx, wfvtx);
	return skvtx;
}


double StraightSkeleton::isWfEdgeExpanding(unsigned u, unsigned v) const
{
#ifndef NDEBUG
	unsigned e = wf.getCommonEdge(u, v);
	assert( wfeorigin.count(e) > 0 );
#endif

	const Vector uvel = wf.getVertex(u).vel.getDir();
	const Vector vvel = wf.getVertex(v).vel.getDir();

	return Vector::sin2(vvel, uvel);
}

Vector StraightSkeleton::getVertexSpeed(Vector n1, Vector n2) const
{
	// If the three vertices have no proper distance --> return zero vector
	if( (n1+n2).lensq() < sq(eps.ZERO) )
	{
		loggerSk.log(LL_WARNING, "getVertexSpeed: Velocities sum up to zero (%e), "
				"return infinitely fast vertex!\n",
				TO_MDOUBLE((n1+n2).len()) );

		const Vector vel = eps.INFSPEED * (n1-n2).norm().cw();

		if( vel*(n1+n2) < 0 )
			return -vel;
		return vel;
	}

	assert( fabs(n1.len()-1.0) < eps.ZERO );
	assert( fabs(n2.len()-1.0) < eps.ZERO );

	const Vector dir = (n1+n2).norm();
	const double speed = 2.0/( n1*dir + n2*dir );

	assert( speed > 1-eps.ZERO );

	return speed*dir;
}


Vector StraightSkeleton::getVertexEscapePos(unsigned vtx) const
{
    assert( wf.isVertex(vtx) );

    const Ray vel = wf.getVertex(vtx).vel;
    return ((eps.INFDIST/2)*vel.norm()).getEnd();
}


Ray StraightSkeleton::getVertexSpeed(unsigned eccw, unsigned vccw, unsigned ecw,
									unsigned vcw, Vector place, double time) const
{
	assert( wf.isEdgeIncident(eccw, vccw) );
	assert( wf.isEdgeIncident(ecw, vcw) );

	const unsigned uccw = wf.getOppositeVertex(eccw,vccw);
	const unsigned ucw = wf.getOppositeVertex(ecw,vcw);

	const Vector vecw = getWfEdgeSpeed(ecw);
	const Vector veccw = getWfEdgeSpeed(eccw);


	// The two wavefronts are clashing parallel
	if( (vecw+veccw).lensq() < sq(eps.ZERO_ANGLE) )
	{
		loggerSk.log(LL_DEBUG, "    vertex vel: parallel case for edge %d and %d.\n", eccw, ecw);

		// Actually, should be 2.0, but not zero.
		assert( (vecw.cw() + veccw.ccw()).lensq() > 1 );
		Ray dir( place, (vecw.cw() + veccw.ccw()).norm() );

		const Vector ucwpos = wf.getVertexPos(ucw, time);
		const Vector uccwpos = wf.getVertexPos(uccw, time);
		const Vector upos = (ucwpos+uccwpos)/2.0;

		const double projdist = dir.projDistance(upos);

#if 0
		if( fabs(projdist) < sq(eps.ZERO) )
			loggerSk.log(LL_WARNING, "getVertexSpeed: edge lengths are very small: %e\n", TO_MDOUBLE(projdist) );
#endif

		// Make dir pointing to upos
		if( projdist < 0 )
			dir = -dir;

		const Vector vel = eps.INFSPEED*dir.getDir();
		return Ray( place - time*vel, vel);
	}

	const Vector vel = getVertexSpeed(vecw, veccw);
	return Ray( place - time*vel, vel);
}



Vector StraightSkeleton::getWfEdgeSpeedInDir(unsigned e, const Vector &dir) const
{
	// Get speed of wf and direction of trace
	Vector wfespeed = getWfEdgeSpeed(e);
	const Vector tracedir = dir.norm();

	// The speed would be infinite
	if( fabs(wfespeed*tracedir) < eps.ZERO )
	{
		loggerSk.log(LL_WARNING, "Speed of edge %d in direction (%f, %f) is infinite!\n",
				e, TO_MDOUBLE(dir.x), TO_MDOUBLE(dir.y));
		return eps.INFSPEED*tracedir;
	}

	// Set speed vector accordingly
	return tracedir / fabs(wfespeed * tracedir);
}


Ray StraightSkeleton::getWfEdgeSpeedInDir(unsigned e, unsigned edir, unsigned v, double time) const
{
	assert( wf.isEdge(edir) );
	assert( wf.isVertex(v) );
	assert( wf.isEdgeIncident(edir, v));

	Vector wfespeed = getWfEdgeSpeed(e);
	const Vector tracedir = getWfEdgeDir(edir, v, time-eps.ZERO);

	//Parallel case
	if( fabs(Vector::cos2(wfespeed,tracedir)) < eps.ZERO_ANGLE )
	{
		loggerSk.log(LL_WARNING, "edge speed in dir: parallel case for edge %d and %d.\n", e, edir);

		const Vector vel = eps.INFSPEED * tracedir.norm();
		const Vector pos = wf.getVertexPos(v, time);
		return Ray( pos - time*vel, vel);
	}

	const Vector vel = tracedir / fabs(wfespeed * tracedir);
	const Vector pos = wf.getVertexPos(v, time);
	return Ray( pos - time*vel, vel);
}


Vector StraightSkeleton::getWfEdgeDir(unsigned e, unsigned u, double time) const
{
	assert( wf.isVertex(u) );
	assert( wf.isEdge(e) );
	assert( wf.isEdgeIncident(e,u) );

	const unsigned v = wf.getOppositeVertex(e,u);
	// Get the approximate direction by the wavefront vertices
	const Vector approxDir = wf.getVertexPos(v,time) - wf.getVertexPos(u,time);

	// If there is an original input pair, use this more accurate information.
	if( wfeorigin.count(e) )
	{
		const Vector dir = getWfEdgeSpeed(e).cw();
		return (approxDir*dir>=0) ? dir : -dir;
	}
	else
	{
		wfvtxtype ut = getWfVertexInfo(u).type;
		wfvtxtype vt = getWfVertexInfo(v).type;

		if( (ut==CONVEX_MULTI || ut==REFLEX)  &&  (vt==STEINER_RESTING || vt==STEINER_MULTI) )
			return wf.getVertex(u).vel.getDir();
		if( (vt==CONVEX_MULTI || vt==REFLEX)  &&  (ut==STEINER_RESTING || ut==STEINER_MULTI) )
			return -wf.getVertex(v).vel.getDir();

		return approxDir;
	}
}


void StraightSkeleton::insertEvent( unsigned e, double mintime )
{
	const KineticSLGraph::Edge ed = wf.getEdge(e);

	//Get the crashing time between vertex e.s and e.d
	double crashtime = getCollapseTime(e);

	//Crash is in the future, add it
	if( crashtime >= mintime-eps.ZERO )
	{
		loggerSk.log(LL_DEBUG, "  Adding event at %e on edge %d ( %d -> %d).\n",
				TO_MDOUBLE(crashtime), e, ed.s, ed.d);
		events.push( getCollapseEvent(e) );
	}
	else
	{
		loggerSk.log(LL_DEBUG, "  Not adding event at %e on edge %d ( %d -> %d).\n",
				TO_MDOUBLE(crashtime), e, ed.s, ed.d);
	}
}


void StraightSkeleton::insertEvents(const vector<unsigned> &edges, double mintime)
{
	for(vector<unsigned>::const_iterator it = edges.begin(); it!=edges.end(); it++)
		if( wf.isEdge(*it) )
			insertEvent(*it, mintime);
}


void StraightSkeleton::insertMcgIntoWavefront(const skmcgraph& mcg)
{
	// Get all the motorcycles
	typedef const vector<Motorcycle> mclist;
	mclist& mcs = mcg.getMotorcycles();


	// Geometric hash on the end vertices of motorcycles, which crashed into other mcs
	GeomHash<unsigned> mcendhash(wf.getBoundingBox().enlargeFactor(2), mcs.size()+1);
	// The end vertices of motorcycles
	vector<unsigned> mcendvtx;
	// Save for every motorcycle m, which crashes into a wf-wall,
	// the index of the edge e, where m crashed into.
	map<unsigned, unsigned> mcCrashWfe;

	// Run through the motorcycles and save the end point as wf-vertices
	for(unsigned mcidx=0; mcidx<mcs.size(); mcidx++ )
	{
		const Motorcycle& mc = mcs.at(mcidx);
		const Vector crashpnt = mc.getTrace().getEnd();
		const Vector startpnt = mc.getTrace().getStart();

		// Create the end vertex
		unsigned endvtx = 0;
		if( mc.isCrashedIntoWall() )
		{
			// Compute speed of vertex
			const unsigned wfe = getWfEdgeByMc(mcg, mcidx);
			mcCrashWfe[mcidx] = wfe;
			const Segment wfeseg = wf.getEdgeSegment(wfe,0);

			// crashpoint coincides with wf-vertex --> maybe it should become
			// a multi convex vertex.
			if( (crashpnt-wfeseg.getStart()).lensq() <= sq(eps.ZERO_EVENT) ||
					(crashpnt-wfeseg.getEnd()).lensq() <= sq(eps.ZERO_EVENT) )
			{
				// Get the vertex where mcidx crashed into
				const unsigned wfv =  (crashpnt-wfeseg.getEnd()).lensq() <
					(crashpnt-wfeseg.getStart()).lensq()  ?
					wf.getEdge(wfe).d : wf.getEdge(wfe).s;
				// Velocity of wfv
				const Ray& vel = wf.getVertex(wfv).vel;

				if( fabs( Vector::sin2( vel.getDir(), (startpnt-crashpnt))) < eps.ZERO_ANGLE )
				{
					endvtx = wfv;
					getWfVertexInfo(endvtx).type = CONVEX_MULTI;
				}
			}


			if( endvtx==0 )
			{
				const Vector speed = getWfEdgeSpeedInDir(wfe, startpnt-crashpnt);
				// Add end vertex
				endvtx = addWfVertex( Ray(crashpnt, speed), STEINER_MOVING);
			}
		}
		else if( mc.isCrashedIntoMc() )
		{
			const Rect area = Rect::fromCenter(crashpnt,eps.ZERO,
					eps.ZERO);

			// Search for an already inserted endvertex
			double bestsq=HUGE_DBL;
			set<unsigned> objs = mcendhash.getObjectsAt(area);
			for( set<unsigned>::iterator it = objs.begin();
				it != objs.end(); it++)
			{
				const double dist =  (crashpnt - wf.getVertexPos(*it,0)).lensq();
				if( dist < bestsq )
				{
					endvtx = *it;
					bestsq = dist;
				}
			}


			// There was already a candidate
			if( bestsq <= sq(eps.ZERO) )
			{
				getWfVertexInfo(endvtx).type = STEINER_MULTI;
			}
			else
			{
				endvtx = addWfVertex( Ray(crashpnt, Vector(0,0)), STEINER_RESTING);
				mcendhash.addObjectAt(endvtx, area);
			}
		}
		// Didn't crash --> escaped
		else
		{
			assert( ! mc.isCrashed());
			endvtx = addWfVertex( Ray(crashpnt, Vector(0,0)), STEINER_RESTING);

			// Too close crash point
			if( crashpnt.lensq() <= sq(eps.INFDIST) )
			{
				loggerSk.log(LL_ERROR, "Mc %d never crashed but crash point (%e,%e) is too close.\n",
						mcidx, TO_MDOUBLE(crashpnt.x), TO_MDOUBLE(crashpnt.y) );
			}
		}
		mcendvtx.push_back(endvtx);
	}



	// Save the traces as wf-edges
	vector<unsigned> mctraceedge;
	for(unsigned mcidx=0; mcidx<mcs.size(); mcidx++ )
	{
		const Motorcycle& mc = mcs.at(mcidx);
		const Vector startpnt = mc.getTrace().getStart();

		// Get the end vertex
		const unsigned endvtx = mcendvtx.at(mcidx);


		// Get start vertex of motorcycle
		unsigned startvtx = 0;
		if( mcg.mcStartedAtVertex(mcidx) )
		{
			startvtx = mcg.getStartVertexOfMc(mcidx);
		}
		else
		{
			const Rect area = Rect::fromCenter(startpnt,eps.ZERO, eps.ZERO);

			// Search for an already inserted endvertex
			double bestsq=HUGE_DBL;
			set<unsigned> objs = mcendhash.getObjectsAt(area);

			for( set<unsigned>::iterator it = objs.begin();
					it != objs.end(); it++)
			{
				const double dist =  (startpnt - wf.getVertexPos(*it,0)).lensq();
				if( dist < bestsq )
				{
					startvtx = *it;
					bestsq = dist;
				}
			}


			// No vertex found!? WTF?
			if( startvtx == 0 )
			{
				loggerSk.log(LL_ERROR, "Could not find start vertex of mc %d ending at vertex %d!\n",
						mcidx, endvtx );
				startvtx = addWfVertex( Ray(startpnt, Vector(0,0)), STEINER_MULTI);
			}
			else
			{
				if( bestsq > sq(eps.ZERO) )
				{
#ifndef NDEBUG
					loggerSk.log(LL_WARNING, "Bad start vertex of mc %d ending at vertex %d! pos-diff: %e\n",
							mcidx, endvtx, TO_MDOUBLE(sqrt(bestsq)) );
#endif
				}


				// So this vertex is a vertex belonging to split event of higher degree
				if( getWfVertexInfo(startvtx).type != STEINER_MULTI )
				{
#ifndef NDEBUG
					loggerSk.log(LL_WARNING, "Start vertex %d of mc %d is not a multi steiner!\n",
							endvtx, mcidx);

					assert(false);
#endif
				}
			}
		}




		// Add trace as wf edge
		const unsigned e = wf.addEdge(startvtx, endvtx, eps.ZERO );
		mctraceedge.push_back(e);

		if( e==0 )
		{
			loggerSk.log(LL_ERROR, "Cannot add wf-edge between vertex %d and %d for mc %d.\n",
					startvtx, endvtx, mcidx );
			continue;
		}
	}


	// There must be as many end vertices as motorcycles
	assert( mcendvtx.size() == mcs.size() );
	assert( mctraceedge.size() == mcs.size() );


	// For every mc that escaped a pair <phi, v> of
	// the trace-end vertex v and its polar coordinate phi.
	vector< pair<double, unsigned> > infVertices;
	// Save for corresponding wf-edges by which vertices they are split
	map< unsigned, set<unsigned> > splitPnts;

	// Fill splitPnts and infVertices
	for(unsigned mcidx=0; mcidx<mcs.size(); mcidx++ )
	{
		// Get motorcycle and its end vertex
		const Motorcycle& mc = mcs.at(mcidx);
		const unsigned endvtx = mcendvtx.at(mcidx);
		unsigned wfe = 0;

		if( mc.isCrashedIntoWall() )
			wfe = mcCrashWfe.at(mcidx);
		if( mc.isCrashedIntoMc() )
			wfe = mctraceedge.at(mc.getTargetIdx());

		// If mc crashed --> at appropriate entry in splitPnts
		if( mc.isCrashed() )
		{
			if( wfe==0 )
				continue;

			// Save for the wf edge that it is split by mcidx
			if( splitPnts.count(wfe) == 0 )
				splitPnts[wfe] = set<unsigned>();

			// Add the split point if it does not coincide with one of the
			// endpoints of wfe
			const KineticSLGraph::Edge& ed = wf.getEdge(wfe);
			if( endvtx != ed.s  &&  endvtx != ed.d )
			{
				loggerSk.log(LL_DEBUG, "  the end vertex %d of mc %d belongs to wfe %d.\n", endvtx, mcidx, wfe);
				splitPnts[wfe].insert(endvtx);
			}
		}
		// Didn't crash --> vertex at infinity
		else
		{
			assert(wfe == 0);
			const Vector pos = wf.getVertexPos(endvtx, 0);
			const double phi = atan2(pos.y, pos.x);
			infVertices.push_back( pair<double, unsigned>( phi, endvtx) );
		}
	}


	// Connect the infinite vertices
	assert( infVertices.size() >= 3);
	sort(infVertices.begin(), infVertices.end() );
	for( unsigned i=0; i<infVertices.size(); i++ )
	{
		const unsigned i2 = (i+1)%infVertices.size();
		const unsigned v = infVertices.at(i).second;
		const unsigned v2 = infVertices.at(i2).second;
		wf.addEdge(v, v2, 0);
	}


	// Split the wf-edges accordingly
	for( map< unsigned, set<unsigned> >::iterator it = splitPnts.begin();
			it != splitPnts.end(); it++)
	{
		const unsigned wfe = it->first;
		const set<unsigned>& splits = it->second;

		vector<unsigned> splitsvec;
		splitsvec.insert(splitsvec.begin(), splits.begin(), splits.end());

		// Split wfe by its split-points
		vector<unsigned> newedges = wf.splitEdgeByVertices(wfe, splitsvec, 10*eps.ZERO_EVENT);

		if( wfeorigin.count(wfe) > 0 )
			for( vector<unsigned>::iterator it = newedges.begin(); it != newedges.end(); it++)
				if( *it != 0 )
					cpyWfeorign(*it, wfe);
	}
}


void StraightSkeleton::createWavefront()
{
	// Saves for every directed edge (s,d) of input the wave-front edge (s',d')
	// left to (s,d). Hence, for every pair (s,d) there is also a (diffrent)
	// entry for (d,s)
	map<upair, upair> wfvertices;


	// Fill wfvertices with dummy values
	set<unsigned> eset =input.getEdgeSet();
	for( set<unsigned>::iterator eit = eset.begin(); eit != eset.end(); eit++ )
	{
		KineticSLGraph::Edge ed = input.getEdge(*eit);
		wfvertices[ upair(ed.s,ed.d) ] = upair(0,0);
		wfvertices[ upair(ed.d,ed.s) ] = upair(0,0);
	}


	// For every vertex shoot offset copies and save them in wfvertices
	set<unsigned> vset = input.getVertexSet();
	for( set<unsigned>::iterator vit = vset.begin(); vit != vset.end(); vit++)
	{
		const unsigned vidx = *vit;
		const Vector vpos = input.getVertexPos(vidx, 0);

		// Vertex is isolated --> ignore it
		if( input.isVertexIsolated(vidx) )
			continue;

		// Is Vertex terminal?
		if( input.isVertexTerminal(vidx) )
		{
			//Get oposite vertex on the one incident edge
			const unsigned vother = input.getOppositeVertex( input.getAnIncidentEdge(vidx), vidx);

			const Vector edvec = (vpos - input.getVertexPos(vother)).norm();

			//                 edvec    ---- newccw
			//              <--------| /
			//    o------------------o
			//    vother         vidx \                     .
			//                         ----- newcw

			// Create new two vertices, rotatated by +/- 3pi/4
			const unsigned newccw = addWfVertex( Ray(vpos, edvec + edvec.ccw()), CONVEX);
			const unsigned newcw = addWfVertex( Ray(vpos, edvec + edvec.cw()), CONVEX);

			// Set start resp. dest vertex of the two offset edges incident
			// to vidx accordingly.
			wfvertices[ upair(vidx,vother) ].first = newcw;
			wfvertices[ upair(vother,vidx) ].second = newccw;
			wfvertices[ upair(vidx,vidx) ] = upair(newccw, newcw);
		}
		else
		{
			assert( input.getVertexDegree(vidx) > 1 );

			const unsigned estart = input.getAnIncidentEdge(vidx);
			unsigned eccw, ecw;
			ecw = estart;

			//Run around vertex until we again reach estart
			do
			{
				//Advance to next edge
				eccw = ecw;
				ecw = input.getCWEdge(eccw, vidx);


				//               -------------------
				//              /     alpha         \       .
				//             v                    v
				//
				//     vccw   eccw      vidx      ecw     vcw
				//     o-----------------o-----------------o
				//          <-----|      |    |------->
				//            dccw       |    dcw
				//                       |

				// The oposite vertices
				const unsigned vccw = input.getOppositeVertex(eccw, vidx);
				const unsigned vcw = input.getOppositeVertex(ecw, vidx);

				// Get speed vector of vertex
				const upair ieccw = upair(vccw, vidx);
				const upair iecw = upair(vidx, vcw);
				const Ray rccw = getInputEdgeDir(ieccw);
				const Ray rcw = getInputEdgeDir(iecw);
				const Vector vel = getVertexSpeed(rccw.getDir(), rcw.getDir());

				//Create new vertex
				const unsigned newv = addWfVertex( Ray(vpos, vel), CONVEX);

				// Set start resp. dest vertex of the two offset edges incident
				// to vidx accordingly.
				wfvertices[ upair(vidx,vcw) ].first = newv;
				wfvertices[ upair(vccw,vidx) ].second = newv;

			}while(ecw != estart);
		}
	}


	// Add all edges to wavefront
	for( map<upair, upair>::iterator it = wfvertices.begin();
		it != wfvertices.end(); it++ )
	{
		const upair inputedge = (*it).first;
		const upair e = (*it).second;
		const unsigned wfe = wf.addEdge(e.first, e.second, eps.ZERO);
		wfeorigin[wfe] = inputedge;
		wfedges[inputedge] = wfe;
	}

	// Set types of wf-vertices
	vset = wf.getVertexSet();
	for( set<unsigned>::iterator vit = vset.begin(); vit != vset.end(); vit++)
	{
		const unsigned v = *vit;
		getWfVertexInfo(v).type = isWfVertexReflex(v) ? REFLEX : CONVEX;
	}
}


Vector StraightSkeleton::getWfEdgeSpeed(unsigned e) const
{
	assert( wf.isEdge(e) );
	assert( wfeorigin.count(e)>0 );

	upair ie = wfeorigin.at(e);
	return getInputEdgeDir(ie).getDir();
}


unsigned StraightSkeleton::getWfEdgeByMc(const skmcgraph& mcg, unsigned mcidx) const
{
	const Motorcycle& mc = mcg.getMotorcycles().at(mcidx);
	const Vector crashpnt = mc.getTrace().getEnd();
	const Vector startpnt = mc.getTrace().getStart();

	assert( mc.isCrashedIntoWall() );

	// Get the wall and source/dest vertices of the wall-edge
	unsigned wall = mcg.getEdgeByWall(mc.getTargetIdx());
	unsigned s = input.getEdge(wall).s;
	unsigned d = input.getEdge(wall).d;
	Vector sp = input.getVertexPos(s);
	Vector dp = input.getVertexPos(d);

	// The motorcycle crashes into a vertex
	if( (sp-crashpnt).lensq() < sq(eps.ZERO_EVENT) || (dp-crashpnt).lensq() < sq(eps.ZERO_EVENT) )
	{
		// v is the input vertex where mc crashed into
		const unsigned v = (crashpnt-sp).lensq() < (crashpnt-dp).lensq() ? s : d;
		const Vector pv = input.getVertexPos(v);

		if( input.isVertexTerminal(v) )
		{
			const unsigned u = input.getOppositeVertex(wall,v);
			const Ray dir (pv, input.getVertexSegment(u,v).getDir().norm() );

			// We hit the wavefront emanating at v
			if( dir.projDistance(startpnt) >= fabs(dir.orthoDistance(startpnt)) )
				return wfedges.at(upair(v, v));
			// Right of dir...
			else if( dir.orthoDistance(startpnt) >= 0 )
				return wfedges.at(upair(v, u));
			// Left of dir...
			else
				return wfedges.at(upair(u, v));
		}
		else
		{
			const unsigned eccw = input.getConeCCWEdge(v, startpnt);
			const unsigned ecw = input.getCWEdge(eccw, v);

			if( ecw == eccw )
			{
				s = v;
				d = input.getOppositeVertex(eccw,v);
			}
			else
			{
				const unsigned vccw = input.getOppositeVertex(eccw, v);
				const unsigned vcw = input.getOppositeVertex(ecw, v);

				const Vector dccw = input.getVertexSegment(v, vccw).getDir().norm().cw();
				const Vector dcw = input.getVertexSegment(v, vcw).getDir().norm().ccw();
				const Vector vel = getVertexSpeed(dccw,dcw);

				// cw edge is closer
				if( Vector::det2(startpnt-pv, vel) > 0 )
					return wfedges.at(upair(v, input.getOppositeVertex(ecw,v) ));
				// ccw edge is closer
				else
					return wfedges.at(upair(input.getOppositeVertex(eccw,v), v));
			}
		}
	}

	// If motorcycle started left of edge (s,d) --> return wfedges( (s,d) ).
	// Otherwise the opposite wfedge
	if( Vector::sin3( sp, dp, startpnt ) >= 0 )
		return wfedges.at(upair(s,d) );
	else
		return wfedges.at(upair(d,s) );
}



bool StraightSkeleton::isWfVertexReflex(unsigned v) const
{
	assert( wf.isVertex(v) );

	// The position of the vertex
	const Vector p = wf.getVertexPos(v, 0);
	assert( wf.getVertexDegree(v) == 2);

	//The two incident edges
	const unsigned e1 = wf.getAnIncidentEdge(v);
	const unsigned e2 = wf.getCWEdge(e1, v);
	//Get the opposite vertices
	const unsigned v1 = wf.getOppositeVertex(e1, v);
	const unsigned v2 = wf.getOppositeVertex(e2, v);

	//The two positions
	const Vector p1 = wf.getVertexPos(v1, 0);
	const Vector p2 = wf.getVertexPos(v2, 0);

	//The inner product of velocity and edge directions
	const double d1 = (p1-p)*wf.getVertex(v).vel.getDir();
	const double d2 = (p2-p)*wf.getVertex(v).vel.getDir();


	// The edges have >= 180°.
	return (d1+d2) <= eps.ZERO;
}


void StraightSkeleton::moveWfVertexAlongEdge(unsigned v, unsigned e, double time, double eps)
{
	assert( wf.isVertex(v) );
	assert( wf.isEdge(e) );
	assert( wf.isEdgeIncident(e, v) );

	const unsigned u = wf.getOppositeVertex(e,v);
	const Vector dir = wf.getVertexSegment(v,u,time).getDir().norm();
	wf.getVertex(v).vel = Ray( wf.getVertexPos(v,time)+eps*dir, Vector(0,0) );
}


double StraightSkeleton::getCollapseTime(unsigned e) const
{
	assert( wf.isEdge(e) );

	double time;
	Vector place;
	getCollapseTimePlace(e, time, place);

	return time;
}


StraightSkeleton::event StraightSkeleton::getCollapseEvent(unsigned e) const
{
	double t;
	Vector p;
	getCollapseTimePlace(e, t, p);

	return event(e, t, p);
}


void StraightSkeleton::getCollapseTimePlace(unsigned e, double &time, Vector &place) const
{
	assert( wf.isEdge(e) );
	unsigned u = wf.getEdge(e).s;
	unsigned v = wf.getEdge(e).d;
	wfvtxtype ut = getWfVertexInfo(u).type;
	wfvtxtype vt = getWfVertexInfo(v).type;

	// Sort on vertex type
	if( ut > vt )
	{
		swap(u, v);
		swap(ut, vt);
	}

	const Ray uvel = wf.getVertex(u).vel;
	const Ray vvel = wf.getVertex(v).vel;

	// Both vertices are not moving
	if( (ut==STEINER_RESTING || ut==STEINER_MULTI) &&
		(vt==STEINER_RESTING || vt==STEINER_MULTI) )
	{
		assert( uvel.getDir().lensq() <= sq(eps.ZERO) );
		assert( vvel.getDir().lensq() <= sq(eps.ZERO) );
		time = -HUGE_DBL;
		return;
	}

	// The vertex u is moving
	assert( uvel.getDir().len() >= 1-eps.ZERO );
	// Test whether the vertex v lies on the line of u
	bool vOnULine = fabs(Vector::sin3( uvel.getEnd(), uvel.getStart(), vvel.getStart())) <= eps.ZERO_ANGLE;


	// The vertex v is not moving
	if( vt==STEINER_RESTING || vt==STEINER_MULTI )
	{
		assert( vvel.getDir().lensq() <= sq(eps.ZERO) );
		assert( ut==REFLEX || ut==CONVEX_MULTI || ut==STEINER_MOVING );

#ifndef NDEBUG
		if(!vOnULine)
			loggerSk.log(LL_WARNING, "Vertex %d does not meet %d (non-moving vertex)!?\n", u, v);
#endif

		const unsigned ecw = wf.getCWEdge(e, u);
		const unsigned eccw = wf.getCCWEdge(e, u);
		assert( wfeorigin.count(ecw) > 0 );
		assert( wfeorigin.count(eccw) > 0 );
		const upair icw = wfeorigin.at(ecw);
		const upair iccw = wfeorigin.at(eccw);
		Ray rcw = getInputEdgeDir(icw);
		Ray rccw = getInputEdgeDir(iccw);

		place = vvel.getStart();
		const double tcw = rcw.projDistance(place);
		const double tccw = rccw.projDistance(place);

		if( fabs(tcw-tccw) > eps.ZERO_EVENT &&
				(uvel.getStart()-vvel.getStart()).lensq() < sq(eps.INFDIST)/2 )
		{
			loggerSk.log(LL_WARNING,
					"Inaccurate collapse time for wfe %d: diff=%e (%.3e-%.3e). l:(%d, %d), r:(%d, %d)\n",
					e, TO_MDOUBLE(tcw-tccw), TO_MDOUBLE(tcw), TO_MDOUBLE(tccw),
					icw.first, icw.second, iccw.first, iccw.second);
		}

		// Take the mean of both input edges's ortho distance to v
		time = (tcw+tccw)/2.0;

		return;
	}


	// The vertex u and v is moving
	assert( vvel.getDir().len() >= 1-eps.ZERO );
	assert( uvel.getDir().len() >= 1-eps.ZERO );


	/* Test whether both are moving in parallel directions */
	bool vuparallel = fabs(Vector::sin2( uvel.getDir(), vvel.getDir())) <= eps.ZERO_ANGLE;

	// The vertex u moves in direction of vertex v and both move parallel
	if( !wfeorigin.count(e) && vOnULine && vuparallel )
	{
		assert( (ut==REFLEX && vt==CONVEX_MULTI)  ||
				(ut==REFLEX && vt==STEINER_MOVING)  ||
				(ut==CONVEX_MULTI && vt==STEINER_MOVING) ||
				(ut==CONVEX_MULTI && vt==CONVEX_MULTI) ||
				(ut==STEINER_MOVING && vt==STEINER_MOVING) );

#ifndef NDEBUG
		if( Vector::cos2(vvel.getDir(), uvel.getDir()) >= 1-eps.ZERO_ANGLE )
			loggerSk.log(LL_WARNING, "Vertex %d and %d do not move against each other?\n", u, v);
#endif


		const unsigned eucw = wf.getCWEdge(e, u);
		const unsigned euccw = wf.getCCWEdge(e, u);

		if( wfeorigin.count(eucw) && wfeorigin.count(euccw) )
		{
			const upair iucw = wfeorigin.at(eucw);
			const upair iuccw = wfeorigin.at(euccw);
			const Ray sucw = getInputEdgeDir(iucw);
			const Ray succw = getInputEdgeDir(iuccw);

			const unsigned evcw = wf.getCWEdge(e, v);
			const unsigned evccw = wf.getCCWEdge(e, v);
			assert( wfeorigin.count(evcw) > 0 );
			assert( wfeorigin.count(evccw) > 0 );
			const upair ivcw = wfeorigin.at(evcw);
			const upair ivccw = wfeorigin.at(evccw);
			const Ray svcw = getInputEdgeDir(ivcw);
			const Ray svccw = getInputEdgeDir(ivccw);


			// Get eight solutions and take the better one
			double times[8];
			Vector places[8];
			double quals[8];

			quals[0] = getSolSegRay(svcw, uvel, times[0], places[0]);
			quals[1] = getSolSegRay(svccw, uvel, times[1], places[1]);
			quals[2] = getSolSegRay(sucw, vvel, times[2], places[2]);
			quals[3] = getSolSegRay(succw, vvel, times[3], places[3]);

			quals[4] = getSolSegSegSeg(sucw, succw, svcw, times[4], places[4]);
			quals[5] = getSolSegSegSeg(sucw, succw, svccw, times[5], places[5]);
			quals[6] = getSolSegSegSeg(svcw, svccw, succw, times[6], places[6]);
			quals[7] = getSolSegSegSeg(svcw, svccw, sucw, times[7], places[7]);

			// Get maximum qualsity
			const unsigned max = maxidx(quals, 8);
			time = times[max];
			place = places[max];
		}
		else
		{
			if( !wfeorigin.count(eucw) )
				loggerSk.log(LL_ERROR, "getCollapseTime: missing directed input edge %d.\n", eucw);
			if( !wfeorigin.count(euccw) )
				loggerSk.log(LL_ERROR, "getCollapseTime: missing directed input edge %d.\n", euccw);

			time = (uvel.getStart()-vvel.getStart()).len() /
					(uvel.getDir().len() + vvel.getDir().len());
			place = ((time*uvel).getEnd() + (time*vvel).getEnd())/2.0;
		}
		return;
	}


	// They are moving parallel. Hence, the edge does not collapse
	if( vuparallel && Vector::cos2(uvel.getDir(),vvel.getDir())>0 )
	{
		time = -HUGE_DBL;
		return;
	}


	// This is the default case: 'e' is a wavefront edge that does not belong
	// to the motorcycle graph.
	assert(wfeorigin.count(e));

	const unsigned eucw = wf.getCWEdge(e,u);
	const unsigned euccw = wf.getCCWEdge(e,u);
	const unsigned evcw = wf.getCWEdge(e,v);
	const unsigned evccw = wf.getCCWEdge(e,v);

	const unsigned eu = wfeorigin.count(eucw) ? eucw : euccw;
	const unsigned ev = wfeorigin.count(evcw) ? evcw : evccw;

	// The other wf-edges do not have a corresponding input edge!
	assert( euccw==eucw || !wfeorigin.count(eu==eucw ? euccw : eucw) );
	assert( evccw==evcw || !wfeorigin.count(ev==evcw ? evccw : evcw) );

	double times[4];
	Vector places[4];
	double quals[4] = {0};

	if( wfeorigin.count(e) && wfeorigin.count(eu) && wfeorigin.count(ev) )
	{
		const upair ieu = wfeorigin.at(eu);
		const upair ie = wfeorigin.at(e);
		const upair iev = wfeorigin.at(ev);

		const Ray seu = getInputEdgeDir(ieu);
		const Ray se = getInputEdgeDir(ie);
		const Ray sev = getInputEdgeDir(iev);

		quals[0] = getSolSegSegSeg(seu, se, sev, times[0], places[0]);
		quals[1] = getSolSegRay(sev, uvel, times[1], places[1]);
		quals[2] = getSolSegRay(seu, vvel, times[2], places[2]);
	}
	else
	{
		if( !wfeorigin.count(e) )
			loggerSk.log(LL_ERROR, "getCollapseTime: missing directed input edge %d.\n", e);

		if( !wfeorigin.count(eu) )
			loggerSk.log(LL_ERROR, "getCollapseTime: missing directed input edge %d.\n", eu);

		if( !wfeorigin.count(ev) )
			loggerSk.log(LL_ERROR, "getCollapseTime: missing directed input edge %d.\n", ev);
	}

	quals[3] = getSolRayRay(uvel, vvel, times[3], places[3]);
	const unsigned max = maxidx(quals, 4);
	time = times[max];
	place = places[max];
	const double qual = quals[max];

	if( qual <= 0.0 )
	{
		time = -HUGE_DBL;
		return;
	}

#ifndef NDEBUG
	if( qual <= eps.BADQUALITY && time<eps.INFDIST )
		loggerSk.log(LL_WARNING, "Bad condition for seg-seg-seg collapse of e%d. Qual: %e\n",
				e, TO_MDOUBLE(qual));
#endif
}


double StraightSkeleton::getSolRayRay(const Ray& u, const Ray& v, double& time, Vector& place) const
{

	// it has to hold that	det( u.s + utime*u.dir - v.s, v.dir) = 0
	// 						utime*det(u.dir,v.dir) = det(v.s-u.s,v.dir)
	// 						utime = det(v.s-u.s,v.dir) / det(u.dir,v.dir)
	const double det = Vector::det2(u.getDir(), v.getDir());
	double utime = Vector::det2(v.getStart()-u.getStart(),
							v.getDir()) / det ;
	double vtime = Vector::det2(v.getStart()-u.getStart(),
							u.getDir()) / det ;

	time = (utime+vtime)/2.0;
	place = ((time*u).getEnd() + (time*v).getEnd())/2.0;

	if( det == 0.0 )
		return 0;
	return fabs(det)/(u.getDir().len()*v.getDir().len());
}


double StraightSkeleton::getSolSegRay(const Ray& segnormal, const Ray& ray, double& time, Vector& place) const
{
	// We have to equations
	//    1.  place = ray.start + time * ray.dir
	//    2.  time = (place-seg.start)*seg.normal
	// Plugging 2. into 1. gives the solution for time:
	//    time = ((seg.start-ray.start)*n)/(ray.dir*seg.normal-1)
	// This solution has a numerically instable solution for
	// ray.start*seg.normal=1.
	// However, we always assume that ray.start*seg.normal is negative. Hence,
	// we can rephrase:
	//    time = ((seg.start-ray.start)*seg.normal)/(-fabs(ray.dir*seg.normal)-1)


	const double denom = segnormal.getDir()*ray.getDir()-1.0;

	time = (segnormal.getStart()-ray.getStart())*segnormal.getDir()/denom;
	place = (time*ray).getEnd();

	if( ray.getDir().lensq() == 0.0 )
		return 0;
	return fabs(denom)/ray.getDir().len();
}


double StraightSkeleton::getSolSegSegSeg(const Ray& s1, const Ray& s2, const Ray& s3,
				double& time, Vector& place) const
{
	// We have three equations
	//    1.  (place-s1.start)*s1.normal = t
	//    2.  (place-s2.start)*s2.normal = t
	//    3.  (place-s3.start)*s3.normal = t
	//
	// Solving the linear system for place.x, place.y and t gives a solution
	// which has det(s1.normal, s2.normal, s3.normal) as denominator. If this
	// is zero, we cannot compute a proper solution

	const Vector n1 = s1.getDir();
	const Vector n2 = s2.getDir();
	const Vector n3 = s3.getDir();

	const Vector p1 = s1.getStart();
	const Vector p2 = s2.getStart();
	const Vector p3 = s3.getStart();

	const double det = -Vector::det3(n1, n2, n3);

	const double a = (p1*n1);
	const double b = (p2*n2);
	const double c = (p3*n3);

	place.x = -( (n2.y-n3.y)*a + (-n1.y+n3.y)*b + (n1.y-n2.y)*c );
	place.y = (n2.x-n3.x)*a + (-n1.x+n3.x)*b + (n1.x-n2.x)*c;
	time = Vector::det2(n2,n3)*a + Vector::det2(n3,n1)*b + Vector::det2(n1,n2)*c;

	time /= det;
	place /= det;

	if( time < 0 )
		return -HUGE_DBL;

	return fabs(det);
}


bool StraightSkeleton::getSkfaceVertices( upair directedInputEdge, list<unsigned>& faceVertices ) const
{
#ifndef NDEBUG
	const unsigned infi = directedInputEdge.first;
	const unsigned inse = directedInputEdge.second;

	// The two vertices given by directedInputEdge must be input vertices
	assert( input.isVertex(infi) );
	assert( input.isVertex(inse) );
	// If the vertices are the same --> the vertex must be a terminal
	assert( infi != inse || input.isVertexTerminal(infi) );
	// If the vertices are not the same --> the vertices must be adjacent
	assert( infi == inse || input.isAdjacent(infi, inse) );
#endif

	// There must be an entry in skfaceEndpoints
	assert( skfaceEndpoints.count(directedInputEdge) > 0 );

	// Get the endpoints of the sk face
	const upair skEnds = skfaceEndpoints.at(directedInputEdge);
	const unsigned faceStart = skEnds.first;
	const unsigned faceEnd = skEnds.second;

	// Check whether the vertices belong to the straight skeleton, and they must be terminal
	assert( sk.isVertexTerminal(faceStart) );
	assert( sk.isVertexTerminal(faceEnd) );

	// Get the only incident edge of the start
	const unsigned e = sk.getAnIncidentEdge(faceStart);

	faceVertices.clear();
	return sk.getCCWPath(faceStart, e, faceEnd, faceVertices);
}


void StraightSkeleton::computeOffsetCurve( KineticSLGraph& curves, const double offset) const
{
    try
    {
        curves.clear();
        loggerSk.log(LL_INFO, "Compute offset curves with distance %f.\n", TO_MDOUBLE(offset));
        timings.startTiming("sk-offset");

        // This is the set of all sk arcs
        set<unsigned> todo = sk.getEdgeSet();
        // Save the directed edges already visited
        set<upair> visited;


        // While this structure is not empty, compute offset curves...
        while( !todo.empty() )
        {
            const unsigned eseed = *(todo.begin());
            todo.erase(eseed);
            loggerSk.log(LL_DEBUG, "check sk arc %d\n", eseed);


            unsigned vpre = sk.getEdge(eseed).s;
            unsigned vpost = sk.getEdge(eseed).d;
            Vector ppre = sk.getVertexPos(vpre);
            Vector ppost = sk.getVertexPos(vpost);

            if( getSkarcFaces().count(upair(vpre,vpost)) == 0 ||
                    getSkarcFaces().count(upair(vpost,vpre)) == 0)
            {
                loggerSk.log(LL_ERROR, "At least one face unknown for sk-arc %d!\n", eseed);
                continue;
            }

            upair ie = getSkarcFaces().at( upair(vpre,vpost) );
            Ray iedir = getInputEdgeDir(ie);
            double opre = iedir.projDistance(ppre);
            double opost = iedir.projDistance(ppost);

            // We are upside-down --> turn around about 180°
            if( opre > opost )
            {
                swap(vpre, vpost);
                swap(ppre, ppost);
                swap(opre, opost);
                ie = getSkarcFaces().at( upair(vpre,vpost) );
                iedir = getInputEdgeDir(ie);
            }
            assert( opre <= opost);


            // No offset point on this sk arc
            if( ! (opre+eps.ZERO < offset && offset < opost-eps.ZERO) )
                continue;

            // Compute the offset point
            double rel = (offset-opre)/(opost-opre);
            Vector p = ppre + rel*(ppost-ppre);

            // Remember the seeding vertex and seeding sk arc
            const unsigned vseed = curves.addVertex( Ray(p, Vector(0,0)) );
            assert( vseed != 0 );
            visited.insert(ie);
            loggerSk.log(LL_DEBUG, "  add seeding point %d at sk edge %d with dir input edge (%d, %d).\n",
                    vseed, eseed, ie.first, ie.second);

            const double diff = offset - iedir.projDistance(p);
            if( fabs(diff) > 1e3*eps.ZERO )
                loggerSk.log(LL_WARNING, "Numerical inaccuracy with seeding point %d: %e\n", vseed, TO_MDOUBLE(diff));


            // Run around, until we again reach eseed
            unsigned vlast=vseed;
            unsigned ecurr=eseed;
            while(true)
            {
                // Go to the next sk arc
                vpre = vpost;
                ecurr = sk.getCCWEdge(ecurr, vpre);
                vpost = sk.getOppositeVertex(ecurr, vpre);

                loggerSk.log(LL_DEBUG, "  visit sk edge %d (pre=%d, post=%d).\n", ecurr, vpre, vpost);

                // Add the final and closing edge and break
                if( ecurr == eseed )
                {
                    loggerSk.log(LL_DEBUG, "  close cycle by connecting %d and %d.\n", vlast, vseed);
                    curves.addEdge(vlast, vseed);
                    break;
                }

                // We found a cycle?
                if( visited.count(upair(vpre, vpost)) > 0 )
                {
                    loggerSk.log(LL_WARNING, "Found a cycle in offset traversing at sk arc %d (%d, %d)!\n", ecurr, vpre, vpost);
                    break;
                }

                // remove the arc now
                todo.erase(ecurr);

                if( getSkarcFaces().count(upair(vpre,vpost)) == 0 )
                {
                    loggerSk.log(LL_ERROR, "No face known right of sk-arc %d (%d -> %d)!\n", ecurr, vpre, vpost);
                    break;
                }

                // Get the input edge and propagation direction
                ie = getSkarcFaces().at(upair(vpre,vpost));
                iedir = getInputEdgeDir(ie);

                // Mark this arc as visited
                visited.insert( upair(vpre, vpost) );

                // Compute offset distances
                ppre = sk.getVertexPos(vpre);
                opre = iedir.projDistance(ppre);
                ppost = sk.getVertexPos(vpost);
                opost = iedir.projDistance(ppost);

                // We get a new offset point
                if( opost-eps.ZERO <= offset && offset < opre-eps.ZERO )
                {

                    // Attention: If opost==opre==offset --> p = ppost

                    if( offset <= opost+eps.ZERO )
                    {
                        p = ppost;
                    }
                    else
                    {
                        // Get relation and new offset point
                        rel = (offset-opre)/(opost-opre);
                        p = ppre + rel*(ppost-ppre);
                    }

                    // Add new offset point and edge
                    const unsigned vnew = curves.addVertex( Ray(p, Vector(0,0)) );
                    loggerSk.log(LL_DEBUG, "  connect new %d and last %d.\n", vnew, vlast);
                    curves.addEdge(vlast, vnew);
                    vlast = vnew;

                    const double diff = offset - iedir.projDistance(p);
                    if( fabs(diff) > 1e3*eps.ZERO )
                        loggerSk.log(LL_WARNING, "Numerical inaccuracy with offset vertex %d: %e\n", vnew, TO_MDOUBLE(diff));

                    loggerSk.log(LL_DEBUG, "    place offset vertex %d.\n", vnew);

                    // Reverse traverse direction
                    swap(vpre, vpost);
                }
            }
        }

        timings.stopTiming("sk-offset");
    }
    catch (...)
    {
        throw StraightSkeletonError("StraightSkeleton::computeOffsetCurve()");
    }
}


Ray StraightSkeleton::getInputEdgeDir( upair directedInputEdge) const
{
	unsigned infi = directedInputEdge.first;
	unsigned inse = directedInputEdge.second;

	// The two vertices given by directedInputEdge must be input vertices
	assert( input.isVertex(infi) );
	assert( input.isVertex(inse) );
	// If the vertices are the same --> the vertex must be a terminal
	assert( infi != inse || input.isVertexTerminal(infi) );
	// If the vertices are not the same --> the vertices must be adjacent
	assert( infi == inse || input.isAdjacent(infi, inse) );


	// Different vertices
	if( infi != inse )
	{
		assert( input.isAdjacent(infi, inse) );
		const Vector p1 = input.getVertexPos(infi);
		const Vector p2 = input.getVertexPos(inse);

		return Ray( p1, (p2-p1).norm().ccw() );
	}
	// A terminal vertex
	else
	{
		assert( input.isVertexTerminal(infi) );
		const unsigned inedge = input.getAnIncidentEdge(infi);
		const Vector p = input.getVertexPos(infi);
		const Vector po = input.getVertexPos( input.getOppositeVertex(inedge, infi) );

		return Ray( p, (p-po).norm() );
	}
}


#ifdef WITH_OGL

void StraightSkeleton::redraw(glStalgoWindow & wnd, int timeshift) const
{
	glplane* skLayer = wnd.getSkLayer();
	gltextplane* skTextLayer = wnd.getSkTextLayer();

	skLayer->clear();
	skTextLayer->clear();

	// Get the current simulation time corrected by a given timeshift
	const double time = getLastEventTime() + (timeshift * 1e-2) / getInput().getNoVertices();



	//Plot selected motorcycle
	if( wf.isVertex( wnd.getSelectedSkWfVtxIdx() ) )
	{
		unsigned i = wnd.getSelectedSkWfVtxIdx();
		const Vector pos = wf.getVertexPos(i, time);

		//Draw selection circle
		skLayer->addObject( new glcircle(TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y), 0.02,  glcolor::fromRGBA(1,1,0, 0.25)) );
	}


	// Draw long direction lines
	set<unsigned> vset = wf.getVertexSet();
	for( set<unsigned>::iterator vit = vset.begin();
		vit != vset.end(); vit++)
	{
		const Vector& pos = wf.getVertexPos(*vit, time);
		const Vector& pos3 = wf.getVertexPos(*vit, time+100);
		skLayer->addObject( new glline( TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y), TO_MDOUBLE(pos3.x), TO_MDOUBLE(pos3.y),
					glcolor::fromRGB(0,0.2,0) ));
	}

	// Draw speed vectors
	for( set<unsigned>::iterator vit = vset.begin();
		vit != vset.end(); vit++)
	{
		const Vector& pos = wf.getVertexPos(*vit, time);
		const Vector& pos2 = wf.getVertexPos(*vit, time+1e-3);

		skLayer->addObject( new glline( TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y), TO_MDOUBLE(pos2.x), TO_MDOUBLE(pos2.y),
					glcolor::fromRGBA(0,0.8,0,0.3) ));
	}

	//input PSLG
	glStalgoWindow::addKslgToPlane(*skLayer, *skTextLayer, input, glcolor::fromRGB(1, 1, 1));

	//wf PSLG
	glStalgoWindow::addKslgToPlane(*skLayer, *skTextLayer, getWavefront(),
			glcolor::fromRGB(0.5, 0, 0.5), time);

	//sk
	glStalgoWindow::addKslgToPlane(*skLayer, *skTextLayer, sk, glcolor::fromRGB(0, 0, 1));

	// Draw in-progress lines of sk
	for( set<unsigned>::iterator vit = vset.begin();
		vit != vset.end(); vit++)
	{
		if( getWfVertexInfo(*vit).skstart == 0 )
			continue;

		const unsigned skvtx = getWfVertexInfo(*vit).skstart;

		const Vector& pos = wf.getVertexPos(*vit, time);
		const Vector& pos2 = sk.getVertexPos(skvtx);

		skLayer->addObject( new glline( TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y), TO_MDOUBLE(pos2.x), TO_MDOUBLE(pos2.y),
					glcolor::fromRGBA(0, 0, 1, 0.5) ));
	}


	//Plot selected motorcycle
	if( wf.isVertex( wnd.getSelectedSkWfVtxIdx() ) )
	{
		unsigned i = wnd.getSelectedSkWfVtxIdx();
		const Vector pos = wf.getVertexPos(i, time);

		//Start point
		skLayer->addObject( new glpoint( TO_MDOUBLE(pos.x), TO_MDOUBLE(pos.y), glcolor::fromRGB(1, 1, 0) ) );
	}
}
#endif



StraightSkeleton::epsilons StraightSkeleton::getDefaultEpsilons()
{
	epsilons eps;
	double factor=1;

#ifdef WITH_MPFRBACKEND
	factor = sqrt(mpfr_get_default_prec()/53.0);
#endif

	eps.ZERO = pow(10, -13*factor);
	eps.ZERO_EVENT = pow(10, -10*factor);
	eps.ZERO_ANGLE = pow(10, -7*factor);
//    eps.INFSPEED = pow(10, 6*factor);
	eps.INFSPEED = 1;
	eps.BADQUALITY = pow(10, -20*factor);

	eps.INFDIST = mcgraph::getDefaultEpsilons().MIN_ESCAPED_DIST / C_2;

	return eps;
}


ostream& operator<< (ostream& os, const StraightSkeleton::wfvtxtype& t)
{
	switch(t)
	{
		case StraightSkeleton::CONVEX:
			return os << "convex";
		case StraightSkeleton::CONVEX_MULTI:
			return os << "multi convex";
		case StraightSkeleton::REFLEX:
			return os << "reflex";
		case StraightSkeleton::STEINER_MOVING:
			return os << "moving steiner";
		case StraightSkeleton::STEINER_RESTING:
			return os << "resting steiner";
		case StraightSkeleton::STEINER_MULTI:
			return os << "multi steiner";
	}

	return os;
}





