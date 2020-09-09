
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "logger.h"

using namespace std;


bool Logger::lastrelog = false;

Logger::Logger() :
	minLevel(LL_INFO),
#ifdef _WIN32
	useColor(false),
#else
	useColor(true),
#endif
	cntErrors(0),
	cntWarnings(0)
{
}


bool Logger::doPrint(loglevel level) const
{
	//Log-level not fulfilled
	if( level < minLevel )
		return false;

	//It is not allowed to print in silence level
	if( level >= LL_SILENCE )
		return false;

	return true;
}


void Logger::log(loglevel level, const char* fmt, ... )
{
	//Save some statistics
	if(level == LL_ERROR)
		cntErrors++;
	if(level == LL_WARNING)
		cntWarnings++;


	//Check if level suffices...
	if( ! doPrint(level) )
		return;

	//Default output of log message
	//ostream & os  = level >= LL_WARNING ? cerr : cout;
	ostream & os  = cout;

	char text[1024];
	memset(text, 0, sizeof(char)*1023);

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(text, 1023, fmt, ap);
	va_end(ap);


	//Make a new line if last cmd was a relog command
	if(lastrelog)
	{
		lastrelog=false;
		os << endl;
	}
	
	//Switch color
	if( useColor )
	{
		switch( level )
		{
			case LL_ERROR:
				os << "\033[1;31m";
				break;
			case LL_WARNING:
				os << "\033[1;33m";
				break;
			case LL_GOODNEWS:
				os << "\033[1;32m";
				break;
			case LL_DEBUG:
				os << "\033[1;30m";
				break;
			default:
				break;
		}
	}


	switch( level )
	{
		case LL_ERROR:
			os << "LL_ERROR: ";
			break;
		case LL_WARNING:
			os << "LL_WARNING: ";
			break;
		default:
			break;
	}

	
	os << text;


	if( useColor )
	{
		if( level==LL_ERROR || level==LL_WARNING || level==LL_GOODNEWS || level==LL_DEBUG )	
			os << "\033[0;m";
	}

	os.flush();
}


void Logger::log( loglevel level, string &str)
{
	log(level, str.c_str());
}


void Logger::relog(loglevel level, const char* fmt, ... )
{	
	char text[1024];
	memset(text, 0, sizeof(char)*1023);

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(text, 1023, fmt, ap);
	va_end(ap);

	lastrelog = false;
	log(level, "\r%s", text);

	//Check if level suffices...
	if( doPrint(level) )
		lastrelog = true;
}

