#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Road.generated.h"

class USceneComponent;
class USplineComponent;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataRoad
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool reverseRoad = false;
	UPROPERTY(EditAnywhere)
	float speed = 500.0f;
	UPROPERTY(EditAnywhere)
	float accelerationTime = 0.5f;
	UPROPERTY(EditAnywhere)
	float decelerationTime = 0.1f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	int rarity = 100;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000"))
	int roadPriority = 0;
};

UCLASS()
class VISITPLUGINRUNTIME_API ARoad : public AActor
{
	GENERATED_BODY()
	
public:	
	ARoad();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USplineComponent* _SplineComponent;

private:	
	UPROPERTY(EditAnywhere, Category = "VisitData/RoadAttribut")
	FDataRoad _DataRoad;

public:
	FVector GetStartSplinePoint();
	FVector GetEndSplinePoint();

	FVector GetSplineLocationAtValue(float value);
	FRotator GetSplineRotationAtValue(float value);

	TArray<FVector> GetAllSplinePoint();
	TArray<FVector> GetSplinePointBetweenDistance(float distance1, float distance2);

	float GetSplineLenght();
	float GetSplineLenghtAtPoint(int pointIndex);

	int GetPointIndexAtLocation(FVector location);

	FDataRoad GetDataRoad();

	FVector RoundThisVector(FVector vector);
};
