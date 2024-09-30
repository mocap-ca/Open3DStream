#include "device.h"
#include "layout.h"

#include "o3ds/o3ds.h" // for version#
#include <sstream>
#include <fbsdk/fbaudio.h>

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
	// Create the UI Elements and populate static values

	int lS = 6;
	int lH = 25;
	int labelWidth = 80;
	int fieldWidth = 150;
	int listWidth = 90;
	int leftWidth = 260;
	int buttonWidth = listWidth / 2 - 3;

	// Left Panel

	// Left Layout - Attached to the left
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

	// Middle Panel

	// Middle Layout - Attached to the right side of the left panel
	AddRegion("MiddleLayout", "MiddleLayout",
		lS * 2, kFBAttachRight, "LeftLayout", 1.00,
		lS, kFBAttachTop, "", 1.00,
		250, kFBAttachNone, "", 1.00,
		-lS, kFBAttachBottom, "", 1.00);
	SetControl("MiddleLayout", mLayoutMiddle);

	// Middle Label
	AddRegion("MiddleLabel", "MiddleLabel",
		5, kFBAttachLeft, "MiddleLayout", 1.00,
		-10, kFBAttachTop, "MiddleLayout", 1.00,
		50, kFBAttachNone, "", 1.00,
		17, kFBAttachNone, "", 1.00);
	SetControl("MiddleLabel", mLabelMiddle);
	mLabelMiddle.Caption = "Link";

	// Right Panel (log) - Left is attached to the right of Middle
	//                   - Right is attached to the right
	AddRegion("RightLayout", "RightLayout",
		lS, kFBAttachRight, "MiddleLayout", 1.00,
		lS, kFBAttachTop, "", 1.00,
		lS, kFBAttachRight, "", 1.00,
		-lS, kFBAttachBottom, "", 1.00);
	SetControl("RightLayout", mMemoLog);


	// LEFT - Elements

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
	mButtonAdd.Caption = "+";
	mButtonAdd.OnClick.Add(this, (FBCallback)&Open3D_Device_Layout::EventAdd);

	// Del button - under sources list, right of add button
	mLayoutLeft.AddRegion("DelButton", "DelButton",
		lS,           kFBAttachRight,  "AddButton",   1.00,
		lS,           kFBAttachBottom, "SourcesList",  1.00,
		buttonWidth,  kFBAttachNone,   NULL,        1.00,
		30,           kFBAttachNone, "",          1.00);
	mLayoutLeft.SetControl("DelButton", mButtonDel);
	mButtonDel.Caption = "-";
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
		2, kFBAttachBottom, "LabelSource", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone,  NULL, 1.00);
	mLayoutLeft.SetControl("EditSource", mEditSource);
	mEditSource.ReadOnly = true;

	// Subject label - Top, right of SourcesList
	mLayoutLeft.AddRegion("LabelSubject", "LabelSubject",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachTop, "", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelSubject", mLabelSubject);
	mLabelSubject.Caption = "Subject:";

	// Subject edit - Under subject label
	mLayoutLeft.AddRegion("EditSubject", "EditSubject",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "LabelSubject", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("EditSubject", mEditSubject);
	mEditSubject.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditSubject);

	// Delta label - under EditSubject
	mLayoutLeft.AddRegion("LabelDelta", "LabelDelta",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "EditSubject", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelDelta", mLabelDelta);
	mLabelDelta.Caption = "Delta Threshold:";

	// Delta list - Under Delta Label
	mLayoutLeft.AddRegion("EditDelta", "EditDelta",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "LabelDelta", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("EditDelta", mEditDelta);
	mEditDelta.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditDelta);

	// Joints label - under EditDelta
	mLayoutLeft.AddRegion("LabelJoints", "LabelJoints",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "EditDelta", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelJoints", mLabelJoints);
	mLabelJoints.Caption = "Joints:";

	// Joints list - Under Joints Label
	mLayoutLeft.AddRegion("JointsList", "JointsList",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "LabelJoints", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("JointsList", mMemoJoints);
	mMemoJoints.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditJoints);

	// Microphone Source label - under JointsList
	mLayoutLeft.AddRegion("LabelMicSource", "LabelMicSource",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "JointsList", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("LabelMicSource", mLabelMicSource);
	mLabelJoints.Caption = "Microphone Source:";

	// Microphones list - Under Microphone Source Label
	mLayoutLeft.AddRegion("ListMicSource", "ListMicSource",
		lS, kFBAttachRight, "SourcesList", 1.00,
		2, kFBAttachBottom, "LabelMicSource", 1.00,
		155, kFBAttachNone, "", 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutLeft.SetControl("ListMicSource", mListMicSource);
	mListMicSource.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventSelectMicSource);
	mListMicSource.Style = FBListStyle::kFBDropDownList;

	// Version info - at bottom
	mLayoutLeft.AddRegion("VersionInfo", "VersionInfo",
		lS, kFBAttachRight, "SourcesList", 1.00,
		-lH, kFBAttachBottom, NULL, 1.00,
		155, kFBAttachNone, NULL, 1.00,
		0, kFBAttachBottom, "", 1.00);
	mLayoutLeft.SetControl("VersionInfo", mLabelPluginVersion);
	//mPluginVersion.ReadOnly = true;
	mLabelPluginVersion.Caption = "---";


	/* MIDDLE Elements */

	// Middle: DestIp Label - Top Left
	mLayoutMiddle.AddRegion("LabelDestIp", "LabelDestIp",
		lS, kFBAttachLeft, "", 1.00,
		10, kFBAttachTop, "", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("LabelDestIp", mLabelDestIp);
	mLabelDestIp.Caption = "IP:";

	// Middle: DestIp Edit - Top, Right of DestIp Label
	mLayoutMiddle.AddRegion("EditDestIp", "EditDestIp",
		lS, kFBAttachRight, "LabelDestIp", 1.00,
		10, kFBAttachTop,   "", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("EditDestIp", mEditDestIp);
	mEditDestIp.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditIP);



	// Middle: Label Dest Port (Under DestIp)
	mLayoutMiddle.AddRegion("LabelDestPort", "LabelDestPort",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelDestIp", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("LabelDestPort", mLabelDestPort);
	mLabelDestPort.Caption = "Port:";

	// Middle: Edit DestPort (Under DestIp)
	mLayoutMiddle.AddRegion("EditDestPort", "EditDestPort",
		lS, kFBAttachRight, "LabelDestPort", 1.00,
		lS, kFBAttachBottom, "EditDestIp", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("EditDestPort", mEditDestPort);
	mEditDestPort.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditPort);


	// Middle: Label Protocol (Under DestPort)
	mLayoutMiddle.AddRegion("LabelProtocol", "LabelProtocol",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelDestPort", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("LabelProtocol", mLabelProtocol);
	mLabelProtocol.Caption = "Protocol:";

	// Middle: Edit Protocol (Under DestPort)
	mLayoutMiddle.AddRegion("EditProtocol", "EditProtocol",
		lS, kFBAttachRight, "LabelProtocol", 1.00,
		lS, kFBAttachBottom, "EditDestPort", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("EditProtocol", mListProtocol);
	mListProtocol.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditProtocol);


	// Middle: Label Key (Under Protocol)
	mLayoutMiddle.AddRegion("LabelKey", "LabelKey",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelProtocol", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("LabelKey", mLabelKey);
	mLabelKey.Caption = "Key:";

	// Middle: Edit Key (Under Protocol)
	mLayoutMiddle.AddRegion("EditKey", "EditKey",
		lS, kFBAttachRight, "LabelKey", 1.00,
		lS, kFBAttachBottom, "EditProtocol", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("EditKey", mEditKey);
	mEditKey.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditKey);


	// Middle: Label SampleRate (Under Key)
	mLayoutMiddle.AddRegion("LabelSampleRate", "LabelSampleRate",
		lS, kFBAttachLeft, "", 1.00,
		lS, kFBAttachBottom, "LabelKey", 1.00,
		labelWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("LabelSampleRate", mLabelSamplingRate);
	mLabelSamplingRate.Caption = "Sample Rate:";

	// Middle: Edit Samplerate (Under Protocol)
	mLayoutMiddle.AddRegion("EditSampleRate", "EditSampleRate",
		lS, kFBAttachRight, "LabelSampleRate", 1.00,
		lS, kFBAttachBottom, "EditKey", 1.00,
		fieldWidth, kFBAttachNone, NULL, 1.00,
		lH, kFBAttachNone, NULL, 1.00);
	mLayoutMiddle.SetControl("EditSampleRate", mEditSamplingRate);
	mEditSamplingRate.OnChange.Add(this, (FBCallback)&Open3D_Device_Layout::EventEditSampleRate);

	SetBorder("LeftLayout", kFBEmbossBorder, false, true, 2, 1, 90.0, 0);
	SetBorder("MiddleLayout", kFBEmbossBorder, false, true, 2, 1, 90.0, 0);

}

void Open3D_Device_Layout::UIConfigure()
{
	// Populate the dynamic elements of the ui
	
	mListProtocol.Items.Clear();
	mListProtocol.Items.Add("NNG-PipelinePush");
	mListProtocol.Items.Add("NNG-PairServer");
	mListProtocol.Items.Add("NNG-PairClient");
	mListProtocol.Items.Add("NNG-Publish");
	mListProtocol.Items.Add("UDP");
	mListProtocol.Items.Add("TCP Server");
	mListProtocol.Items.Add("TCP Client");

	Open3D_Device::TProtocol protocol = mDevice->GetProtocol();
	if (protocol == Open3D_Device::kNNGPipeline) mListProtocol.ItemIndex = 0;
	if (protocol == Open3D_Device::kNNGPairServer) mListProtocol.ItemIndex = 1;
	if (protocol == Open3D_Device::kNNGPairClient) mListProtocol.ItemIndex = 2;
	if (protocol == Open3D_Device::kNNGPublish) mListProtocol.ItemIndex = 3;
	if (protocol == Open3D_Device::kUDP) mListProtocol.ItemIndex = 4;
	if (protocol == Open3D_Device::kTCPServer) mListProtocol.ItemIndex = 5;
	if (protocol == Open3D_Device::kTCPClient) mListProtocol.ItemIndex = 6;
	 
	PopulateSubjectList();
	PopulateSubjectFields();
	PopulateMicrophoneList();

	mMemoLog.Enabled = false;


	mEditSamplingRate.Value = mDevice->GetSamplingRate();
	mEditDestIp.Text = mDevice->GetNetworkAddress();
	mEditKey.Text = mDevice->GetKey();

	char buffer[40];
	sprintf_s(buffer, 40, "%d", mDevice->GetNetworkPort());
	mEditDestPort.Text = buffer;
	mEditDestPort.Enabled = false; // enabed by the combo changing

	sprintf_s(buffer, 40, "%.4f", mDevice->GetDeltaThreshold());
	mEditDelta.Text = buffer;

	mLabelPluginVersion.Caption = FBString("Version: ") + FBString(O3DS::getVersion());
}

void Open3D_Device_Layout::PopulateSubjectList()
{
	// draw the subject list with info from the device
	mSourcesList.Items.Clear();
	for (int i = 0; i < mDevice->Items.size(); i++)
	{
		mSourcesList.Items.Add(FBString(mDevice->Items[i]->mName.c_str()));
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

	mDevice->Items[id]->mName = newValue.operator char *();
	mSourcesList.Items.RemoveAt(id);
	mSourcesList.Items.InsertAt(id, newValue);
	mSourcesList.Selected(id, true);
}

void Open3D_Device_Layout::EventEditDelta(HISender pSender, HKEvent pEvent)
{
	FBString newValue = mEditDelta.Text.AsString();
	float value = atof(newValue);
	mDevice->SetDeltaThreshold(value);
}

void Open3D_Device_Layout::EventDeviceStatusChange( HISender pSender, HKEvent pEvent )
{
	UIReset();
}

void Open3D_Device_Layout::EventUIIdle( HISender pSender, HKEvent pEvent )
{
	bool active = mDevice->Online;
	mEditDestIp.Enabled = !active;
	mEditDestPort.Enabled = !active;
	mListProtocol.Enabled = !active;
	mEditKey.Enabled = !active;
	mEditSamplingRate.Enabled = !active;	
	mButtonAdd.Enabled = !active;
	mButtonDel.Enabled = !active;

	bool subjectConnected = mSourcesList.ItemIndex >= 0;
	mMemoJoints.Enabled = subjectConnected;
	mEditSubject.Enabled = subjectConnected;
	mEditSource.Enabled = subjectConnected;
	mEditDelta.Enabled = subjectConnected;
	mLabelDelta.Enabled = subjectConnected;
	mLabelSubject.Enabled = subjectConnected;
	mLabelJoints.Enabled = subjectConnected;

	if( mDevice->Online && mDevice->Live )
	{
		UIRefresh();
	}
}

void Open3D_Device_Layout::PopulateSubjectFields()
{
	size_t count = 0;
	// Populate the subject name and source fields based on the current selected item
	int id = mSourcesList.ItemIndex;
	if (id < 0)
	{
		mEditSubject.Text = "";
		mEditSource.Text = "";
		mEditDelta.Text = "";
	}
	else
	{
		if (id < mDevice->Items.size())
		{
			O3DS::Subject* subject = mDevice->Items[id];
			FBModel *model = static_cast<FBModel*>(subject->mReference);

			mEditSubject.Text = mDevice->Items[id]->mName.c_str();
			mEditSource.Text = model->GetFullName();

			std::ostringstream imploded, oss;
			std::copy(mDevice->Items[id]->mJoints.begin(), mDevice->Items[id]->mJoints.end(),
				std::ostream_iterator<std::string>(imploded, " "));

			mMemoJoints.Text = imploded.str().c_str();

			O3DS::Mobu::TraverseSubject(subject, model);

			if (model->Is(FBModelNull::TypeInfo))
				oss << "Null" << std::endl;
			if (model->Is(FBModelRoot::TypeInfo))
				oss << "Root" << std::endl;
			if (model->Is(FBCamera::TypeInfo))
				oss << "Camera" << std::endl;
			if (model->Is(FBModelSkeleton::TypeInfo))
				oss << "Joint" << std::endl;

			oss << "Items: " << mDevice->Items[id]->mTransforms.size() << std::endl;

			oss << "Joints: " << mDevice->Items[id]->mJoints.size() << std::endl;

			std::vector<char> buf;
			mDevice->Items.Serialize(buf, count);
			oss << "Packet1: " << buf.size() << std::endl;

			mDevice->Items.SerializeUpdate(buf, count);
			oss << "Packet2: " << buf.size() << std::endl;

			mMemoLog.Text = oss.str().c_str();
		}
	}
}

void Open3D_Device_Layout::PopulateMicrophoneList() {
	mListMicSource.Items.Clear();
	auto micList = FBPropertyListAudioIn();
	FBAudioIn* mic;
	for (auto i=0; (mic = micList[i]) != nullptr; i++) {
		mListMicSource.Items.Add(mic->Name);
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
		mDevice->Items.mItems.erase(mDevice->Items.begin() + id);
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
	mEditDestPort.Enabled = true;
	int id = mListProtocol.ItemIndex;
	if (id == 0) mDevice->SetProtocol(Open3D_Device::TProtocol::kNNGPipeline);
	if (id == 1) mDevice->SetProtocol(Open3D_Device::TProtocol::kNNGPairServer);
	if (id == 2) mDevice->SetProtocol(Open3D_Device::TProtocol::kNNGPairClient);
	if (id == 3) mDevice->SetProtocol(Open3D_Device::TProtocol::kNNGPublish);
	if (id == 4) mDevice->SetProtocol(Open3D_Device::TProtocol::kUDP);
	if (id == 5) mDevice->SetProtocol(Open3D_Device::TProtocol::kTCPServer);
	if (id == 6) mDevice->SetProtocol(Open3D_Device::TProtocol::kTCPClient);

}

void Open3D_Device_Layout::EventEditSampleRate(HISender pSender, HKEvent pEvent)
{
	mDevice->SetSamplingRate(mEditSamplingRate.Value);
}

void Open3D_Device_Layout::EventEditKey(HISender pSender, HKEvent pEvent)
{
	mDevice->SetKey(mEditKey.Text);
}

void Open3D_Device_Layout::EventEditJoints(HISender pSender, HKEvent pEvent)
{
	// The subject name has been changed, updae the device list and mSourcesList
	int id = mSourcesList.ItemIndex;
	if (id < 0) return	;

	std::string s(mMemoJoints.Text.AsString());
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	mDevice->Items[id]->mJoints.clear();
	std::copy(vstrings.begin(), vstrings.end(), std::back_inserter(mDevice->Items[id]->mJoints));
}

void Open3D_Device_Layout::EventSelectMicSource(HISender pSender, HKEvent pEvent) {
}