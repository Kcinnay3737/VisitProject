#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoxCollisionSetter.generated.h"

class UBoxComponent;
class USceneComponent;

UCLASS()
class VISITPLUGINEDITOR_API ABoxCollisionSetter : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoxCollisionSetter();
	virtual void Tick(float DeltaTime) override;

	UBoxComponent* GetCollision();
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* _BoxComponent;
};
