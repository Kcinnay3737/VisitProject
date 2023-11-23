#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AInteractable : public AActor
{
	GENERATED_BODY()

public:
	AInteractable();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact();

protected:
	virtual void BeginPlay() override;

	//Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;

private:
	
};