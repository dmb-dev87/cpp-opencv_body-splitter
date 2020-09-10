#pragma once
/* Must be created in a valid glcontext
Used to determine which extensions are available to us */
#include "Singleton.h"

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <string>

class OpenglExt :
	public Singleton<OpenglExt>
{
private:
	const GLubyte* strExtentions;
	unsigned int major_version;
	unsigned int minor_version;
public:
	OpenglExt();
	~OpenglExt();
	bool isSupported(const std::string &name) const;
	bool isSupported(const char* ptrName) const;
	unsigned int getMajorVersion() const
	{
		return major_version;
	}
	unsigned int getMinorVersion() const
	{
		return minor_version;
	}
};

