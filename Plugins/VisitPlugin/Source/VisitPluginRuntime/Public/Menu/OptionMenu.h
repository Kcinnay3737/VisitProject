#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Menu/BotMenu.h"
#include "OptionMenu.generated.h"

class UUserWidget;
class UGameInstanceSubsystemVisit;
class UButton;
class AGameStateManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenOption);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCloseOption);

UCLASS()
class VISITPLUGINRUNTIME_API AOptionMenu : public AActor
{
	GENERATED_BODY()

public:
	FOnCloseOption OnCloseOption;
	FOnOpenOption OnOpenOption;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _OptionWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _OptionWidget = nullptr;

	UPROPERTY()
	UButton* _ButtonOption = nullptr;

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;

	bool _bIsOpen = true;

	bool _bIsInGame = false;

public:	
	AOptionMenu();
	virtual void Tick(float DeltaTime) override;

	void CloseOption();
	void OpenOption();

protected:
	virtual void BeginPlay() override;

private:	
	void InitOption();
	void InitOptionWidget();

	UFUNCTION()
	void OnBotMenuStateChange(EBotMenuState BotMenuState);

	UFUNCTION()
	void OnClickOption();

	UFUNCTION()
	void DisableOptionButton();
	UFUNCTION()
	void EnableOptionButton();

	UFUNCTION()
	void OnComboBoxResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSliderScreenPercentChanged(float Value);
	UFUNCTION()
	void OnSliderSecondaryScreenPercentChanged(float Value);
	UFUNCTION()
	void OnComboBoxWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnComboBoxFPSChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnCheckBoxDLSSChanged(bool bIsChecked);
	UFUNCTION()
	void OnCheckBoxTSRChanged(bool bIsChecked);
	UFUNCTION()
	void OnCheckBoxVSYNCChanged(bool bIsChecked);
	
public:
	//Getter, Setter
	UGameInstanceSubsystemVisit* GetGameInstanceSubsystemVisit();

	bool GetIsOpen();
};
