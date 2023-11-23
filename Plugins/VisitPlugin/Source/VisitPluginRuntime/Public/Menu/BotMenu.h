#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BotMenu.generated.h"

class UUserWidget;
class AGameStateManager;
class UButton;

UENUM(BlueprintType)
enum class EBotMenuState : uint8
{
	Home UMETA(DisplayName = "Home"),
	Rooms UMETA(DisplayName = "Rooms"),
	Plan UMETA(DisplayName = "Plan"),
	Partners UMETA(DisplayName = "Partners"),
	Interactable UMETA(DisplayName = "Interactable")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBotMenuStateChange, EBotMenuState, BotMenuState);

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FParamOnClickedButton
{
	GENERATED_BODY()
public:
	UPROPERTY()
	EBotMenuState BotMenuState = EBotMenuState::Home;
};

UCLASS()
class VISITPLUGINRUNTIME_API ABotMenu : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FOnBotMenuStateChange OnBotMenuStateChange;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _BotWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _BotWidget = nullptr;

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;

	UPROPERTY()
	UButton* _HomeButton = nullptr;
	UPROPERTY()
	UButton* _RoomsButton = nullptr;
	UPROPERTY()
	UButton* _PlanButton = nullptr;
	UPROPERTY()
	UButton* _PartnersButton = nullptr;

	UPROPERTY()
	EBotMenuState _BotMenuState = EBotMenuState::Home;

public:
	ABotMenu();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ShowBotWidget();
	UFUNCTION()
	void HideBotWidget();

protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnHomeButtonClick();
	UFUNCTION()
	void OnRoomsButtonClick();
	UFUNCTION()
	void OnPlanButtonClick();
	UFUNCTION()
	void OnPartnersButtonClick();

public:
	EBotMenuState GetBotMenuState();
	void SetBotMenuState(EBotMenuState BotMenuState);
};
