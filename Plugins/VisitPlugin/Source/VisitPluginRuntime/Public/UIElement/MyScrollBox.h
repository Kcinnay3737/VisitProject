#pragma once

#include "CoreMinimal.h"
#include "Components/ScrollBox.h"
#include "MyScrollBox.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API UMyScrollBox : public UScrollBox
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FVector2D _LastMousePosition = FVector2D::ZeroVector;
	UPROPERTY()
	bool _bLastMousePositionIsSet = false;

	UPROPERTY(EditAnywhere, Category = "ScrollSpeed")
	float _ScrollSpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Spacing")
	float _SpaceBetweenElement = 0.0f;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateScrollBox(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void UpdateSpacing();

protected:
	virtual void OnSlotAdded(UPanelSlot* AddedSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* RemovedSlot) override;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
