#ifndef __OPEN3D_DEVICE_H__
#define __OPEN3D_DEVICE_H__

#include <fbsdk/fbsdk.h>
#include <vector>

#include "audio.h"
#include "mobuModel.h"
#include "o3ds/base_connector.h"
#include "o3ds/tcp.h"

#define OPEN3D_DEVICE__CLASSNAME	Open3D_Device
#define OPEN3D_DEVICE__CLASSSTR		"Open3D_Device"

constexpr auto audioSubjectName = "Audio";


class Open3D_Device : public FBDevice, IAudioSubscriber
{
	FBDeviceDeclare( Open3D_Device, FBDevice );
public:

	typedef enum eProtocols {
		kUDP = 0,
		kTCPServer,
		kTCPClient,
		kNNGPipeline,
		kNNGPairClient,
		kNNGPairServer,
		kNNGPublish
	} TProtocol;


	virtual bool FBCreate();		
	virtual void FBDestroy();

	virtual bool AnimationNodeNotify	(	FBAnimationNode* pAnimationNode,	FBEvaluateInfo* pEvaluateInfo			);	//!< Real-time evaluation for node.
	virtual void DeviceIONotify			(	kDeviceIOs  pAction,				FBDeviceNotifyInfo &pDeviceNotifyInfo	);	//!< Notification of/for Device IO.
    virtual bool DeviceEvaluationNotify	(	kTransportMode pMode,				FBEvaluateInfo* pEvaluateInfo			);	//!< Evaluation the device (write to hardware).
	virtual bool DeviceOperation		(	kDeviceOperations pOperation												);	//!< Operate device.

	virtual bool FbxStore	( FBFbxObject* pFbxObject, kFbxObjectStore pStoreWhat	);	//!< Store in FBX file.
	virtual bool FbxRetrieve( FBFbxObject* pFbxObject, kFbxObjectStore pStoreWhat	);	//!< Retrieve from FBX file.

	bool		Init();
	bool		Start();
	bool		Reset();
	bool		Stop();
	bool		Done();

	void		AddItem(FBModel* model);

	void		DeviceRecordFrame( FBDeviceNotifyInfo &pDeviceNotifyInfo );

	void		SetProtocol(TProtocol protocol)			{ mProtocol = protocol; }
	TProtocol	GetProtocol()							{ return mProtocol; }
	void		SetNetworkAddress(const char* address)	{ mNetworkAddress = address; }
	const char*	GetNetworkAddress()						{ return mNetworkAddress; }
	void		SetNetworkPort(int port)				{ mNetworkPort = port; }
	int			GetNetworkPort()						{ return mNetworkPort; }

	double		GetSamplingRate();
	void		SetSamplingRate(double rate);

	const char* GetKey()                                { return mKey; }
	void        SetKey(const char *value)               { mKey = value; }

	void        SetDeltaThreshold(double value) { Items.mDeltaThreshold = value;  }
	double      GetDeltaThreshold() { return Items.mDeltaThreshold; }

	FBDeviceSamplingMode	GetSamplingType()								{ return mSamplingType;			}
	void					SetSamplingType( FBDeviceSamplingMode pType )	{ mSamplingType = pType;		}

	void SetSelectedMicrophone(FBAudioIn* mic);

	bool  IsActive();

	void InData(void *, size_t);

	uint32_t WriteTcp(O3DS::TcpSocket& socket, void *data, uint32_t bucketSize);

	O3DS::SubjectList       Items;
	O3DS::TcpSocket         mTcpIp;
	O3DS::Connector*        mServer;
	SOCKET                  mNetworkSocket;
	std::vector<int>        mClients;

	Open3D_AudioInput    mAudioRecord;

private:
	void audio_captured(const BYTE *captureBuffer, UINT32 nFrames) override;

	double                mSamplingRate;
	FBDeviceSamplingMode  mSamplingType;
	FBPlayerControl       mPlayerControl;

	TProtocol       mProtocol;
	FBString		mNetworkAddress;
	FBString        mKey;
	int				mNetworkPort;
	bool			mStreaming;
	int             mFrameCounter;
	uint32_t        mIdSeq;	
};

#endif 
