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
	UTexture2D* PartnerLogo = nullptr;
	UPROPERTY(EditAnywhere)
	FString ParterLink = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataAssetPartner : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TMap<FString, FDataPartner> DataPartner;
};
