#pragma once

#include "CoreMinimal.h"
#include "Open3DStreamData.generated.h"

// E:\Unreal\UE_4.25\Engine\Plugins\Animation\LiveLink\Source\LiveLink\Public
//    LiveLinkMessageBusFinder.h

USTRUCT(BlueprintType)
struct OPEN3DSTREAM_API FOpen3DStreamData
{
public:
	GENERATED_USTRUCT_BODY()
	
	FOpen3DStreamData() = default;
	
	FOpen3DStreamData(const FText& InUrl, double InTimeOffset)
		: Url(InUrl)
		, TimeOffset(InTimeOffset)
	{}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Open3DStream")
	FText			Url;
	
	UPROPERTY()
	double			TimeOffset;

	UPROPERTY()
	FText           Key;

	UPROPERTY()
	FText           Protocol;
};

typedef TSharedPtr<FOpen3DStreamData, ESPMode::ThreadSafe> FOpen3DStreamDataPtr;
