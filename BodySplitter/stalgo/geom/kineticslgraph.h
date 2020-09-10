#ifndef kineticslgraph_h
#define kineticslgraph_h


#ifndef DOUBLE_OVERRIDE
#include <assert.h>

#include <map>
#include <set>
#include <vector>
#include <list>
#include <stdexcept>
#endif

#include "ray.h"
#include "segment.h"
#include "polychainvector.h"



/**
 * A straight-line graph consisting of vertices and edges, where vertices are
 * points in the plane and edges are straight-line segments connecting the
 * vertices. We implement a doubly-connected edge list here.
 */
class KineticSLGraph
{

public:
	struct Vertex;
	struct Edge;
	typedef std::vector<Vertex*> vertex_cont;
	typedef std::vector<Edge*> edge_cont;
	
public:

	/** List of vertices */
	vertex_cont vertices;
	/** List of edges */
	edge_cont edges;


public:

	KineticSLGraph();
	KineticSLGraph(const KineticSLGraph& copy);
	virtual ~KineticSLGraph();


	/** Create a kinetic straight-line graph from given polygonal chains. Vertices
	 * with distance <= eps are identified. */
	static KineticSLGraph* createFromPolychains(const PolychainVector& pchains, double eps);


	/** Add a new vertex with specific velocity and auxiliary data. Returns the
	 * index of the vertex. */
	unsigned addVertex(const Ray& vel, void* data=0);
	/** Add a new vertex with zero-speed at specific position and with given
	 * data. Returns the index of the vertex. */
	unsigned addVertex(const Vector& pos, void* data=0)	{ return addVertex(Ray(pos,Vector(0,0)),data); }
	/** Add an edge from vertex s to vertex d at some time. Returns the index
	 * of the new edge. Return 0 if edge already existed. 
	 * If the adjacent vertices of s are closer than eps then we successively
	 * consider adjacent vertices which are more far away to decide cyclic order. */
	unsigned addEdge(unsigned s, unsigned d, double time=0, double eps=0);


	/** Get set of vertices */
	std::set<unsigned> getVertexSet() const;
	/** Get set of edges */
	std::set<unsigned> getEdgeSet() const;

	/** Get number of vertices */
	unsigned getNoVertices() const;
	/** Get number of edges */
	unsigned getNoEdges() const;

	/** Clear the graph */
	void clear();


	/** Get vertex of index i */
	Vertex& getVertex(unsigned i)
    {
        assert(isVertex(i));
        if (!isVertex(i))
            throw std::out_of_range("KineticSLGraph::isVertex()");
        return *vertices[i];
    }
	/** Get vertex of index i */
	const Vertex& getVertex(unsigned i) const
    {
        assert(isVertex(i));
        if (!isVertex(i))
            throw std::out_of_range("KineticSLGraph::isVertex()");
        return *vertices[i];
    }
	/** Get edge of index i */
	Edge& getEdge(unsigned i)
    {
        assert(isEdge(i));
        if (!isEdge(i))
            throw std::out_of_range("KineticSLGraph::isEdge()");
        return *edges[i];
    }
	/** Get edge of index i */
	const Edge& getEdge(unsigned i) const
    {
        assert(isEdge(i));
        if (!isEdge(i))
            throw std::out_of_range("KineticSLGraph::isEdge()");
        return *edges[i];
    }


	/** Get position of vertex at specific time */
	Vector getVertexPos(unsigned i, double time=0) const
		{ return (time*getVertex(i).vel).getEnd(); };
	/** Get segment of edge at given time  */
	Segment getEdgeSegment(unsigned e, double time=0) const;
	/** Get the segment between two vertices at given time */
	Segment getVertexSegment(unsigned v1, unsigned v2, double time=0) const;


	/** Is vertex i an isolated vertex? */
	bool isVertexIsolated(unsigned i) const;
	/** Is vertex i an terminal vertex, e.g. has degree 1? */
	bool isVertexTerminal(unsigned v) const;
	/** Get degree of vertex */
	unsigned getVertexDegree(unsigned v) const;
	/** Get all incident edges at vertex v */
	std::vector<unsigned> getIncidentEdges(unsigned v, bool cw=true) const;
	/** Get an incident edge of vertex v */
	unsigned getAnIncidentEdge(unsigned v) const;
	/** Get all adjacent vertices to v */
	std::vector<unsigned> getAdjacentVertices(unsigned v) const;
	/** We seek the cone bounded by incident edges of v, where the point pos is
	 * contained in. Return the ccw edge of the cone. */
	unsigned getConeCCWEdge(unsigned v, const Vector& pos, double time=0, double eps=0) const;


	/** Remove some edge e */
	void removeEdge(unsigned e);
	/** Remove the vertex v and all incident edges */
	void removeVertex(unsigned v);

	/** Collapse an edge. Returns the index of the resulting vertex. */
	unsigned collapseEdge(unsigned e, const Ray& vel, void* data=0);
	/** Split an edge by an isolated vertex v. The old edge is removed and
	 * replaced by two new edges, which are returned. */
	std::pair<unsigned,unsigned> splitEdge(unsigned e, unsigned v, double time=0, double eps=0);
	/** Split edge 'e' by vertices in 'splits' for time 'time'. Removes edge e
	 * and returns the list of newly inserted edges. */
	std::vector<unsigned> splitEdgeByVertices(unsigned e, const std::vector<unsigned>&
			splits, double time=0, double eps=0);
	/** Connect vertex v with given vertices in 'vert' at some time. If two
	 * vertices, which are connected, are closer than eps then we successively
	 * consider neighboring vertices which are more far away.
	 * Requires: v is an isolated vertex
	 * Returns the created edges in the order as 'vert'. */
	std::vector<unsigned> addEdgeStar(unsigned v, const std::vector<unsigned>& vert,
			double time=0, double eps=0);
	/** Consider the edge e which is incident to v and v'. Remove the edge e
	 * and insert a new edge e' between v' and u. The incidencies at v' are not
	 * changed, besides replacing e by e'.
	 * Returns e'*/
	unsigned repotEdge(unsigned e, unsigned v, unsigned u, double time=0, double eps=0);
	/** Remove edge e and replace it by a new edge, which is returned. */
	unsigned renewEdge(unsigned e);
	/** Removes edges e1 and e2 from vertex v and merges both together to one
	 * edge. Hence, the two opposite eges u1, u2 of e1 and e2 become adjacent.
	 * Returns the new created edge. */
	unsigned mergeEdges(unsigned v, unsigned e1, unsigned e2);

	

	/** Are vertices s and d adjacent? */
	bool isAdjacent(unsigned s, unsigned d) const;
	/** Is edge e incident to vertex v? */
	bool isEdgeIncident(unsigned e, unsigned v) const;


	/** Consider edge e at vertex v; return next CW edge at v. */
	unsigned getCWEdge(unsigned e, unsigned v) const
	{
		assert(isEdgeIncident(e,v));
		return getEdge(e).s==v ? getEdge(e).sCW : getEdge(e).dCW;
	};
	/** Consider edge e at vertex v; return next CCW edge at v. */
	unsigned getCCWEdge(unsigned e, unsigned v) const
	{
		assert(isEdgeIncident(e,v));
		return getEdge(e).s==v ? getEdge(e).sCCW : getEdge(e).dCCW;
	};
	/** Get opposite vertex of v at edge e */
	unsigned getOppositeVertex(unsigned e, unsigned v) const
	{
		assert(isEdgeIncident(e,v));
		return v==getEdge(e).s ? getEdge(e).d : getEdge(e).s;
	};

	/** Get common vertex of edge e1 and e2 */
	unsigned getCommonVertex(unsigned e1, unsigned e2) const;
	/** Get the edge connecting vertex s and d */
	unsigned getCommonEdge(unsigned s, unsigned d) const;

	


	/** Check whether i is a valid vertex index */
	bool isVertex(unsigned i) const		{ return 0<i && i<vertices.size() && vertices[i]!=0; };
	/** Check whether i is a valid edge index */
	bool isEdge(unsigned i) const		{ return 0<i && i<edges.size() && edges[i]!=0; };


	/** Get bounding box at specific time */
	Rect getBoundingBox(double time=0.0) const;


	/** Check whether this graph is topologically a wood. Topologically means,
	 * only adjacency is considered, but not geometrical coincidence. */
	bool isTopologicalWood() const;
	/** Get some topological informations. */
	void getTopologicalInfo(bool& cycleFree, unsigned& cntComponents) const;


	/** Walk from vertex s along edge e to vertex d by always talking the next
	 * CCW edge. Stop if we come into a cycle or reach d. Vertices contains the sequence
	 * of vertices visited. Return true, if a path has been found, otherwise false. */
	bool getCCWPath(unsigned s, unsigned e, unsigned d, std::list<unsigned>& path) const;

	/** Walk from from vertex v along e, always counter-clockwise (if ccw is
	 * set, other wise cw) until we reach a vertex w whose distance to v is >=
	 * eps. Save all vertices in 'path'. Return true if a path exists,
	 * false if not. */
	bool getEscapePath(unsigned e, unsigned v, bool ccw, double time, double
			eps, std::list<unsigned>& path) const;


private:

	/** Connect edge e with other edges (and vice versa) at source resp.
	 * destination, depending on the variable atSource, at some specific time.
	 * If vertices are closer than eps, consider neighboring vertices that are
	 * more far away.
	 * */
	void connectEdgeEnd(unsigned e, bool atSource, double time, double eps);

	/** Set the CCW edge of edge e at v */
	void setCCWEdge(unsigned e, unsigned v, unsigned newe);
	/** Set the CCW edge of edge e at v */
	void setCWEdge(unsigned e, unsigned v, unsigned newe);

	/** Add a new edge without linking anything. */
	unsigned addEdge();
	/** Free memory associated with edge */
	void deleteEdge(unsigned e);


public:

	struct Vertex
	{
		/** Velocity ray of vertex */
		Ray vel;
		/** Some data, like colors */
		void* data;

		/** Some incident edge */
		unsigned inEdge;

		Vertex(const Vertex& v) :
			vel (v.vel),
			data (v.data),
			inEdge (v.inEdge)
		{
		}

		Vertex(const Ray& vel, void* data=0) :
			vel( vel),
			data(data),
			inEdge(0)
		{
		}

	};

	struct Edge
	{
		/** Source and destination vertex */
		unsigned s, d;
		/** Edges in CCW and CW direction at source and destination */
		unsigned sCCW, sCW, dCCW, dCW;

		Edge(const Edge& e) :
			s (e.s),
			d (e.d),
			sCCW (e.sCCW),
			sCW (e.sCW),
			dCCW (e.dCCW),
			dCW (e.dCW)
		{
		}

		Edge()
		{
			s = d = sCCW = sCW = dCCW = dCW = 0;
		}
	};
};


std::ostream& operator<< (std::ostream& os, const KineticSLGraph::Vertex& v);
std::ostream& operator<< (std::ostream& os, const KineticSLGraph::Edge& e);
std::ostream& operator<< (std::ostream& os, const KineticSLGraph& kslg);


#endif

