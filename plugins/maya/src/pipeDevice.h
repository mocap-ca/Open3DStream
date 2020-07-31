#ifndef PEEL_PIPE_DEVICE_H
#define PEEL_PIPE_DEVICE_H

#include "threadedDevice.h"


class PipeDevice : public ThreadedDevice
{
public:
	PipeDevice();

	static void*  creator() { return new PipeDevice; }

	virtual bool connect();
	virtual bool disconnect();
	virtual bool isConnected();
	size_t receiveData(char*, size_t);

	static MObject      port;

	int       iPort;

	HANDLE hPipe;


	static MTypeId id;
};


#endif