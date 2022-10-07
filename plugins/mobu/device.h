#ifndef __OPEN3D_DEVICE_H__
#define __OPEN3D_DEVICE_H__

#include <fbsdk/fbsdk.h>
#include <vector>
#include "mobuModel.h"
#include "o3ds/base_connector.h"


#define OPEN3D_DEVICE__CLASSNAME	Open3D_Device
#define OPEN3D_DEVICE__CLASSSTR		"Open3D_Device"


class Open3D_Device : public FBDevice
{
	FBDeviceDeclare( Open3D_Device, FBDevice );
public:

	typedef enum eProtocols {
		kUDP = 0,
		kTCPServer,
		kTCPClient,
		kNNGClient,
		kNNGServer,
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

	FBDeviceSamplingMode	GetSamplingType()								{ return mSamplingType;			}
	void					SetSamplingType( FBDeviceSamplingMode pType )	{ mSamplingType = pType;		}

	void InData(void *, size_t);

	int32_t WriteTcp(int socket, void *data, int32_t bucketSize);

	O3DS::SubjectList       Items;
	FBTCPIP                 mTcpIp;
	O3DS::Connector*        mServer;
	int                     mNetworkSocket;
	std::vector<int>        mClients;

private:
	double                mSamplingRate;
	FBDeviceSamplingMode  mSamplingType;
	FBPlayerControl       mPlayerControl;

	TProtocol       mProtocol;
	FBString		mNetworkAddress;
	FBString        mKey;
	int				mNetworkPort;
	bool			mStreaming;

};

#endif 
