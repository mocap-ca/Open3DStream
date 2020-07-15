#include "Open3DStreamSource.h"
#include "LiveLinkRoleTrait.h"

#include "Roles/LiveLinkAnimationTypes.h"
#include "Roles/LiveLinkAnimationRole.h"

#include "schema_generated.h"
using namespace MyGame::Sample;


// E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\LiveLinkMessageBusSource.cpp
// E:\Unreal\UE_4.25\Engine\Source\Runtime\LiveLinkInterface\Public\LiveLinkTypes.h
// E:\Unreal\UE_4.25\Engine\Plugins\Runtime\AR\Apple\AppleARKit\Source\AppleARKitPoseTrackingLiveLink\Private\AppleARKitPoseTrackingLiveLinkSource.cpp

FOpen3DStreamSource::FOpen3DStreamSource(int InPort, double InTimeOffset)
	: bIsInitialized(false)
	, Port(InPort)
	, TcpSource(nullptr)
	, TimeOffset(InTimeOffset)
	, bIsValid(false)
	, debugval(0)
{
	SourceStatus = NSLOCTEXT("Open3DStream", "ConnctionStatus", "Inactive");
	SourceType = NSLOCTEXT("Open3DStream", "ConnctionType", "Open 3D Stream");
	SourceMachineName = NSLOCTEXT("Open3DStream", "SourceMachineName", "-");

	buffer = (uint8*) malloc(NET_BUFFER_SIZE);
	buffer_ptr = buffer;

}

FOpen3DStreamSource::~FOpen3DStreamSource()
{
	free(buffer);
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

	// The message bus builder and callback was here, replaced with tcp listener for now.
	TcpServerSocket = FTcpSocketBuilder(TEXT("Open3DStreamServer"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToPort(Port)
		.Listening(1)
		.Build();

	UpdateConnectionLastActive();
}


void FOpen3DStreamSource::Listen()
{

	debugval++;

	if (TcpSource == nullptr)
	{
		TcpSource = TcpServerSocket->Accept("Open3DStreamSource");
	}

	if (TcpSource != nullptr)
	{
		int32 bytesread = 0;
		size_t data_size = buffer_ptr - buffer;

		TcpSource->Recv(buffer_ptr, NET_BUFFER_SIZE - data_size, bytesread, ESocketReceiveFlags::None);
		buffer_ptr += bytesread;
		data_size = buffer_ptr - buffer;

		if (data_size > 8)
		{
			size_t   start_ptr  = 0;
			int32_t  headers    = *(int32_t*)buffer;
			int32_t  blocksize  = *(int32_t*)buffer + 4;
			size_t   end_ptr    = blocksize;

			while(data_size > end_ptr + 8)
			{
				int32_t nextblock = *(int32_t*)(buffer + end_ptr) + 8;
				if (end_ptr + nextblock > data_size)
					break;

				start_ptr = end_ptr;
				end_ptr += nextblock;
				blocksize = nextblock;
			}			
			 
			if (end_ptr < data_size)
			{
				auto root = GetSubjectList(buffer + start_ptr + 8);

				auto subjects = root->subjects();

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
						auto node        = nodes->Get(n);
						BoneParents.Emplace(node->parent());
						auto translation = node->translation();
						auto rotation    = node->rotation();

						FTransform t = FTransform::Identity;
						t.SetTranslation(FVector(translation->x(), translation->y(), translation->z()));
						FQuat rot = FQuat::MakeFromEuler(FVector(rotation->x(), rotation->y(), rotation->z()));
						t.SetRotation(rot);

						if ( names->size() == nodes->size())
						{
							std::string name = names->Get(n)->str();
							BoneNames.Emplace(name.c_str());
						}
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
					}

					FLiveLinkFrameDataStruct FrameDataStruct(FLiveLinkAnimationFrameData::StaticStruct());
					FLiveLinkAnimationFrameData& FrameData = *FrameDataStruct.Cast<FLiveLinkAnimationFrameData>();
					FrameData.WorldTime = FPlatformTime::Seconds();

					Client->PushSubjectFrameData_AnyThread(SubjectKey, MoveTemp(FrameDataStruct));
				}

				if (end_ptr == data_size)
				{
					buffer_ptr = buffer;
				}
				else
				{
					// copy remaining data
					size_t remaining = data_size - end_ptr;
					memcpy(buffer, buffer + end_ptr, remaining);
					buffer_ptr = buffer + remaining;
				}
			}
		}

	}
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
	debugval++;
	Listen();
}

FORCEINLINE void FOpen3DStreamSource::UpdateConnectionLastActive()
{
	FScopeLock ConnectionTimeLock(&ConnectionLastActiveSection);
	ConnectionLastActive = FPlatformTime::Seconds();
}

#undef LOCTEXT_NAMESPACE