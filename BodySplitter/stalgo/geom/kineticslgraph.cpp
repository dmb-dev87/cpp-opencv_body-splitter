#include <algorithm>
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <set>
#include <stack>
#include <vector>
#include <stdio.h>

#include "../fpkernel.h"

#include "kineticslgraph.h"

#include "geomhash.h"
#include "../util/util.h"

#include "../constants.h"

using namespace std;


typedef pair<unsigned,unsigned> upair;




KineticSLGraph::KineticSLGraph()
{
	// this guarantees that isVertex(0) and isEdge(0) is always false
	clear();
}


KineticSLGraph::KineticSLGraph(const KineticSLGraph& copy)
{
	for(size_t i=0; i<copy.vertices.size(); ++i)
	{
		Vertex* vtx = 0;
		if( copy.vertices[i] )
		{
			vtx = new Vertex(copy.getVertex(i));
		}
		vertices.push_back(vtx);
	}

	for(size_t i=0; i<copy.edges.size(); ++i)
	{
		Edge* ed = 0;
		if( copy.edges[i] )
		{
			ed = new Edge(copy.getEdge(i));
		}
		edges.push_back(ed);
	}
}

KineticSLGraph::~KineticSLGraph()
{
	clear();
}


unsigned KineticSLGraph::addVertex(const Ray& vel, void* data)
{
	Vertex *v = new Vertex(vel, data);

	vertices.push_back(v);
	return vertices.size()-1;
}


bool KineticSLGraph::isAdjacent(unsigned s, unsigned d) const
{
	// Guarantee that s is not isolated, if d was non-isolated
	if( isVertexIsolated(s) )
		swap(s,d);

	// Both vertices are isolated
	if( isVertexIsolated(s) )
		return false;

    if( s==d )
        return false;

	// Get some incident edge to s
	const unsigned e1 = getVertex(s).inEdge;

	//Run around the vertex s and check if d can be reached
	unsigned e = e1;
	do
	{
		// s must be incident!
		assert( isEdgeIncident(e,s) );

		// Is d also incident to e?
		if( isEdgeIncident(e, d) )
			return true;

		// Goto next edge
		e = getCWEdge(e, s);
	}while( e != e1);


	// Couldn't reach d
	return false;
}


bool KineticSLGraph::isEdgeIncident(unsigned e, unsigned v) const
{
	assert( isEdge(e) );
	assert( isVertex(v) );

	return getEdge(e).s==v || getEdge(e).d==v;
}


unsigned KineticSLGraph::addEdge()
{
	//Insert the edge
	Edge *newe = new Edge;
	unsigned e = edges.size();
	edges.push_back(newe);

	return e;
}


unsigned KineticSLGraph::addEdge(unsigned s, unsigned d, double time, double eps)
{
	assert( isVertex(s) );
	assert( isVertex(d) );

	if( s==d )
		return 0;

	// Vertex v and d are already connected
	if( isAdjacent(s,d) )
		return 0;

	// Create a new edge
	const unsigned idx = addEdge();
	Edge& newe = getEdge(idx);
	newe.s = s;
	newe.d = d;

	//Connect edge e with other edges at start vertex
	connectEdgeEnd(idx, true, time, eps);
	//Connect edge e with other edges at end vertex
	connectEdgeEnd(idx, false, time, eps);

	return idx;
}


vector<unsigned> KineticSLGraph::addEdgeStar(unsigned v, const vector<unsigned>& vert, double time, double eps)
{
	assert( isVertexIsolated(v) );
	assert( vert.size() > 0 );

	// Special case: only one vertex taken
	if( vert.size() == 1 )
	{
		vector<unsigned> newEdges;
		newEdges.push_back( addEdge(v, vert.at(0), time, eps) );
		return newEdges;
	}


	// Position of v
	const Vector vpos = getVertexPos(v, time);

	// Get the positions of vertices in 'vert'
	vector<Vector> vertPos( vert.size() );
	for( unsigned i=0; i<vert.size(); i++)
	{
		const unsigned u = vert.at(i);
		assert( isVertex(u) );
		assert( v!=u );
		Vector upos = getVertexPos(u, time);

		// Vertex v is too close, search for an adjacent vertex more far away
		if( !isVertexIsolated(u) && (vpos-upos).lensq() <= sq(eps) )
		{
			assert( isEdge( getVertex(u).inEdge) );
			list<unsigned> path;
			if( getEscapePath( getVertex(u).inEdge, u, true, time, eps, path ) )
				upos = getVertexPos( path.back(), time );
		}

		vertPos[i] = upos;
	}

	// Create new edges, not yet linked
	vector<unsigned> newEdges( vert.size() );
	for( unsigned i=0; i<vert.size(); i++)
	{
		const unsigned e = addEdge();
		Edge& ed = getEdge(e);
		ed.s = v;
		ed.d = vert.at(i);
		newEdges[i] = e;
	}

	// Sort vert around vpos in counter-clockwise order
	vector< pair<double, unsigned> > phis( vert.size() );
	for( unsigned i=0; i<vert.size(); i++)
	{
		const Vector dir = vertPos.at(i)-vpos;
		phis[i] = pair<double,unsigned>( dir.pseudophi(), i);
	}
	sort(phis.begin(), phis.end() );


	// Link the new edges at vertex v
	for( unsigned i=0; i<vert.size(); i++)
	{
		const unsigned e = newEdges.at(phis.at(i).second);
		const unsigned eccw = newEdges.at(phis.at( (i+1)%vert.size() ).second);
		const unsigned ecw = newEdges.at(phis.at( (i+vert.size()-1)%vert.size() ).second);
		setCCWEdge(e, v, eccw);
		setCWEdge(e, v, ecw);
	}



	// Connecting the very first edge is a bit tricky...
	//   1. we only temporarily move the destination of the second edge away
	//   2. then we set the inedge of v to this second edge
	const unsigned tmpe = newEdges.at(1);
	Edge& ed = getEdge(tmpe);
	ed.dCCW = ed.dCW = tmpe;
	Vertex& tmpv = getVertex(ed.d);
	const Ray tmpvel = tmpv.vel;
	tmpv.vel = Ray( vertPos.at(1), Vector(0,0) );

	//   3. then we relink the first edge at the destination. If getEscapePath searches
	//      at the v, it moves along the second edge prepared above
	getVertex(v).inEdge = tmpe;
	assert( isEdge(getVertex(v).inEdge) );
	connectEdgeEnd(newEdges.at(0), false, time, eps);
	//   4. finally, we move tmpv back
	tmpv.vel = tmpvel;
	ed.dCCW = ed.dCW = 0;


	// The final inEdge of v will be the first edge created
	getVertex(v).inEdge = newEdges.at(0);
	assert( isEdge(getVertex(v).inEdge) );
	// Link the remaining new edges at the other vertices
	for( unsigned i=1; i<vert.size(); i++)
	{
		connectEdgeEnd(newEdges.at(i), false, time, eps);
	}

	return newEdges;
}


unsigned KineticSLGraph::repotEdge(unsigned e, unsigned v, unsigned u, double time, double eps)
{
	assert( isVertex(v) );
	assert( isVertex(u) );
	assert( isEdge(e) );
	assert( isEdgeIncident(e,v) );


	const unsigned v2 = getOppositeVertex(e,v);
	const unsigned e2 = addEdge();

	Edge& ed = getEdge(e);
	Edge& ed2 = getEdge(e2);

	ed2.s = u;
	ed2.d = v2;

	// Connect e2 at vertex v2
	if( isVertexTerminal(v2) )
	{
		ed2.dCCW = e2;
		ed2.dCW = e2;
	}
	else
	{
		ed2.dCCW = getCCWEdge(e,v2);
		ed2.dCW = getCWEdge(e,v2);
		setCWEdge(ed2.dCCW, v2, e2);
		setCCWEdge(ed2.dCW, v2, e2);
		getVertex(v2).inEdge = e2;
	}


	// Remove edge e at vertex v
	const unsigned edCCW = ed.s==v ? ed.sCCW : ed.dCCW;
	const unsigned edCW =  ed.s==v ? ed.sCW : ed.dCW;

	setCWEdge(edCCW, v, getCWEdge(e,v));
	setCCWEdge(edCW, v, getCCWEdge(e,v));

	// Repair edge points from source and destination vertex
	getVertex(v).inEdge = getCCWEdge(e, v);
	// Still the same --> vertex is terminal
	if( getVertex(v).inEdge == e )
		getVertex(v).inEdge = 0;


	// Connect e2 at vertex u
	connectEdgeEnd(e2, true, time, eps);

	deleteEdge(e);

	return e2;
}


unsigned KineticSLGraph::renewEdge(unsigned e)
{
	assert( isEdge(e) );

	const unsigned e2 = addEdge();
	Edge& ed = getEdge(e);
	Edge& ed2 = getEdge(e2);


	// Copy data
	ed2.s = ed.s;
	ed2.d = ed.d;


	// Relink the edge e2
	if( isVertexTerminal(ed.s) )
	{
		ed2.sCCW = e2;
		ed2.sCW = e2;
	}
	else
	{
		ed2.sCCW = ed.sCCW;
		ed2.sCW = ed.sCW;
		setCCWEdge(ed2.sCW, ed2.s, e2);
		setCWEdge(ed2.sCCW, ed2.s, e2);
	}
	if( isVertexTerminal(ed.d) )
	{
		ed2.dCCW = e2;
		ed2.dCW = e2;
	}
	else
	{
		ed2.dCCW = ed.dCCW;
		ed2.dCW = ed.dCW;
		setCCWEdge(ed2.dCW, ed2.d, e2);
		setCWEdge(ed2.dCCW, ed2.d, e2);
	}


	// Update the inedges of the two vertices
	getVertex(ed.s).inEdge = e2;
	getVertex(ed.d).inEdge = e2;

	deleteEdge(e);
	return e2;
}


unsigned KineticSLGraph::mergeEdges(unsigned v, unsigned e1, unsigned e2)
{
	assert( isVertex(v) );
	assert( isEdge(e1) );
	assert( isEdge(e2) );
	assert( isEdgeIncident(e1, v));
	assert( isEdgeIncident(e2, v));
	assert( e1 != e2 );


	const unsigned e = addEdge();
	Edge& ed = getEdge(e);
	Edge& ed1 = getEdge(e1);
	Edge& ed2 = getEdge(e2);

	const unsigned u1 = getOppositeVertex(e1, v);
	const unsigned u2 = getOppositeVertex(e2, v);


#ifndef NDEBUG
	const unsigned olddegv = getVertexDegree(v);
	const unsigned olddegu1 = getVertexDegree(u1);
	const unsigned olddegu2 = getVertexDegree(u2);
#endif

	ed.s = u1;
	ed.d = u2;

	// Connect e at u1
	if( isVertexTerminal(u1) )
	{
		setCWEdge(e, u1, e);
		setCCWEdge(e, u1, e);
	}
	else
	{
		setCWEdge(e, u1, getCWEdge(e1, u1));
		setCCWEdge(e, u1, getCCWEdge(e1, u1));
		setCWEdge( getCCWEdge(e,u1), u1, e);
		setCCWEdge( getCWEdge(e,u1), u1, e);
	}
	// Connect e at u2
	if( isVertexTerminal(u2) )
	{
		setCWEdge(e, u2, e);
		setCCWEdge(e, u2, e);
	}
	else
	{
		setCWEdge(e, u2, getCWEdge(e2, u2));
		setCCWEdge(e, u2, getCCWEdge(e2, u2));
		setCWEdge( getCCWEdge(e,u2), u2, e);
		setCCWEdge( getCWEdge(e,u2), u2, e);
	}

	// Set inEdge pointers
	getVertex(u1).inEdge = e;
	getVertex(u2).inEdge = e;


	// Remove e1 at v
	const unsigned e1ccw = ed1.s==v ? ed1.sCCW : ed1.dCCW;
	const unsigned e1cw = ed1.s==v ? ed1.sCW : ed1.dCW;
	setCWEdge(e1ccw, v, getCWEdge(e1,v));
	setCCWEdge(e1cw, v, getCCWEdge(e1,v));
	// Update inedge of vertex v
	getVertex(v).inEdge = getCCWEdge(e1, v);
	// This would have mean that v had only one incident edge.
	assert( getVertex(v).inEdge != e1 );


	// Remove e2 at v
	const unsigned e2ccw = ed2.s==v ? ed2.sCCW : ed2.dCCW;
	const unsigned e2cw = ed2.s==v ? ed2.sCW : ed2.dCW;
	setCWEdge(e2ccw, v, getCWEdge(e2,v));
	setCCWEdge(e2cw, v, getCCWEdge(e2,v));
	// Update inedge of vertex v
	getVertex(v).inEdge = getCCWEdge(e2, v);
	if( getVertex(v).inEdge == e2 )
		getVertex(v).inEdge = 0;


	deleteEdge(e1);
	deleteEdge(e2);

#ifndef NDEBUG
	assert( olddegv == getVertexDegree(v)+2 );
	assert( olddegu1 == getVertexDegree(u1) );
	assert( olddegu2 == getVertexDegree(u2) );
#endif

	return e;
}


unsigned KineticSLGraph::getConeCCWEdge(unsigned v, const Vector& pos, double time, double eps) const
{
	assert( isVertex(v) );
	assert( !isVertexTerminal(v) );

	//Get positions of the vertices
	const Vector vpos = getVertexPos(v, time);
    const double phi = (pos-vpos).pseudophi();

	// Quality of best solution found so far
	double bestphidiff=HUGE_DBL;

	const unsigned estart = getVertex(v).inEdge;
	unsigned ccw=0;
	unsigned e1 = estart;
    // Look for the one edge whose pseudophi()-phi mod 8 is minimized, which is
    // the next edge in ccw-direction.
	do
	{
		Vector v1pos = getVertexPos( getOppositeVertex(e1, v), time);

        // The vertex v1 is too close to v, search for a vertex more far away
        if( (v1pos-vpos).lensq() <= sq(eps) )
        {
            list<unsigned> path;
            if( getEscapePath(e1, v, true, time, eps, path) )
                v1pos = getVertexPos(path.back(), time);
        }

        const double phi1 = (v1pos - vpos).pseudophi();
        double phidiff = phi1 - phi;
        if( phidiff < 0 )
            phidiff += 8.0;

        // We found a better candidate
        if( phidiff < bestphidiff )
        {
            bestphidiff = phidiff;
            ccw = e1;
        }

		// e1 and e2 are consecutive edges incident to v
#ifndef NDEBUG
		const unsigned e2 = e1;
#endif
		e1 = getCCWEdge(e1, v);

		// s must be incident!
		assert( isEdge(e1) );
		assert( isEdgeIncident(e1,v) );
		assert( e1!=e2 );
	}while( e1 != estart );

	return ccw;
}


void KineticSLGraph::connectEdgeEnd(unsigned e, bool atSource, double time, double eps)
{
	// Get edge and corresponding vertices
	Edge& edge = getEdge(e);
	// Link the edge e at its endpoint v1; v2 is the other endpoint
	unsigned v1 = atSource ? edge.s : edge.d;
	unsigned v2 = atSource ? edge.d : edge.s;
	assert( isVertex(v1) );
	assert( isVertex(v2) );

	// The CW and CCW edge of e at vertex v1
	unsigned cw = 0;
	unsigned ccw = 0;


	// The vertex v was isolated --> we are our next edges in CW and CCW
	// direction
	if( isVertexIsolated(v1) )
	{
		// Link edge to this vertex
		getVertex(v1).inEdge = e;
		cw = ccw = e;
	}
	// There is only one edge incident to v1, also easy to handle
	else if( isVertexTerminal(v1) )
	{
		cw = getVertex(v1).inEdge;
		ccw = getVertex(v1).inEdge;
	}
	// General case: there are at least two edges incident to v1
	else
	{
		//Get positions of the vertices
		const Vector v1pos = getVertexPos(v1, time);
		Vector v2pos = getVertexPos(v2, time);

		// The vertex v2 is too close to v1, search for a vertex more far away
		if( (v1pos-v2pos).lensq() <= sq(eps) )
		{
			list<unsigned> path;
			if( !isVertexIsolated(v2) && getEscapePath(getVertex(v2).inEdge, v2, true, time, eps, path) )
				v2pos = getVertexPos(path.back(), time);
		}

		ccw = getConeCCWEdge(v1, v2pos, time, eps);
		cw = getCWEdge(ccw,v1);
	}


	// Set CW and CCW edges of the new edge e
	assert( cw!=0 && ccw!=0 );
	setCWEdge(e, v1, cw);
	setCCWEdge(e, v1, ccw);

	//Let the old edges know that I am here now!
	setCCWEdge(cw, v1, e);
	setCWEdge(ccw, v1, e);
}


unsigned KineticSLGraph::getAnIncidentEdge(unsigned v) const
{
	assert( ! isVertexIsolated(v) );
	return getVertex(v).inEdge;
}


vector<unsigned> KineticSLGraph::getIncidentEdges(unsigned v, bool cw) const
{
	vector<unsigned> res;

	//No incident edge
	if( isVertexIsolated(v) )
		return res;

	// Get some incident edge to v
	const unsigned e1 = getVertex(v).inEdge;

	//Run around the vertex v
	unsigned e2 = e1;
	do
	{
		assert( isEdgeIncident(e2,v) );
		//Add this edge
		res.push_back(e2);
		//Got to next
		e2 = cw ? getCWEdge(e2, v) : getCCWEdge(e2, v);
	}while( e2 != e1);

	return res;
}


vector<unsigned> KineticSLGraph::getAdjacentVertices(unsigned v) const
{
	vector<unsigned> res;

	//No incident edge
	if( isVertexIsolated(v) )
		return res;

	// Get some incident edge to v
	const unsigned e1 = getVertex(v).inEdge;

	//Run around the vertex v
	unsigned e2 = e1;
	do
	{
		assert( isEdgeIncident(e2,v) );
		//Add this edge
		res.push_back( getOppositeVertex(e2,v) );
		//Got to next
		e2 = getCWEdge(e2, v);
	}while( e2 != e1);

	return res;
}


unsigned KineticSLGraph::getVertexDegree(unsigned v) const
{
	unsigned cnt=0;

	if( isVertexIsolated(v) )
		return 0;

	// Get some incident edge to v
	const unsigned e1 = getVertex(v).inEdge;

	//Run around the vertex v
	unsigned e2 = e1;
	do
	{
		assert( isEdgeIncident(e2,v) );
		//Advance counter
		cnt ++;
		//Got to next
		e2 = getCWEdge(e2, v);
	}while( e2 != e1);

	return cnt;
}


bool KineticSLGraph::isVertexIsolated(unsigned i) const
{
	assert( isVertex(i) );
	return getVertex(i).inEdge==0;
}


bool KineticSLGraph::isVertexTerminal(unsigned v) const
{
	assert( isVertex(v) );

	if( isVertexIsolated(v) )
		return false;

	const unsigned e = getVertex(v).inEdge;
	assert( isEdge(e) );

	// Does not matter if we take CW or CCW
	assert( (getCCWEdge(e, v) == e) == (getCWEdge(e, v) == e) );
	return getCCWEdge(e, v) == e;
}


void KineticSLGraph::setCCWEdge(unsigned e, unsigned v, unsigned newe)
{
	assert(isEdge(e));
	assert(isVertex(v));

	if( getEdge(e).s == v)
		getEdge(e).sCCW = newe;
	else if( getEdge(e).d == v)
		getEdge(e).dCCW = newe;
	else
		assert(false);
}

void KineticSLGraph::setCWEdge(unsigned e, unsigned v, unsigned newe)
{
	assert(isEdge(e));
	assert(isVertex(v));

	if( getEdge(e).s == v)
		getEdge(e).sCW = newe;
	else if( getEdge(e).d == v)
		getEdge(e).dCW = newe;
	else
		assert(false);
}


void KineticSLGraph::removeEdge(unsigned e)
{
	assert(isEdge(e));

	const Edge& ed = getEdge(e);


	// Repair linkage at source vertex of e
	//              --------  ed.sCW
	//     ed  ed.s/
	//   ---------0---------
	//            '
	//             --------- ed.sCCW
	setCWEdge(ed.sCCW, ed.s, getCWEdge(e,ed.s));
	setCCWEdge(ed.sCW, ed.s, getCCWEdge(e,ed.s));
	// Same for destination
	setCWEdge(ed.dCCW, ed.d, getCWEdge(e,ed.d));
	setCCWEdge(ed.dCW, ed.d, getCCWEdge(e,ed.d));


	// Repair edge points from source and destination vertex
	getVertex(ed.s).inEdge = getCCWEdge(e, ed.s);
	getVertex(ed.d).inEdge = getCCWEdge(e, ed.d);

	// If they are still the same --> they were terminal vertices
	if( getVertex(ed.s).inEdge == e )
		getVertex(ed.s).inEdge = 0;
	if( getVertex(ed.d).inEdge == e )
		getVertex(ed.d).inEdge = 0;


	// Actually delete the edge
	deleteEdge(e);
}


void KineticSLGraph::deleteEdge(unsigned e)
{
	assert(isEdge(e));
	delete edges[e];
	edges[e] = 0;
}


unsigned KineticSLGraph::collapseEdge(unsigned e, const Ray& vel, void* data)
{
	const Edge& ed = getEdge(e);
	unsigned oldes = ed.s;
	unsigned olded = ed.d;


	// If there is a terminal vertex, let olded be terminal
	if( isVertexTerminal(oldes) )
		swap(oldes, olded);

	// We guarantee that the source vertex will survive
	// Set properties of new vertex
	getVertex(oldes).vel = vel;
	getVertex(oldes).data = data;

	// At least one vertex, olded, is termnal
	if( isVertexTerminal(olded) )
	{
		removeEdge(e);
		removeVertex(olded);
		return oldes;
	}

	// Both vertices are not terminal
	assert( !isVertexTerminal(oldes) );
	assert( !isVertexTerminal(olded) );


	// General case: source and destination are non-terminal

	assert(ed.sCCW != e);
	assert(ed.sCW != e);
	assert(ed.dCCW != e);
	assert(ed.dCW != e);


	// Let inedges of e's destination point to oldes
	vector<unsigned> inedges = getIncidentEdges(olded);
	for( vector<unsigned>::iterator it = inedges.begin(); it != inedges.end(); ++it )
	{
		if( *it != e )
		{
			Edge& ed = getEdge(*it);
			if( ed.s == olded )
				ed.s = oldes;
			else
			{
				assert( ed.d == olded );
				ed.d = oldes;
			}
		}
	}


	// Relink sCCW, sCW, dCCW and dCW
	//
	//
	//  ed.sCCW -------+                  +----- ed.dCW
	//                  \  ed.s    ed.d  /
	//          ---------o--------------0
	//                   '              '
	//  ed.sCW  ---------+              +------- ed.dCCW

	// Top side
	setCWEdge(ed.sCCW, oldes, ed.dCW);
	setCCWEdge(ed.dCW, oldes, ed.sCCW);
	// Bottom side
	setCWEdge(ed.dCCW, oldes, ed.sCW);
	setCCWEdge(ed.sCW, oldes, ed.dCCW);


	//Set an edge pointer of new vertex oldes different to e
	getVertex(oldes).inEdge = ed.sCCW;
	assert( getVertex(oldes).inEdge != e );

	// olded has no incident edges any more...
	getVertex(olded).inEdge = 0;

	// Clean stale destination vertex and edge e
	deleteEdge(e);
	removeVertex(olded);

	return oldes;
}

pair<unsigned,unsigned> KineticSLGraph::splitEdge(unsigned e, unsigned u, double time, double eps)
{
	assert( isEdge(e) );
	assert( isVertex(u) );
	assert( !isEdgeIncident(e, u) );

	const unsigned e1 = addEdge();
	const unsigned e2 = addEdge();

	const Edge& ed = getEdge(e);
	Edge& ed1 = getEdge(e1);
	Edge& ed2 = getEdge(e2);

	assert( !isAdjacent(ed.s, u));
	assert( !isAdjacent(ed.d, u));

#ifndef NDEBUG
	const unsigned olddegu = getVertexDegree(u);
	const unsigned olddegs = getVertexDegree(ed.s);
	const unsigned olddegd = getVertexDegree(ed.d);
#endif


	//  ed.sCCW -------+                  +----- ed.dCW
	//                  \ ed.s      ed.d /
	//          ---------o--------------0
	//                   '              '
	//  ed.sCW  ---------+              +------- ed.dCCW
	//
	//
	//  ed1.sCCW --------+        e1.s         +----- ed2.dCW
	//                    \ e1.d  e2.s   e2.d /
	//           ---------o-------o---------0
	//                    '       v         '
	//  ed1.sCW  ---------+                 +------- ed2.dCCW



	ed1.s = u;
	ed2.s = u;
	ed1.d = ed.s;
	ed2.d = ed.d;


	// Connect e1 at vertex ed.s
	if( isVertexTerminal(ed.s) )
	{
		ed1.dCCW = e1;
		ed1.dCW = e1;
	}
	else
	{
		ed1.dCCW = getCCWEdge(e,ed.s);
		ed1.dCW = getCWEdge(e,ed.s);
		setCWEdge(ed1.dCCW, ed.s, e1);
		setCCWEdge(ed1.dCW, ed.s, e1);
		getVertex(ed.s).inEdge = e1;
	}


	// Connect e2 at vertex ed.d
	if( isVertexTerminal(ed.d) )
	{
		ed2.dCCW = e2;
		ed2.dCW = e2;
	}
	else
	{
		ed2.dCCW = getCCWEdge(e,ed.d);
		ed2.dCW = getCWEdge(e,ed.d);
		setCWEdge(ed2.dCCW, ed.d, e2);
		setCCWEdge(ed2.dCW, ed.d, e2);
		getVertex(ed.d).inEdge = e2;
	}

	// Repair edge points from source and destination vertex
	getVertex(ed.s).inEdge = e1;
	getVertex(ed.d).inEdge = e2;

	// Connect e1 and e2 at vertex u
	connectEdgeEnd(e1, true, time, eps);
	connectEdgeEnd(e2, true, time, eps);

#ifndef NDEBUG
	assert( olddegu+2 == getVertexDegree(u) );
	assert( olddegs == getVertexDegree(ed.s) );
	assert( olddegd == getVertexDegree(ed.d) );
#endif

	deleteEdge(e);

	return pair<unsigned,unsigned>(e1,e2);
}


vector<unsigned> KineticSLGraph::splitEdgeByVertices(unsigned e,
			const vector<unsigned>& splits, double time, double eps)
{
#ifndef NDEBUG

	// Check validity of input
	assert( isEdge(e) );
	for( unsigned i=0; i<splits.size(); i++)
	{
		assert( isVertex(splits.at(i)) );
		assert( !isAdjacent(splits.at(i), getEdge(e).s));
		assert( !isAdjacent(splits.at(i), getEdge(e).d));
	}

	for( unsigned i=0; i<splits.size(); i++)
		for( unsigned j=i+1; j<splits.size(); j++)
			assert( !isAdjacent(splits.at(i), splits.at(j)));

	// Check for unique entries
	set<unsigned> _splits;
	_splits.insert(splits.begin(), splits.end());
	assert(_splits.size() == splits.size());
#endif


	// List of new edges
	vector<unsigned> newe;

	// Guarantee that there is at least one split point
	if( splits.size() == 0 )
		return newe;
	newe.reserve(splits.size()+1);

	// This will be the sorted splits
	vector< pair<double, unsigned> > srtsplits;

	// Get segment of e and ray from e's start to e's end
	const Segment eseg = getEdgeSegment(e, time);
	const Ray eray( eseg.getStart(), eseg.getDir() );

	// Fill array with split points and projection distance to eseg.getStart()
	for(unsigned i=0; i<splits.size(); i++)
	{
		const unsigned vtx = splits.at(i);
		const Vector vpos = getVertexPos(vtx, time);
		srtsplits.push_back( pair<double,unsigned>(eray.projDistance(vpos), vtx) );
	}


	//Sort the array according to projection distance
	sort(srtsplits.begin(), srtsplits.end());

#ifndef NDEBUG
	const unsigned edd = getEdge(e).d;
#endif

	// Split the edge e successively
	unsigned splite = e;
	for(unsigned i=0; i<srtsplits.size(); i++)
	{
		upair nedges = splitEdge(splite, srtsplits.at(i).second, time, eps );
		splite = nedges.second;
		newe.push_back( nedges.first );

#ifndef NDEBUG
		assert( isEdge(nedges.first) );
		assert( isEdge(nedges.second) );
		assert( isEdgeIncident(nedges.second, edd) );
#endif
	}
	newe.push_back( splite );


	return newe;
}


void KineticSLGraph::removeVertex(unsigned v)
{
	assert(isVertex(v));

	// Remove all incident edges
	vector<unsigned> inedges = getIncidentEdges(v);
	vector<unsigned>::iterator it = inedges.begin();
	while(it != inedges.end() )
	{
		removeEdge(*it);
		it++;
	}

	// Remove vertex itself
	delete vertices[v];
	vertices[v] = 0;
}


set<unsigned> KineticSLGraph::getVertexSet() const
{
	set<unsigned> vset;

	for(unsigned i=0; i<vertices.size(); i++)
		if( isVertex(i) )
			vset.insert(i);

	return vset;
}


set<unsigned> KineticSLGraph::getEdgeSet() const
{
	set<unsigned> eset;

	for(unsigned i=0; i<edges.size(); i++)
		if( isEdge(i) )
			eset.insert(i);

	return eset;
}


unsigned KineticSLGraph::getNoVertices() const
{
	unsigned cnt = 0;

	for(unsigned i=0; i<vertices.size(); i++)
		if( isVertex(i) )
			cnt++;

	return cnt;
}


unsigned KineticSLGraph::getNoEdges() const
{
	unsigned cnt = 0;

	for(unsigned i=0; i<edges.size(); i++)
		if( isEdge(i) )
			cnt++;

	return cnt;
}


Segment KineticSLGraph::getEdgeSegment(unsigned e, double time) const
{
	assert( isEdge(e) );
	const Edge& ed = getEdge(e);
	return getVertexSegment( ed.s, ed.d, time);
}


Segment KineticSLGraph::getVertexSegment(unsigned v1, unsigned v2, double time) const
{
	assert( isVertex(v1) );
	assert( isVertex(v2) );
	return Segment( getVertexPos(v1,time), getVertexPos(v2,time) );
}


unsigned KineticSLGraph::getCommonVertex(unsigned e1, unsigned e2) const
{
	assert( isEdge(e1));
	assert( isEdge(e2));

	const Edge& ed = getEdge(e1);

	if( isEdgeIncident(e2, ed.s) )
		return ed.s;
	else if( isEdgeIncident(e2, ed.d) )
		return ed.d;

	assert(false);
	return 0;
}


unsigned KineticSLGraph::getCommonEdge(unsigned s, unsigned d) const
{
	vector<unsigned> inEdges = getIncidentEdges(s);

	for(unsigned i=0; i<inEdges.size(); i++)
	{
		const unsigned e = inEdges.at(i);
		if( isEdgeIncident(e, d) )
			return e;
	}

	assert(false);
	return 0;
}


Rect KineticSLGraph::getBoundingBox(double time) const
{
	unsigned first = 0;
	for(; first<vertices.size(); ++first)
		if( isVertex(first) )
			break;

	// There are no vertices
	if( first >= vertices.size() )
		return Rect(HUGE_DBL,-HUGE_DBL,HUGE_DBL,-HUGE_DBL);

	assert( isVertex(first) );
	double l, r, t, b;
	l = r = getVertexPos(first, time).x;
	b = t = getVertexPos(first, time).y;

	for( unsigned i=first+1; i<vertices.size(); i++)
	{
		const Vector pos = getVertexPos( i, time);

		r = max(r, pos.x);
		l = min(l, pos.x);
		t = max(t, pos.y);
		b = min(b, pos.y);
	}

	return Rect(l,r,b,t);
}


bool KineticSLGraph::isTopologicalWood() const
{
	bool cycleFree;
	unsigned cntComponents;
	getTopologicalInfo(cycleFree, cntComponents);

	return cycleFree;
}


void KineticSLGraph::getTopologicalInfo(bool& cycleFree, unsigned& cntComponents) const
{
	cntComponents=0;
	cycleFree=true;

	// Standard algorithm: put all vertices in notVisited, start from one point
	// and flood the graph. A vertex must not be visited twice. That is, a
	// vertex visited must be in notVisited, is then removed.

	set<unsigned> notVisited = getVertexSet();
	stack<upair> todo;

	while( ! notVisited.empty() )
	{
		//Stack is empty, a whole component has been flooded
		if( todo.empty() )
		{
			cntComponents++;

			// Get a vertex from notVisited
			set<unsigned>::iterator seedit = notVisited.begin();
			const unsigned seed = *seedit;
			notVisited.erase(seedit);
			assert( isVertex(seed) );

			// The vertex was isolated, start all over again
			if( isVertexIsolated(seed) )
				continue;

			// Push all outgoing edges to the todo
			vector<unsigned> inedges = getIncidentEdges(seed);
			for( vector<unsigned>::iterator it = inedges.begin();
					it != inedges.end(); it++ )
			{
				// Get an adjacent vertex
				const unsigned e = *it;
				const unsigned next = getOppositeVertex(e, seed);

				// Since this is an unexplored component, next cannot be known yet
				assert( notVisited.count(next) == 1 );
				// remember this edge
				todo.push( upair(seed,next) );
			}
		}

		// Stack can not be empty here
		assert( !todo.empty() );

		// Get next edge (vertex pair) we walk along
		const unsigned prev = todo.top().first;
		const unsigned next = todo.top().second;
		todo.pop();

		// We have been here already? -- then we found a cycle. Do not follow
		// this dark path of the force, young Jedi, or we'll never break out!
		if( notVisited.count(next) == 0 )
		{
			cycleFree = false;
			continue;
		}


#ifndef NDEBUG
		// We visit next the first time
		const unsigned cntErased = notVisited.erase(next);
		assert( cntErased == 1);
#endif

		// Push all outgoing edges to the todo
		vector<unsigned> inedges = getIncidentEdges(next);
		for( vector<unsigned>::iterator it = inedges.begin();
				it != inedges.end(); it++ )
		{
			// Get an adjacent vertex
			const unsigned e = *it;
			const unsigned n2 = getOppositeVertex(e, next);

			// Do not look back on your previous path, young Jedi, go forward!
			if( n2==prev )
				continue;

			// remember this edge
			todo.push( upair(next,n2) );
		}
	}
}


bool KineticSLGraph::getCCWPath(unsigned s, unsigned e, unsigned d, list<unsigned>& path) const
{
	assert( isEdgeIncident(e, s) );
	path.clear();

	// Set of already visited path
	set<unsigned> visited;

	// Start with vertex s
	path.push_back(s);
	visited.insert(s);

	// The start is the goal
	if( d == s )
		return true;

	// This is the current vertex in question
	unsigned curr = getOppositeVertex(e, s);

	while( true )
	{
		// Found a cycle, there is no such path
		if( visited.count(curr) > 0 )
			return false;

		// Visit this vertex
		path.push_back(curr);
		visited.insert(curr);

		// We did it!
		if( curr == d )
			return true;

		// Take the next CCW edge
		const unsigned nexte = getCCWEdge(e, curr);
		const unsigned nextv = getOppositeVertex(nexte, curr);
		e = nexte;
		curr = nextv;
	}

	assert( false );
	return true;
}


bool KineticSLGraph::getEscapePath(unsigned e, unsigned v, bool ccw, double
		time, double eps, list<unsigned>& path) const
{
	assert( eps >= 0 );
	assert( isEdgeIncident(e, v) );
	path.clear();

	// Set of already visited path
	set<unsigned> visited;

	// Start with vertex s
	path.push_back(v);
	visited.insert(v);
	const Vector vpos = getVertexPos(v, time);

	// This is the current vertex in question
	unsigned curr = getOppositeVertex(e, v);

	while( true )
	{
		// Found a cycle, there is no such path
		if( visited.count(curr) > 0 )
			return false;

		// Visit this vertex
		path.push_back(curr);
		visited.insert(curr);
		const Vector vcurr = getVertexPos(curr, time);

		// We did it!
		if( (vpos-vcurr).lensq() >= sq(eps) )
			return true;

		// Take the next CCW edge
		const unsigned nexte = ccw ? getCCWEdge(e, curr) : getCWEdge(e, curr);
		const unsigned nextv = getOppositeVertex(nexte, curr);
		e = nexte;
		curr = nextv;
	}

	assert( false );
	return true;
}


ostream& operator<< (ostream& os, const KineticSLGraph::Vertex& v)
{
	return os << "[ vel=" <<  v.vel << ", inedge=" << v.inEdge << ", data=" << v.data << "]";
}

ostream& operator<< (ostream& os, const KineticSLGraph::Edge& e)
{
	return os << "[ s=" <<  e.s << ", d=" << e.d << ", sCCW=" << e.sCCW <<
			", sCW=" << e.sCW << ", dCCW=" << e.dCCW << ", dCW=" << e.dCW << "]";
}

ostream& operator<< (ostream& os, const KineticSLGraph& kslg)
{
	os << kslg.getNoVertices() << " vertices:" << endl;
	for(unsigned i=0; i<kslg.getNoVertices(); i++)
		if( kslg.isVertex(i) )
			os << "  v" << i << ": \t" << kslg.getVertex(i) << endl;

	os << kslg.getNoEdges() << " edges:" << endl;
	for(unsigned i=0; i<kslg.getNoEdges(); i++)
		if( kslg.isEdge(i) )
			os << "  v" << i << ": \t" << kslg.getEdge(i) << endl;

	return os;
}





/** Pseudo-inner function of createFromPolychains:
 * If a vertex exists in kslg at position pos (with distance <= eps), then
 * return the one which is closest. Otherwise, return a new vertex.*/
static
unsigned addVertexToKslg(KineticSLGraph& kslg, GeomHash<unsigned>& hash, const Vector& pos, double eps)
{
	unsigned res = 0;
	bool makenew=true;
	const Rect area = Rect::fromCenter(pos, eps, eps);


	//Check if there is already an appropriate graph vertex
	set<unsigned> objs = hash.getObjectsAt(area);
	set<unsigned>::iterator it = objs.begin();
	double best = eps;
	while(it != objs.end() )
	{
		//Get distance to new vertex
		const double dist = ( kslg.getVertexPos(*it, 0.0) - pos ).len();

		//Found better candidate
		if( dist <= best )
		{
			best = dist;
			res = *it;
			makenew = false;
		}
		it++;
	}


	//There was no new vertes --> create a new vertex
	if( makenew )
	{
		res = kslg.addVertex( Ray(pos, Vector(0,0)), 0 );
		hash.addObjectAt(res, area);
	}
	else
		assert(res != 0);

	return res;
}


KineticSLGraph* KineticSLGraph::createFromPolychains(const PolychainVector& pchains, double eps)
{
	KineticSLGraph* kslg = new KineticSLGraph();
	GeomHash<unsigned> hash(pchains.getBoundingBox(), pchains.getNoVertices()+1 );


	// Add all vertices and all segments to kslg
	for(unsigned cidx=0; cidx<pchains.size(); cidx++)
	{
		const Polychain& ch = pchains.at(cidx);

		//Connect the vertices
		for(unsigned sidx=0; sidx<ch.getNoSegs(); sidx++)
		{
			//Get the appropriate segment
			const Segment& seg = ch.getSegment(sidx);

			//Add start and end vertex
			const unsigned sv = addVertexToKslg(*kslg, hash, seg.getStart(), eps);
			const unsigned ev = addVertexToKslg(*kslg, hash, seg.getEnd(), eps);

			assert( sv != 0);
			assert( ev != 0);

			//Add the corresponding edge
			kslg->addEdge(sv, ev);
		}
	}


	return kslg;
}

void KineticSLGraph::clear()
{
	for( vertex_cont::iterator it = vertices.begin(); it!=vertices.end(); ++it)
		if( *it != 0 )
			delete *it;

	for( edge_cont::iterator it = edges.begin(); it!=edges.end(); ++it)
		if( *it != 0 )
			delete *it;

	vertices.clear();
	edges.clear();

	vertices.push_back(0);
	edges.push_back(0);
}


