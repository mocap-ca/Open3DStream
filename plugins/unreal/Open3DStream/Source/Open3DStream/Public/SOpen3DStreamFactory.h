#pragma once
#include "Engine/Engine.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"
#include "Open3DStreamData.h"

// E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Private\SLiveLinkMessageBusSourceFactory.h

DECLARE_DELEGATE_OneParam(FOnOpen3DStreamSelected, FOpen3DStreamDataPtr);

class OPEN3DSTREAM_API SOpen3DStreamFactory : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SOpen3DStreamFactory) {}
	SLATE_EVENT(FOnOpen3DStreamSelected, OnSelectedEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

	void SetUrl(const FText &InUrl) { mUrl = InUrl; }
	FText GetUrl() const { return mUrl; }

	void SetKey(const FText &InKey) { mKey = InKey; }
	FText GetKey() const { return mKey; }

	FOpen3DStreamDataPtr GetSourceData() const { return Result;  }

	FOpen3DStreamDataPtr Result;

	FOnOpen3DStreamSelected OnSelectedEvent;

	// Protocol Combo
	typedef TSharedPtr<FString> FComboItemType;
	TSharedRef<SWidget> MakeWidgetForOption(FComboItemType InOption);
	void OnProtocolChanged(FComboItemType NewValue, ESelectInfo::Type);
	FText GetCurrentProtocol() const;


private:
	FReply OnSource();
	FText mUrl;
	FText mKey;

	TArray<FComboItemType> Options;
	FComboItemType CurrentProtocol;

};