#include "device.h"

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
#define FBX_NETWORk_IP      "NetworkIp"
#define FBX_NETWORK_PORT    "NetworkPort"
#define FBX_NETWORK_PROTO   "NetworkProtocol"


FBDeviceImplementation	(	OPEN3D_DEVICE__CLASS	);
FBRegisterDevice		(	OPEN3D_DEVICE__NAME,
							OPEN3D_DEVICE__CLASS,
							OPEN3D_DEVICE__LABEL,
							OPEN3D_DEVICE__DESC,
							FB_DEFAULT_SDK_ICON		);	// Icon filename (default=Open Reality icon)


bool Open3D_Device::FBCreate()
{
	mNetworkAddress = "127.0.0.1";
	mNetworkPort = 3001;
	mStreaming = true;
	mProtocol = kTCP;

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
		Items.push_back(O3DS::SubjectItem(model, name));
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
	FBProgress	lProgress;
	lProgress.Caption	= "Starting up device";

	// Step 1: Open device communications
	lProgress.Text	= "Opening device communications";
	Status			= "Opening device communications";

	if (mTcpIp.CreateSocket(mNetworkSocket, kFBTCPIP_Stream))
	{
		bool ret = mTcpIp.Connect(mNetworkSocket, mNetworkAddress, mNetworkPort);
		if (!ret)
		{
			mTcpIp.CloseSocket(mNetworkSocket);
			mNetworkSocket = -1;
			Status = "Error";
			return false;
		}
	}

	Status = "Ok";
	return true;
}

bool Open3D_Device::Stop()
{
	FBProgress	lProgress;
	lProgress.Caption	= "Shutting down device";

	// Step 1: Stop streaming data
	lProgress.Text	= "Stopping device communications";
	Status			= "Stopping device communications";

	if (mTcpIp.CloseSocket(mNetworkSocket))
	{
		Status = "Error";
		return false;
	}

	lProgress.Caption = "";
	lProgress.Text = "";
	Status = "";

    return true;
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

void Open3D_Device::DeviceIONotify( kDeviceIOs  pAction,FBDeviceNotifyInfo &pDeviceNotifyInfo)
{
    switch (pAction)
	{
		// Output devices
		case kIOPlayModeWrite:
		case kIOStopModeWrite:
		{
			for (auto subject : Items)
			{
				const char *name = subject.mName;
			}
			AckOneSampleSent();
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
		pFbxObject->FieldWriteI(FBX_SUBJECT_COUNT, Items.size());

		for (int i = 0; i < Items.size(); i++)
		{
			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_NAME, i);
			pFbxObject->FieldWriteC(buf, Items[i].mName);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_MODEL, i);
			pFbxObject->FieldWriteC(buf, Items[i].mModel->GetFullName());

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_REF, i);
			pFbxObject->FieldWriteObjectReference(buf, Items[i].mModel);

		}

		pFbxObject->FieldWriteC(FBX_NETWORk_IP, GetNetworkAddress());
		pFbxObject->FieldWriteI(FBX_NETWORK_PORT, GetNetworkPort());
		pFbxObject->FieldWriteI(FBX_NETWORK_PROTO, (int)GetProtocol());

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
			FBString subject = pFbxObject->FieldReadC(buf);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_MODEL, i);
			FBString model = pFbxObject->FieldReadC(buf);

			sprintf_s(buf, 40, "%s%d", FBX_SUBJECT_REF, i);
			HIObject ref = pFbxObject->FieldReadObjectReference(buf);

			FBComponent *component = FBGetFBComponent(ref);

			if (component)
			{
				O3DS::SubjectItem item(component, subject);
				Items.push_back(item);
			}
		}

		SetNetworkAddress(pFbxObject->FieldReadC(FBX_NETWORk_IP, GetNetworkAddress()));
		SetNetworkPort(pFbxObject->FieldReadI(FBX_NETWORK_PORT, GetNetworkPort()));
		SetProtocol(static_cast<TProtocol>(pFbxObject->FieldReadI(FBX_NETWORK_PROTO)));


	}

	return true;
}
