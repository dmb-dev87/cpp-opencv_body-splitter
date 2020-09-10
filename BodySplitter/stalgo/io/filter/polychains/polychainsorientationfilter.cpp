#include <assert.h>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "../../../util/logger.h"
#include "../../../fpkernel.h"

#include "../../../geom/polychainvector.h"
#include "polychainsorientationfilter.h"

#include "../../io.h"


using namespace std;


PolychainsOrientationFilter::PolychainsOrientationFilter() :
	PolychainsFilter()
{
}

PolychainsOrientationFilter::PolychainsOrientationFilter(PolychainsFilter& prev) :
	PolychainsFilter(prev)
{
}


void PolychainsOrientationFilter::applyThis(PolychainVector& pchains)
{
	loggerIo.log(LL_DEBUG, "Apply orientation filter...\n");

	for( unsigned pos=0; pos<pchains.size(); pos++ )
	{
		const Polychain& p = pchains.at(pos);

		if( ! p.isClosed() )
			continue;

		const vector<Vector>& vs = p.getVertices();
		if( vs.size() < 3 )
			continue;

		//find startidx -- the vertex with minimum y-coordinate
		unsigned startidx=0;
		for( unsigned idx=1; idx<vs.size(); idx++)
			if( vs.at(idx).y < vs.at(startidx).y )
				startidx = idx;

		unsigned pre = (startidx+vs.size()-1) % vs.size();
		unsigned post = (startidx+1) % vs.size();

		//Get vertices and determinant
		const Vector& a = vs.at(pre);
		const Vector& c = vs.at(post);

		// Reverse orientation
		if( a.x > c.x )
			pchains.reverse();
		// Reverse all or nothing
		return;
	}
}
