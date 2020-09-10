#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <ctype.h>

#include "util.h"

using namespace std;

Logger loggerCommon;
Timings timings;



list<string> tokenize(const string& str, const string sep)
{
	list<string> tokens;
	size_t pos=0;

	// Seperator has no length
	if( sep.empty() )
	{
		tokens.push_back(str);
		return tokens;
	}


	size_t newpos;
	do
	{
		newpos = str.find(sep, pos);
		tokens.push_back( str.substr(pos, newpos-pos) );
		pos = newpos + sep.size();
	}while( newpos != string::npos );

	return tokens;
}


string lstrip(const string& str)
{
	unsigned s=0;
	while( s<str.size() && isspace(str[s]) )
		s++;

	return str.substr(s);
}

string rstrip(const string& str)
{
	int s=str.size()-1;
	while( s>=0 && isspace(str[s]) )
		s--;

	return str.substr(0, s+1);
}

string strip(const string& str)
{
	return lstrip(rstrip(str));
}
