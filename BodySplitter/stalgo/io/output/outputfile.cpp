#include <assert.h>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <math.h>
#include <string>
#include <set>
#include <utility>


#ifdef WITH_OGL
	#include "../../glwindows/glplanewindow.h"
	#include "../../glwindows/gltextplane.h"
#endif

#include "../../fpkernel.h"

#include "outputfile.h"


using namespace std;


OutputFile::OutputFile(const string &path) :
	path(path),
	pchains(NULL),
	mcg(NULL),
	sk(NULL)
{
}
