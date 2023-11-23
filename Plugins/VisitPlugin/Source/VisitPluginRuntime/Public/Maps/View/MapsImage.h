#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapsImage.generated.h"

class USceneComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsImage : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapsImage();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;

private:	


};
