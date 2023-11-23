#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "InteractableStateChanger.generated.h"

class UUSerWidget;

UCLASS()
class VISITPLUGINRUNTIME_API AInteractableStateChanger : public AInteractable
{
	GENERATED_BODY()
public:
	AInteractableStateChanger();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* _InteractableComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USceneComponent* _StateChangerSceneComponent;

	//Change state on interact
	UPROPERTY(EditAnywhere, Category = "VisitData/StateChanger")
	bool _bActiveStateChanger = true;

	UPROPERTY(/*EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger")*/)
	bool _bWithIntensity = false;
	UPROPERTY(/*EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger && _bWithIntensity")*/)
	float _BaseIntensity = 1.0f;
	UPROPERTY(/*EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger && _bWithIntensity")*/)
	float _MinIntensity = 0.0f;
	UPROPERTY(/*EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger && _bWithIntensity")*/)
	float _MaxIntensity = 1.0f;
	float _CurrIntensity;
	UPROPERTY(/*EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger && _bWithIntensity")*/)
	TSubclassOf<UUserWidget> _IntensityWidgetClass;
	UPROPERTY()
	UUserWidget* _IntensityWidget;
	void OpenIntensityWidget();
	UFUNCTION()
	void CloseIntensityWidget();

	UPROPERTY(EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger && !_bWithIntensity"))
	bool _bEnableAtStartup = true;

	UPROPERTY(EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger"))
	TArray<AActor*> _ChangeStateActor;
	UPROPERTY(EditAnywhere, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger"))
	TArray<FString> _ChangeStateStartLabel;
	UFUNCTION(CallInEditor, Category = "VisitData/StateChanger", meta = (EditCondition = "_bActiveStateChanger"))
	void AddStartLabelInChangeStateActor();

	bool _bCurrState = false;

	void SetEnableComponent(bool state);

	UFUNCTION()
	void OnSliderIntensityChanged(float value);
	void SetIntensity(float value);

	bool _bWidgetIntensityIsOpen = false;
};