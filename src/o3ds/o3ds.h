#ifndef O3DS_HEADER
#define O3DS_HEADER


#define O3DS_VERSION_MAJOR 0
#define O3DS_VERSION_MINOR 9
#define O3DS_VERSION       "0.9"

namespace O3DS
{
	static const char* version = O3DS_VERSION;
	static const int   version_major = O3DS_VERSION_MAJOR;
	static const int   version_minor = O3DS_VERSION_MINOR;

	const char* getVersion() { return version; }
}

#endif