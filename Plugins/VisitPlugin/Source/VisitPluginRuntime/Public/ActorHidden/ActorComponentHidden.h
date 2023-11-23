#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorComponentHidden.generated.h"

class USceneComponent;

UCLASS(BlueprintType)
class VISITPLUGINRUNTIME_API AActorComponentHidden : public AActor
{
	GENERATED_BODY()
	
public:	
	AActorComponentHidden();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Hide")
	void HideAll();
	UFUNCTION(BlueprintCallable, Category = "Hide")
	void UnhideAll();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData")
	TArray<AActor*> _Actors;
	UPROPERTY(EditAnywhere, Category = "VisitData")
	TArray<FName> _HiddenComponentsTag;

};
