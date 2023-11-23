#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrafficParent.generated.h"

class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API ATrafficParent : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrafficParent();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;

private:	
	

};
