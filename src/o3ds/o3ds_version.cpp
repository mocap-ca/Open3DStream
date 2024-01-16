#include "o3ds_version.h"

#define QUOTE_(str) #str
#define QUOTE(str) QUOTE_(str)

std::string O3DS::getVersion()
{
	return std::string(QUOTE(VERSION_TAG));
}
