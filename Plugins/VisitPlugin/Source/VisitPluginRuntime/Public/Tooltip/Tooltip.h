#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tooltip.generated.h"

class USceneComponent;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataAlignmentTooltip
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool alignsPitch = false;
	UPROPERTY(EditAnywhere)
	bool alignsYaw = true;
	UPROPERTY(EditAnywhere)
	bool alignsRoll = false;
};

UCLASS()
class VISITPLUGINRUNTIME_API ATooltip : public AActor
{
	GENERATED_BODY()
	
public:	
	ATooltip();
	virtual void Tick(float DeltaTime) override;

	void Interact();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneComponentTooltipClose;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneComponentTooltipOpen;

	UPROPERTY(EditAnywhere, Category = "VisitData/Alignment")
	FDataAlignmentTooltip _AlignmentTooltipClose;
	UPROPERTY(EditAnywhere, Category = "VisitData/Alignment")
	FDataAlignmentTooltip _AlignmentTooltipOpen;

	void ShowTooltipClose();
	void HideTooltipClose();
	void ShowTooltipOpen();
	void HideTooltipOpen();

	void RotateTooltip();

	bool _IsOpen = false;

private:

};
