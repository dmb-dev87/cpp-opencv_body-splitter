#include "OpenglExt.h"
#include <cstring>
#include "Settings.h"
#include <spdlog\spdlog.h>

OpenglExt::OpenglExt()
{
#ifndef CONSOLE
	strExtentions = glGetString(GL_EXTENSIONS);
#endif
	if (strExtentions == nullptr)
	{
		//Needs a valid gl context...
		throw;
	}
#ifndef CONSOLE
	std::string versionStr((char*)glGetString(GL_VERSION));
	major_version = std::stoi(versionStr.substr(0, versionStr.find(".")));
	minor_version = std::stoi(versionStr.substr(versionStr.find(".")+1,versionStr.find(".", versionStr.find(".") + 1)));
	auto logger = spdlog::get("DEBUG_FILE");
	logger->info("OpenGL Version: " + versionStr);
#endif
}


OpenglExt::~OpenglExt()
{
}

bool OpenglExt::isSupported(const std::string &name) const
{
	return (strstr((const char *)strExtentions, name.c_str())
		!= NULL);
}

bool OpenglExt::isSupported(const char* ptrName) const
{
	return (strstr((const char *)strExtentions, ptrName)
		!= NULL);
}