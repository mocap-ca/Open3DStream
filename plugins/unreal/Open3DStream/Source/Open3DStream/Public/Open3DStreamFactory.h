#pragma once

#include "LiveLinkSourceFactory.h"
#include "Open3DStreamData.h"
#include "Open3DStreamFactory.generated.h"

/* 
https://docs.unrealengine.com/en-US/Engine/Animation/LiveLinkPlugin/LiveLinkPluginDevelopment/index.html
This is how Live Link knows about the sources it can use. The source factory must be a UObject and derive
from ULiveLinkSourceFactory. The factory provides functions to create sources and create custom editor UI
for source creation. Source factories are automatically picked up by the client (there is no manual
registration process).
*/

UCLASS()
class OPEN3DSTREAM_API UOpen3DStreamFactory : public ULiveLinkSourceFactory
{
public:
	GENERATED_BODY()

	virtual FText GetSourceDisplayName() const override;
	virtual FText GetSourceTooltip() const override;	
	virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }

	virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const override;
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const override;

private:
	void OnSourceEvent(FOpen3DStreamDataPtr SourceData, FOnLiveLinkSourceCreated OnLiveLinkSourceCreated) const;
};
