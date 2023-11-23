#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable/ButtonIndex.h"
#include "Menu/BotMenu.h"
#include "InteractableMenu.generated.h"

class AInteractableMM;
class UUserWidget;
class UScrollBox;
class UTextBlock;
class UTexture;
class ABotMenu;

UENUM()
enum class ESelectionState
{
	Mesh UMETA(DisplayName = "Mesh"),
	Material UMETA(DisplayName = "Material")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FParamShowViewPart
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FText Name = FText::GetEmpty();
	UPROPERTY()
	FText Description = FText::GetEmpty();
	UPROPERTY()
	UTexture* Image = nullptr;

};

UCLASS()
class VISITPLUGINRUNTIME_API AInteractableMenu : public AActor, public IButtonIndexReceiver
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	AInteractableMM* _CurrInteractableMM = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _ObjectSelectionWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _ObjectSelectionWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _ObjectSelectionSlotWidgetClass = nullptr;

	virtual void OnButtonIndexClick(FString Type, int Index) override;

	UPROPERTY()
	TArray<UButtonIndex*> _ButtonIndex;

	UPROPERTY()
	UTextBlock* _TextBlockViewName = nullptr;
	UPROPERTY()
	UTextBlock* _TextBlockViewDescription = nullptr;
	UPROPERTY()
	UImage* _ImageView;
	UPROPERTY()
	UScrollBox* _ScrollBoxSelection = nullptr;

	UPROPERTY()
	ABotMenu* _BotMenu = nullptr;

	int _IndexMeshSelected = 0;
	int _IndexMaterialSelected = 0;

	ESelectionState _CurrentSelectionState = ESelectionState::Mesh;

	bool _bViewPartIsOpen = false;

public:	
	AInteractableMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	void InitWidget();

	void ApplyCurrentSettings();
	void ViewCurrentSettings();

	UFUNCTION()
	void OnButtonMeshClicked();
	UFUNCTION()
	void OnButtonMaterialClicked();

	UFUNCTION()
	void OnButtonCloseViewClicked();
	UFUNCTION()
	void OnButtonApplyViewClicked();

	void OpenMenu();
	void CloseMenu();

	UFUNCTION()
	void OnBotMenuStateChanged(EBotMenuState BotMenuState);

	void CallHideViewPart();
	void CallShowViewPart(FParamShowViewPart& Params);

public:
	void SetInteractableMM(AInteractableMM* InteractableMM);
};
