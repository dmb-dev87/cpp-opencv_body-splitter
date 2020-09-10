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

#include "event.h"


#include "../geom/rect.h"
#include "mcgraph.h"

using namespace std;




McgraphEvent::McgraphEvent(double time, unsigned mcidx, double priority) :
	time(time),
	priority(priority),
	mcidx(mcidx)
{
}


bool McgraphEvent::operator==(const McgraphEvent& e) const
{
	return getTimePlusPriority() == e.getTimePlusPriority();
}


bool McgraphEvent::operator<(const McgraphEvent& e) const
{
	return getTimePlusPriority() < e.getTimePlusPriority();
}


bool McgraphEvent::isValid(mcgraph& graph) const
{
	Motorcycle& mc = graph.mcs.at(getMcIdx());

	//Check if we already crashed
	if( mc.isCrashed() )
		return false;

	//Maximum possible time due to trace
	const double maxtime = mc.getTimeAtPos( mc.getTrace().getEnd());

	//Event is after time that corresponds to trace length
	//Attention: We had problems here when traces get very longs
	if( getTime() >= maxtime*(1.0+graph.eps.ZERO_EVENT) + graph.eps.ZERO_EVENT )
		return false;

	return true;
}



McgraphCrashEvent::McgraphCrashEvent(double time, int mcidx, bool intoMc, int targetidx, double priority) :
	McgraphEvent(time, mcidx, priority),
	intoMc(intoMc),
	targetidx(targetidx)
{

}


McgraphCrashEvent* McgraphCrashEvent::createCrashIntoWall(mcgraph& graph, double time, int mcidx, int targetidx)
{
	return new McgraphCrashEvent(time, mcidx, false, targetidx, -20*graph.eps.ZERO_EVENT);
}

McgraphCrashEvent* McgraphCrashEvent::createCrashIntoMc(mcgraph& graph, double time, int mcidx, int targetidx, bool priority)
{
	return new McgraphCrashEvent(time, mcidx, true, targetidx, priority ? -10*graph.eps.ZERO_EVENT : 0);
}


void McgraphCrashEvent::handleEvent(mcgraph& graph)
{
	//Get index of our motorcycle
	const unsigned mcidx = getMcIdx();
	Motorcycle& mc = graph.mcs.at(mcidx);

	//Print some status messages
	if( isTargetMc() )
		loggerMcg.log(LL_DEBUG, "Potential crash-event: time %e, mc %d in mc %d\n",
				TO_MDOUBLE(getTime()), mcidx, getTargetIdx());
	else
		loggerMcg.log(LL_DEBUG, "Potential crash-event: time %e, mc %d.\n",
				TO_MDOUBLE(getTime()), mcidx);


	//Check if event is still valid
	if( !isValid(graph) )
		return;

	//Get the place and area where the event takes place
	const Vector evplace = mc.getPosAtTime(getTime());
	const Rect evarea = Rect::fromCenter(evplace, graph.eps.ZERO, graph.eps.ZERO);
	mcgraph::CrashEventTree& ourCrashes = graph.crashEvents.at(mcidx);


	//We crashed into a motorcycle, not a wall
	if( isTargetMc() )
	{
		const Motorcycle& depmc = graph.mcs.at(getTargetIdx());
		Vector p;
		bool isCrash = mc.getCrashpoint(depmc, p);

		//Get time when dependency mc reaches this place
		const double depreach = depmc.getTimeAtPos(p);

		loggerMcg.log(LL_DEBUG, "  dependency motorcycle got there at time %f.\n", TO_MDOUBLE(depreach));

		//The motorcycle we depend on strictly crashed before it reached
		//this place
		if( (depmc.isCrashed() && depreach > depmc.getCrashtime() + graph.eps.ZERO_EVENT) || !isCrash )
		{
			//We have to add the earliest crash event in our crash-event
			//data structure to the event queue
			mcgraph::CrashEventTree::iterator eit = ourCrashes.begin();
			if( eit != ourCrashes.end() )
			{
				graph.events.push( (*eit).second );
				ourCrashes.erase(eit);
			}

			loggerMcg.log(LL_DEBUG, "  event got obsolete. Other motorcycle crashed at time %e.\n",
					TO_MDOUBLE(depmc.getCrashtime()));
			return;
		}
	}


	//Now we have evidence: this is an actual crash
	loggerMcg.log(LL_DEBUG, "  Got evidence that crash-event happened.\n");


	//Remove all crashes of other motorcycles in our trace
	//(if they occur after this crash)
	set<unsigned> othermcs = graph.mchash.getObjectsAt(evarea);
	for( set<unsigned>::iterator it2 = othermcs.begin();  it2 != othermcs.end(); ++it2 )
	{
		if( *it2 == mcidx )
			continue;

		//Traces intersect --> their is a potential crash...
		const Motorcycle& omc = graph.mcs.at(*it2);

		//Get crash point and the times when we are there...
		Vector crash;
		mc.getCrashpoint(omc, crash);
		double ourTime = mc.getTimeAtPos(crash);
		double hisTime = omc.getTimeAtPos(crash);


		//Aha -- he gets there later, and we would reach it
		//after our crash: all crash events with omc are stale!
		if( hisTime > ourTime - graph.eps.ZERO_EVENT &&
				ourTime > getTime() + graph.eps.ZERO_EVENT )
		{
			//Search for crashes of omc into mc at some time around 'hisTime'
			mcgraph::CrashEventTree& hisCrashes = graph.crashEvents.at(*it2);
			mcgraph::CrashEventTree::iterator it3 =
					hisCrashes.lower_bound( hisTime-sqrt(graph.eps.ZERO_EVENT) );

			//Search for crash(es)
			while( it3 != hisCrashes.end() )
			{
				//Too late event...
				if( (*it3).second->getTime() > hisTime+sqrt(graph.eps.ZERO_EVENT) )
					break;

				//Get iterator to remove
				mcgraph::CrashEventTree::iterator rmit = it3;
				McgraphCrashEvent* ev = (*rmit).second;
				it3++;

				//Crashes into us...
				if( ev->isTargetMc() && ev->getTargetIdx() == (int)mcidx )
				{
					loggerMcg.log(LL_DEBUG, "  erase future crash of mc %d at time %f.\n",
							*it2, TO_MDOUBLE(ev->getTime()));
					delete (*rmit).second;
					hisCrashes.erase(rmit);
				}
			}
		}


#ifndef NDEBUG
		//Search for stale events
		//This is a sanity check...
		mcgraph::CrashEventTree& hisCrashes = graph.crashEvents.at(*it2);
		mcgraph::CrashEventTree::iterator cit = hisCrashes.begin();
		while( cit != hisCrashes.end()  )
		{
			McgraphCrashEvent* ev = (*cit).second;

			//Aha! A crash into our trace...
			if( ev->isTargetMc() && ev->getTargetIdx()==int(mcidx) )
			{
				//The event is in his data-structure --> it must lie in future
				assert( ev->getTimePlusPriority() > getTimePlusPriority() - graph.eps.ZERO_EVENT );

				//Place of crash of other motorcycle
				const Motorcycle&  omc = graph.mcs.at(ev->getMcIdx());
				const Vector v = omc.getPosAtTime(ev->getTime());
				//Get time when we are there...
				const double t = mc.getTimeAtPos(v);

				//Check that v is on speed ray
				const double det = Vector::det3(v, mc.getSpeed().getStart(),
						mc.getSpeed().getEnd());
				assert( fabs(det) < graph.eps.ZERO_ANGLE );

				//We would reach it after crash...
				if( t > getTime() + graph.eps.ZERO_EVENT )
				{
					loggerMcg.log(LL_ERROR, "  erase future crash of mc %d at time %f.\n",
							*it2, TO_MDOUBLE(ev->getTime()));

					//Save iterator to rit and go on
					mcgraph::CrashEventTree::iterator rit = cit;
					cit++;

					//Delete object and node
					delete (*rit).second;
					hisCrashes.erase(rit);
					continue;
				}
			}
			cit++;
		}
#endif
	}


	//Set crashed time resp. place
	if( isTargetMc() )
	{
		loggerMcg.log(LL_DEBUG, "  recall that mc %d crashed into mc %d.\n", mcidx, getTargetIdx() );
		mc.setCrashedIntoMc(getTargetIdx(), getTime() );
	}
	else
	{
		loggerMcg.log(LL_DEBUG, "  recall that mc %d crashed into wall %d.\n", mcidx, getTargetIdx() );
		mc.setCrashedIntoWall(getTargetIdx(), getTime() );
	}

	//Log this motorcycle into crashed set
	graph.lastCrashedMcs.push_back( pair<double,unsigned>(getTime(),mcidx) );


	//Delete the events in crash-event data structure
	mcgraph::CrashEventTree::iterator it = ourCrashes.begin();
	while( it != ourCrashes.end() )
	{
		delete (*it).second;
		it++;
	}

	//Clear crash-event and switch-event data structures
	ourCrashes.clear();
	graph.switchEventTimes.at(mcidx).clear();
}









McgraphSwitchEvent::McgraphSwitchEvent(double time, unsigned mcidx) :
	McgraphEvent(time,mcidx)
{
}


void McgraphSwitchEvent::handleEvent(mcgraph& graph)
{
	//Get index of our motorcycle
	const unsigned mcidx = getMcIdx();
	Motorcycle& mc = graph.mcs.at(mcidx);

	//Check if event is still valid
	if( !isValid(graph) )
		return;


	//Print some status messages
	loggerMcg.log(LL_DEBUG, "Switch-event: time %e, mc %d.\n", TO_MDOUBLE(getTime()), mcidx);


	//Clear our binary-tree of crash-events
	mcgraph::CrashEventTree& ourCrashes = graph.crashEvents.at(mcidx);
	if( !ourCrashes.empty() )
	{
		//Delete the events in crash-event data structure
		mcgraph::CrashEventTree::iterator it = ourCrashes.begin();
		while( it != ourCrashes.end() )
		{
			delete (*it).second;
			it++;
		}

		ourCrashes.clear();
	}



	//Add new switch-event to priority-queue
	vector<double> &mySwitchEvents = graph.switchEventTimes.at(mcidx);
	while( true )
	{
		//No switch-events -- get next ones
		if( mySwitchEvents.empty() )
		{
			loggerMcg.log(LL_DEBUG, "  Empty switch-event structure.\n");
			graph.getNextSwitchEvents(mcidx, mySwitchEvents );
		}

		//No more switch-events
		if( mySwitchEvents.empty() )
		{
			loggerMcg.log(LL_DEBUG, "  Set trace to infinity.\n");
			mc.setTraceEndpointToInf();
			break;
		}

		//Get and pop the next event...
		const double time = mySwitchEvents.back();
		mySwitchEvents.pop_back();

		//Check if switch-event is indeed in strict future...
		assert( time >= getTime());
		if( time <= getTime() )
			continue;

		//Add the new event...
		graph.events.push( new McgraphSwitchEvent( time, mcidx) );
		//Extend trace to endpoint
		mc.setTraceEndpoint( mc.getPosAtTime(time) );
		break;
	}


	//Get the place and area where the event takes place
	const Vector& evplace = mc.getPosAtTime(getTime());
	const Rect evarea = Rect::fromCenter(evplace, graph.eps.ZERO, graph.eps.ZERO);


	//Check for crash into walls
	set<unsigned> res = graph.whash.getObjectsAt(evarea);
	double walltime = -1;
	int wallidx = -1;
	for(set<unsigned>::iterator it = res.begin();
		it != res.end(); it++ )
	{
		const Segment& seg = graph.walls.at(*it);
		Vector crash;
		if( !mc.getIntersection(seg, crash) )
			continue;

		const double time = mc.getTimeAtPos(crash);

		//Crash was not at start. Note: we do not say
		//time > mc.getStarttime()+graph.eps.ZERO because there is no
		//a-posteriori mc starting from polygon boundary
		if( time > graph.eps.ZERO_EVENT && (wallidx<0 || time<walltime) )
		{
			walltime = time;
			wallidx = *it;
		}
	}

	//There was a crash into polygon...
	if( wallidx >= 0 && getTime()<=walltime && walltime >= mc.getStarttime() + graph.eps.ZERO_EVENT )
	{
		loggerMcg.log(LL_DEBUG, "  we crash into wall %d, time %f\n", wallidx, TO_MDOUBLE(walltime));
		//Add crash to event-queue
		graph.events.push( McgraphCrashEvent::createCrashIntoWall(graph, walltime, mcidx, wallidx) );
		//Reduce trace...
		mc.setTraceEndpoint( mc.getPosAtTime(walltime) );
	}




	//1. Re-build our crash-event data structure
	//2. Add crashes of other mcs into us to their data structure
	set<unsigned> othermcs = graph.mchash.getObjectsAt(evarea);
	for( set<unsigned>::iterator it2 = othermcs.begin();  it2 != othermcs.end(); ++it2 )
	{
		if( *it2 == mcidx )
			continue;

		//Traces intersect --> their is a potential crash...
		const Motorcycle& omc = graph.mcs.at(*it2);
		Vector crash;
		if( mc.getCrashpoint(omc, crash) )
		{
			double ourTime = mc.getTimeAtPos(crash);
			double hisTime = omc.getTimeAtPos(crash);

			//We crashed into him, if:
			// - we are there later then he
			// - the crash occurs in future
			// - the crash does not occur after we reach polygon
			// - we do not crash at starting time

			//TODO: maybe motorcycle *it2 crashed here too;
			//lets say into mc j. Then it2 would be nice to
			//drop all crash-events except the crash into j.
			//Similar things hold, when mcidx and *it2 crash together...
			if( ourTime >  hisTime - graph.eps.ZERO_EVENT &&
				ourTime >= getTime() - graph.eps.ZERO_EVENT &&
				ourTime >= mc.getStarttime() + graph.eps.ZERO_EVENT &&
				(wallidx==-1 || ourTime < walltime) )
			{
				// if ourTime>=getTime()-graph.eps.ZERO, even though ourTime<getTime()
				ourTime = max(ourTime, getTime());
				bool priority=false;

				loggerMcg.log(LL_DEBUG, "  we crash into mc %d, time %e\n", *it2, TO_MDOUBLE(ourTime));

				// If he is there strictly earlier we reduce ourTime a bit such that
				// if we crash simultaneousely with a third motorcycle, omc is actually
				// detected as the target mc.
				if( hisTime < ourTime-graph.eps.ZERO_EVENT )
				{
					loggerMcg.log(LL_DEBUG, "    he was there strictly earlier.\n");
					priority=true;
				}

				McgraphCrashEvent* newev = McgraphCrashEvent::createCrashIntoMc(graph, ourTime, mcidx, *it2, priority);
				ourCrashes.insert( pair<double, McgraphCrashEvent*>(newev->getTimePlusPriority(), newev) );
			}

			//He crashed into us, if:
			// - he is there after we are
			// - the crash occurs in future
			// - he does not crash at starting time
			if( hisTime >  ourTime - graph.eps.ZERO_EVENT &&
				hisTime >= getTime() - graph.eps.ZERO_EVENT &&
				hisTime >= omc.getStarttime() + graph.eps.ZERO_EVENT )
			{
				// if hisTime>=getTime()-graph.eps.ZERO, even though ourTime<getTime()
				hisTime = max(hisTime, getTime());
				bool priority=false;


				// If we are there strictly earlier we reduce hisTime a bit
				// such that if he crashes simultaneousely with a third
				// motorcycle, mc is actually detected as the target mc.
				if( ourTime < hisTime-graph.eps.ZERO_EVENT )
				{
					loggerMcg.log(LL_DEBUG, "    we were there strictly earlier.\n");
					priority = true;
				}


				//Check if a new earliest crash-event has been found
				bool newEarliest=true;
				mcgraph::CrashEventTree& hisCrashes = graph.crashEvents.at(*it2);

				//If data structure is not empty and the earliest element is before
				//or at the current found...
				if( hisCrashes.begin() != hisCrashes.end() )
					if( (*hisCrashes.begin()).second->getTime() <= hisTime )
						newEarliest = false;

				//Create crash element
				McgraphCrashEvent* ev = McgraphCrashEvent::createCrashIntoMc(graph, hisTime, *it2, mcidx, priority);

				//If a new earliest crash-time is found, we add the element
				//straight to the priority queue...
				if( newEarliest )
				{
					//TODO: if we could remove from a priority_queue, we could remove
					//the older crash-events from the queue...
					graph.events.push(ev);
					loggerMcg.log(LL_DEBUG, "  mc %d crashes into us (MIN):  time %f\n", *it2, TO_MDOUBLE(hisTime));
				}
				//Otherwise, we remember the crash in the crash-event data structure
				else
				{
					loggerMcg.log(LL_DEBUG, "  mc %d crashes into us:  time %f\n", *it2, TO_MDOUBLE(hisTime));
					hisCrashes.insert( pair<double, McgraphCrashEvent*>(ev->getTimePlusPriority(), ev ));
				}
			}
		}
		else
		{
			loggerMcg.log(LL_DEBUG, "  no interaction with mc %d.\n", *it2);
		}
	}


	//Add our earliest crash-event to the event-queue and
	mcgraph::CrashEventTree::iterator eit = ourCrashes.begin();
	if( eit != ourCrashes.end() )
	{
		McgraphCrashEvent* ev = (*eit).second;
		loggerMcg.log(LL_DEBUG, "  insert our minimal crash:  time %f\n", TO_MDOUBLE(ev->getTime()));
		graph.events.push(ev);
		ourCrashes.erase(eit);
	}



	//Add us to the geometric hash
	graph.mchash.addObjectAt( mcidx, evarea);
	assert( graph.mchash.getObjectsAt(evarea).count(mcidx) > 0 );
}



