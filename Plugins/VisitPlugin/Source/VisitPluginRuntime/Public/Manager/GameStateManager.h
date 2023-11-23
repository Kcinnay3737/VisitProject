#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStateManager.generated.h"

class UUserWidget;
class UGameInstanceSubsystemVisit;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterStartMenuState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitStartMenuState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterGameState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitGameState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCanInteract);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCannotInteract);

UENUM(BlueprintType)
enum class EGameState : uint8
{
	StartMenu UMETA(DisplayName = "StartMenu"),
	Game UMETA(DisplayName = "Game")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FHideAllParams
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	bool bCheckAnimationEnd = false;
	UPROPERTY(BlueprintReadWrite)
	bool bHideInstante = false;
};

UCLASS()
class VISITPLUGINRUNTIME_API AGameStateManager : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FOnEnterStartMenuState OnEnterStartMenuStat;
	UPROPERTY()
	FOnExitStartMenuState OnExitStartMenuState;
	UPROPERTY()
	FOnEnterGameState OnEnterGameState;
	UPROPERTY()
	FOnExitGameState OnExitGameState;
	UPROPERTY()
	FOnPlayerCanInteract OnPlayerCanInteract;
	UPROPERTY()
	FOnPlayerCannotInteract OnPlayerCannotInteract;


private:
	//Menu camera
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "VisitData/Camera")
	FVector _MenuCameraPosition = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "VisitData/Camera")
	FRotator _MenuCameraRotation = FRotator::ZeroRotator;

	//Spawn
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "VisitData/Spawn")
	FVector _SpawnPosition = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "VisitData/Spawn")
	FRotator _SpawnRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, Category = "VisitData/Spawn")
	float _TransitionTime = 2.0f;
	UPROPERTY(EditAnywhere, Category = "VisitData/Spawn")
	float _TransitionFrameRate = 60.0f;
	float _CurrTransitionTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "VisitData/Pawn")
	TSubclassOf<APawn> _MenuPawnClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Pawn")
	TSubclassOf<APawn> _GamePawnClass = nullptr;

	EGameState _GameState = EGameState::StartMenu;

	FTimerHandle _TimerHandle;

	bool _FirstTick = true;

public:	
	AGameStateManager();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void StopGame();

private:	
	void SetCameraPositionAndRotation();
	UFUNCTION()
	void CameraTransitionCoroutine(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World, FVector StartPosition, FVector EndPosition, FRotator StartRotation, FRotator EndRotation, bool bSpawnInVisit);

	void SpawnInStartMenu(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World);
	void SpawnInVisit(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World);

	//Start
	void InitStartWidget();
	void CloseStartWidget();
	UFUNCTION()
	void OnClickedStart();
	UFUNCTION()
	void OnClickedOption();
	UFUNCTION()
	void OnClickedQuit();

	//Main
	void InitMainWidget();
	void CloseMainWidget();

	//Pause
	void InitPauseWidget();
	void ClosePauseWidget();
	void OpenHidePauseMenu();
	void CloseHidePauseMenu();
	void OpenPauseMenu();
	void ClosePauseMenu();
	UFUNCTION()
	void OnClickedPause();
	UFUNCTION()
	void OnClickedStartPause();
	UFUNCTION()
	void OnClickedOptionPause();
	UFUNCTION()
	void OnClickedQuitPause();

public:
	//Getter, Setter
	EGameState GetGameState();
};
