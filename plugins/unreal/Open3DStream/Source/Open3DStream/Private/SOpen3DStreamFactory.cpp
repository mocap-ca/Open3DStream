#include "SOpen3DStreamFactory.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "Open3DStream"

void SOpen3DStreamFactory::Construct(const FArguments& Args)
{
	//LastTickTime = 0.0;
	OnSelectedEvent = Args._OnSelectedEvent;

	mUrl = LOCTEXT("Open3DStreamUrlValue", "tcp://localhost:5555");

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
				.Text(LOCTEXT("Open3DStreamUrl", "Url"))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
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
	Data->Url = mUrl;
	OnSelectedEvent.ExecuteIfBound(Data);
	return FReply::Handled();
}