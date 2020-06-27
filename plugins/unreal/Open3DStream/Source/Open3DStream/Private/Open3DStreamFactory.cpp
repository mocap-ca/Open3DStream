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

	TSharedPtr<FOpen3DStreamSource> SharedPtr = MakeShared<FOpen3DStreamSource>(SourceData->Port, SourceData->TimeOffset);
	
	FString ConnectionString;
	InOnLiveLinkSourceCreated.ExecuteIfBound(StaticCastSharedPtr<ILiveLinkSource>(SharedPtr), MoveTemp(ConnectionString));
}


#undef LOCTEXT_NAMESPACE