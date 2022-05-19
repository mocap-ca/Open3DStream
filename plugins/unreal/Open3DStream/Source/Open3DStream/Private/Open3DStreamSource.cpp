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


#define LOCTEXT_NAMESPACE "Open3DStream"

FOpen3DStreamSource::FOpen3DStreamSource()
:FOpen3DStreamSource(GetDefault<UOpen3DStreamSettingsObject>()->Settings)
{
}

FOpen3DStreamSource::FOpen3DStreamSource(const FOpen3DStreamSettings& Settings)
	: SourceType(LOCTEXT("ConnctionType", "Open 3D Stream"))
	, SourceMachineName(LOCTEXT("SourceMachineName", "-"))
	, SourceStatus(LOCTEXT("ConnctionStatus", "Inactive"))
	, Settings(nullptr)
	, Client(nullptr)
	, ArrivalTimeOffset(0.0)
	, Frame(0)
	, bIsValid(true)
	, mBuffer(nullptr)
	, mBufferSize(0)
	, mRemainder(0)
	, mPtr(0)
	, mAddr(ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr())
{
	Url        = Settings.Url;
	Protocol   = Settings.Protocol;
	Key        = Settings.Key;
	TimeOffset = Settings.TimeOffset;

	
}

FOpen3DStreamSource::~FOpen3DStreamSource()
{}

void FOpen3DStreamSource::InitializeSettings(ULiveLinkSourceSettings* InSettings)
{
}

TSubclassOf < ULiveLinkSourceSettings > FOpen3DStreamSource::GetSettingsClass() const
{
	return UOpen3DStreamSourceSettings::StaticClass();
}


void FOpen3DStreamSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	// "The source has been added to the Client and a Guid has been associated"
	Client = InClient;
	SourceGuid = InSourceGuid;
	bIsValid = true;

	SourceStatus = FText::Format(LOCTEXT("ConnectingString", "Connecting {0}"), Protocol);

	server.OnData.BindRaw(this, &FOpen3DStreamSource::OnPackage);
	server.start(TCHAR_TO_ANSI(*Url.ToString()), TCHAR_TO_ANSI(*Protocol.ToString()));
	UpdateConnectionLastActive();
}

/*
void FOpen3DStreamSource::Tick(float DeltaTime)
{
	 NOW HANDLED BY LAMBDA/THREAD
	if (this->server.mUdp)
	{
		if (mBuffer == nullptr)
		{
			mBuffer = (uint8*)malloc(65507u);
			mBufferSize = 65507u;
		}
		if (mBufferSize < 65507u)
		{
			mBuffer = (uint8*)realloc(mBuffer, 65507u);
		}

		if (mBuffer)
		{
			uint32 Size;
			while (this->server.mUdp->HasPendingData(Size))
			{
				int32 read;
				this->server.mUdp->RecvFrom(mBuffer, FMath::Min(Size, 65507u), read, *this->mAddr);
				if(read > 0)
					OnPackage((uint8*)mBuffer, read);
			}
		}
	}

	if (!Client) return;

	if (this->server.mTcp)
	{

		while(1)
		{
			int32 read = 0;

			if (mRemainder)
			{
				// Finish a fragment
				if (!this->server.mTcp->Recv(mBuffer + mPtr, mRemainder, read))
				{
					return;			
				}

				if (read < mRemainder)
				{
					mRemainder-= read;
					mPtr += read;
					return;
				}

				mRemainder = 0;
			}
			else
			{
				// Get a header
				this->server.mTcp->Recv(mHeader, 8, read, ESocketReceiveFlags::Peek);
				if (read != 8) return;

				this->server.mTcp->Recv(mHeader, 8, read);
				uint32_t heading = ((uint32_t*)mHeader)[0];
				uint32_t bucketSize = ((uint32_t*)mHeader)[1];

				if (mBuffer == nullptr)
				{
					mBuffer = (uint8*)malloc(bucketSize);
					mBufferSize = bucketSize;
				}
				else
				{
					if (mBufferSize < bucketSize)
					{
						mBuffer = (uint8*)realloc(mBuffer, bucketSize);
						mBufferSize = bucketSize;
					}
				}

				if (!this->server.mTcp->Recv(mBuffer, bucketSize, read))
				{
					return;
				}

				if (read < (int32)bucketSize)
				{
					mRemainder = bucketSize - read;
					mPtr = read;
					return;
				}


				if (heading != 0x0203)
				{
					this->server.mTcp->Close();
					this->server.mTcp = nullptr;
					return;
				}

				// Process
				TArray<uint8> Data;
				Data.Append((uint8*)mBuffer, bucketSize);
				OnPackage(Data);

			}
		}
	}
		

}


bool FOpen3DStreamSource::IsTickable() const
{
	return this->server.mTcp != nullptr || this->server.mUdp != nullptr;
}*/

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

void FOpen3DStreamSource::OnPackage(const TArray<uint8>& data)
{
	if (!bIsValid)
		return;

	SourceStatus = FText::Format(LOCTEXT("ReceivingString", "Receiving {0}"), Protocol);


	//O3DS::Unreal::UnBuilder builder;
	mSubjects.Parse((const char*)data.GetData(), data.Num(), 0);// &builder);


	TArray<FName>      BoneNames;
	TArray<int32>      BoneParents;

	for (O3DS::Subject* subject : mSubjects)
	{

		FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkAnimationFrameData::StaticStruct());
		FLiveLinkAnimationFrameData& FrameData = *FrameDataStruct.Cast<FLiveLinkAnimationFrameData>();

		//ArrivalTimeOffset = FPlatformTime::Seconds() - mSubjects.mTime;
		FrameData.WorldTime = FPlatformTime::Seconds();
		FFrameRate FrameRate(60, 1);
		FFrameTime FrameTime = FFrameTime(FrameRate.AsFrameTime(mSubjects.mTime));
		FrameData.FrameId = Frame++;
		FrameData.MetaData.SceneTime = FQualifiedFrameTime(FrameTime, FrameRate);
		// FQualifiedFrameTime QualifiedFrameTime = MobuUtilities::GetSceneTimecode(GetTimecodeMode());

		// FOR EACH SUBJECT 
		BoneNames.Empty();
		BoneParents.Empty();

		//TArray<FTransform> BoneTransforms;

		FLiveLinkSubjectName SubjectName(subject->mName.c_str());
		const FLiveLinkSubjectKey SubjectKey(SourceGuid, SubjectName);

		size_t transformCount = subject->mTransforms.size();
		auto &transforms = subject->mTransforms;

		BoneNames.Reserve(transformCount);
		BoneParents.Reserve(transformCount);
		//BoneTransforms.Reserve(transformCount);

		for (auto& transform : transforms)
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

			BoneNames.Emplace(name.c_str());
			BoneParents.Emplace(transform->mParentId);
			FrameData.Transforms.Add(fTransform);
		}

		// Cjeck if skeleton has not been initialized yet
		if (InitializedSubjects.Find(SubjectName) == INDEX_NONE)
		{
			// Do we have a full packet or just an update)
			if (BoneNames.Num() == 0)
				continue;
			
			// We have a complete packet of data 
			FLiveLinkStaticDataStruct LiveLinkSkeletonStaticData;
			LiveLinkSkeletonStaticData.InitializeWith(FLiveLinkSkeletonStaticData::StaticStruct(), nullptr);
			FLiveLinkSkeletonStaticData* SkeletonDataPtr = LiveLinkSkeletonStaticData.Cast<FLiveLinkSkeletonStaticData>();

			SkeletonDataPtr->SetBoneNames(BoneNames);
			SkeletonDataPtr->SetBoneParents(BoneParents);

			Client->RemoveSubject_AnyThread(SubjectKey);
			Client->PushSubjectStaticData_AnyThread(SubjectKey, ULiveLinkAnimationRole::StaticClass(), MoveTemp(LiveLinkSkeletonStaticData));

			InitializedSubjects.Add(SubjectName);
		}

		Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));

	}


}

bool FOpen3DStreamSource::RequestSourceShutdown()
{
	bIsValid = false;
	this->server.OnData.Unbind();
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