#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "VisitorController.generated.h"

class AMinimaps;
class AVisitor;
class UDataVisitorControl;
class UInputAction;
class UInputMappingContext;
class AGameStateManager;
class UWidget;
class UPanelWidget;

UENUM(BlueprintType)
enum class EIgnoreState : uint8
{
	NotIngoreChild UMETA(DisplayName = "NotIgnoreChild"),
	IngoreChild UMETA(DisplayName = "IgnoreChild")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FTouchData
{
	GENERATED_BODY()
public:
	FVector2D oldLocalPos = FVector2D::ZeroVector;
	FVector2D localPos = FVector2D::ZeroVector;
	FVector2D oldPlatformPos = FVector2D::ZeroVector;
	FVector2D platformPos = FVector2D::ZeroVector;
	float pressedTime = 0.0f;
	float pressedDistance = 0.0f;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FAllTouchData
{
	GENERATED_BODY()
public:
	TMap<int, FTouchData> touchData;

	TArray<int> touchOnViewport;
	TArray<int> touchOnMap;
	TArray<int> touchOnJoystick;
};

UCLASS()
class VISITPLUGINRUNTIME_API AVisitorController : public APlayerController
{
	GENERATED_BODY()

public:
	AVisitorController();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	AVisitor* GetVisitor();

	UFUNCTION(BlueprintCallable)
	void AddInIgnoreWidget(UWidget* Widget, EIgnoreState IgnoreState);
	UFUNCTION(BlueprintCallable)
	void RemoveInIgnoreWidget(UWidget* Widget);

protected:
	virtual void BeginPlay() override;

	//Input -------------------------------------
	UPROPERTY(EditAnywhere, Category = "VisitData/InputMappingContext")
	UInputMappingContext* _InputMappingContext;

	//Mouse and Keyboard action
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyBoardMovingAction;
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyboardZoomingAction;
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyboardTapingAction;
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyboardPressedAction;
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyboardHoldingAction;
	UPROPERTY(EditAnywhere, Category = "VisitData/InputActionMouseAndKeyboard")
	UInputAction* _MouseAndKeyboardReleasedAction;

	//widget
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _JoystickWidgetClass;
	UPROPERTY()
	UUserWidget* _JoystickWidget;

	UPROPERTY(EditAnywhere, Category = "VisitData/HiddenActor")
	TArray<TSubclassOf<AActor>> _HiddenActorsClass;

	bool _bPause;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData/DataControl")
	UDataVisitorControl* _DataVisitorControl;

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;
	void InitGameStateManager();

	bool _bIsInGame = false;
	UFUNCTION()
	void OnEnterGame();
	UFUNCTION()
	void OnExitGame();

	bool _bCanInteract = false;
	UFUNCTION()
	void OnPlayerCanInteract();
	UFUNCTION()
	void OnPlayerCannotInteract();

	//Mouse and keyboard data
	FVector2D _OldMousePosition = FVector2D::ZeroVector;

	//Touch data
	FAllTouchData _AllTouchData;
	void CheckPressedTouchData();
	void CheckHoldingTouchData(float deltaTime);
	void CheckReleasedTouchData();
	void CheckLooking(float deltaTime);
	void CheckZooming();
	void CheckMoving();

	UFUNCTION()
	void InitJoystickWidget();
	UFUNCTION()
	void HideJoystickWidget();
	void MovingJoystick(FVector2D platformPos);
	void ResetJoystickLocation();

	void InitializeMappingContext();
	
	//Mouse and Keyboard action
	UFUNCTION()
	void OnMouseAndKeyBoardMovingAction(const FInputActionValue& value);
	UFUNCTION()
	void OnMouseAndKeyboardZoomingAction(const FInputActionValue& value);
	UFUNCTION()
	void OnMouseAndKeyboardTapingAction(const FInputActionValue& value);
	UFUNCTION()
	void OnMouseAndKeyboardPressedAction(const FInputActionValue& value);
	UFUNCTION()
	void OnMouseAndKeyboardHoldingAction(const FInputActionValue& value);
	UFUNCTION()
	void OnMouseAndKeyboardReleasedAction(const FInputActionValue& value);

	void Moving(FVector2D value);
	void Zooming(float value, bool bZoomMap);
	void Taping(FVector2D localPos, FVector2D platformPos);
	void Looking(FVector2D value, bool bLookingMap);

	UFUNCTION()
	void HideActor();

	bool CheckVisibility(UWidget* widget);

	UPROPERTY()
	TMap<UWidget*, EIgnoreState> _IgnoreWidget;
	UPROPERTY(EditAnywhere, Category = "VisitData/IgnoreWidget")
	TMap<TSubclassOf<UWidget>, EIgnoreState> _IgnoreWidgetClass;
	UPROPERTY()
	TMap<UWidget*, UWidget*> _TempIgnoreWidget;

public:
	//Get and Set function
	FVector2D GetGameViewportSize();
	FVector2D GetGameResolution();

	FVector2D GetMousePos();
	FVector2D GetMousePlatformPos();

	bool CheckPointIsOnWidget(FVector2D point);
	bool CheckWidgetForPoint(UWidget* widget, FVector2D point);
	bool CheckPointIsOnTheMap(FVector2D point);
	bool CheckPointIsOnJoystick(FVector2D point);
	bool CheckMapIsOpen();

	void SetPauseState(bool value);

	AMinimaps* GetMap();

	TArray<UWidget*> GetAllChildWidgetsRecursive(UPanelWidget* PanelWidget);
};
