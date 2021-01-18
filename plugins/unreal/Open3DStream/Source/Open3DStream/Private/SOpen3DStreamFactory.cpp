#include "SOpen3DStreamFactory.h"
#include "Widgets/Input/SEditableTextBox.h"

#define LOCTEXT_NAMESPACE "Open3DStream"

void SOpen3DStreamFactory::Construct(const FArguments& Args)
{
	//LastTickTime = 0.0;
	OnSelectedEvent = Args._OnSelectedEvent;

	Options.Add(MakeShareable(new FString("Subscribe")));
	Options.Add(MakeShareable(new FString("Client")));
	Options.Add(MakeShareable(new FString("Server")));
	CurrentProtocol = Options[0];


	//mUrl = LOCTEXT("Open3DStreamUrlValue", "tcp://3.131.65.210:6001");
	mUrl = LOCTEXT("Open3DStreamUrlValue", "tcp://127.0.0.1:6001");

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
				SNew(STextBlock).Text(LOCTEXT("Open3DStreamUrl", "Url")).MinDesiredWidth(150)
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
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			[
				SNew(STextBlock).Text(LOCTEXT("Open3DStreamProtocol", "Protocol")).MinDesiredWidth(150)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.7f)
			[	
				SNew(SComboBox<FComboItemType>)
				.OptionsSource(&Options)
				.OnSelectionChanged(this, &SOpen3DStreamFactory::OnProtocolChanged)
				.OnGenerateWidget(this, &SOpen3DStreamFactory::MakeWidgetForOption)
				.InitiallySelectedItem(CurrentProtocol)
				[
					SNew(STextBlock)
					.Text(this, &SOpen3DStreamFactory::GetCurrentProtocol)
				]
			]
		]
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			[
				SNew(STextBlock).Text(LOCTEXT("Open3DStreamKey", "Key")).MinDesiredWidth(150)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.7f)
			[	
				SNew(SEditableTextBox)
				.Text(this, &SOpen3DStreamFactory::GetKey)
				.OnTextChanged(this, &SOpen3DStreamFactory::SetKey)
			]
		]
		+ SVerticalBox::Slot()
		.Padding(5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(0.3f)
			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
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
   		    + SHorizontalBox::Slot()
			.FillWidth(0.3f)
		]
		+ SVerticalBox::Slot()
			.Padding(5)
			[
				SNew(STextBlock).Text(LOCTEXT("Open3DStreamVersion", "V0.4"))
			]

	];
}

FReply SOpen3DStreamFactory::OnSource()
{
	TSharedPtr<FOpen3DStreamSettings, ESPMode::ThreadSafe> Settings = MakeShared<FOpen3DStreamSettings, ESPMode::ThreadSafe>();
	Settings->TimeOffset = 0;
	Settings->Url = mUrl;
	Settings->Protocol = GetCurrentProtocol();
	OnSelectedEvent.ExecuteIfBound(Settings);
	return FReply::Handled();
}

TSharedRef<SWidget> SOpen3DStreamFactory::MakeWidgetForOption(FComboItemType InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SOpen3DStreamFactory::OnProtocolChanged(FComboItemType NewValue, ESelectInfo::Type)
{
	CurrentProtocol = NewValue;
}

FText SOpen3DStreamFactory::GetCurrentProtocol() const
{
	if (CurrentProtocol.IsValid())
	{
		return FText::FromString(*CurrentProtocol);
	}

	return LOCTEXT("InvalidComboEntryText", "<<Invalid option>>");
}