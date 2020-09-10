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
#include <sstream>
#include <string>
#include <utility>
#include <vector>

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

#include "mcgraph.h"


#include "../geom/vector.h"
#include "../geom/rect.h"


using namespace std;


#ifdef WITH_ALGOOUTPUT
	Logger loggerMcg;
#else
	NullLogger loggerMcg;
#endif


mcgraph::mcgraph(const Rect& hashbox, int gridcells) :
	hashbox( hashbox ),
	computed(false),
	lastEventTime(0),
	noEvents(0),
	whash( GeomHash<unsigned>(hashbox, gridcells) ),
	mchash( GeomHash<unsigned>(hashbox, gridcells) ),
	eps(getDefaultEpsilons())
{
}

mcgraph::~mcgraph()
{
	freeAllMem();
}


void mcgraph::freeAllMem()
{
	//Free all crash-events
	for(unsigned i=0; i<mcs.size(); i++)
	{
		CrashEventTree& crashes = crashEvents.at(i);
		CrashEventTree::iterator cit = crashes.begin();
		while( cit != crashes.end() )
		{
			delete (*cit).second;
			cit++;
		}
		crashes.clear();
	}


	//Delete remaining events
	while( !events.empty() )
	{
		McgraphEvent* curr = events.top();
		events.pop();
		delete curr;
	}

	mchash.clear();
	whash.clear();
	walls.clear();
	lastCrashedMcs.clear();
}


unsigned mcgraph::insertWall(const Segment& wall)
{
	const unsigned idx = walls.size();

	walls.push_back(wall);
	whash.addObjectAt( idx, wall, eps.ZERO);
	return idx;
}


unsigned mcgraph::launchMotorcycle(const Vector speed, const Vector point, double time)
{
	assert( time >= 0);
	const Ray vel( point, speed);
	const Motorcycle mc(time, vel, *this);

	return launchMotorcycle(mc);
}


unsigned mcgraph::launchMotorcycle(const Motorcycle& mc)
{
	const unsigned idx = mcs.size();

	mcs.push_back( mc );
	untilSwitchEvents.push_back( mc.getTrace().getStart() );
	switchEventTimes.push_back( vector<double>() );
	crashEvents.push_back( CrashEventTree() );

	assert( mcs.size() == untilSwitchEvents.size());
	assert( mcs.size() == switchEventTimes.size());
	assert( mcs.size() == crashEvents.size());

	// Let the motorcycle start
	events.push( new McgraphSwitchEvent(mc.getStarttime(), idx));

	return idx;
}



void mcgraph::compute()
{
	while( ! isComputed() )
		computeStep();
}


void mcgraph::setFinished()
{
	computed = true;

	timings.stopTiming("mc-graph");
	loggerMcg.log(LL_GOODNEWS, "Finished computing mc-graph.\n");

	freeAllMem();
}


void mcgraph::getNextSwitchEvents(unsigned mcidx, vector<double>& switchEvents)
{
	Motorcycle& mc = mcs.at(mcidx);
	list<Vector> pnts;

	const Rect ca = mchash.getCellArea(0,0);
	const double celldia = ( Vector(ca.left, ca.top) - Vector(ca.right,ca.bottom)).len();
	const Vector step = (eps.SWITCH_EVENTS_STEP*celldia) * mc.getSpeed().getDir().norm();


	// Compute next intersection points with grid
	do
	{
		//Compute start vector segment to test
		Vector vstart = untilSwitchEvents.at(mcidx);

		//Just see if we do not get over the border
		if( mc.getTimeAtPos(vstart) < 0 )
			vstart = mc.getTrace().getStart();

		//Already out of bounding box and not startet right now
		if( !hashbox.doesContain(vstart) && mc.getTimeAtPos(vstart)>mc.getStarttime()+eps.ZERO_EVENT )
		{
			loggerMcg.log(LL_DEBUG, "    check for grid-intersections of mc %d: finished.\n", mcidx);
			return;
		}

		//End is start extended by eps.SWITCH_EVENTS_STEP cell-diagonals
		Vector vend =  vstart + step;

		//End is already out of hashbox --> extend to pseudo infinity
		if( !hashbox.doesContain(vend) )
		{
			loggerMcg.log(LL_DEBUG, "    checking grid-intersections of mc %d out of hashbox.\n", mcidx);
			vend += eps.MIN_ESCAPED_DIST* mc.getSpeed().getDir().norm();
		}

		//Save end of search-segment
		untilSwitchEvents.at(mcidx) = vend;
		//Intersection points with grid
		pnts = whash.getGridSegmentIntersections( Segment(vstart, vend) );


		loggerMcg.log(LL_DEBUG, "    check for grid-intersections of mc %d "
				"in time-interval [%g, %g]: %d results\n",
				mcidx, TO_MDOUBLE(mc.getTimeAtPos(vstart)), TO_MDOUBLE(mc.getTimeAtPos(vend)), pnts.size());

	}while( pnts.empty() );


	//Add the switch-event times to the array
	for( list<Vector>::iterator it = pnts.begin();
		it != pnts.end(); it++ )
	{
		const double t = mc.getTimeAtPos(*it);

		if( is_nan(t) )
		{
			loggerMcg.log(LL_WARNING, "mc %d with switch-event at (%e,%d) at nan-time.\n",
					mcidx, TO_MDOUBLE((*it).x), TO_MDOUBLE((*it).y) );
			continue;
		}

		if( t < mc.getStarttime() )
		{
			//loggerMcg.log(LL_ERROR, "Created switch-event at %e before start time %e of mc %d.\n",
			//		t, mc.getStarttime(), mcidx);
			continue;
		}

		switchEvents.push_back(t);
	}

	//Sort the array such that earliest events are at the back
	sort(switchEvents.begin(), switchEvents.end(), greater<double>());
}



void mcgraph::computeStep()
{
	if( isComputed() )
		return;

	if( noEvents==0 )
	{
		loggerMcg.log(LL_GOODNEWS, "Computing mc-graph...\n");
		timings.startTiming("mc-graph");
	}

	if( events.empty() )
	{
		setFinished();
		return;
	}


	loggerMcg.log(LL_DEBUG, "%d. event\n", noEvents);


	//Get next (in sense of time) event
	McgraphEvent* curr = events.top();
	events.pop();


	// Remove elements which are too old
	while( !lastCrashedMcs.empty() &&
			lastCrashedMcs.front().first < curr->getTime() - eps.ZERO_EVENT )
		lastCrashedMcs.pop_front();

	//Handle current event
	curr->handleEvent(*this);


#ifndef NDEBUG
	//Wrong temporal order!
	if( curr->getTime() < lastEventTime-eps.ZERO_EVENT )
		loggerMcg.log(LL_ERROR, "Processed event at time %e. Event before time %e.\n",
				TO_MDOUBLE(curr->getTime()), TO_MDOUBLE(lastEventTime) );

	assert( curr->getTime() >= lastEventTime-eps.ZERO_EVENT);
#endif

	//Save time of this event
	lastEventTime = curr->getTimePlusPriority();

	if( !events.empty() )
	{
		loggerMcg.log(LL_DEBUG, "Next event in last+%e.\n",
				TO_MDOUBLE(events.top()->getTimePlusPriority() - lastEventTime));
	}

	//There are no more events or the next event is strictly in the future
	if( events.empty() || lastEventTime + eps.ZERO_EVENT < events.top()->getTimePlusPriority()  )
		if( lastCrashedMcs.size() >= 2 )
			handleSimultaneousCrash();

	//Little progress report
	noEvents++;
	if(noEvents%10000 == 0 || isComputed())
	{
		const int alive = getNoAliveMcs();
		loggerMcg.relog(LL_INFO, "Remaining no. of alive mcs: %8d of %d  (%.1f%% crashed)",
				alive, mcs.size(), 100.0-100.0*alive/mcs.size());
	}

	//If queue is empty --> we are finished
	if( events.empty() )
		setFinished();
	
	//Delete event
	delete curr;
}



void mcgraph::handleSimultaneousCrash()
{
	loggerMcg.log(LL_DEBUG, "Handle simultaneous crash-events at time %e.\n", TO_MDOUBLE(lastEventTime) );


	// Print the mcs
	loggerMcg.log(LL_DEBUG, "recently crashed mcs: ");
	for( list<pair<double,unsigned> >::iterator it = lastCrashedMcs.begin(); it!=lastCrashedMcs.end(); it++)
		loggerMcg.log(LL_DEBUG, "%e: %d,  ", TO_MDOUBLE(it->first), it->second);
	loggerMcg.log(LL_DEBUG, "\n");


	// motorcycles which are put back into lastCrashedMcs.
	list<pair<double,unsigned> > putback;


	//Find groups of motorcycles in lastCrashedMcs that crashed
	//together in space and time. For each such group check
	//if we launch a new motorcycle resp. add them to newmcs
	while( !lastCrashedMcs.empty() )
	{
		const pair<double,unsigned> currpair = *lastCrashedMcs.begin();
		const unsigned mcidx = currpair.second;
		const Motorcycle& mc = mcs.at(mcidx);
		const Vector place = mc.getTrace().getEnd();
		assert( mc.isCrashed() );


		loggerMcg.log(LL_DEBUG, "  search for crashes at (%e, %e), like mc %d.\n",
			   TO_MDOUBLE(place.x), TO_MDOUBLE(place.y), mcidx);



		//Create a set of motorcycles out of lastCrashedMcs that crashed at
		//same place and time
		vector<unsigned> affected;
		affected.push_back(mcidx);
		lastCrashedMcs.erase(lastCrashedMcs.begin());


		for(list<pair<double,unsigned> >::iterator it = lastCrashedMcs.begin();
				it != lastCrashedMcs.end();)
		{
			const unsigned omcidx = it->second;
			const Motorcycle& omc = mcs.at(omcidx);
			const Vector oplace = omc.getTrace().getEnd();

			assert( omc.isCrashed() );

			// Found a motorcycle crashing at the same place
			if( (place-oplace).lensq() < sq(eps.ZERO_EVENT) )
			{
				loggerMcg.log(LL_DEBUG, "    mc %d crashed here, too.\n", omcidx);
				affected.push_back(omcidx);

				if( fabs(omc.getCrashtime() - mc.getCrashtime()) > eps.ZERO_EVENT )
					loggerMcg.log(LL_ERROR, "Mc %d and mc %d did and did not crash at the same time.\n",
							mcidx, omcidx);

				it = lastCrashedMcs.erase(it);
			}
			else
			{
				loggerMcg.log(LL_DEBUG, "    mc %d did not crashed here, too: %e\n", omcidx,
						TO_MDOUBLE((place-oplace).len()) );
				it++;
			}
		}


		// Is there one affected motorcycle that crashed into a wall?
		bool wallinvolved=false;
		for( vector<unsigned>::iterator it=affected.begin(); it != affected.end(); it++)
		{
			const unsigned omcidx = *it;
			if( mcs.at(omcidx).isCrashedIntoWall() )
			{
				loggerMcg.log(LL_DEBUG, "    mc %d crashed into a wall.\n", omcidx);
				wallinvolved = true;
			}
		}
		if( wallinvolved )
			continue;



		// Compute the center of mass of all trace endpoints
		Vector midplace(0,0);
		double midtime = 0.0;
		for( vector<unsigned>::iterator it=affected.begin(); it != affected.end(); it++)
		{
			const Motorcycle& omc = mcs.at(*it);
			midplace += omc.getTrace().getEnd();
			midtime += omc.getCrashtime();
		}
		midplace /= affected.size();
		midtime /= affected.size();





		// Get all motorcylces in that area an check whether there was a
		// motorcycle strictly earlier.
		const Rect area = Rect::fromCenter(midplace, eps.ZERO, eps.ZERO);
		set<unsigned> objs = mchash.getObjectsAt(area);
		//At least we have to be in the hash
		assert( objs.size() >= 1);

		bool mcwastherealready=false;
		for(set<unsigned>::iterator it = objs.begin(); it != objs.end(); it++ )
		{
			const unsigned omcidx = *it;
			const Motorcycle& omc = mcs.at(omcidx);
			const double t = omc.getTimeAtPos(midplace);

			//The point is on the trace of mc i
			if( omc.getTrace().getDistance(midplace) < eps.ZERO  &&
				t < midtime-eps.ZERO_EVENT )
			{
				loggerMcg.log(LL_DEBUG, "    mc %d was there already. time-diff: %e\n", omcidx, TO_MDOUBLE(midtime-t));
				mcwastherealready=true;
			}
		}

		if( mcwastherealready )
			continue;


		//Two or more motorcycles crashed at the same time, at the same place
		if( affected.size() >= 2 )
		{
			loggerMcg.log(LL_DEBUG, "  %d mcs crashed at (%f, %f).\n", affected.size(),
				   TO_MDOUBLE(place.x), TO_MDOUBLE(place.y));

			// Set motorcycle trace's endpoints	to the same point
			for( vector<unsigned>::iterator it=affected.begin(); it != affected.end(); it++)
				mcs.at(*it).setTraceEndpoint(midplace);

			//Handle crash of higher degree
			Vector speed;
			//Check if a new motorcycle is launched. The speed ray is
			//saved in 'speed'
			if( handleHigherDegreeCrash(affected, midplace, speed) )
			{
				const unsigned nmcidx = launchMotorcycle(speed, midplace, midtime);
				loggerMcg.log(LL_DEBUG, "    launch mc new %d\n", nmcidx);
			}
		}
		else
		{
			// We put this mc back into the list, since it could become still
			// relevant if another mc crashes eps.ZERO after this mc, but still
			// not more than eps.ZERO_EVENT.
			putback.push_back(currpair);
		}
	}

	// Put them to back to the front of the list
	for( list<pair<double,unsigned> >::reverse_iterator it = putback.rbegin();
			it != putback.rend(); it++)
		lastCrashedMcs.push_front(*it);
}


int mcgraph::getNumberHashCells(unsigned nomcs)
{
	return max(1, int(nomcs*4) );
}


unsigned mcgraph::getNoAliveMcs() const
{
	unsigned cnt=mcs.size();

	//count alive motorcylces
	vector<Motorcycle>::const_iterator it =  mcs.begin();
	while( it != mcs.end() )
	{
		if( (*it).isCrashed() )
			cnt--;
		it++;
	}

	return cnt;
}



void mcgraph::doPostChecks(bool onlyfast) const
{
	assert( isComputed() );

	loggerMcg.log(LL_GOODNEWS, "Do some a-posteriori checks.\n");
	timings.startTiming("mc-graph postcheck");

	
	loggerMcg.log(LL_INFO, "Check whether motorcycles end is not free.\n");
	for(unsigned i=0; i<mcs.size(); i++)
	{
		//Some status messages
		if( i==mcs.size()-1 || i%100==0 )
			loggerMcg.relog(LL_INFO, "%8d (%.1f%%) of %d done.    ", i, 100.0f*(i+1.0f)/mcs.size(), mcs.size() );

		const Motorcycle& mc = mcs.at(i);

		//Didn't crashed
		if( !mc.isCrashed())
		{
			//Too short trace
			if( mc.getTrace().len() < eps.MIN_ESCAPED_DIST/2 )
				loggerMcg.log(LL_ERROR, "Motorcycle mc %d did not crashed, but trace is too short: %g\n",
						i, TO_MDOUBLE(mc.getTrace().len()));
			continue;
		}

		const Vector& end = mc.getTrace().getEnd();
		const Rect endarea = Rect::fromCenter(end, eps.ZERO, eps.ZERO);
		bool hit=false;

		//Check for other motorcycles
		set<unsigned> objs = mchash.getObjectsAt(endarea);
		set<unsigned>::iterator it = objs.begin();
		while( it!=objs.end() && !hit)
		{
			const Segment& trace = mcs.at(*it).getTrace();

			if(i != *it)
				if( trace.getDistance(end) < sqrt(eps.ZERO) )
					hit = true;
			it++;
		}


		//Check for intersection with walls
		set<unsigned> reswalls = whash.getObjectsAt( endarea );
		for( set<unsigned>::iterator it2 = reswalls.begin();
			it2!=reswalls.end() && !hit; it2++ )
		{
			const Segment& seg = walls.at(*it2);
			if( seg.getDistance(end) < eps.ZERO )
			{
				//loggerMcg.log(LL_DEBUG, "  mc %d ends at polygon seg %d.", i, j);
				hit = true;
			}
		}

		//Motorcycle has a free end
		if( !hit )
		{
			if( mc.isCrashedIntoMc() )
				loggerMcg.log(LL_ERROR, "mc %d has a free end, but crashed into mc %d.\n",
						i, mc.getTargetIdx());
			else
				loggerMcg.log(LL_ERROR, "mc %d has a free end, but crashed, though not in another mc.\n", i);
		}
	}


	if(!onlyfast)
	{


		loggerMcg.log(LL_INFO, "Check whether motorcycle intersections are not in interior of both.\n");
		for(unsigned i=0; i<mcs.size(); i++)
		{
			//Some status messages
			if( i==mcs.size()-1 || i%100==0 )
				loggerMcg.relog(LL_INFO, "%8d (%.1f%%) of %d done.    ",
						i, 100.0f*(i+1.0f)/mcs.size(), mcs.size() );

			const Segment& tracei = mcs.at(i).getTrace();

			for(unsigned j=i+1; j<mcs.size(); j++)
			{
				const Segment& tracej = mcs.at(j).getTrace();

				//Traces intersect. If they clearly intersect, we get
				//the value 0.0f back from getDistance.
				if( tracei.getDistance(tracej) <= 0 )
				{
					const Vector is = tracei.getStart();
					const Vector ie = tracei.getEnd();
					const Vector js = tracej.getStart();
					const Vector je = tracej.getEnd();

					//End point of mc i (j) lies on trace j (i)
					if(	tracei.getDistance(je) < sqrt(eps.ZERO) ||
							tracej.getDistance(ie) < sqrt(eps.ZERO) )
						continue;

					//Start point of mc j lies on trace i
					if(	tracei.getDistance(js) < sqrt(eps.ZERO) )
					{
						loggerMcg.log(LL_WARNING, "Start point of mc %d lies on trace of mc %d.\n",
								j, i);
						continue;
					}

					//Start point of mc i lies on trace j
					if(	tracej.getDistance(is) < sqrt(eps.ZERO) )
					{
						loggerMcg.log(LL_WARNING, "Start point of mc %d lies on trace of mc %d.\n",
								i, j);
						continue;
					}

					loggerMcg.log(LL_ERROR, "mc %d and mc %d intersect in boths interior! "
							"Trace-end dist: %e and %e.\n", i, j,
							TO_MDOUBLE(tracei.getDistance(je)), TO_MDOUBLE(tracej.getDistance(ie)) );
				}
			}
		}
	}


	timings.stopTiming("mc-graph postcheck");
	loggerMcg.log(LL_GOODNEWS, "Finished checks.\n");
}


Rect mcgraph::getInputBoundingBox() const
{
	assert (mcs.size() > 0);

	double l, r, t, b;
	l = r = mcs.at(0).getTrace().getStart().x;
	b = t = -mcs.at(0).getTrace().getStart().y;

	//Get all start points of mcs
	for(unsigned i=1; i<mcs.size(); i++)
	{
		const Vector v = mcs.at(i).getTrace().getStart();
		l = min(l, v.x);
		r = max(r, v.x);
		b = min(b, v.y);
		t = max(t, v.y);
	}

	//Get start and end points of walls
	for(vector<Segment>::const_iterator it = walls.begin(); it!=walls.end(); it++)
	{
		const Vector v = it->getStart();
		l = min(l, v.x);
		r = max(r, v.x);
		b = min(b, v.y);
		t = max(t, v.y);

		const Vector w = it->getEnd();
		l = min(l, w.x);
		r = max(r, w.x);
		b = min(b, w.y);
		t = max(t, w.y);
	}

	return Rect(l,r,b,t);
}


void mcgraph::printStatistics() const
{
	double sumDistSq = 0.0;
	double sumLength = 0.0;
	unsigned maxHistoVal = 1;
	unsigned noMcsEscaping = 0;
	unsigned noMcsCrashingOutsideHash = 0;
	unsigned noMcsAgainstBboxBorder = 0;

	loggerMcg.log(LL_INFO, "\nMore mcgraph statistics:\n");


	//Get crash stats
	const Rect hashRectEps = getHashBox().enlarge(-eps.ZERO);
	const Rect bboxRect = getInputBoundingBox();
	const Rect bboxRectEps = bboxRect.enlarge(-eps.ZERO);
	for(unsigned i=0; i<mcs.size(); i++)
	{
		const Vector end = mcs.at(i).getTrace().getEnd();

		if( bboxRect.doesContain(end) && !bboxRectEps.doesContain(end) )
			noMcsAgainstBboxBorder++;

		if( mcs.at(i).isCrashed() && !hashRectEps.doesContain(end) )
			noMcsCrashingOutsideHash++;

		if( !mcs.at(i).isCrashed() )
			noMcsEscaping++;
	}


	//Get average trace length...
	for(unsigned i=0; i<mcs.size(); i++)
	{
		if( !mcs.at(i).isCrashed() )
			continue;

		const Segment& trace = mcs.at(i).getTrace();
		const double len = (trace.getEnd() - trace.getStart()).len();
		sumLength += len;
	}
	//... and variance
	for(unsigned i=0; i<mcs.size(); i++)
	{
		if( !mcs.at(i).isCrashed() )
			continue;

		const Segment& trace = mcs.at(i).getTrace();
		const double len = (trace.getEnd() - trace.getStart()).len();

		const double diffsq = len - sumLength/mcs.size();
		sumDistSq += diffsq*diffsq;
	}


	assert(mcs.size() >= noMcsEscaping );
	const unsigned noNotEscaping = mcs.size() - noMcsEscaping;

	if( noNotEscaping > 0 )
		loggerMcg.log(LL_INFO, "  Trace length sample mean:  %f\n", TO_MDOUBLE(sumLength/noNotEscaping));
	else
		loggerMcg.log(LL_INFO, "  Trace length sample mean:  0\n");

	if( noNotEscaping > 1 )
		loggerMcg.log(LL_INFO, "  Trace length variance:     %f\n", TO_MDOUBLE(sumDistSq/(noNotEscaping-1)) );
	else
		loggerMcg.log(LL_INFO, "  Trace length variance:     0\n");

	loggerMcg.log(LL_INFO, "  No mcs escaping:                      %d\n", noMcsEscaping);
	loggerMcg.log(LL_INFO, "  No mcs crashing outside hash:         %d\n", noMcsCrashingOutsideHash);
	loggerMcg.log(LL_INFO, "  No mcs crashing against bbox border:  %d\n", noMcsAgainstBboxBorder);



	//Get histogram on trace lengths
	////////////////////////////////
	
	const unsigned histoRes = 50;
	const unsigned histoHeight = 10;
	unsigned histoData[histoRes];

	loggerMcg.log(LL_INFO, "  Histogram:\n");

	//Diagonal of hashbox
	const double hashboxdia = ( Vector(hashbox.right, hashbox.top) - Vector(hashbox.left, hashbox.bottom)).len()/2;
	//Set maximum length in histogram to 4 times expectation
	const double maxlen = min(hashboxdia, 4*sumLength/mcs.size() );
	//Interval length of a histogram class
	const double histoDelta = maxlen/(histoRes-1);


	for(unsigned i=0; i<histoRes; i++)
		histoData[i] = 0;

	for(unsigned i=0; i<mcs.size(); i++)
	{
		const Segment& trace = mcs.at(i).getTrace();
		const double len = (trace.getEnd() - trace.getStart()).len();
		const unsigned histClass = min(histoRes-1, unsigned(REAL_TO_INT(len/histoDelta)));

		assert( histClass<histoRes);
		histoData[histClass] += 1;
	}

	for(unsigned i=0; i<histoRes; i++)
		maxHistoVal = max( maxHistoVal, histoData[i]);

	for(int l=histoHeight-1; l>0; l--)
	{
		string line = "";
		const unsigned minCnt = uintceil( double(l*maxHistoVal) / histoHeight ) ;

		for(unsigned i=0; i<histoRes; i++)
		{
			if( histoData[i] >= minCnt )
				line += "O";
			else
				line += " ";
		}

		if( l==histoHeight-1 )
			loggerMcg.log(LL_INFO, "  |%s|  %d\n", line.c_str(), maxHistoVal);
		else
			loggerMcg.log(LL_INFO, "  |%s|\n", line.c_str());
	}

	string line = "";
	for(unsigned i=0; i<histoRes; i++)
		line += "-";
	loggerMcg.log(LL_INFO, "  +%s+\n", line.c_str());


	line = "";
	for(unsigned i=0; i<histoRes-9; i++)
		line += " ";
	loggerMcg.log(LL_INFO, "   %8f%s>=%8f\n", TO_MDOUBLE(histoDelta), line.c_str(), TO_MDOUBLE(maxlen));
}



#ifdef WITH_OGL

void mcgraph::redraw(glStalgoWindow& wnd) const
{
	glplane* mcplane = wnd.getMcLayer();
	glplane* hashplane = wnd.getMcHashLayer();
	gltextplane* textplane = wnd.getMcTextLayer();

	//Clear the planes
	mcplane->clear();
	hashplane->clear();
	textplane->clear();


	//Draw the geomhash
	for( int y=-1; y<=mchash.getLines(); y++)
	{
		for( int x=-1; x<=mchash.getCols(); x++)
		{
			const Rect area = mchash.getCellArea(x,y).enlarge(-eps.ZERO);
			const int noobjs = mchash.getObjectsAt(area).size();
			const double rel = double(noobjs)*mchash.getCols()*mchash.getLines()/mcs.size();

			hashplane->addObject( new glrect(
					TO_MDOUBLE(area.left), TO_MDOUBLE(area.right), TO_MDOUBLE(area.bottom), TO_MDOUBLE(area.top),
					glcolor::fromRGBA(0,0,1, TO_MDOUBLE(sqrt(rel)/16 )) ));
		}
	}
	for( int y=0; y<=mchash.getLines(); y++)
	{
		const double ry = mchash.getCellArea(0,y).bottom;
		hashplane->addObject( new glline(TO_MDOUBLE(mchash.getArea().left), TO_MDOUBLE(ry),
				    TO_MDOUBLE(mchash.getArea().right), TO_MDOUBLE(ry),
					glcolor::fromRGB(0,0,0.5) ));
	}
	for( int x=0; x<=mchash.getCols(); x++)
	{
		const double rx = mchash.getCellArea(x,0).left;
		hashplane->addObject( new glline(TO_MDOUBLE(rx), TO_MDOUBLE(mchash.getArea().bottom),
					TO_MDOUBLE(rx), TO_MDOUBLE(mchash.getArea().top),
					glcolor::fromRGB(0,0,0.5) ));
	}



	//Draw the motorcycles
	for( unsigned i=0; i<mcs.size(); i++)
	{
		const Motorcycle& mc = mcs.at(i);
		const Segment& t = mc.getTrace();
		const double time = min(mc.getTimeAtPos(t.getEnd()), mc.isCrashed() ? mc.getCrashtime() : getLastEventTime());
		const Vector& currpos = isComputed() ? t.getEnd() : mc.getPosAtTime(time);

		const glcolor startpntcol = glcolor::fromRGB(1, 0, 0);
		const glcolor tracecol = glcolor::fromRGBA(1, 0.25, 0.25, 0.4);
		const glcolor mccol = glcolor::fromRGB(1, 0.25, 0.25);
		glcolor crashpntcol = glcolor::fromRGB(1, 0.5, 0.5);
	

		//Start point
		mcplane->addObject( new glpoint( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y),
				   startpntcol) );

		//Current trace
		mcplane->addObject( new glline( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y),
					TO_MDOUBLE(t.getEnd().x), TO_MDOUBLE(t.getEnd().y), tracecol) );

		//Current motorcycle
		mcplane->addObject( new glline( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y),
					TO_MDOUBLE(currpos.x), TO_MDOUBLE(currpos.y), mccol) );

		if( ! mc.isCrashed() )
			crashpntcol.setAlpha(0.5);

		//Current position
		mcplane->addObject( new glpoint( TO_MDOUBLE(currpos.x), TO_MDOUBLE(currpos.y), crashpntcol) );

		gltext* text = new gltext( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y),
				glcolor::fromRGB(1, 0, 0), "mc%d", i);

		textplane->addText(text);
	}

	//Plot selected motorcycle
	if( 0<= wnd.getSelectedMcIdx() && wnd.getSelectedMcIdx() < (int)mcs.size() )
	{
		int i = wnd.getSelectedMcIdx();
		const Motorcycle& mc = mcs.at(i);
		const Segment& t = mc.getTrace();
		const double time = min(mc.getTimeAtPos(t.getEnd()),  mc.isCrashed() ? mc.getCrashtime() : getLastEventTime());
		const Vector& currpos = isComputed() ? t.getEnd() : mc.getPosAtTime(time);

		const glcolor startpntcol = glcolor::fromRGB(1, 1, 0);
		const glcolor mccol = glcolor::fromRGB(1, 1, 0.25);

		//Draw selection circle
		mcplane->addObject( new glcircle(TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y), 0.05,
					glcolor::fromRGBA(1,1,0, 0.4)) );

		//Start point
		mcplane->addObject( new glpoint( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y), startpntcol) );
		//Current motorcycle
		mcplane->addObject( new glline( TO_MDOUBLE(t.getStart().x), TO_MDOUBLE(t.getStart().y),
					TO_MDOUBLE(currpos.x), TO_MDOUBLE(currpos.y), mccol) );
	}
}





#endif



mcgraph::epsilons mcgraph::getDefaultEpsilons()
{
	epsilons eps;
	double factor=1;

#ifdef WITH_MPFRBACKEND
	factor = sqrt(mpfr_get_default_prec()/53.0);
#endif

	eps.ZERO = pow(10, -11*factor);
	eps.ZERO_ANGLE = pow(10, -6*factor);
	eps.ZERO_EVENT = pow(10, -9*factor);
	eps.SWITCH_EVENTS_STEP = 3;

	eps.MIN_ESCAPED_DIST = 1e5;

	return eps;
}




