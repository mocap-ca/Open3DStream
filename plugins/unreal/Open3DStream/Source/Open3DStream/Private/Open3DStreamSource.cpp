#include "Open3DStreamSource.h"
#include "LiveLinkRoleTrait.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Common/UdpSocketBuilder.h"

#include "schema_generated.h"
//#include "get_time.h"
using namespace MyGame::Sample;


// E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\LiveLinkMessageBusSource.cpp
// E:\Unreal\UE_4.25\Engine\Source\Runtime\LiveLinkInterface\Public\LiveLinkTypes.h
// E:\Unreal\UE_4.25\Engine\Plugins\Runtime\AR\Apple\AppleARKit\Source\AppleARKitPoseTrackingLiveLink\Private\AppleARKitPoseTrackingLiveLinkSource.cpp

FOpen3DStreamSource::FOpen3DStreamSource(int InPort, double InTimeOffset)
	: bIsInitialized(false)
	, Port(InPort)
	, TimeOffset(InTimeOffset)
	, ArrivalTimeZeroA(0.0)
	, ArrivalTimeZeroB(0.0)
	, bIsValid(false)
	, bUdp(true)
	, TcpThread(nullptr)
{
	SourceStatus = NSLOCTEXT("Open3DStream", "ConnctionStatus", "Inactive");
	SourceType = NSLOCTEXT("Open3DStream", "ConnctionType", "Open 3D Stream");
	SourceMachineName = NSLOCTEXT("Open3DStream", "SourceMachineName", "-");
}

FOpen3DStreamSource::~FOpen3DStreamSource()
{
	TcpThread->Stop();
}

void FOpen3DStreamSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	// "The source has been added to the Client and a Guid has been associated"
	Client = InClient;
	SourceGuid = InSourceGuid;
	bIsValid = true;

	/*for (const TSubclassOf<ULiveLinkRole>& RoleClass : FLiveLinkRoleTrait::GetRoles())
	{
		RoleInstances.Add(RoleClass->GetDefaultObject<ULiveLinkRole>());
	}*/

	if (bUdp)
	{
		this->UdpSocket = FUdpSocketBuilder(TEXT("Open3DStreamUDP"))
			.AsNonBlocking()
			.AsReusable()
			.BoundToAddress(FIPv4Address::Any)
			.BoundToPort(Port)
			.Build();

		Receiver = new FUdpSocketReceiver(this->UdpSocket, FTimespan::FromMilliseconds(100), TEXT("Open3DStreamUDPListener"));
		Receiver->OnDataReceived().BindRaw(this, &FOpen3DStreamSource::OnUdpData);
		Receiver->Start();
	}
	else
	{
		this->TcpThread = new O3DS_TcpThread(Port);
		this->TcpThread->DataDelegate.BindRaw(this, &FOpen3DStreamSource::OnTcpData);
	}

	UpdateConnectionLastActive();
}


void FOpen3DStreamSource::OnUdpData(const FArrayReaderPtr & reader, const FIPv4Endpoint &addr)
{
	int64 bytes = reader->TotalSize();
	if (bytes < 1) return;

	uint8* data = reader->GetData();
	OnPackage(data, bytes);
}

void FOpen3DStreamSource::OnPackage(uint8 *data, size_t sz)
{
	auto root = GetSubjectList(data);
	auto subjects = root->subjects();

	FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkAnimationFrameData::StaticStruct());
	FLiveLinkAnimationFrameData& FrameData = *FrameDataStruct.Cast<FLiveLinkAnimationFrameData>();
	double tt = root->time();

	auto &transforms = FrameData.Transforms;

	for (uint32_t i = 0; i < subjects->size(); i++)
	{
		// FOR EACH SUBJECT 

		TArray<FName>      BoneNames;
		TArray<int32>      BoneParents;
		TArray<FTransform> BoneTransforms;

		auto subject = subjects->Get(i);
		std::string subject_name = subject->name()->str();

		FLiveLinkSubjectName SubjectName(subject_name.c_str());
		const FLiveLinkSubjectKey SubjectKey(SourceGuid, SubjectName);

		auto nodes = subject->nodes();
		auto names = subject->names();

		if (names->size() == nodes->size())
			BoneNames.Reserve(nodes->size());

		BoneParents.Reserve(nodes->size());
		BoneTransforms.Reserve(nodes->size());

		for (uint32_t n = 0; n < nodes->size(); n++)
		{
			// FOR EACH NODE

			auto node = nodes->Get(n);
			BoneParents.Emplace(node->parent());
			auto translation = node->translation();
			auto rotation = node->rotation();

			FTransform t = FTransform::Identity;
			t.SetTranslation(FVector(translation->x(), translation->y(), translation->z()));
			//FQuat rot = FQuat::MakeFromEuler(FVector(rotation->x(), rotation->y(), rotation->z()));
			t.SetRotation(FQuat(rotation->x(), rotation->y(), rotation->z(), rotation->w()));

			if (names->size() == nodes->size())
			{
				std::string name = names->Get(n)->str();
				BoneNames.Emplace(name.c_str());
			}

			transforms.Emplace(t);
		}

		if (!bIsInitialized)
		{
			FLiveLinkStaticDataStruct LiveLinkSkeletonStaticData;
			LiveLinkSkeletonStaticData.InitializeWith(FLiveLinkSkeletonStaticData::StaticStruct(), nullptr);
			FLiveLinkSkeletonStaticData* SkeletonDataPtr = LiveLinkSkeletonStaticData.Cast<FLiveLinkSkeletonStaticData>();

			SkeletonDataPtr->SetBoneNames(BoneNames);
			SkeletonDataPtr->SetBoneParents(BoneParents);

			Client->RemoveSubject_AnyThread(SubjectKey);
			Client->PushSubjectStaticData_AnyThread(SubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(LiveLinkSkeletonStaticData));
			bIsInitialized = true;

			ArrivalTimeZeroA = FPlatformTime::Seconds();
			ArrivalTimeZeroB = tt;
			ArrivalTimeZeroD = ArrivalTimeZeroA - ArrivalTimeZeroB;
		}

		FrameData.WorldTime = FLiveLinkWorldTime(tt + ArrivalTimeZeroD);
		Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
	}

}

bool FOpen3DStreamSource::OnTcpData(FSocket *TcpSource)
{
	// Returns true if the is no data or the data was processed okay
	// Returns false if the data is wrong and the connection is closed

	int32 bytesread = 0;

	while (1)
	{
		TcpSource->Recv(temp_buffer, 1024 * 12, bytesread, ESocketReceiveFlags::None);

		if (bytesread < 1)
			return true;

		buffer.push(temp_buffer, bytesread);

		size_t blocksize = 0;

		uint8 *ptr = 0;

		while (1)
		{
			blocksize = buffer.pull(&ptr);

			if (blocksize == 0)
				break;

			if (blocksize == (size_t)-1)
			{
				TcpSource->Close();
				return false;
			}
			
			buffer.punt();
		}
	}
	return true;
}

bool FOpen3DStreamSource::RequestSourceShutdown()
{
	Client = nullptr;
	SourceGuid.Invalidate();
	return true;
}


void FOpen3DStreamSource::Update()
{
	// Called during the game thread, return quickly.
}

FORCEINLINE void FOpen3DStreamSource::UpdateConnectionLastActive()
{
	FScopeLock ConnectionTimeLock(&ConnectionLastActiveSection);
	ConnectionLastActive = FPlatformTime::Seconds();
}

#undef LOCTEXT_NAMESPACE