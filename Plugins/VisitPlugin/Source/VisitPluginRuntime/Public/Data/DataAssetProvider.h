#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAssetProvider.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataProvider
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* _Image = nullptr;
	UPROPERTY(EditAnywhere)
	FString Description = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataAssetProvider : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FString, FDataProvider> ProviderNameData;
};
