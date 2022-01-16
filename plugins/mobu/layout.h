#ifndef __OPEN3D_DEVICE_LAYOUT_H__
#define __OPEN3D_DEVICE_LAYOUT_H__

#include "device.h"

class Open3D_Device_Layout : public FBDeviceLayout
{
	FBDeviceLayoutDeclare( Open3D_Device_Layout, FBDeviceLayout );
public:
	virtual bool FBCreate();
	virtual void FBDestroy();

	void	UICreate();
	void	UIConfigure();
	void	UIRefresh();
	void	UIReset();

	void    PopulateSubjectList();
	void    PopulateSubjectFields();

	// Events
	void	EventDeviceStatusChange( HISender pSender, HKEvent pEvent );
	void	EventUIIdle( HISender pSender, HKEvent pEvent );
	void	EventAdd( HISender pSender, HKEvent pEvent );
	void	EventDel(HISender pSender, HKEvent pEvent);
	void	EventSelectDevice(HISender pSender, HKEvent pEvent);
	void	EventEditSubject(HISender pSender, HKEvent pEvent);
	void	EventEditIP(HISender pSender, HKEvent pEvent);
	void	EventEditPort(HISender pSender, HKEvent pEvent);
	void	EventEditProtocol(HISender pSender, HKEvent pEvent);
	void	EventEditSampleRate(HISender pSender, HKEvent pEvent);
	void	EventEditKey(HISender pSender, HKEvent pEvent);

private:
	FBLayout      mLayoutLeft;
	FBLayout      mLayoutRight;
	FBLabel       mLabelLeft;
	FBLabel       mLabelRight;
	FBList        mSourcesList;
	FBButton	  mButtonAdd;
	FBButton      mButtonDel;
	FBLabel       mLabelSource;
	FBEdit        mEditSource;
	FBLabel       mLabelSubject;
	FBEdit        mEditSubject;
	FBMemo        mMemoSourceInfo;
	FBLabel       mLabelDestIp;
	FBEdit        mEditDestIp;
	FBLabel       mLabelDestPort;
	FBEdit        mEditDestPort;
	FBLabel       mLabelProtocol;
	FBList        mListProtocol;
	FBLabel       mLabelKey;
	FBEdit        mEditKey;
	FBLabel		  mLabelSamplingRate;
	FBEditNumber  mEditSamplingRate;
	FBLabel       mLabelPluginVersion;

	FBSystem    mSystem;
	Open3D_Device*	mDevice;
};

#endif