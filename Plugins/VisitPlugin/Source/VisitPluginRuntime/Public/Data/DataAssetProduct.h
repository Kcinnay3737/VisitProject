#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAssetProduct.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataProduct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* _Image = nullptr;
	UPROPERTY(EditAnywhere)
	FString Description = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataAssetProduct : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FString, FDataProduct> ProductNameData;
};
