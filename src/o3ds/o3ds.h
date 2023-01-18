#ifndef O3DS_HEADER
#define O3DS_HEADER

#define PQ(x) #x
#define PQUOTE(x) PQ(x)


namespace O3DS
{
	static const char* version = PQUOTE(O3DS_VERSION);
	static const int   version_major = O3DS_VERSION_MAJOR;
	static const int   version_minor = O3DS_VERSION_MINOR;
    static const int   version_patch = O3DS_VERSION_PATCH;

	const char* getVersion() { return version; }
}

#endif