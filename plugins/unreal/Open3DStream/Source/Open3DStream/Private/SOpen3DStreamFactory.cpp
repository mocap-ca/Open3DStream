#include "SOpen3DStreamFactory.h"
#include "Widgets/Input/SNumericEntryBox.h"

#define LOCTEXT_NAMESPACE "Open3DStream"

void SOpen3DStreamFactory::Construct(const FArguments& Args)
{
	//LastTickTime = 0.0;
	OnSelectedEvent = Args._OnSelectedEvent;

	mPort = 5566;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Open3DStreamPort", "Port1"))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[	
			SNew(SNumericEntryBox<int>)
			.Value(this, &SOpen3DStreamFactory::GetPort)
			.OnValueChanged(this, &SOpen3DStreamFactory::SetPort)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			SNew(SButton)
			.OnClicked(this, &SOpen3DStreamFactory::OnSource)
			.Content()
			[
				SNew(STextBlock)
				.MinDesiredWidth(150)
				.Justification(ETextJustify::Center)
				.Text(LOCTEXT("OkayButton", "Okay"))
			]
		]
	];


}


FReply SOpen3DStreamFactory::OnSource()
{
	TSharedPtr<FOpen3DStreamData, ESPMode::ThreadSafe> Data = MakeShared<FOpen3DStreamData, ESPMode::ThreadSafe>();
	Data->TimeOffset = 0;
	Data->Port = mPort;
	OnSelectedEvent.ExecuteIfBound(Data);
	return FReply::Handled();
}