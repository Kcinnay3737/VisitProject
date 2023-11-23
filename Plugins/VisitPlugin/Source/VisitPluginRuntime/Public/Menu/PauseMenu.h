#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PauseMenu.generated.h"

class UGameInstanceSubsystemVisit;
class UUserWidget;

UCLASS()
class VISITPLUGINRUNTIME_API APauseMenu : public AActor
{
	GENERATED_BODY()
public:
	APauseMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetSolarTime(float value);
	UFUNCTION(BlueprintImplementableEvent)
	void GetSolarTime();
	UFUNCTION(BlueprintCallable)
	void ReceiveSolarTime(float value);

private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _HideMenuWidgetClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _PauseMenuWidgetClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _OptionMenuWidgetClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _BackgroudPauseWidgetClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _WorldSettingWidgetClass;

	UPROPERTY(EditAnywhere, Category = "VisitData/PauseMenuAttribute")
	FName _QuitLevelName;

	UPROPERTY()
	UUserWidget* _HideMenuWidget;
	UPROPERTY()
	UUserWidget* _PauseMenuWidget;
	UPROPERTY()
	UUserWidget* _OptionMenuWidget;
	UPROPERTY()
	UUserWidget* _BackgroundWidget;
	UPROPERTY()
	UUserWidget* _WorldSettingWidget;

	void InitOption();

	UFUNCTION()
	void OpenHideMenu();
	UFUNCTION()
	void CloseHideMenu();
	UFUNCTION()
	void OpenMenu();
	UFUNCTION()
	void CloseMenu();
	UFUNCTION()
	void OpenOption();
	UFUNCTION()
	void CloseOption();
	UFUNCTION()
	void OpenWorldSetting();
	UFUNCTION()
	void CloseWorldSetting();

	void OpenBackgound();
	void CloseBackground();

	UFUNCTION()
	void Quit();
	
	UGameInstanceSubsystemVisit* GetGameInstanceSubsystemVisit();

	UFUNCTION()
	void OnComboBoxResolutionChanged(FString selectedItem, ESelectInfo::Type selectionType);
	UFUNCTION()
	void OnSliderScreenPercentChanged(float value);
	UFUNCTION()
	void OnSliderSecondaryScreenPercentChanged(float value);
	UFUNCTION()
	void OnComboBoxWindowModeChanged(FString selectedItem, ESelectInfo::Type selectionType);
	UFUNCTION()
	void OnComboBoxFPSChanged(FString selectedItem, ESelectInfo::Type selectionType);
	UFUNCTION()
	void OnCheckBoxTSRChanged(bool bIsChecked);
	UFUNCTION()
	void OnCheckBoxVSYNCChanged(bool bIsChecked);

	UFUNCTION()
	void OnSliderSolarTimeChanged(float value);

	void SetPauseState(bool value);
};