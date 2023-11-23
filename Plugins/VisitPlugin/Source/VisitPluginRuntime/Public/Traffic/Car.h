#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Car.generated.h"

class AIntersection;
class ARoad;
class ATraffic;
class UBoxComponent;
class UDataCar;
class USceneComponent;
class UStaticMesh;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FMaxMinRarity
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int min = 0;
	UPROPERTY()
	int max = 0;

	UPROPERTY()
	FVector splinePoint = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FRayStartEndPoint
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector startPoint = FVector::ZeroVector;
	UPROPERTY()
	FVector endPoint = FVector::ZeroVector;
};

UCLASS()
class VISITPLUGINRUNTIME_API ACar : public AActor
{
	GENERATED_BODY()
	
public:	
	ACar();
	virtual void Tick(float DeltaTime) override;

	void SpawnCar(ARoad* road, float distance, UDataCar* carData);
	void DestroyCar();

	void Move(ATraffic* traffic, float deltaTime);

	void EnterIntersection(AIntersection* intersection);
	void ExitIntersection();

	void Accelerate(float deltaTime);
	void Decelerate(float deltaTime);
	void ApplyMovement(float deltaTime, float lerpValue);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* _StaticMeshComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	UBoxComponent* _BoxComponent;

private:	
	UPROPERTY()
	UDataCar* _CarData;

	UPROPERTY()
	ARoad* _CurrRoad;
	UPROPERTY()
	float _CurrDistance;

	UPROPERTY()
	bool _IsDestroy;

	bool LookForward(ATraffic* traffic);
	bool CheckCollision(ATraffic* traffic, float deltaTime);
	bool CheckHasOtherRoad(ATraffic* traffic, float deltaTime);
	bool EndRoad(ATraffic* traffic);

	float _CurrTimeCollision = 0.0f;
	float _CurrTimeWait = 10.0f;

	TMap<ACar*, ACar*> _BlockCar;

	bool _bStop = false;
	float _CurrStopTime = 0.0f;

	float _DelayChangeRoadAgain = 0.3f;

	bool _bIsInIntersection = false;
	bool _bIsChangeRoadDuringIntersection = false;

	float _CurrLerpSpeedValue = 0.0f;

	AIntersection* _CurrIntersection = nullptr;

	bool _bAccelerateLastFrame;

public:
	bool GetIsDestroy();

	ARoad* GetCurrRoad();
	float GetCurrDistance();

	bool CheckIsBlockingByThisCar(ACar* car);
	
	void SetStop(bool bValue);
	bool GetStop();
	float GetCurrStopTime();

	void SetIntersection(AIntersection* intersection);

	void InitCollision(ATraffic* traffic);
};
