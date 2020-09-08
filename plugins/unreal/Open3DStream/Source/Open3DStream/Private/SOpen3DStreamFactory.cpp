#include "SOpen3DStreamFactory.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "Open3DStream"

void SOpen3DStreamFactory::Construct(const FArguments& Args)
{
	//LastTickTime = 0.0;
	OnSelectedEvent = Args._OnSelectedEvent;

	mUrl = LOCTEXT("Open3DStreamUrlValue", "tcp://3.131.65.210:6000");
	//mUrl = LOCTEXT("Open3DStreamUrlValue", "tcp://127.0.0.1:6000");

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Open3DStreamUrl", "Url"))
				.MinDesiredWidth(170)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.7f)
			[	
			SNew(SEditableTextBox)
			.Text(this, &SOpen3DStreamFactory::GetUrl)
			.OnTextChanged(this, &SOpen3DStreamFactory::SetUrl)
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
				.MinDesiredWidth(100)
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
	Data->Url = mUrl;
	OnSelectedEvent.ExecuteIfBound(Data);
	return FReply::Handled();
}