#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimeSetter.generated.h"

class USceneComponent;
class USlider;
class UTextBlock;
class UUserWidget;

UCLASS()
class VISITPLUGINRUNTIME_API ATimeSetter : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeSetter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetTime(float CurrentTime);
	UFUNCTION(BlueprintImplementableEvent)
	void SetInitialTime();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "VisitData/TimeSetter")
	float _MinimumTime = 6.0f;
	UPROPERTY(EditAnywhere, Category = "VisitData/TimeSetter")
	float _MaximumTime = 22.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(EditAnywhere, Category = "VisitData/TimeSetter")
	TSubclassOf<UUserWidget> _TimeWidgetClass;
	UPROPERTY()
	UUserWidget* _TimeWidget = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USlider* _SliderTime = nullptr;
	UPROPERTY()
	UTextBlock* _TextBlockTime = nullptr;

	void InitWidget();

	UFUNCTION()
	void OnSliderTimeValueChange(float Value);

	void SetTimeWithSliderValue();
};
