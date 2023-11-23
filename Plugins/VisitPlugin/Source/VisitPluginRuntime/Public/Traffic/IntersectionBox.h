#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntersectionBox.generated.h"

class ACar;
class AIntersection;
class UBoxComponent;
class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AIntersectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	AIntersectionBox();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* _BoxComponent;

	UPROPERTY()
	AIntersection* _Intersection;

	virtual void OnCarBeginOverlappeStop(ACar* otherCar);
	virtual void OnCarBeginOverlappeTrafficLight(ACar* otherCar);

private:	
	UFUNCTION()
	void OnCarBeginOverlappe(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

public:
	void SetIntersection(AIntersection* intersection);
	TArray<ACar*> GetOverlappingCars();

	void SetCCD(bool state);
};
