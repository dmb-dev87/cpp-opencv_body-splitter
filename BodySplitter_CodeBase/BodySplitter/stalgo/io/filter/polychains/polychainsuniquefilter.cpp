#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "../../../util/logger.h"
#include "../../../fpkernel.h"

#include "../../../geom/polychainvector.h"
#include "polychainsuniquefilter.h"

#include "../../io.h"

using namespace std;


PolychainsUniqueFilter::PolychainsUniqueFilter(double minDist) :
	PolychainsFilter(),
	minDist(minDist)
{
}


PolychainsUniqueFilter::PolychainsUniqueFilter(PolychainsFilter& prev, double minDist) :
	PolychainsFilter(prev),
	minDist(minDist)
{
}


void PolychainsUniqueFilter::applyThis(PolychainVector& pchains)
{
	loggerIo.log(LL_DEBUG, "Apply unique filter...\n");

	unsigned norm=0;

	//Shift/scale the vertices
	for(unsigned c=0; c<pchains.size(); c++)
	{
		Polychain& ch = pchains[c];
		vector<Vector>& vs = ch.getVertices();

		if( vs.size() <= 1 )
			continue;


		vector<Vector>::iterator vit = vs.begin();
		Vector last = *vit;
		vit++;
		Vector curr = *vit;

		//Check for close adjacent vertices in polychain...
		while( vit != vs.end() )
		{
			curr = *vit;

			//Remove vertex...
			if( (last-curr).len() < minDist )
			{
				norm++;
				vit = vs.erase(vit);
			}
			//Go to next one
			else
			{
				last = curr;
				vit++;
			}
		}


		//Check if tails are connected...
		curr = *(vs.begin());
		vit = vs.end();
		vit--;
		last = *vit;


		if( (curr-last).len() < minDist )
		{
			loggerIo.log(LL_DEBUG, "Close chain %d.\n", c);

			// Avoid removing a two-chain completely, if those two vertices coincide
			if( vs.size() > 1 )
				vs.erase(vit);
			ch.setClosed(true);
		}
	}

	if( norm > 0 )
		loggerIo.log(LL_DEBUG, "Removed %d vertices that were to close.\n", norm);
}
