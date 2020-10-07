#include "Open3DStreamSource.h"
#include "LiveLinkRoleTrait.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Common/UdpSocketBuilder.h"

#include "schema_generated.h"
#include "UnrealModel.h"
#include "o3ds/model.h"

//#include "get_time.h"
using namespace O3DS::Data;


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

void operator >>(const O3DS::Matrixd& src, FMatrix& dst)
{
	FMatrix m;
	for (int u = 0; u < 4; u++)
	{
		for (int v = 0; v < 4; v++)
		{
			m.M[u][v] = src.m[u][v];
		}
	}

	FMatrix rot = FRotationMatrix::Make(FQuat(sin(0.785388573), 0, 0, sin(0.785388573)));

	dst = m * rot;

	// No flip: looks like Y needs to be flipped

	//dst.Mirror(EAxis::Y, EAxis::X);
	//dst.Mirror(EAxis::Y, EAxis::Z);  -- no effect?

	//dst.Mirror(EAxis::Z, EAxis::X);  -- upside down

	//dst.Mirror(EAxis::Y, EAxis::X); // - X is the wrong way
		
	// made sin() negative

	// no flip: upside down

	//dst.Mirror(EAxis::Y, EAxis::X); // upside down
	//dst.Mirror(EAxis::Z, EAxis::X);  -- x is the wrong way

	// made sin() positive

	//dst.Mirror(EAxis::X, EAxis::Y);  // - NEARLY
	//dst.Mirror(EAxis::Z, EAxis::Y);  -- upside down
	//dst.Mirror(EAxis::Z, EAxis::X);

	dst.Mirror(EAxis::X, EAxis::Y);  // - NEARLY

}

void FOpen3DStreamSource::OnPackage(uint8 *data, size_t sz)
{
	//O3DS::Unreal::UnBuilder builder;
	mSubjects.Parse((const char*)data, sz, 0);// &builder);

	FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkAnimationFrameData::StaticStruct());
	FLiveLinkAnimationFrameData& FrameData = *FrameDataStruct.Cast<FLiveLinkAnimationFrameData>();

	for (O3DS::Subject* subject : mSubjects)
	{
		// FOR EACH SUBJECT 

		TArray<FName>      BoneNames;
		TArray<int32>      BoneParents;
		TArray<FTransform> BoneTransforms;

		FLiveLinkSubjectName SubjectName(subject->mName.c_str());
		const FLiveLinkSubjectKey SubjectKey(SourceGuid, SubjectName);

		size_t transformCount = subject->mTransforms.size();
		auto &transforms = subject->mTransforms;

		BoneNames.Reserve(transformCount);
		BoneParents.Reserve(transformCount);
		BoneTransforms.Reserve(transformCount);

		for (auto transform : transforms)
		{


			//FVector tr = ftrans.GetTranslation();
			//FQuat = ro = ftrans.GetRotation();
			

			// FOR EACH NODE
			//auto translation = transform->translation.value.v;
			//auto rotation    = transform->rotation.value.v;

			//FTransform t = FTransform::Identity;
			//t.SetTranslation(FVector(translation[0], translation[1], translation[2]));
			//FQuat rot = FQuat::MakeFromEuler(FVector(rotation->x(), rotation->y(), rotation->z()));
			//FQuat q(rotation[0], rotation[1], rotation[2], rotation[3]);
			//q.Normalize();
			//t.SetRotation(q);

			FTransform fTransform;
			FMatrix fMatrix, fParentWorldMatrix;
				transform->mWorldMatrix >> fMatrix;

			if (transform->mParentId != -1)
			{
				auto parent = transforms[transform->mParentId];
				parent->mWorldMatrix >> fParentWorldMatrix;
				fTransform.SetFromMatrix(fMatrix * fParentWorldMatrix.Inverse());
			}
			else
			{		
				fTransform.SetFromMatrix(fMatrix);
			}

			std::string name = transform->mName.c_str();
			size_t pos = name.rfind(':');
			if (pos != std::string::npos)
				name = name.erase(0, pos+1);

			BoneParents.Emplace(transform->mParentId);
			BoneTransforms.Emplace(fTransform);
			BoneNames.Emplace(name.c_str());
			FrameData.Transforms.Add(fTransform);
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

			ArrivalTimeOffset = FPlatformTime::Seconds() - mSubjects.mTime;
		}

		FrameData.WorldTime = FLiveLinkWorldTime(mSubjects.mTime + ArrivalTimeOffset);
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