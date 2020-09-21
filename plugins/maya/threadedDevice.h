#ifndef PEEL_THREADED_DEVICE_H
#define PEEL_THREADED_DEVICE_H

#include <maya/MPxThreadedDeviceNode.h>
#include <maya/MStatus.h>
#include <maya/MStringArray.h>
#include "o3ds/base_server.h"
#include "o3ds/model.h"


#ifdef _WIN32
#include <Windows.h>
#endif

#define MODE_SUBSCRIBE  0

class ThreadedDevice : public MPxThreadedDeviceNode
{
public:
    ThreadedDevice() {};
    ~ThreadedDevice() ;

    //void threadHandler(const char* serverName, const char *deviceName );
    void threadHandler();
    void threadShutdownHandler();
    void postConstructor();

    virtual MStatus     compute( const MPlug& plug, MDataBlock& data );

    static MStatus      initialize();

	static void*  creator() { return new ThreadedDevice(); }

    void passMessage(const char*message, size_t msglen=0, unsigned char mode=0);

	//virtual size_t receiveData(char *, size_t);

    static MObject      mocap;
	static MObject      scale;
    static MObject      outputName;
    static MObject      outputTranslate;
    static MObject      outputRotate;
    static MObject      url;
	static MObject      info;
	static MObject      mode;
	static MObject      subject;

	static MTypeId id;

	O3DS::BlockingConnector *mConnector;
	O3DS::SubjectList mSubjectList;

    MStringArray  mNames;
	MString       mUrl;
	MString       mSubject;
	int           mMode;

	std::vector<std::string> mNameMap;


#ifdef _WIN32
	HANDLE hPipe;
#endif

};
#endif // PEEL_THREADED_DEVICE_H
