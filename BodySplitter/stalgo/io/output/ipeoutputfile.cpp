#include <assert.h>
#include <fstream>
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
	#include "../../glwindows/glplanewindow.h"
	#include "../../glwindows/gltextplane.h"
#endif

#include "../../fpkernel.h"

#include "ipeoutputfile.h"

#include "../io.h"

#include "../../geom/kineticslgraph.h"
#include "../../straightskeleton/straightskeleton.h"

using namespace std;



const Vector IpeOutputFile::midVector = Vector(300,450);
const double IpeOutputFile::scale = 200;
const double IpeOutputFile::maxDist = 2e1;


IpeOutputFile::IpeOutputFile(const string &path, bool useIpe7) :
	OutputFile(path),
	useIpe7(useIpe7)
{
}


void IpeOutputFile::save()
{
	loggerIo.log(LL_GOODNEWS, "Output result to ipe xml-file `%s`...\n", getPath().c_str());

	ofstream ofs( getPath().c_str(), ofstream::out );
	ofs.exceptions ( ofstream::eofbit | ofstream::failbit | ofstream::badbit );
	ofs.precision(16);


	// Ipe 6 file format
	if( !useIpe7 )
	{
		ofs << "<ipe version=\"00000\" creator=\"Stalgo\">" << endl;
		ofs << "<info bbox=\"cropbox\"/>" << endl;
	}
	// Ipe 7 file format
	else
	{
		ofs << "<?xml version=\"1.0\"?>" << endl;
		ofs << "<!DOCTYPE ipe SYSTEM \"ipe.dtd\">" << endl;

		ofs	<< "<ipe version=\"70000\" creator=\"Stalgo\">" << endl
			<< "<info bbox=\"cropbox\"/>" << endl << endl;

		ofs	<< "<ipestyle name=\"stalgo\">" << endl
			<< "  <color name=\"red\" value=\"1 0 0\"/>" << endl
			<< "  <color name=\"green\" value=\"0 1 0\"/>" << endl
			<< "  <color name=\"blue\" value=\"0 0 1\"/>" << endl
			<< "  <color name=\"gray4\" value=\"0.5 0.5 0.5\"/>" << endl
			<< "  <pen name=\"heavier\" value=\"0.8\"/>" << endl
			<< "  <symbolsize name=\"tiny\" value=\"1.1\"/>" << endl
			<< "</ipestyle>" << endl;
	}


	ofs << "<page gridsize=\"16\">" << endl;

	//Define layers
	if( hasPolychains() )
		ofs << "<layer name=\"polychains\"/>" << endl;
	if( hasMcgraph() )
		ofs << "<layer name=\"mcgraph\"/>" << endl;
	if( hasStraightSkeleton() )
	{
		ofs << "<layer name=\"sk\"/>" << endl;

		if( !getStraightSkeleton().isComputed() )
			ofs << "<layer name=\"sk-wf\"/>" << endl;
		else
		{
			if( hasStraightSkeletonOffset() )
				ofs << "<layer name=\"offsets\"/>" << endl;
		}
	}

	ofs << endl;


	if( hasPolychains() )
	{
		PolychainVector::pchaincont::const_iterator it = getPolychains().getElements().begin();

		ofs << "<group layer=\"polychains\">" << endl;

		while( it != getPolychains().getElements().end() )
		{
			const vector<Vector> vert = (*it).getVertices();

			ofs << "<path layer=\"polychains\" stroke=\"black\" pen=\"heavier\">" << endl;
			const Vector v = project(vert.at(0));
			ofs << "  " << v.x << " " << v.y << " m" << endl;

			for( unsigned i=1; i<vert.size(); i++)
			{
				const Vector v = project(vert.at(i));
				ofs << "  " << v.x << " " << v.y << " l" << endl;
			}

			//Close polychain
			if( (*it).isClosed() )
				ofs << "  " << v.x << " " << v.y << " l" << endl;

			ofs << "</path>" << endl;
			ofs << endl;
			it++;
		}

		ofs << "</group>" << endl;
	}


	if( hasMcgraph() )
	{
		const mcgraph& mcg = getMcgraph();
		const vector<Motorcycle>& mcs = mcg.getMotorcycles();


		//Print the traces...
		ofs << "<group layer=\"mcgraph\">" << endl;
		vector<Motorcycle>::const_iterator it = mcs.begin();
		while( it != mcs.end() )
		{
			const Motorcycle& mc = *it;

			const Segment& t = mc.getTrace();
			ofs << "  <path stroke=\"red\" pen=\"normal\">" << endl;

			const Vector s = project(t.getStart());
			ofs << "    " << s.x << " " << s.y << " m" << endl;


			const double time = min(mc.getTimeAtPos(t.getEnd()),
					mc.isCrashed() ? mc.getCrashtime() : mcg.getLastEventTime());
			const Vector& currpos = mc.getPosAtTime(time);
			const Vector e = project(currpos);
			ofs << "    " << e.x << " " << e.y << " l" << endl;

			ofs << "  </path>" << endl;
			it++;
		}
		ofs << "</group>" << endl;
	}

	if( hasStraightSkeleton() )
	{
		const KineticSLGraph& sk = getStraightSkeleton().getStraightSkeleton();
		const KineticSLGraph& wf = getStraightSkeleton().getWavefront();
		const double time = getStraightSkeleton().getLastEventTime();

		outputKslg(ofs, sk, "sk", "blue");

		if( !getStraightSkeleton().isComputed() )
			outputKslg(ofs, wf, "sk-wf", "green", time);

		else
		{
			vector<double> skOffset = getStraightSkeletonOffset();
			for( vector<double>::const_iterator it =skOffset.begin();
					it != skOffset.end(); it++ )
			{
				const double offset = *it;

				KineticSLGraph curves;
				getStraightSkeleton().computeOffsetCurve(curves, offset);
				outputKslg(ofs, curves, "offsets", "gray4");
			}
		}
	}

	ofs << "</page>" << endl;
	ofs << "</ipe>" << endl;
}



void IpeOutputFile::outputKslg(std::ofstream& ofs, const KineticSLGraph& graph, const
				std::string& layer, const std::string& stroke, double time)
{
	//Print the arcs...
	ofs << "<group layer=\"" << layer << "\">" << endl;
	set<unsigned> edges = graph.getEdgeSet();
	for( set<unsigned>::iterator it = edges.begin();
			it != edges.end(); it++)
	{
		const Segment& arc = project(graph.getEdgeSegment(*it, time));
		ofs << "  <path stroke=\"" << stroke << "\" pen=\"normal\">" << endl;

		const Vector s = arc.getStart();
		ofs << "    " << s.x << " " << s.y << " m" << endl;
		const Vector e = arc.getEnd();
		ofs << "    " << e.x << " " << e.y << " l" << endl;

		ofs << "  </path>" << endl;
	}

	ofs << "</group>" << endl;
}


Vector IpeOutputFile::project(const Vector&v) const
{
	return midVector + scale*v;
}

Segment IpeOutputFile::project(const Segment& seg) const
{
	Vector s = seg.getStart();
	Vector e = seg.getEnd();

	if( s.lensq() < e.lensq() )
		swap(s,e);

	// Bring infinity to our real world. ps or pdf doesn't like
	// too large lines
	if( s.len()>maxDist )
		s = e + maxDist*(s-e).norm();
	else if( e.len()>maxDist )
		e = s + maxDist*(e-s).norm();

	return Segment(project(s), project(e));
}

