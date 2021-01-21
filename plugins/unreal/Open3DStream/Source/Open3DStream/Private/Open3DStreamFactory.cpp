#include "Open3DStreamFactory.h"

#include "ILiveLinkClient.h"
#include "Misc/MessageDialog.h"
#include "Open3DStreamSourceSettings.h"
#include "SOpen3DStreamFactory.h"

class FOpen3DStreamSource;

#define LOCTEXT_NAMESPACE "Open3DStream"

FText UOpen3DStreamFactory::GetSourceDisplayName() const
{
	return LOCTEXT("StreamSourceDisplayName", "Open 3D Stream Source");
}

FText UOpen3DStreamFactory::GetSourceTooltip() const
{
	return LOCTEXT("StreamSourceTooltip", "Open 3D Stream Source");
}

TSharedPtr<SWidget> UOpen3DStreamFactory::BuildCreationPanel(FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	return SNew(SOpen3DStreamFactory)
		.OnSelectedEvent(FOnOpen3DStreamSelected::CreateUObject(this, &UOpen3DStreamFactory::OnSourceEvent, InOnLiveLinkSourceCreated));
}

void UOpen3DStreamFactory::OnSourceEvent(FOpen3DStreamSettingsPtr SourceData, FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	// This is called by the UI - SOpen3DStreamFactory::OnSource()

	FString ConnectionString;
	//FOpen3DStreamSettings::StaticStruct()->ExportText(ConnectionString, &GetDefault<UOpen3DStreamSettingsObject>()->Settings, nullptr, nullptr, PPF_None, nullptr);
	FOpen3DStreamSettings::StaticStruct()->ExportText(ConnectionString, &(*SourceData), nullptr, nullptr, PPF_None, nullptr);
	GetMutableDefault<UOpen3DStreamSettingsObject>()->SaveConfig();

	TSharedPtr<FOpen3DStreamSource> SharedPtr = MakeShared<FOpen3DStreamSource>(*SourceData);
	InOnLiveLinkSourceCreated.ExecuteIfBound(StaticCastSharedPtr<ILiveLinkSource>(SharedPtr), MoveTemp(ConnectionString));
}


TSharedPtr<ILiveLinkSource> UOpen3DStreamFactory::CreateSource(const FString& ConnectionString) const
{
	// returns a working FOpen3DStreamSource object
	// This isn't used by the livelink gui, maybe it's for blueprint use?

	FOpen3DStreamSettings Setting;
	if (!ConnectionString.IsEmpty())
	{
		FOpen3DStreamSettings::StaticStruct()->ImportText(*ConnectionString, &Setting, nullptr, PPF_None, GLog, TEXT("UOpen3DStreamSourceFactory"));
	}

	TSharedPtr<FOpen3DStreamSource> SharedPtr = MakeShared<FOpen3DStreamSource>(Setting);

	return StaticCastSharedPtr<ILiveLinkSource>(SharedPtr);

}

#undef LOCTEXT_NAMESPACE