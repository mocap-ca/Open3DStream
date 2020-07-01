#pragma once
#include "Engine/Engine.h"
#include "Widgets/SCompoundWidget.h"
#include "Open3DStreamData.h"

// E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\SLiveLinkMessageBusSourceFactory.h

DECLARE_DELEGATE_OneParam(FOnOpen3DStreamSelected, FOpen3DStreamDataPtr);

class OPEN3DSTREAM_API SOpen3DStreamFactory : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SOpen3DStreamFactory) {}
	SLATE_EVENT(FOnOpen3DStreamSelected, OnSelectedEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

	void SetPort(int InPort) { mPort = InPort; }
	TOptional<int> GetPort() const { return mPort; }

	FOpen3DStreamDataPtr GetSourceData() const { return Result;  }

	FOpen3DStreamDataPtr Result;

	FOnOpen3DStreamSelected OnSelectedEvent;

private:
	FReply OnSource();
	int  mPort;

};