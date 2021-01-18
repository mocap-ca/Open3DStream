#pragma once

#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "Containers/Ticker.h"

#include "Open3DStreamSourceSettings.h"
#include "Open3DServer.h"
#include "o3ds/model.h"

#include "LiveLinkRole.h"

#define NET_BUFFER_SIZE (1024 * 48)

class ILiveLinkClient;

/*
https://docs.unrealengine.com/en-US/API/Runtime/LiveLinkInterface/ILiveLinkSource/index.html

E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\
    LiveLinkMessageBusSource.h
*/

class OPEN3DSTREAM_API FOpen3DStreamSource : public ILiveLinkSource, public TSharedFromThis<FOpen3DStreamSource>
{
public:
	FOpen3DStreamSource();
	FOpen3DStreamSource(const FOpen3DStreamSettings& Settings);
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

	// Settings
	virtual void InitializeSettings(ULiveLinkSourceSettings* Settings);
	virtual TSubclassOf < ULiveLinkSourceSettings > GetSettingsClass() const;
	
	ULiveLinkSourceSettings* Settings;

	ILiveLinkClient* Client;
	FGuid            SourceGuid;
	TArray<FName>    InitializedSubjects;	
	bool             bIsInitialized;
	FText            Url;
	FText            Key;
	FText            Protocol;
	float            TimeOffset;
	double           ArrivalTimeOffset;

	FThreadSafeBool  bIsValid;

	UServer server;
	std::vector<char> buffer;

	O3DS::SubjectList mSubjects;


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

