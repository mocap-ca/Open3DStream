#include "Open3DStreamSource.h"
#include "LiveLinkRoleTrait.h"


FOpen3DStreamSource::FOpen3DStreamSource(int InPort, double InTimeOffset)
	: bIsInitialized(false)
	, Port(InPort)
	, TimeOffset(InTimeOffset)
	, bIsValid(false)
{
	SourceStatus = NSLOCTEXT("Open3DStream", "ConnctionStatus", "Inactive");
	SourceType = NSLOCTEXT("Open3DStream", "ConnctionType", "Open 3D Stream");
	SourceMachineName = NSLOCTEXT("Open3DStream", "SourceMachineName", "-");
}

FOpen3DStreamSource::~FOpen3DStreamSource()
{
}

void FOpen3DStreamSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	Client = InClient;
	SourceGuid = InSourceGuid;
	bIsValid = true;

	for (const TSubclassOf<ULiveLinkRole>& RoleClass : FLiveLinkRoleTrait::GetRoles())
	{
		RoleInstances.Add(RoleClass->GetDefaultObject<ULiveLinkRole>());
	}

	UpdateConnectionLastActive();
}

bool FOpen3DStreamSource::RequestSourceShutdown()
{
	Client = nullptr;
	SourceGuid.Invalidate();
	return true;
}


void FOpen3DStreamSource::Update()
{}

FORCEINLINE void FOpen3DStreamSource::UpdateConnectionLastActive()
{
	FScopeLock ConnectionTimeLock(&ConnectionLastActiveSection);
	ConnectionLastActive = FPlatformTime::Seconds();
}

#undef LOCTEXT_NAMESPACE