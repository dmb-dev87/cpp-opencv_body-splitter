
#ifndef polychain_h
#define polychain_h


#ifndef DOUBLE_OVERRIDE
#include <vector>
#include <list>
#endif


#include "vector.h"
#include "segment.h"
#include "rect.h"


/** A polygonal chain is a tuple of vectors and two successive vectors are
 * thought to be inter-connected by straight-line segments.  A closed polygonal
 * chain may be closed and is then equivalent to a polygon.*/
class Polychain
{
	protected:
		std::vector<Vector> vertices;

		/** is the chain closed?*/
		bool closed;


	public:
		Polychain();
		Polychain(const Polychain& p);
		~Polychain();

		/** Add a new vertex to the polygon */
		void addVertex(Vector p);

		/** Get the i-th vertex */
		Vector getVertex(unsigned i) const;
		/** Get array of vertices */
		std::vector<Vector>& getVertices()	{ return vertices; };
		/** Get array of vertices */
		const std::vector<Vector>& getVertices() const	{ return vertices; };

		/** Get size of polygon */
		unsigned size() const	{ return vertices.size(); };
		/** Get number of segments */
		unsigned getNoSegs() const;
		/** Is polychain closed? */
		bool isClosed() const	{ return closed; };
		/** Set polychain to be closed or not. */
		void setClosed(bool closed)	{ this->closed=closed; };

		/** Get all cw-oriented vertices */
		void getCwVertices( std::list<unsigned>& reflx ) const;
		/** Get number of cw-oriented vertices */
		unsigned getNoCwVertices() const;

		/** Get segment between i-th and (i+1)-th vertex. If isClosed(), then
		 * i=size()-1 is valid. */
		Segment getSegment(unsigned i) const;
		/** Get a std::list of segments */
		std::vector<Segment> getSegments() const;

		/** Get the bounding box of polychain */
		Rect getBoundingBox() const;

		/** Reverse direction of polychain */
		void reverse();

		//RH
		bool doesContain(const Vector &vec) const;
};


#endif

