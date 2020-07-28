#pragma once

#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "Containers/Ticker.h"

#include "Runtime/Sockets/Public/Sockets.h"
#include "Runtime/Networking/Public/Common/TcpSocketBuilder.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Common/UdpSocketReceiver.h"


#include "Open3DBuffer.h"
#include "Open3DTcpThread.h"

#include "LiveLinkRole.h"

#define NET_BUFFER_SIZE (1024 * 48)

class ILiveLinkClient;

/*
https://docs.unrealengine.com/en-US/API/Runtime/LiveLinkInterface/ILiveLinkSource/index.html

E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\
    LiveLinkMessageBusSource.h
*/

class OPEN3DSTREAM_API FOpen3DStreamSource : public ILiveLinkSource
{
public:
	FOpen3DStreamSource(int InPort, double InTimeOffset);
	virtual ~FOpen3DStreamSource();

	// ILiveLinkSource Overrides
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
	virtual bool RequestSourceShutdown() override;

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;

	virtual FText GetSourceType() const override { return SourceType; };
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }

	ILiveLinkClient* Client;
	FGuid            SourceGuid;
	TArray<FName>    InitializedSubjects;	
	bool             bIsInitialized;
	int              Port;
	float            TimeOffset;
	double           ArrivalTimeOffset;

	FThreadSafeBool  bIsValid;
	bool             bUdp;


	Open3DBuffer     buffer;
	uint8            temp_buffer[1024 * 12];

	O3DS_TcpThread  *TcpThread;
	FSocket         *UdpSocket;
	FUdpSocketReceiver* Receiver;

	bool OnTcpData(FSocket *);
	void OnUdpData(const FArrayReaderPtr&, const FIPv4Endpoint&);
	void OnPackage(uint8 *data, size_t sz);

	FORCEINLINE void UpdateConnectionLastActive();
	FCriticalSection ConnectionLastActiveSection;
	double ConnectionLastActive;

	virtual bool IsSourceStillValid() const override
	{
		return Client != nullptr && bIsValid;
	}

	//TArray<TWeakObjectPtr<ULiveLinkRole>> RoleInstances;
	virtual void Update() override;


};

