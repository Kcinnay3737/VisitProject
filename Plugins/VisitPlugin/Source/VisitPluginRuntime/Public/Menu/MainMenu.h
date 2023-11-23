#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainMenu.generated.h"

class UGameInstanceSubsystemVisit;
class UTexture2D;
class UUserWidget;

UCLASS()
class VISITPLUGINRUNTIME_API AMainMenu : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainMenu();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Category = "VisitData/Cursor")
	bool _bShowMouseCursor;

	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _BackgroundClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _MenuWidgetClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _OptionWidgetClass;

	UPROPERTY(EditAnywhere, Category = "VisitData/BackgroundImage")
	UTexture2D* _ImageBackground = nullptr;

	UPROPERTY()
	UUserWidget* _BackgroundWidget;
	UPROPERTY()
	UUserWidget* _MenuWidget;
	UPROPERTY()
	UUserWidget* _OptionWidget;

	UPROPERTY(EditAnywhere, Category = "VisitData/MenuAttribute")
	FName _StartLevelName;

	UGameInstanceSubsystemVisit* GetGameInstanceSubsystemVisit();

	void InitOption();

	void SetBackground();

	UFUNCTION()
	void OpenMenu();
	UFUNCTION()
	void CloseMenu();
	UFUNCTION()
	void StartLevel();
	UFUNCTION()
	void QuitApplication();
	
	UFUNCTION()
	void OpenOption();
	UFUNCTION()
	void CloseOption();
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
};
