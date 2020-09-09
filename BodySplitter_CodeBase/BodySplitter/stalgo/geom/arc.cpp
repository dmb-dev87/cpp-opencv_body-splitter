#include <assert.h>
#include <iostream>
#include <list>
#include <math.h>
#include <vector>
#include <algorithm>

#include "../fpkernel.h"
#include "arc.h"
#include "../constants.h"

using namespace std;



double Arc::getAngle() const
{
	const Vector sc = start() - center();
	const Vector ec = end() - center();

	const double phie = atan2(ec.y, ec.x);
	const double phis = atan2(sc.y, sc.x);

	double phi = phie - phis;
	if(phi < C_0 )
		phi += C_2*M_PI;

	return phi;
}


Polychain Arc::sampleByEps(double eps) const
{
	assert( eps > 0 );

	// Create polychain starting at s
	Polychain res;
	res.addVertex(s);


	// We have to add intermediate sample points
	if( radsq() >= eps*eps )
	{
		// Error made when using delta-phi of 'dphi':
		//  r(1 - cos( dphi/2 )) = eps
		//            1-eps/r = cos(dphi/2)
		// 2 * acos( 1-eps/r) = dphi
		
		assert( 1 - eps/rad() >= 0);
		const double dphi = C_2 * acos(C_1 - eps/rad() );

		// Get the number 'n' of approximating segments
		const double phi = getAngle();
		const unsigned n = uintceil( phi/dphi );

		const Vector sc = start()-center();

		for( unsigned i=1; i<n; i++)
		{
			const double midphi = i*dphi;
			assert( midphi < phi );
			const Vector mid = center() + sc.rot(midphi);
			res.addVertex(mid);
		}
	}


	// Add end of of arc
	res.addVertex(e);
	return res;
}


void Arc::calibrateCenter()
{
	const Vector es =  (end()-start()).norm().ccw();
	const Vector mid = (end()+start())/C_2;

	// Distance mid to center
	const double dist = sqrt( max(C_0, radsq() - (e-s).lensq()/C_4) );

	if( Vector::det3(s, e, c) > 0 )
		c = mid + dist*es;
	else
		c = mid - dist*es;
}


