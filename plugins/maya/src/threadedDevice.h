#ifndef PEEL_THREADED_DEVICE_H
#define PEEL_THREADED_DEVICE_H

#include <maya/MPxThreadedDeviceNode.h>
#include <maya/MStatus.h>
#include <maya/MStringArray.h>
#include "udpSocket.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#define MODE_UDP    0
#define MODE_PIPE   1
#define MODE_MOTIVE 2

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

    //size_t              parse(const char *buffer, std::vector<Item> &items );
    void sendData(const char*message, size_t msglen=0, const char* data=NULL, size_t datalen = 0);

	virtual bool connect(); 
    virtual bool isConnected();
	virtual bool disconnect();
	virtual size_t receiveData(char *, size_t);


	static MObject      fileName;
	static MObject      save;
    static MObject      mocap;
	static MObject      scale;
    static MObject      outputName;
    static MObject      outputTranslate;
    static MObject      outputRotate;
    static MObject      info;
	static MObject      port;
	static MObject      mode;

	static MTypeId id;

    MStringArray names;

	UdpServer socket;

	MString mFileName;
	volatile bool    mSave;
	FILE *fp;
	int       iPort;
	int       mMode;

#ifdef _WIN32
	HANDLE hPipe;
#endif

};
#endif // PEEL_THREADED_DEVICE_H
