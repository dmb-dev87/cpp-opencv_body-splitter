#include <algorithm>
#include <cassert>
#include <errno.h>
#include <map>
#include <math.h>
#include <string>
#include <cstring>
#include <vector>
#include <limits>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/resource.h>
#include <sys/time.h>
#endif


#include "timings.h"

using namespace std;


Timings::Timings()
{
}


void Timings::startTiming(string name)
{
	times[name] = pair<double,double>(0.0, getTimeUsage() );
}


void Timings::stopTiming(string name)
{
	if( times.count(name) == 0 )
		return;

	double now = getTimeUsage();
	double start = times.at(name).second;
	times[name].second = std::numeric_limits<double>::quiet_NaN();

	// We add some time we consumed
	if( now-start >= 0 )
	{
		times[name].first += (now-start);
		times[name].second = std::numeric_limits<double>::quiet_NaN();
	}
}


vector< pair<string, double> > Timings::getTimings() const
{
	vector< pair<string,double> > sum;

	map<string, pair<double,double> >::const_iterator it = times.begin();
	while( it != times.end() )
	{
		const pair<double,double> t = (*it).second;
		sum.push_back( pair<string,double> ( (*it).first, t.first) );
		it++;
	}

	sort( sum.begin(), sum.end() );
	return sum;
}


double Timings::getTimeUsage()
{
#ifdef _WIN32
	FILETIME createTime, exitTime, kernelTime, userTime;
	if( GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime, &userTime) != -1 )
	{
		__int64 hectonanoseconds = *((__int64*) &userTime);
		return 1e-7 * hectonanoseconds;
	}
	assert(false);
	return 0.0;
#else
	struct rusage uses;

	//Get resource usages and response on error
	if( getrusage(RUSAGE_SELF, &uses) == -1 )
	{
		throw string("Error when calling rusage: ") + 
			string( strerror(errno));
	}
	//Get user time
	return uses.ru_utime.tv_sec + double(uses.ru_utime.tv_usec)/1e6;
#endif
}


