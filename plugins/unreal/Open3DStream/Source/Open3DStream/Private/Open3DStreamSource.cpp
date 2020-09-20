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

FOpen3DStreamSource::FOpen3DStreamSource(const FText &InUrl, const FText &InKey, const FText &InProtocol, double InTimeOffset)
	: bIsInitialized(false)
	, Url(InUrl)
	, Key(InKey)
	, Protocol(InProtocol)
	, TimeOffset(InTimeOffset)
	, ArrivalTimeOffset(0.0)
	, bIsValid(false)
{
	SourceStatus = NSLOCTEXT("Open3DStream", "ConnctionStatus", "Inactive");
	SourceType = NSLOCTEXT("Open3DStream", "ConnctionType", "Open 3D Stream");
	SourceMachineName = NSLOCTEXT("Open3DStream", "SourceMachineName", "-");
}

FOpen3DStreamSource::~FOpen3DStreamSource()
{}

void FOpen3DStreamSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	// "The source has been added to the Client and a Guid has been associated"
	Client = InClient;
	SourceGuid = InSourceGuid;
	bIsValid = true;

	server.DataDelegate.BindRaw(this, &FOpen3DStreamSource::OnNnpData);
	server.start(TCHAR_TO_ANSI(*Url.ToString()), TCHAR_TO_ANSI(*Protocol.ToString()));
	UpdateConnectionLastActive();
}

void FOpen3DStreamSource::OnNnpData()
{
	this->server.mutex.Lock();
	this->buffer.reserve(this->server.buffer.size());
	this->buffer = this->server.buffer;

	this->server.mutex.Unlock();

	OnPackage((uint8*)&this->buffer[0], this->buffer.size());

}

void FOpen3DStreamSource::OnPackage(uint8 *data, size_t sz)
{
	auto root = GetSubjectList(data);
	auto subjects = root->subjects();
	if (subjects == nullptr)
		return;

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
		{
			BoneNames.Reserve(nodes->size());
		}

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
			FQuat q(rotation->x(), rotation->y(), rotation->z(), rotation->w());
			q.Normalize();
			t.SetRotation(q);

			if (names->size() == nodes->size())
			{
				std::string name = names->Get(n)->str();
				BoneNames.Emplace(name.c_str());
			}

			transforms.Emplace(t);
		}

		if (!bIsInitialized && BoneNames.Num() > 0)
		{
			FLiveLinkStaticDataStruct LiveLinkSkeletonStaticData;
			LiveLinkSkeletonStaticData.InitializeWith(FLiveLinkSkeletonStaticData::StaticStruct(), nullptr);
			FLiveLinkSkeletonStaticData* SkeletonDataPtr = LiveLinkSkeletonStaticData.Cast<FLiveLinkSkeletonStaticData>();

			SkeletonDataPtr->SetBoneNames(BoneNames);
			SkeletonDataPtr->SetBoneParents(BoneParents);

			Client->RemoveSubject_AnyThread(SubjectKey);
			Client->PushSubjectStaticData_AnyThread(SubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(LiveLinkSkeletonStaticData));
			bIsInitialized = true;

			ArrivalTimeOffset = FPlatformTime::Seconds() - tt;
		}

		FrameData.WorldTime = FLiveLinkWorldTime(tt + ArrivalTimeOffset);
		Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
	}

}

bool FOpen3DStreamSource::RequestSourceShutdown()
{
	this->server.stop();
	Client = nullptr;
	SourceGuid.Invalidate();
	return true;
}


void FOpen3DStreamSource::Update()
{
	// Called during the game thread, return quickly.
	//if(server.Recv())
	//	OnPackage((uint8*)(server.buffer.data()), server.buffer.size());
}

FORCEINLINE void FOpen3DStreamSource::UpdateConnectionLastActive()
{
	FScopeLock ConnectionTimeLock(&ConnectionLastActiveSection);
	ConnectionLastActive = FPlatformTime::Seconds();
}

#undef LOCTEXT_NAMESPACE