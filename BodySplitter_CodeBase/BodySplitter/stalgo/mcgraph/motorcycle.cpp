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

#include "../fpkernel.h"

#include "motorcycle.h"

#include "mcgraph.h"

#include "../util/util.h"

using namespace std;



Motorcycle::Motorcycle(double starttime, const Ray& speed, mcgraph& graph) :
	starttime(starttime),
	speed(speed),
	trace(speed.getStart(), speed.getStart() + graph.eps.MIN_ESCAPED_DIST*speed.getDir().norm()),
	crashed(false),
	crashtime(-1.0),
	crashIntoMc(false),
	targetidx(0),
	graph(&graph)
{
}


void Motorcycle::setTraceEndpoint(const Vector& v)
{
	trace.setEnd(v);

	//Check if segment direction equals speed direction
#ifndef NDEBUG
	if( trace.lensq() > sq(graph->eps.ZERO) )
	{	
		const double l = ((trace.getEnd()-trace.getStart()).norm() - speed.getDir().norm()).len();
		assert( l < 1e-3);
	}
#endif
}


void Motorcycle::setTraceEndpointToInf()
{
	setTraceEndpoint( trace.getStart() + graph->eps.MIN_ESCAPED_DIST*speed.getDir().norm());
}


double Motorcycle::getCrashtime() const
{
	assert(isCrashed());
	return crashtime;
}


void Motorcycle::setCrashTime(double time)
{
	assert(time >= starttime);

	trace.setEnd( getPosAtTime(time) );
	crashtime = time;
	crashed = true;
}

void Motorcycle::setCrashedIntoWall(unsigned targetidx, double time)
{
	setCrashTime(time);
	this->targetidx = targetidx;
	this->crashIntoMc = false;
}

void Motorcycle::setCrashedIntoMc(unsigned targetidx, double time)
{
	setCrashTime(time);
	this->targetidx = targetidx;
	this->crashIntoMc = true;
}

Vector Motorcycle::getPosAtTime(double time) const
{
	return ((time-starttime)*speed).getEnd();
}


double Motorcycle::getTimeAtPos(const Vector& pos) const
{
	return speed.projDistance(pos)+starttime; 
}


bool Motorcycle::getIntersection(const Segment& seg, Vector& pnt) const
{
	const double ZERO = graph->eps.ZERO;
	const Segment& trace = getTrace();

	// We have zero-length trace
	if( trace.lensq() < sq(ZERO) )
		return false;


	const double os = speed.orthoDistance(seg.getStart());
	const double oe = speed.orthoDistance(seg.getEnd());

	// Segment is entirely on one side of the track
	if( (os>ZERO && oe>ZERO)  ||  (os<-ZERO && oe<-ZERO) )
		return false;

	// Both end points are on the track
	if( fabs(os)<=ZERO && fabs(oe)<=ZERO )
	{
		const double ps = speed.projDistance(seg.getStart());
		const double pe = speed.projDistance(seg.getEnd());

		// Segment is behind motorcycle. If we started from the segment, we did
		// not crash into it.
		if( max(ps,pe) <= ZERO )
			return false;

		// Segment is in front of motorcycle
		if( min(ps,pe) > ZERO )
			// This makes pnt to be "exactly" on the track
			pnt = (min(ps,pe)*speed).getEnd();
		// Motorcycle moves on the segment
		else
			pnt = speed.getStart();

		return true;
	}

	// Exactly one endpoint is on the track
	if( fabs(os) <= ZERO || fabs(oe) <= ZERO )
	{
		pnt = fabs(os) < fabs(oe) ? seg.getStart() : seg.getEnd();
		const double ps = speed.projDistance(pnt);

		if( ps < -ZERO )
			return false;

		pnt = (ps*speed).getEnd();
		return true;
	}

	
	const double sum = fabs(os)+fabs(oe);
	assert( sum > ZERO );
	pnt = ( fabs(oe)*seg.getStart() + fabs(os)*seg.getEnd() )/sum;

	return speed.projDistance(pnt)>ZERO  &&  seg.getDistance(pnt)<ZERO;
}


bool Motorcycle::getCrashpoint(const Motorcycle& omc, Vector& pnt) const
{
	const double ZERO = graph->eps.ZERO;
	const Segment& ourTrace = getTrace();
	const Segment& hisTrace = omc.getTrace();

	// One of the motorcycles did not move yet. This can happen when a
	// motorcycle starts at the boundary of a hash cell.
	if( ourTrace.getDir().lensq() < sq(ZERO) || hisTrace.getDir().lensq() < sq(ZERO) )
		return false;

	// Both started from the same place.
	if( (ourTrace.getStart() - hisTrace.getStart()).lensq() <= sq(ZERO) )
		return false;


	// Non-parallel case
	if( fabs(Vector::sin2(getSpeed().getDir(), omc.getSpeed().getDir()) ) > graph->eps.ZERO_ANGLE )
	{
		pair<double,double> times = getSpeed().intersect(omc.getSpeed());
		const double t1 = times.first;
		const double t2 = times.second;

		const Vector p1 = (t1*getSpeed()).getEnd();
		const Vector p2 = (t2*omc.getSpeed()).getEnd();
		pnt = (p1+p2)/2.0;

		return t1 > ZERO && t2 > ZERO ;
	}
	// Parallel case
	else
	{
		const double one2two = getSpeed().projDistance(omc.getSpeed().getStart());
		const double two2one = omc.getSpeed().projDistance(getSpeed().getStart());

		// Different directions
		if( one2two <= ZERO && two2one <= ZERO )
			return false;

		// omc crashes into *this
		else if( one2two <=ZERO && two2one > ZERO )
			pnt = getSpeed().getStart();

		// *this crashes into omc
		else if( one2two >ZERO && two2one <= ZERO )
			pnt = omc.getSpeed().getStart();

		else
		{
			const Ray ourSpeed = Ray(getPosAtTime(0), getSpeed().getDir());
			const Ray hisSpeed = Ray(omc.getPosAtTime(0), omc.getSpeed().getDir());

			const double minDistTime = ourSpeed.minDistScalar(hisSpeed);

			const Vector ourPos = getPosAtTime(minDistTime);
			const Vector hisPos = omc.getPosAtTime(minDistTime);

			pnt = (ourPos+hisPos)/2.0;
		}

		return ourTrace.getDistance(pnt) <= ZERO && hisTrace.getDistance(pnt) <= ZERO;
	}
}


