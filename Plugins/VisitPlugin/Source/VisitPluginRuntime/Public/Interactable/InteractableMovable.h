#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "InteractableMovable.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API AInteractableMovable : public AInteractable
{
	GENERATED_BODY()
public:
	AInteractableMovable();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* _SceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* _StaticMeshComponent;

	//Transform on interact
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform")
	bool _bApplyTransform = true;
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform", meta = (EditCondition = "_bApplyTransform"))
	FVector _InteractMovement;
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform", meta = (EditCondition = "_bApplyTransform"))
	FRotator _InteractRotation;
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform", meta = (EditCondition = "_bApplyTransform"))
	float _TimeToApplyTransform = 0.0f;
	float _CurrTime;
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform", meta = (EditCondition = "_bApplyTransform"))
	bool _bChangeInteractByDistanceToPlayer = true;
	UPROPERTY(EditAnywhere, Category = "VisitData/Transform", meta = (EditCondition = "_bApplyTransform && _bChangeInteractByDistanceToPlayer"))
	float _InteractDistance = 200.0f;

	bool _bIsInteract = false;
	bool _bMoving = false;

	bool _bPlayerInRange = false;

	void StartMovement();
	void ApplyMovement(float deltaTime);

	FVector _StartLocation;
	FVector _LocationToApply;
	FRotator _StartRotation;
	FRotator _RotationToApply;
};