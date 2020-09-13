#include "device.h"
#include "o3ds/math.h"
#include "o3ds/publisher.h"
#include "o3ds/async_pair.h"

#define OPEN3D_DEVICE__CLASS	OPEN3D_DEVICE__CLASSNAME
#define OPEN3D_DEVICE__NAME	OPEN3D_DEVICE__CLASSSTR
#define OPEN3D_DEVICE__LABEL	"Open3D_Device"
#define OPEN3D_DEVICE__DESC		"Realtime Streaming"
#define OPEN3D_DEVICE__PREFIX	"DeviceTemplate"

#define FBX_VERSION_TAG		"Version"
#define FBX_VERSION_VAL		100
#define FBX_COMMPARAM_TAG	"Communication"
#define FBX_SAMPLERATE      "SampleRate"
#define FBX_SUBJECT_COUNT   "SubjectCount"
#define FBX_SUBJECT_NAME    "SubjectName"
#define FBX_SUBJECT_MODEL   "SubjectModel"
#define FBX_SUBJECT_REF     "SubjectReference"
#define FBX_NETWORK_IP      "NetworkIp"
#define FBX_NETWORK_PORT    "NetworkPort"
#define FBX_NETWORK_PROTO   "NetworkProtocol"
#define FBX_KEY             "Key"

#include <ws2tcpip.h>
#include <sstream>

FBDeviceImplementation	(	OPEN3D_DEVICE__CLASS	);
FBRegisterDevice		(	OPEN3D_DEVICE__NAME,
							OPEN3D_DEVICE__CLASS,
							OPEN3D_DEVICE__LABEL,
							OPEN3D_DEVICE__DESC,
							FB_DEFAULT_SDK_ICON		);	// Icon filename (default=Open Reality icon)


void dataFn(void *ctx, void *data, size_t len)
{
	if (ctx) static_cast<Open3D_Device*>(ctx)->InData(data, len);
}

bool Open3D_Device::FBCreate()
{
	//mNetworkAddress = "tcp://3.131.65.210:6001";
	mNetworkAddress = "tcp://127.0.0.1:6001";
	mNetworkPort = 3001;
	mStreaming = true;
	mProtocol = kNNGServer;
	mKey = nullptr;
	mServer = nullptr;

	FBTime	lPeriod;
	lPeriod.SetSecondDouble(1.0/60.0);
	SamplingPeriod	= lPeriod;
	mSamplingRate = lPeriod.GetFrame();
	return true;
}

void Open3D_Device::FBDestroy()
{
}

bool Open3D_Device::DeviceOperation( kDeviceOperations pOperation )
{
	switch (pOperation)
	{
		case kOpInit:	return Init();
		case kOpStart:	return Start();
		case kOpStop:	return Stop();
		case kOpReset:	return Reset();
		case kOpDone:	return Done();
	}
	return FBDevice::DeviceOperation( pOperation );
}

#pragma warning( disable : 4189 )

void Open3D_Device::AddItem(FBModel *model)
{
	// Add a model (called by the layout) 

	FBString name;

	FBNamespace *ns = model->GetOwnerNamespace();
	if(ns)
	{ 
		name = ns->Name.AsString();
	}
	else
	{
		name = model->Name.AsString();
	}

	if (model->Is(FBModelNull::TypeInfo) ||
		model->Is(FBModelRoot::TypeInfo) ||
		model->Is(FBCamera::TypeInfo) ||
		model->Is(FBModelSkeleton::TypeInfo))
	{
		Items.addSubject(name.operator char *(), new MobuSubjectInfo(model));
	}

}

bool Open3D_Device::Init()
{
    FBProgress	lProgress;

    lProgress.Caption	= "Open3D Stream";
	lProgress.Text		= "Initializing device...";

	return true;
}

bool Open3D_Device::Start()
{
	// Start the device

	FBProgress	lProgress;
	lProgress.Caption	= "Starting up device";
	lProgress.Text	= "Opening device communications";
	Status			= "Opening device communications";

	if (mServer)
	{
		delete mServer;
		mServer = nullptr;
	}

	for (auto& subject : Items)
	{
		MobuSubjectInfo *info = dynamic_cast<MobuSubjectInfo*>(subject->mInfo);
		TraverseSubject(subject, info->mModel);
	}

	if (mProtocol == Open3D_Device::kTCPClient)
	{
		if (mTcpIp.CreateSocket(mNetworkSocket, kFBTCPIP_RAW))
		{
			if (!mTcpIp.Connect(mNetworkSocket, mNetworkAddress, mNetworkPort))
			{
				mTcpIp.CloseSocket(mNetworkSocket);
				mNetworkSocket = -1;
				Status = "Connect Error";
				return false;
			}

			Status = "TCP Started";
			return true;
		}
		else
		{
			Status = "Socket Error";
			mNetworkSocket = -1;
			return false;
		}
	}

	if (mProtocol == Open3D_Device::kUDP)
	{
		if (mTcpIp.CreateSocket(mNetworkSocket, kFBTCPIP_DGRAM))
		{
			Status = "UDP OK";
			return true;
		}

		mNetworkSocket = -1;
		Status = "UDP Error";
		return false;
	}

	if (mProtocol == Open3D_Device::kNNGServer ||
		mProtocol == Open3D_Device::kNNGClient || 
		mProtocol == Open3D_Device::kNNGPublish)
	{
		if (mProtocol == Open3D_Device::kNNGServer)  mServer = new O3DS::AsyncPairServer();
		if (mProtocol == Open3D_Device::kNNGClient)	 mServer = new O3DS::AsyncPairClient();
		if (mProtocol == Open3D_Device::kNNGPublish) mServer = new O3DS::Publisher();

		if (mServer->start(mNetworkAddress))
		{
			Status = "Running";
			return true;
		}
		else
		{
			Status = mServer->err().c_str();
			return false;
		}
	}

	Status = "INVALID";
	return true;
}

void Open3D_Device::InData(void *data, size_t len)
{

}

bool Open3D_Device::Stop()
{
	// Stop the device

	FBProgress	lProgress;
	lProgress.Caption	= "Shutting down device";

	// Step 1: Stop streaming data
	lProgress.Text	= "Stopping device communications";
	Status			= "Stopping device communications";

	if (mNetworkSocket > 0)
	{
		if (mTcpIp.CloseSocket(mNetworkSocket))
		{
			Status = "";
		}
		else
		{
			Status = "Error Closing";
		}
	}

	if (mServer) 
	{
		delete mServer;
		mServer = nullptr;
     }

	lProgress.Caption = "";
	lProgress.Text = "";

    return true;
}

void Open3D_Device::DeviceIONotify(kDeviceIOs  pAction, FBDeviceNotifyInfo &pDeviceNotifyInfo)
{
	uint8_t buf[1024 *12];
	int written;

	uint32_t total = 0;

	// Called per frame to send data
	switch (pAction)
	{
		// Output devices
	case kIOPlayModeWrite:
	case kIOStopModeWrite:
	{
		if (mNetworkSocket != -1)
		{
			Items.update(true);

			int32_t bucket_size = O3DS::Serialize(mKey, Items, buf, 1024 * 12, true);
			if (bucket_size == 0)
				return;

			if (mProtocol == Open3D_Device::kTCPClient)
			{
				// Write a header first 
				int32_t header = 0x0203;
				written = 0;
				if (!mTcpIp.Write(mNetworkSocket, &header, sizeof(int32_t), &written))
				{
					Status = "Error";
					//mTcpIp.CloseSocket(mNetworkSocket);
					//mNetworkSocket = -1;
					return;
				}
				total += written;

				// Write the size of this bucket
				if (!mTcpIp.Write(mNetworkSocket, &bucket_size, sizeof(int32_t), &written))
				{
					Status = "Error";
				}
				total += written;

				// Write the bucket
				if (!mTcpIp.Write(mNetworkSocket, buf, bucket_size, &written))
				{
					Status = "Error";
				}
				total += written;
			}

			if (mProtocol == Open3D_Device::kUDP)
			{
				struct in_addr ret;
				if (!inet_pton(AF_INET, mNetworkAddress.operator char *(), &ret))
				{
					Status = "UDP ERROR";
				}
				mTcpIp.WriteDatagram(mNetworkSocket, buf, bucket_size, &written, ret.S_un.S_addr, mNetworkPort);
				std::ostringstream oss;
				oss << written << " bytes";
				Status = oss.str().c_str();
			}

			if (mProtocol == Open3D_Device::kNNGClient ||
				mProtocol == Open3D_Device::kNNGServer ||
				mProtocol == Open3D_Device::kNNGPublish)
			{
				mServer->write((const char*)buf, bucket_size);
			}
		}
	}
	break;

	// Input devices
	case kIOStopModeRead:
	case kIOPlayModeRead:
	{
		AckOneSampleReceived();
		break;
	}
	}
}

bool Open3D_Device::Done()
{
	return false;
}

bool Open3D_Device::Reset()
{
    Stop();
    return Start();
}

double Open3D_Device::GetSamplingRate()
{
	FBTime t = SamplingPeriod;
	double dt = t.GetSecondDouble();
	if (dt == 0.0) return 0.0;
	return 1.0 / dt;
}

void Open3D_Device::SetSamplingRate(double rate)
{
	if (rate != 0.0)
	{
		FBTime t;
		t.SetSecondDouble(1.0 / rate);
		SamplingPeriod = t;
	}
}


bool Open3D_Device::AnimationNodeNotify(FBAnimationNode* pAnimationNode ,FBEvaluateInfo* pEvaluateInfo)
{
    return true;
}


bool Open3D_Device::DeviceEvaluationNotify( kTransportMode pMode, FBEvaluateInfo* pEvaluateInfo )
{
	return true;
}



void Open3D_Device::DeviceRecordFrame( FBDeviceNotifyInfo &pDeviceNotifyInfo )
{
}



bool Open3D_Device::FbxStore(FBFbxObject* pFbxObject,kFbxObjectStore pStoreWhat)
{
	char buf[40];

	if (pStoreWhat & kAttributes)
	{
		pFbxObject->FieldWriteI		(FBX_VERSION_TAG,FBX_VERSION_VAL);

		pFbxObject->FieldWriteBegin(FBX_COMMPARAM_TAG);
		{
			pFbxObject->FieldWriteEnd();
		}
		
		pFbxObject->FieldWriteD(FBX_SAMPLERATE, GetSamplingRate());
		pFbxObject->FieldWriteI(FBX_SUBJECT_COUNT, (int)Items.size());

		for (int i = 0; i < Items.size(); i++)
		{
			O3DS::Subject* subject = Items[i];
			MobuSubjectInfo *info = dynamic_cast<MobuSubjectInfo*>(subject->mInfo);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_NAME, i);
			pFbxObject->FieldWriteC(buf, subject->mName.c_str());

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_MODEL, i);
			pFbxObject->FieldWriteC(buf, info->mModel->GetFullName());

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_REF, i);
			pFbxObject->FieldWriteObjectReference(buf, info->mModel);

		}

		pFbxObject->FieldWriteC(FBX_NETWORK_IP, GetNetworkAddress());
		pFbxObject->FieldWriteI(FBX_NETWORK_PORT, GetNetworkPort());
		pFbxObject->FieldWriteI(FBX_NETWORK_PROTO, (int)GetProtocol());
		pFbxObject->FieldWriteC(FBX_KEY, GetKey());

	}
	return true;
}


bool Open3D_Device::FbxRetrieve(FBFbxObject* pFbxObject,kFbxObjectStore pStoreWhat)
{
	int			Version;
	FBString	name;
	char buf[40];

	if (pStoreWhat & kAttributes)
	{
		// Get version
		Version	= pFbxObject->FieldReadI(FBX_VERSION_TAG);

		// Get communications settings
		if (pFbxObject->FieldReadBegin(FBX_COMMPARAM_TAG))
		{
			pFbxObject->FieldReadEnd();
		}
		SetSamplingRate(pFbxObject->FieldReadD(FBX_SAMPLERATE, GetSamplingRate()));
		int n = pFbxObject->FieldReadI(FBX_SUBJECT_COUNT);

		for (int i = 0; i < n; i++)
		{
			// Don't use const char* return values as value is not persistent.
			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_NAME, i);
			FBString subjectName = pFbxObject->FieldReadC(buf);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_MODEL, i);
			FBString model = pFbxObject->FieldReadC(buf);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_REF, i);
			HIObject ref = pFbxObject->FieldReadObjectReference(buf);

			FBComponent *component = FBGetFBComponent(ref);

			if (component)
			{
				FBModel *model = dynamic_cast<FBModel*>(component);
				auto s = Items.addSubject(subjectName.operator char *(), new MobuSubjectInfo(model));
				TraverseSubject(s, model);
			}
		}

		SetNetworkAddress(pFbxObject->FieldReadC(FBX_NETWORK_IP, GetNetworkAddress()));
		SetNetworkPort(pFbxObject->FieldReadI(FBX_NETWORK_PORT, GetNetworkPort()));
		SetProtocol(static_cast<TProtocol>(pFbxObject->FieldReadI(FBX_NETWORK_PROTO)));
		SetKey(pFbxObject->FieldReadC(FBX_KEY, GetKey()));
	}

	return true;
}
