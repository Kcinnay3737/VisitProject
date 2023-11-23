#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadSpawnZone.generated.h"

class UBoxComponent;
class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API ADeadSpawnZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeadSpawnZone();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* _BoxComponent;

private:	
	
public:
	bool CheckPointIsOverlapping(FVector point);
};
