#include "o3ds/o3ds.h"

#define PQ(x) #x
#define PQUOTE(x) PQ(x)

const char* O3DS::version       = PQUOTE(O3DS_VERSION);
const int   O3DS::version_major = O3DS_VERSION_MAJOR;
const int   O3DS::version_minor = O3DS_VERSION_MINOR;
const int   O3DS::version_patch = O3DS_VERSION_PATCH;

const char* O3DS::getVersion() { return O3DS::version; }