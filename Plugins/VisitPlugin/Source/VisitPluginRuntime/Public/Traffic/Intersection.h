#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Intersection.generated.h"

class ACar;
class AIntersectionBox;
class AIntersectionBoxEnd;
class AIntersectionBoxStart;
class USceneComponent;

UENUM(BlueprintType)
enum class EIntersectionType : uint8
{
	stop = 0 UMETA(DisplayName = "Stop"),
	trafficLight = 1 UMETA(DisplayName = "TrafficLight")
};

UENUM(BlueprintType)
enum class EStopCirculationType : uint8
{
	oneByOne = 0 UMETA(DisplayName = "One by one"),
	sideBySide = 1 UMETA(DisplayName = "Side by side")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataIntersection
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	EIntersectionType intersectionType = EIntersectionType::stop;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "intersectionType == EIntersectionType::stop"))
	float stopTime = 1.0f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "intersectionType == EIntersectionType::stop"))
	EStopCirculationType stopCirculationType = EStopCirculationType::oneByOne;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "intersectionType == EIntersectionType::trafficLight"))
	float greenLightTime = 5.0f;
	UPROPERTY(EditAnywhere)
	bool bUseContinuousCollisionDetection = true;
	UPROPERTY(EditAnywhere)
	float intersectionTickInterval = 0.0f;
};

UCLASS()
class VISITPLUGINRUNTIME_API AIntersection : public AActor
{
	GENERATED_BODY()

public:
	AIntersection();
	virtual void Tick(float DeltaTime) override;

	void AddInQueueCar(ACar* car);
	void PopInQueueCar(ACar* car);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData/IntersectionAttribute")
	FDataIntersection _DataIntersection;

	UPROPERTY()
	TArray<AIntersectionBoxStart*> _IntersectionsBoxStart;
	UPROPERTY()
	TArray<AIntersectionBoxEnd*> _IntersectionsBoxEnd;

	int _CurrTrafficLightIndex;
	float _CurrTrafficLightTime;

	void InitIntersectionsBox();
	void InitTrafficLight();

	//Stop
	UPROPERTY()
	TArray<ACar*> _StopCarQueue;

	UPROPERTY()
	TMap<ACar*, AIntersectionBox*> _CarIntersection;
	TMap<AIntersectionBox*, TArray<ACar*>> _IntersectionCar;

	UPROPERTY()
	TMap<ACar*, ACar*> _EngagedCarInIntersection;

	bool _bIntersectionUsed = false;

	void StopTick(float deltaTime);
	void TrafficLightTick(float deltaTime);

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "VisitData/IntersectionBox")
	void AddStartIntersectionBox();
	UFUNCTION(CallInEditor, Category = "VisitData/IntersectionBox")
	void AddEndIntersectionBox();
#endif

public:
	FDataIntersection GetDataIntersection();

	bool CheckCarIsFirstOnQueue(ACar* car);

	void AddInCarIntersection(ACar* car, AIntersectionBox* intersectionBox);
	void RemoveInCarIntersection(ACar* car);
	AIntersectionBox* GetInCarIntersection(ACar* car);

	void AddInIntersectionCar(AIntersectionBox* intersection, ACar* car);
	void RemoveInIntersectionCar(AIntersectionBox* intersection);
	void RemoveAtIntersectionCar(AIntersectionBox* intersection, ACar* car);
	TArray<ACar*> GetInInteractionCar(AIntersectionBox* intersection);

	void AddEngagedCar(ACar* car);
	void RemoveEngagedCar(ACar* car);
	bool CheckCarIsEngagedInIntersection(ACar* car);
	bool CheckIntersectionIsEngaged();
};
