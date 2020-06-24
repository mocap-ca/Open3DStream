#include <fbsdk/fbsdk.h>

FBLibraryDeclare(Open3D_Device)
{
	FBLibraryRegister(Open3D_Device);
	FBLibraryRegister(Open3D_Device_Layout	);
}
FBLibraryDeclareEnd;

bool FBLibrary::LibInit()	{ return true; }
bool FBLibrary::LibOpen()	{ return true; }
bool FBLibrary::LibReady()	{ return true; }
bool FBLibrary::LibClose()	{ return true; }
bool FBLibrary::LibRelease(){ return true; }