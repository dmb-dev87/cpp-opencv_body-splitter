#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>

#include "../../../util/logger.h"
#include "../../../fpkernel.h"

#include "../../../geom/polychainvector.h"
#include "polychainsscalefilter.h"

#include "../../io.h"

using namespace std;


PolychainsScaleFilter::PolychainsScaleFilter(const Vector & shift) :
	PolychainsFilter(),
	scale(1),
	shift(shift)
{
}
PolychainsScaleFilter::PolychainsScaleFilter(PolychainsFilter& prev, const Vector & shift) :
	PolychainsFilter(prev),
	scale(1),
	shift(shift)
{
}


void PolychainsScaleFilter::applyThis(PolychainVector& pchains)
{
	loggerIo.log(LL_DEBUG, "Apply scale filter...\n");

	//Get the bounding box
	const Rect bbox = pchains.getBoundingBox();

	const machine_double _shiftx = TO_MDOUBLE((bbox.left+bbox.right)/2);
	const machine_double _shifty = TO_MDOUBLE((bbox.bottom+bbox.top)/2);
	const machine_double _scale = TO_MDOUBLE(max( bbox.right-bbox.left, bbox.top-bbox.bottom));

	//Shift vector
	shift = Vector( _shiftx, _shifty);
	//Scale vector
	scale = _scale;

	// No scaling
	if( fabs(scale) <= 0.0 )
		return;
	
	//Shift/scale the vertices
	PolychainVector::pchaincont::iterator it = pchains.getElements().begin();
	while( it != pchains.getElements().end() )
	{
		Polychain& ch = *it;
		vector<Vector>& vs = ch.getVertices();

		for( unsigned i=0; i<ch.size(); i++)
		{
			vs[i]  -= shift;
			vs[i]  /= scale;
		}
		it++;
	}
}
