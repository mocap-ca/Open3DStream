#include "Open3DStreamFactory.h"

#include "ILiveLinkClient.h"
#include "Misc/MessageDialog.h"
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

void UOpen3DStreamFactory::OnSourceEvent(FOpen3DStreamDataPtr SourceData, FOnLiveLinkSourceCreated InOnLiveLinkSourceCreated) const
{
	if (!SourceData.IsValid()) return;

	TSharedPtr<FOpen3DStreamSource> SharedPtr = MakeShared<FOpen3DStreamSource>(SourceData->Url, SourceData->TimeOffset);
	
	FString ConnectionString;
	InOnLiveLinkSourceCreated.ExecuteIfBound(StaticCastSharedPtr<ILiveLinkSource>(SharedPtr), MoveTemp(ConnectionString));
}

TSharedPtr<ILiveLinkSource> UOpen3DStreamFactory::CreateSource(const FString& ConnectionString) const
{
	// returns a working FOpen3DStreamSource object
	// This isn't used by the livelink gui, maybe it's for blueprint use?

	FText Url;
	if (!FParse::Value(*ConnectionString, TEXT("Url="), Url))
	{
		return TSharedPtr<ILiveLinkSource>();
	}

	const double TimeOffset = 0.0;

	TSharedPtr<FOpen3DStreamSource> ret = MakeShared<FOpen3DStreamSource>(Url, TimeOffset);
	return StaticCastSharedPtr<ILiveLinkSource>(ret);
}

#undef LOCTEXT_NAMESPACE