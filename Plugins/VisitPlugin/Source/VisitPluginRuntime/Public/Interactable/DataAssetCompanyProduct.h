#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAssetCompanyProduct.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FCompanyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText CompanyInfo = FText::GetEmpty();
	UPROPERTY(EditAnywhere)
	UTexture2D* CompanyLogo = nullptr;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FProductInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FString> ProductInfo;
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataAssetCompanyProduct : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<FString, FCompanyInfo> CompanyNameText;
	UPROPERTY(EditAnywhere)
	TMap<FString, FProductInfo> ProductNameText;
};
