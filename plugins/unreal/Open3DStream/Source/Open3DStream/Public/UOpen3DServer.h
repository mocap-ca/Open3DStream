#pragma once

#include "CoreMinimal.h"

#include "Common/UdpSocketReceiver.h"
#include "o3ds/base_connector.h"
#include "Sockets.h"


DECLARE_DELEGATE_OneParam(FOnO3dsData, const TArray<uint8>&);



// NNG Async Callback
void InDataFunc(void *ptr, void *msg, size_t len);

class O3DSServer 
{
public:

	O3DSServer();
	~O3DSServer();

	bool start(const char *url, const char *protocol);
	void stop();
	bool write(const char *data, size_t len);

	void inData(const uint8 *msg, size_t len);

	O3DS::AsyncConnector* mServer;
	FSocket* mTcp;
	FSocket* mUdp;

	FUdpSocketReceiver* mUdpReceiver;

	FOnO3dsData OnData;

	TArray<uint8> buffer;

};

