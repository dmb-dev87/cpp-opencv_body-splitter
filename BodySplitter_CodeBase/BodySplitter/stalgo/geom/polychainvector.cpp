#include <iostream>
#include <list>
#include <math.h>
#include <vector>
#include <algorithm>

#include "../fpkernel.h"
#include "polychainvector.h"
#include "../constants.h"

using namespace std;
		

PolychainVector::PolychainVector(const PolychainVector& p) : 
	elems( p.elems)
{
}
	

Rect PolychainVector::getBoundingBox() const
{
	if(size()==0 )
		return Rect(HUGE_DBL,-HUGE_DBL,HUGE_DBL,-HUGE_DBL);

	Rect bbox = at(0).getBoundingBox();

	for( pchaincont::const_iterator it = elems.begin(); it != elems.end(); it++ )
	{
		const Rect box = (*it).getBoundingBox();
		bbox.right = max(bbox.right, box.right);
		bbox.left = min(bbox.left, box.left);
		bbox.top = max(bbox.top, box.top);
		bbox.bottom = min(bbox.bottom, box.bottom);
	}

	return bbox;
}

		
void PolychainVector::reverse()
{
	pchaincont::iterator it = elems.begin();

	while( it != elems.end() )
	{
		(*it).reverse();
		it++;
	}

}


unsigned PolychainVector::getNoCwVertices() const
{
	unsigned no=0;
	pchaincont::const_iterator it = elems.begin();

	while( it != elems.end() )
	{
		no += (*it).getNoCwVertices();
		it++;
	}

	return no;
}


unsigned PolychainVector::getNoVertices() const
{
	unsigned no=0;
	for( pchaincont::const_iterator it = elems.begin();  it != elems.end(); it++ )
		no += (*it).size();
	return no;
}


unsigned PolychainVector::getNoSegs() const
{
	unsigned no=0;
	for( pchaincont::const_iterator it = elems.begin();  it != elems.end(); it++ )
		no += (*it).getNoSegs();
	return no;
}


