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
#include "polychainsfilter.h"

#include "../../io.h"



PolychainsFilter::PolychainsFilter() :
	prev(0)
{
}

PolychainsFilter::PolychainsFilter(PolychainsFilter& prev) :
	prev( &prev )
{
}

void PolychainsFilter::applyAll(PolychainVector& p) 
{
	if( prev )
		prev->applyAll(p);
	applyThis(p);
}


