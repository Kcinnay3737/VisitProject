#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapsInteractable.generated.h"

class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapsInteractable();
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;

private:	
	

};
