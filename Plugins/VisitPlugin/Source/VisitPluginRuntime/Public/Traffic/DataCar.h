#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataCar.generated.h"

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataStaticMesh
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* staticMesh = nullptr;
	UPROPERTY(EditAnywhere)
	FVector offsetPosition = FVector(0.0f, 0.0f, 0.0f);
	UPROPERTY(EditAnywhere)
	FRotator offsetRotation = FRotator(0.0f, -90.0f, 0.0f);
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FRayData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FVector rightSideOffsetStartPointLocation = FVector(0.0f, 150.0f, 100.0f);
	UPROPERTY(EditAnywhere)
	float rightSideDistanceLooking = 275.0f;
	UPROPERTY(EditAnywhere)
	FVector rightSideForwardOffsetStartPointLocation = FVector(325.0f, 100.0f, 100.0f);
	UPROPERTY(EditAnywhere)
	float rightSideForwardDistanceLooking = 200.0f;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FBoxData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FVector midOffset = FVector(0.0f, 0.0f, 100.0f);
	UPROPERTY(EditAnywhere)
	FVector boxScale = FVector(250.0f, 500.0f, 100.0f);
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataCar : public UDataAsset
{
	GENERATED_BODY()
public:
	UDataCar();

	UPROPERTY(EditAnywhere)
	FDataStaticMesh dataStaticMesh;

	UPROPERTY(EditAnywhere)
	FRayData dataRay;

	UPROPERTY(EditAnywhere)
	FBoxData dataBox;
};
