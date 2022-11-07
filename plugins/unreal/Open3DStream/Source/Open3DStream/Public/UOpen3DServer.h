#pragma once

#include "CoreMinimal.h"

#include "Common/UdpSocketReceiver.h"
#include "o3ds/base_connector.h"
#include "o3ds/udp_fragment.h"
#include "Sockets.h"


DECLARE_DELEGATE_OneParam(FOnO3dsData, const TArray<uint8>&);
DECLARE_DELEGATE_TwoParams(FOnState, const FText, bool warning);


// NNG Async Callback
void InDataFunc(void *ptr, void *msg, size_t len);

class O3DSServer 
{
public:

	O3DSServer();
	~O3DSServer();

	bool start(FText Url, FText Protocol);
	void stop();
	bool write(const char *data, size_t len);

	void inData(const uint8 *msg, size_t len);

	bool ReadTcp(size_t len);

	void tick();

	O3DS::AsyncConnector* mServer;
	FSocket* mTcp;
	FSocket* mUdp;

	FUdpSocketReceiver* mUdpReceiver;
	UdpMapper mUdpMapper;

	FOnO3dsData OnData;
	FOnState    OnState;

	TArray<uint8> buffer;

	uint8*  mBuffer;
	size_t  mBufferSize;
	uint8   mHeader[10];
	size_t  mPtr;
	float   mGoodTime;
	bool    mNoDataFlag;

	enum class eState { SYNC, HEADER, DATA } mState;

};

