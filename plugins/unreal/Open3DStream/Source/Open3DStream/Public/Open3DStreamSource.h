#pragma once

#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "Containers/Ticker.h"

#include "Open3DServer.h"

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
	FOpen3DStreamSource(const FText &InUrl, double InTimeOffset);
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
	FText            Url;
	float            TimeOffset;
	double           ArrivalTimeOffset;

	FThreadSafeBool  bIsValid;

	O3DS_Server server;

	void OnNnpData();

	void OnPackage(uint8 *data, size_t sz);

	FORCEINLINE void UpdateConnectionLastActive();
	FCriticalSection ConnectionLastActiveSection;
	double ConnectionLastActive;

	virtual bool IsSourceStillValid() const override
	{
		return Client != nullptr && bIsValid;
	}

	virtual void Update() override;


};

