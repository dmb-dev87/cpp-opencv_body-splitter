#ifndef straightskeleton_h
#define straightskeleton_h

#include "../fpkernel.h"

#ifndef DOUBLE_OVERRIDE
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <functional>
#endif

#include "../main.h"

#include "../geom/kineticslgraph.h"


class skmcgraph;

#ifdef WITH_ALGOOUTPUT
	extern Logger loggerSk;
#else
	extern NullLogger loggerSk;
#endif

struct StraightSkeletonError : public std::logic_error {
    public:
        explicit StraightSkeletonError(const std::string &msg) :
            std::logic_error(msg)
        {
        };
};

class StraightSkeleton
{
	public:

		/** Pair of unsigned */
		typedef std::pair<unsigned,unsigned> upair;

		/** The type of a vertex in the wavefront */
		enum wfvtxtype
		{
			CONVEX,
			REFLEX,
			CONVEX_MULTI,
			STEINER_MOVING,
			STEINER_RESTING,
			STEINER_MULTI
		};

		/** A wavefront event. That is, a collapsing wf-edge. */
		struct event
		{
			unsigned wfedge;
			double time;
			Vector place;

			event(unsigned wfedge, double time, const Vector &place);
		};

		/** Properties of wf vertices to which wf.getVertex().data points to */
		struct wfvtxproperties
		{
			/** What type of wf vertex? */
			wfvtxtype type;
			/** From which sk-vertex did this wf vertex start? */
			unsigned skstart;

			wfvtxproperties(wfvtxtype type, unsigned skstart=0) :
				type(type),
				skstart(skstart)
			{
			}
		};

		/** The priority std::queue type */
		typedef std::priority_queue<event, std::vector<event>, std::greater<event> >
			EventPriorityQueue;


	private:


		/** The time of the last event */
		double lastEventTime;

		/** The priority std::queue for events */
		EventPriorityQueue events;

		/** The input kinetic straight-line graph */
		const KineticSLGraph input;
		/** The current wavefront */
		KineticSLGraph wf;
		/** The straight skeleton */
		KineticSLGraph sk;


		/** Contains all the wavefront vertex properties */
		std::vector<wfvtxproperties*> wfvtxprop;

		/** Saves for every directed edge (s,d) of input the wave-front edge
		 * e' left to (s,d). Hence, for every std::pair (s,d) there is also a
		 * (different) entry for (d,s) */
		std::map<upair, unsigned> wfedges;

		/** Saves for every directed edge (s,d) of input a std::pair (a,b),
		 * where a the start vertex and the end vertex of the sk face left to
		 * the input edge (s,d) is. Hence, if one starts from vertex a (which
		 * coincides with s) and always walk ccw on the sk one eventually
		 * gets to b, which coincides with the input vertex d. By doing so, one
		 * traverses the face in cw direction.*/
		std::map<upair, upair> skfaceEndpoints;

		/** Save for every directed sk arc (s,d) with s, d being sk vertices
		 * the input edge (a,b) to the right of (s,d) which generates this arc.
		 * Note, the input edge is again given as a directed edge and (s,d) is
		 * left to (a,b). */
		std::map<upair, upair> skarcFaces;

		/** Remember for the steiner points, from which reflex vertex
		 * they have been created. */
		std::map<unsigned, unsigned> steinerorig;

		/** Save for some edges of 'wf' from which directed input edge (s,d) the
		 * wf-edge originated from. Note that the wf-edge is left to the
		 * directed edge (s,d). Note that the edges of 'wf' which correspond to
		 * the motorcycle graph are not contained here. */
		std::map<unsigned, upair> wfeorigin;

		/** Has straight skeleton already computed? */
		bool computed;
		/** Count the number of events processed */
		unsigned noEventsProcessed;
		/** The initial number of wf edges */
		unsigned noInitialWfEdges;


		bool initialized;
		bool initializedWf;

	public:

		/** Create kinetic straight-line graph from input */
		StraightSkeleton(const KineticSLGraph& input);
		virtual ~StraightSkeleton();

		/** Initialize the straight-skeleton algorithm */
		void initialize();
		/** Intialize wavefront without motorcycle graph */
		void initializeWavefront();



		/** Has computation finished? */
		bool isComputed() const		{ return computed; };
		/** Compute next step */
		void computeStep();
		/** Compute the straight skeleton */
		void compute();


		/** Get the time of the last event */
		double getLastEventTime() const	{ return lastEventTime; };

		/** Get current wavefront */
		const KineticSLGraph& getWavefront() const	{ return wf; };
		/** Get input straight-line graph */
		const KineticSLGraph& getInput() const		{ return input; };


		/** Get propagation direction of directed input edge to left side */
		Ray getInputEdgeDir( upair directedInputEdge) const;
		/** Is vertex v in the wavefront wf reflex? */
		bool isWfVertexReflex(unsigned v) const;
		/** Get the propagation speed of specific edge */
		Vector getWfEdgeSpeed(unsigned e) const;
		/** Returns >0 if the wf-edge between wf-vertices u and v is expanding
		 * and <0 if shrinking. The vertex u is left and v is right w.r.t. to
		 * the propagation direction of (u, v). */
		double isWfEdgeExpanding(unsigned u, unsigned v) const;
		/** Get the velocity of a vertex that moves on the bisector of two
		 * edges, whose propagation velocities are n1 an n2, resp. */
		Vector getVertexSpeed(Vector n1, Vector n2) const;
        /** Get the position of the vertex if it escapes to infinity. */
        Vector getVertexEscapePos(unsigned vtx) const;


		/** Get the straight skeleton */
		const KineticSLGraph& getStraightSkeleton() const		{ return sk; };
		/** Get sk face endpoints. For every directed input edge (s,d) we save (start,end)
		 * vertices of the left face of the sk.*/
		const std::map<upair, upair>& getSkfaceEndpoints() const	{ return skfaceEndpoints; };
		/** Get to an directed sk arc the directed input edge right to this arc. */
		const std::map<upair, upair>& getSkarcFaces() const	{ return skarcFaces; };
		/** Fill faceVertices with the vertices of the sk face left to the directed input edge (a,b),
		 * where a and b are two input vertices. Note, b=a is only allowed for terminal vertices a.
		 * Returns false if the face can not be traversed, other wise true. */
		bool getSkfaceVertices( upair directedInputEdge, std::list<unsigned>& faceVertices ) const;

		/** Fill curves with the offset curves with offset distance 'offset'. */
		void computeOffsetCurve( KineticSLGraph& curves, const double offset) const;

#ifdef WITH_OGL
		void redraw(glStalgoWindow & wnd, int timeshift=0) const;
#endif


	private:

		/** Finish the computation */
		void finishComputation();

		/** Create wavefront from input */
		void createWavefront();

		/** Insert the motorcycle graph into wavefront */
		void insertMcgIntoWavefront(const skmcgraph& mcg);


		/** Get propagation speed of edge e in specific direction */
		Vector getWfEdgeSpeedInDir(unsigned e, const Vector &dir) const;
		/** Get the propagation speed of a vertex v driving on the intersection
		 * of the moving edge e and the edge 'edir'.*/
		Ray getWfEdgeSpeedInDir(unsigned e, unsigned edir, unsigned v, double time) const;
		/** Get direction at which e is pointing to, starting at vertex u, at
		 * the given time. */
		Vector getWfEdgeDir(unsigned e, unsigned u, double time) const;

		/** Get the propagation velocity of a point on the bisector of eccw and
		 * ecw. The point is at the given time at the given place. The new
		 * vertex moves towards the opposite vertices of vcw and vccw. */
		Ray getVertexSpeed(unsigned eccw, unsigned vccw, unsigned ecw,
				unsigned vcw, Vector place, double time) const;


		/** Get wavefront edge, where mc crashed against. Note that
		 * it is assumed that mc actually crashed against a wall. */
		unsigned getWfEdgeByMc(const skmcgraph& mcg, unsigned mcidx) const;


		/** Get the time when the wf-edge e is collapsing. Returning a negative
		 * value if there is no collapse. */
		double getCollapseTime(unsigned e) const;
		/** Get the time and place when the wf-edge e is collapsing. Gives a
		 * negative time if there is no collapse. */
		void getCollapseTimePlace(unsigned e, double &time, Vector &place) const;
		/** Find the point 'place' that is reached by ray after time 'time'
		 * and which has orthogonal distance 'time' to the supporting line of
		 * the given segment. The segment is given as an orthogonal ray
		 * starting from the segment.
		 * Returns the condition of the numerical method */
		double getSolSegRay(const Ray& segnormal, const Ray& ray, double& time, Vector& place) const;
		/** Find the point 'place' that has same orthogonal distance 'time' to
		 * the supporting lines of the three given segments. The segments are
		 * given as orthogonal rays starting from a point of the segment.
		 * Returns the condition of the numerical method */
		double getSolSegSegSeg(const Ray& s1, const Ray& s2, const Ray& s3,
				double& time, Vector& place) const;
		/** Find the point 'place' which is concurrently reached after time
		 * 'time' by the two rays u and v.
		 * Returns the condition of the numerical method */
		double getSolRayRay(const Ray& u, const Ray& v, double& time, Vector&
				place) const;
		/** Create an event from the collapse of edge e */
		event getCollapseEvent(unsigned e) const;

		/** Consider the vertex v incident to edge e. Then move vertex at given
		 * time along the edge e by an eps. */
		void moveWfVertexAlongEdge(unsigned v, unsigned e, double time, double eps);


		/** Process the given event */
		void processEvent(const event& ev);
		/** Two vertices u and v met and an edge event occured */
		void processEdgeEvent(const event& ev);
		/** A convex vertex u met a moving steiner point v */
		void processSwitchEvent(const event& ev);
		/** A reflex vertex u met its destination steiner point v */
		void processSplitEvent(const event& ev);
		/** A reflex vertex or a moving steiner point u met a resting steiner
		 * point v */
		void processStartEvent(const event& ev);
		/** Process a split event of higher degree, where a vertex meets
		 * a multi-steiner v. */
		void processMultiSplitEvent(const event& ev);

		/** Add an corresponding event of edge e, if one exists */
		void insertEvent(unsigned e, double mintime);
		/** Call insert insertEvent for all edges. */
		void insertEvents(const std::vector<unsigned> &edges, double mintime);


		/** Remove a wf vertex and associated data */
		void removeWfVertex(unsigned v);
		/** Add a wavefront vertex with specific velocity and type. */
		unsigned addWfVertex(const Ray& vel, wfvtxtype t);
		/** Get properties of a specific wavefront vertex */
		wfvtxproperties& getWfVertexInfo(unsigned v);
		const wfvtxproperties& getWfVertexInfo(unsigned v) const;

		/** Copy the wfeorigin information */
		void cpyWfeorign(unsigned e, unsigned esrc);

		/** Add a new sk-vertex given by wf-vertex 'wfvtx' at specific time */
		unsigned addSkVertex(unsigned wfvtx, double time);
		/** Add a new sk-vertex given by a specific position and by a specific wf-vertex */
		unsigned addSkVertex(unsigned wfvtx, Vector pos);

		/** Walk from v to e and so forth, skipping all resting steiner points.
		 * Consider the wavefront at time 'time'. Hence, if v is a moving
		 * steiner vertex, this function retrieves the steiner origin of v. */
		unsigned skipRestingSteiner(unsigned e, unsigned v, double time) const;


		/** Two parallel sequeneces of collinear edges, starting with eu at
		 * vertex u and ev at vertex v, are merged. That is, the resting
		 * Steiner vertices along the sequence of ev are inserted into the
		 * sequence of eu that lies on the actual wavefront.
		 * This function updates eu and u if they have been altered.*/
		void mergeParallelSteinerEdges(unsigned &eu, unsigned &u, unsigned ev,
				unsigned v, double time, std::vector<unsigned> &nedges);

		void mergeParallelSteinerEdges_fixEndVertex(unsigned lastvtx, unsigned
				e1, double time, std::vector<unsigned> &nedges, unsigned &eu,
				unsigned &u);


		/** Patch the wavefront by creating a new convex wavefront vertex v
		 * which merges vccw and vcw together, which is incident eccw and ecw
		 * and which starts at given place at given time.
		 * The vertex v originates at the straight skeleton node skvtx.
		 * Returns: v*/
		unsigned patchWavefront(unsigned eccw, unsigned vccw, unsigned ecw,
				unsigned vcw, Vector place, double time, unsigned skvtx);

		/** The same as the simple patchWavefront, but also insert emanating
		 * Steiner edges. A steiner edge is given by a pair <edge,vertex>,
		 * where vertex is at given place at the given time. */
		unsigned patchWavefront(unsigned eccw, unsigned vccw, unsigned ecw,
				unsigned vcw, Vector place, double time, unsigned skvtx,
				std::vector<std::pair<unsigned,unsigned> > steineredges );



	public:

		/** A bunch of epsilons used for floating-point comparisons */
		struct epsilons
		{
			// On realistic/lines/random/* the following holds:
			//    1e-13 is better than 1e-12 is better than 1e-11
			//    1e-13 is better than 1e-14
			double ZERO;
			// See realistic/lines/random/r08192-006.line
			double ZERO_EVENT;
			double ZERO_MULTI_STEINER;
			//    realistic/lines/real_world/ --> 1e-8 better than 1e-7 than 1e-6
			double ZERO_ANGLE;
			double INFDIST;
			// On realistic/lines/multiply_connected/21.line --> 1e6
			double INFSPEED;

			// At which quality are we printing a warning?
			double BADQUALITY;
		};

		const epsilons& getEpsilons() const
		{
			return eps;
		}

		static epsilons getDefaultEpsilons();

	public:
		const epsilons eps;

};


bool operator>(const StraightSkeleton::event& e1, const StraightSkeleton::event& e2);
std::ostream& operator<< (std::ostream& os, const StraightSkeleton::wfvtxtype& t);

#endif





