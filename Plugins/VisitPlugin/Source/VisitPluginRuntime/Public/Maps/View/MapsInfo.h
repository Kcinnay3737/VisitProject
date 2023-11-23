#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapsInfo.generated.h"

class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapsInfo();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;

private:	
	

};
