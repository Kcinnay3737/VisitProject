#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAssetPartner.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataPartner
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* Image = nullptr;
	UPROPERTY(EditAnywhere)
	FString Link = "";
	UPROPERTY(EditAnywhere)
	FString Description = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataAssetPartner : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<FString, FDataPartner> PartnerNameData;
};
