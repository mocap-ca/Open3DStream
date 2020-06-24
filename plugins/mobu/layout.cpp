#include "device.h"
#include "layout.h"

#define OPEN3D_DEVICE__LAYOUT	Open3D_Device_Layout

FBDeviceLayoutImplementation(OPEN3D_DEVICE__LAYOUT);
FBRegisterDeviceLayout		(OPEN3D_DEVICE__LAYOUT, OPEN3D_DEVICE__CLASSSTR, FB_DEFAULT_SDK_ICON);

bool Open3D_Device_Layout::FBCreate()
{
	mDevice = ((Open3D_Device *)(FBDevice *)Device);
	UICreate	();
	UIConfigure	();
	UIReset		();

	// Add device & system callbacks
	mDevice->OnStatusChange.Add	( this,(FBCallback)&Open3D_Device_Layout::EventDeviceStatusChange		);
	OnIdle.Add					( this,(FBCallback)&Open3D_Device_Layout::EventUIIdle					);

	return true;
}

void Open3D_Device_Layout::FBDestroy()
{
	// Remove device & system callbacks
	OnIdle.Remove					( this,(FBCallback)&Open3D_Device_Layout::EventUIIdle				);
	mDevice->OnStatusChange.Remove	( this,(FBCallback)&Open3D_Device_Layout::EventDeviceStatusChange	);
}

void Open3D_Device_Layout::UICreate()
{
	int lS = 6;
	int lH = 25;
	int labelWidth = 80;
	int fieldWidth = 150;
	int listWidth = 130;
	int leftWidth = 300;
	int buttonWidth = listWidth / 2 - 3;

	// Left Layout - Attached to the left layout
	AddRegion("LeftLayout", "LeftLayout",
		lS,         kFBAttachLeft,   "",   1.00,
		lS,         kFBAttachTop,    "",   1.00,
		leftWidth,  kFBAttachNone,   "",   1.00,
		-lS,        kFBAttachBottom, "",   1.00);
	SetControl("LeftLayout", mLayoutLeft);

	// Left Label
	AddRegion("LeftLabel", "LeftLabel",
		5,    kFBAttachLeft,   "LeftLayout", 1.00,
		-10,  kFBAttachTop,    "LeftLayout", 1.00,
		50,   kFBAttachNone,   "", 1.00,
		17,   kFBAttachNone,   "", 1.00);
	SetControl("LeftLabel", mLabelLeft);
	mLabelLeft.Caption = "Source";


	// Right Layout - Attached to right of left layout
	AddRegion( "RightLayout",       "RightLayout",	
		10,		kFBAttachRight,		"LeftLayout",	1.00,
		lS,		kFBAttachTop,	    "",		  		1.00,
		-lS,	kFBAttachRight,		"",				1.00,
		-lS,	kFBAttachBottom,	"",				1.00 );
	SetControl("RightLayout", mLayoutRight);

	// Right Label
	AddRegion("RightLabel", "RightLabel",
		5,    kFBAttachLeft,  "RightLayout", 1.00,
		-10,  kFBAttachTop,   "RightLayout", 1.00,
		50,   kFBAttachNone,  "", 1.00,
		17,   kFBAttachNone,  "", 1.00);
	SetControl("RightLabel", mLabelRight);
	mLabelRight.Caption = "Network";


	// Sources list - Top Left
	mLayoutLeft.AddRegion("SourcesList", "SourcesList",
		lS, kFBAttachLeft, "", 1.00,
		10, kFBAttachTop,  "", 1.00,
		listWidth, kFBAttachNone, "", 1.00,
		-50, kFBAttachBottom, "", 1.00);
	mLayoutLeft.SetControl("SourcesList", mSourcesList);
	mSourcesList.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventSelectDevice);
	mSourcesList.Style = FBListStyle::kFBVerticalList;

	// Add Button - under sources list
	mLayoutLeft.AddRegion("AddButton", "AddButton",
		lS,          kFBAttachLeft,   "",            1.00,
		lS,          kFBAttachBottom, "SourcesList", 1.00,
		buttonWidth, kFBAttachNone,   NULL,          1.00,
		30,          kFBAttachNone,   "",            1.00);
	mLayoutLeft.SetControl("AddButton", mButtonAdd);
	mButtonAdd.Caption = "Add";
	mButtonAdd.OnClick.Add(this, (FBCallback)&Open3D_Device_Layout::EventAdd);

	// Del button - under sources list, right of add button
	mLayoutLeft.AddRegion("DelButton", "DelButton",
		lS,           kFBAttachRight,  "AddButton",   1.00,
		lS,           kFBAttachBottom, "SourcesList",  1.00,
		buttonWidth,  kFBAttachNone,   NULL,        1.00,
		30,           kFBAttachNone, "",          1.00);
	mLayoutLeft.SetControl("DelButton", mButtonDel);
	mButtonDel.Caption = "Del";
	mButtonDel.OnClick.Add(this, (FBCallback)&Open3D_Device_Layout::EventDel);


	// Source label - Top, right of SourcesList
	mLayoutLeft.AddRegion("LabelSource", "LabelSource",
		lS, kFBAttachRight, "SourcesList", 1.00,
		lS, kFBAttachTop, "", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelSource", mLabelSource);
	mLabelSource.Caption = "Source:";

	// Source edit - Under source label
	mLayoutLeft.AddRegion("EditSource", "EditSource",
		lS, kFBAttachRight,  "SourcesList", 1.00,
		0, kFBAttachBottom, "LabelSource", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone,  NULL, 1.00);
	mLayoutLeft.SetControl("EditSource", mEditSource);
	mEditSource.ReadOnly = true;

	// Subject label - under EditSource
	mLayoutLeft.AddRegion("LabelSubject", "LabelSubject",
		lS, kFBAttachRight, "SourcesList", 1.00,
		lS, kFBAttachBottom, "EditSource", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelSubject", mLabelSubject);
	mLabelSubject.Caption = "Subject:";

	// Subject edit - Under subject label
	mLayoutLeft.AddRegion("EditSubject", "EditSubject",
		lS, kFBAttachRight, "SourcesList", 1.00,
		0, kFBAttachBottom, "LabelSubject", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("EditSubject", mEditSubject);
	mEditSubject.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditSubject);

	// Source info - Under EditSubject
	mLayoutLeft.AddRegion("EditSourceInfo", "EditSourceInfo",
		lS, kFBAttachRight,  "SourcesList", 1.00,
		lS, kFBAttachBottom, "EditSubject", 1.00,
		155, kFBAttachNone,  NULL, 1.00,
		lH, kFBAttachBottom, NULL, 1.00);
	mLayoutLeft.SetControl("EditSourceInfo", mEditSourceInfo);
	mEditSourceInfo.ReadOnly = true;


	/* RIGHT */

	// Right: DestIp Label - Top Left
	mLayoutRight.AddRegion("LabelDestIp", "LabelDestIp",
		lS, kFBAttachLeft, "", 1.00,
		10, kFBAttachTop, "", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("LabelDestIp", mLabelDestIp);
	mLabelDestIp.Caption = "IP:";

	// Right: DestIp Edit - Top, Right of DestIp Label
	mLayoutRight.AddRegion("EditDestIp", "EditDestIp",
		lS, kFBAttachRight, "LabelDestIp", 1.00,
		10, kFBAttachTop,   "", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("EditDestIp", mEditDestIp);
	mEditDestIp.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditIP);



	// Right: Label Dest Port (Under DestIp)
	mLayoutRight.AddRegion("LabelDestPort", "LabelDestPort",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelDestIp", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("LabelDestPort", mLabelDestPort);
	mLabelDestPort.Caption = "Port:";

	// Right: Edit DestPort (Under DestIp)
	mLayoutRight.AddRegion("EditDestPort", "EditDestPort",
		lS, kFBAttachRight, "LabelDestPort", 1.00,
		lS, kFBAttachBottom, "EditDestIp", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("EditDestPort", mEditDestPort);
	mEditDestPort.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditPort);


	// Right: Label Protocol (Under DestPort)
	mLayoutRight.AddRegion("LabelProtocol", "LabelProtocol",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelDestPort", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("LabelProtocol", mLabelProtocol);
	mLabelProtocol.Caption = "Protocol:";

	// Right: Edit Protocol (Under DestPort)
	mLayoutRight.AddRegion("EditProtocol", "EditProtocol",
		lS, kFBAttachRight, "LabelProtocol", 1.00,
		lS, kFBAttachBottom, "EditDestPort", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("EditProtocol", mListProtocol);


	// Right: Label SampleRate (Under Protocol)
	mLayoutRight.AddRegion("LabelSampleRate", "LabelSampleRate",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelProtocol", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("LabelSampleRate", mLabelSamplingRate);
	mLabelSamplingRate.Caption = "Sample Rate:";

	// Right: Edit Samplerate (Under Protocol)
	mLayoutRight.AddRegion("EditSampleRate", "EditSampleRate",
		lS, kFBAttachRight, "LabelSampleRate", 1.00,
		lS, kFBAttachBottom, "EditProtocol", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutRight.SetControl("EditSampleRate", mEditSamplingRate);
	mEditSamplingRate.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditSampleRate);
}

void Open3D_Device_Layout::UIConfigure()
{
	char buffer[40];
	
	SetBorder("LeftLayout", kFBEmbossBorder,  false, true,  2, 1, 90.0, 0);
	SetBorder("RightLayout", kFBEmbossBorder,  false, true, 2, 1, 90.0, 0);

	mListProtocol.Items.Add("UDP");
	mListProtocol.Items.Add("TCP");

	Open3D_Device::TProtocol protocol = mDevice->GetProtocol();
	if (protocol == Open3D_Device::kUDP) mListProtocol.ItemIndex = 0;
	if (protocol == Open3D_Device::kTCP) mListProtocol.ItemIndex = 1;


	PopulateSubjectList();
	PopulateSubjectFields();

	mEditSamplingRate.Value = mDevice->GetSamplingRate();
	mEditDestIp.Text = mDevice->GetNetworkAddress();

	sprintf_s(buffer, 40, "%d", mDevice->GetNetworkPort());
	mEditDestPort.Text = buffer;

}

void Open3D_Device_Layout::PopulateSubjectList()
{
	// draw the subject list with info from the device
	mSourcesList.Items.Clear();
	for (int i = 0; i < mDevice->Items.size(); i++)
	{
		mSourcesList.Items.Add(mDevice->Items[i].mName);
	}
}


void Open3D_Device_Layout::UIRefresh()
{
	// Update realtime values
}

void Open3D_Device_Layout::UIReset()
{
	// Update from the device

}

void Open3D_Device_Layout::EventEditSubject(HISender pSender, HKEvent pEvent)
{
	// The subject name has been changed, updae the device list and mSourcesList
	int id = mSourcesList.ItemIndex;
	if (id < 0) return;

	FBString newValue = mEditSubject.Text.AsString();

	mDevice->Items[id].mName = newValue;
	mSourcesList.Items.RemoveAt(id);
	mSourcesList.Items.InsertAt(id, newValue);
	mSourcesList.Selected(id, true);

}

void Open3D_Device_Layout::EventDeviceStatusChange( HISender pSender, HKEvent pEvent )
{
	UIReset();
}

void Open3D_Device_Layout::EventUIIdle( HISender pSender, HKEvent pEvent )
{
	if( mDevice->Online && mDevice->Live )
	{
		UIRefresh();
	}
}

void Open3D_Device_Layout::PopulateSubjectFields()
{
	// Populate the subject name and source fields based on the current selected item
	int id = mSourcesList.ItemIndex;
	if (id < 0)
	{
		mEditSubject.Text = "";
		mEditSource.Text = "";
	}
	else
	{
		if (id < mDevice->Items.size())
		{
			FBComponent *model = mDevice->Items[id].mModel;
			mEditSubject.Text = mDevice->Items[id].mName;
			mEditSource.Text = model->LongName;
			if (model->Is(FBModelNull::TypeInfo))
				mEditSourceInfo.Text = "Null\n";
			if (model->Is(FBModelRoot::TypeInfo))
				mEditSourceInfo.Text = "Root\n";
			if (model->Is(FBCamera::TypeInfo))
				mEditSourceInfo.Text = "Camera\n";
			if (model->Is(FBModelSkeleton::TypeInfo))
				mEditSourceInfo.Text = "Joint\n";
		}
	}
}

void Open3D_Device_Layout::EventSelectDevice(HISender pSender, HKEvent pEvent)
{
	PopulateSubjectFields();
}

void Open3D_Device_Layout::EventAdd(HISender pSender, HKEvent pEvent)
{
	FBModelList lModels;
	FBGetSelectedModels(lModels);
	for (int i = 0; i < lModels.GetCount(); i++)
	{
		FBModel *mod = lModels[i];
		mDevice->AddItem(mod);
	}

	FBScene* lScene = (FBScene*)FBSystem::TheOne().Scene;

	for (int i = 0; i < lScene->Cameras.GetCount(); i++)
	{
		FBCamera *cam = lScene->Cameras[i];
		if (cam->Selected)
		{
			mDevice->AddItem(cam);
		}
	}

	PopulateSubjectList();

	int id = mSourcesList.Items.GetCount() - 1;
	if(id >= 0)
		mSourcesList.Selected(id, true);

	PopulateSubjectFields();

}

void Open3D_Device_Layout::EventDel(HISender pSender, HKEvent pEvent)
{
	int id = mSourcesList.ItemIndex;
	if (id >= 0)
	{
		mDevice->Items.erase(mDevice->Items.begin() + id);
		PopulateSubjectList();
	}
}

void Open3D_Device_Layout::EventEditIP(HISender pSender, HKEvent pEvent)
{
	mDevice->SetNetworkAddress(mEditDestIp.Text.AsString());
}

void Open3D_Device_Layout::EventEditPort(HISender pSender, HKEvent pEvent)
{
	int port;
	char buffer[40];
	sscanf(mEditDestPort.Text.AsString(), "%d", &port);
	mDevice->SetNetworkPort(port);
	sprintf_s(buffer, 40, "%d", port);
	mEditDestPort.Text = buffer;
}

void Open3D_Device_Layout::EventEditProtocol(HISender pSender, HKEvent pEvent)
{
	//int id = mListProtocol.ItemIndex;
}

void Open3D_Device_Layout::EventEditSampleRate(HISender pSender, HKEvent pEvent)
{
	mDevice->SetSamplingRate(mEditSamplingRate.Value);
}
