#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Traffic.generated.h"

class ACar;
class ADeadSpawnZone;
class ARoad;
class ATrafficParent;
class UDataCar;
class UStaticMesh;
class USceneComponent;

UENUM(BlueprintType)
enum class ECarOverlappeBehavior : uint8
{
	destroy UMETA(DisplayName = "Destroy"),
	decelerate UMETA(DisplayName = "Decelerate")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FCarsAttribut
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float carDistanceOffsetToDelete = 10.0f;
	UPROPERTY(EditAnywhere)
	ECarOverlappeBehavior carOverlappeBehavior = ECarOverlappeBehavior::destroy;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "carOverlappeBehavior == ECarOverlappeBehavior::destroy"))
	float timeOverlappeToDestroyCar = 0.2f;
	UPROPERTY(EditAnywhere)
	float timeWaitAfterCollision = 0.0f;
	UPROPERTY(EditAnywhere)
	bool bUseContinuousCollisionDetection = true;
	UPROPERTY(EditAnywhere)
	bool bTraceComplexOnMove = true;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FTrafficData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<UDataCar*> carsData;
	UPROPERTY(EditAnywhere)
	FCarsAttribut carsAttribut;
	UPROPERTY(EditAnywhere)
	bool drawDebugLine = false;
	UPROPERTY(EditAnywhere)
	int trafficDensity = 10;
	UPROPERTY(EditAnywhere)
	TArray<ARoad*> startRoad;
	UPROPERTY(EditAnywhere)
	float carMoveTimeInterval = 0.0f;
};

UCLASS()
class VISITPLUGINRUNTIME_API ATraffic : public AActor
{
	GENERATED_BODY()

public:
	ATraffic();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	//Component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;

	//Parent actor
	UPROPERTY()
	ATrafficParent* _RoadParent;
	UPROPERTY()
	ATrafficParent* _IntersectionParent;
	UPROPERTY()
	ATrafficParent* _DeadSpawnZoneParent;
	UPROPERTY()
	ATrafficParent* _CarParent;

private:
	//Data
	UPROPERTY(EditAnywhere, Category = "VisitData/TrafficAttribut")
	FTrafficData _TrafficData;

	//Road
	UPROPERTY()
	TArray<ARoad*> _Roads;
	TMap<FVector, TArray<ARoad*>> _RoadsAtPoint;

	//Car
	UPROPERTY()
	TArray<ACar*> _Cars;

	//DeadSpawnZone
	TArray<ADeadSpawnZone*> _DeadSpawnZones;

	void InitDeadSpawnZone();
	void InitRoad();
	void InitCar();

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "VisitData/Road")
	void AddRoad();
	UFUNCTION(CallInEditor, Category = "VisitData/Intersection")
	void AddIntersection();
	UFUNCTION(CallInEditor, Category = "VisitData/DeadSpawnZone")
	void AddDeadSpawnZone();
#endif

public:
	TArray<ARoad*> GetRoadsAtThisLocation(FVector location);

	FTrafficData GetTrafficData();
};
