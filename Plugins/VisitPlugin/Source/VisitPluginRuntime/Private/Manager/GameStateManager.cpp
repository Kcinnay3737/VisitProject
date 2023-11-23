#include "Manager/GameStateManager.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Character/VisitorController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"

AGameStateManager::AGameStateManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameStateManager::BeginPlay()
{
	Super::BeginPlay();
	
	//InitOption();

	SetCameraPositionAndRotation();

	//InitStartWidget();
}

void AGameStateManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_FirstTick)
	{
		_FirstTick = false;

		SpawnInStartMenu(UGameplayStatics::GetPlayerController(GetWorld(), 0), UGameplayStatics::GetPlayerPawn(GetWorld(), 0), GetWorld());
	}
}

void AGameStateManager::StartGame()
{
	if (_GameState == EGameState::Game) return;

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* CurrPlayerController = World->GetFirstPlayerController();
	if (!CurrPlayerController) return;
	APawn* CurrPawn = CurrPlayerController->GetPawn();
	if (!CurrPawn) return;

	_GameState = EGameState::Game;
	OnExitStartMenuState.Broadcast();

	_CurrTransitionTime = 0.0f;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CameraTransitionCoroutine"), CurrPlayerController, CurrPawn, World, _MenuCameraPosition, _SpawnPosition, _MenuCameraRotation, _SpawnRotation, true);
	World->GetTimerManager().SetTimer(_TimerHandle, TimerDelegate, 1 / _TransitionFrameRate, true);
}

void AGameStateManager::StopGame()
{
	if (_GameState == EGameState::StartMenu) return;

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* CurrPlayerController = World->GetFirstPlayerController();
	if (!CurrPlayerController) return;
	APawn* CurrPawn = CurrPlayerController->GetPawn();
	if (!CurrPawn) return;

	APawn* NewPawn = World->SpawnActor<APawn>(_MenuPawnClass, CurrPawn->GetActorLocation(), CurrPawn->GetActorRotation());
	if (!NewPawn) return;

	CurrPlayerController->UnPossess();
	CurrPlayerController->Possess(NewPawn);

	CurrPawn->Destroy();

	_GameState = EGameState::StartMenu;
	OnPlayerCannotInteract.Broadcast();
	OnExitGameState.Broadcast();

	_CurrTransitionTime = 0.0f;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CameraTransitionCoroutine"), CurrPlayerController, NewPawn, World, NewPawn->GetActorLocation(), _MenuCameraPosition, NewPawn->GetActorRotation(), _MenuCameraRotation, false);
	World->GetTimerManager().SetTimer(_TimerHandle, TimerDelegate, 1 / _TransitionFrameRate, true);
}

void AGameStateManager::SetCameraPositionAndRotation()
{
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;
	APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn) return;

	FVector Location = GetActorLocation() + _MenuCameraPosition;

	Pawn->SetActorLocation(Location);
	PlayerController->SetControlRotation(_MenuCameraRotation);
}

void AGameStateManager::CameraTransitionCoroutine(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World, FVector StartPosition, FVector EndPosition, FRotator StartRotation, FRotator EndRotation, bool bSpawnInVisit)
{
	if (!CurrPlayerController || !CurrPawn || !World) return;

	_CurrTransitionTime += 1 / _TransitionFrameRate;

	float Alpha = _CurrTransitionTime / _TransitionTime;
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FVector NewLocation = UKismetMathLibrary::VLerp(StartPosition, EndPosition, Alpha);
	FRotator NewRotation = UKismetMathLibrary::RLerp(StartRotation, EndRotation, Alpha, true);

	if (Alpha < 1.0f)
	{
		CurrPawn->SetActorLocation(NewLocation);
		CurrPlayerController->SetControlRotation(NewRotation);
	}
	else
	{
		CurrPlayerController->SetControlRotation(NewRotation);

		if (bSpawnInVisit)
		{
			SpawnInVisit(CurrPlayerController, CurrPawn, World);
		}
		else
		{
			SpawnInStartMenu(CurrPlayerController, CurrPawn, World);
		}
		
		World->GetTimerManager().ClearTimer(_TimerHandle);
	}
}

void AGameStateManager::SpawnInStartMenu(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World)
{
	OnEnterStartMenuStat.Broadcast();
	//InitStartWidget();
}

void AGameStateManager::SpawnInVisit(APlayerController* CurrPlayerController, APawn* CurrPawn, UWorld* World)
{
	if (!CurrPlayerController || !CurrPawn || !World) return;

	APawn* NewPawn = World->SpawnActor<APawn>(_GamePawnClass, _SpawnPosition, _SpawnRotation);
	if (!NewPawn) return;

	CurrPlayerController->UnPossess();
	CurrPlayerController->Possess(NewPawn);

	CurrPawn->Destroy();

	OnEnterGameState.Broadcast();

	//InitMainWidget();

	//OpenHidePauseMenu();
}

//Start widget -----------------------------------------------

void AGameStateManager::InitStartWidget()
{
	//_MenuState = EMenuState::StartMenu;

	//if (!_StartMenuWidgetClass || !_StartMenuBackgroundWidgetClass) return;

	//UWorld* World = GetWorld();
	//if (!World) return;
	//APlayerController* PlayerController = World->GetFirstPlayerController();
	//if (!PlayerController) return;

	//if (_StartMenuWidget && _StartMenuBackgroundWidget)
	//{
	//	_StartMenuBackgroundWidget->AddToViewport();
	//	_StartMenuWidget->AddToViewport();
	//	return;
	//}

	//_StartMenuBackgroundWidget = CreateWidget<UUserWidget>(PlayerController, _StartMenuBackgroundWidgetClass);
	//if (!_StartMenuBackgroundWidget) return;
	//_StartMenuBackgroundWidget->AddToViewport();

	//_StartMenuWidget = CreateWidget<UUserWidget>(PlayerController, _StartMenuWidgetClass);
	//if (!_StartMenuWidget) return;
	//_StartMenuWidget->AddToViewport();

	//UButton* ButtonStart = Cast<UButton>(_StartMenuWidget->GetWidgetFromName("Button_Start"));
	//if (ButtonStart)
	//{
	//	ButtonStart->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedStart);
	//}

	//UButton* ButtonOption = Cast<UButton>(_StartMenuWidget->GetWidgetFromName("Button_Option"));
	//if (ButtonOption)
	//{
	//	ButtonOption->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedOption);
	//}

	//UButton* ButtonQuit = Cast<UButton>(_StartMenuWidget->GetWidgetFromName("Button_Quit"));
	//if (ButtonQuit)
	//{
	//	ButtonQuit->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedQuit);
	//}
}

void AGameStateManager::CloseStartWidget()
{
	//if (_StartMenuWidget && _StartMenuWidget->IsInViewport())
	//{
	//	_StartMenuWidget->RemoveFromParent();
	//}
	//if (_StartMenuBackgroundWidget && _StartMenuBackgroundWidget->IsInViewport())
	//{
	//	_StartMenuBackgroundWidget->RemoveFromParent();
	//}
}

void AGameStateManager::OnClickedStart()
{
	/*UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* CurrPlayerController = World->GetFirstPlayerController();
	if (!CurrPlayerController) return;
	APawn* CurrPawn = CurrPlayerController->GetPawn();
	if (!CurrPawn) return;

	CloseStartWidget();

	_CurrTransitionTime = 0.0f;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CameraTransitionCoroutine"), CurrPlayerController, CurrPawn, World, _MenuCameraPosition, _SpawnPosition, _MenuCameraRotation, _SpawnRotation, true);
	World->GetTimerManager().SetTimer(_TimerHandle, TimerDelegate, 1 / _TransitionFrameRate, true);*/
}

void AGameStateManager::OnClickedOption()
{
	//OpenOptionMenu();
}

void AGameStateManager::OnClickedQuit()
{
	/*UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	UKismetSystemLibrary::QuitGame(world, playerController, EQuitPreference::Quit, true);*/
}

//------------------------------------------------------------

//Main widget ------------------------------------------------

void AGameStateManager::InitMainWidget()
{
	/*_MenuState = EMenuState::GameMenu;

	if (!_MainWidgetClass) return;

	if (_MainWidget)
	{
		_MainWidget->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_MainWidget = CreateWidget<UUserWidget>(PlayerController, _MainWidgetClass);
	if (!_MainWidget) return;
	_MainWidget->AddToViewport();*/
}

void AGameStateManager::CloseMainWidget()
{
	/*if (_MainWidget && _MainWidget->IsInViewport())
	{
		_MainWidget->SetVisibility(ESlateVisibility::Hidden);
	}*/
}

//------------------------------------------------------------

//Pause widget -----------------------------------------------

void AGameStateManager::InitPauseWidget()
{
	//if (!_PauseWidgetClass || !_PauseBackgroundWidgetClass || !_PauseHideWidgetClass) return;

	//if (_PauseWidget && _PauseBackgroundWidget && _PauseHideWidget) return;

	//UWorld* World = GetWorld();
	//if (!World) return;
	//APlayerController* PlayerController = World->GetFirstPlayerController();
	//if (!PlayerController) return;

	//_PauseWidget = CreateWidget<UUserWidget>(PlayerController, _PauseWidgetClass);
	//_PauseBackgroundWidget = CreateWidget<UUserWidget>(PlayerController, _PauseBackgroundWidgetClass);
	//_PauseHideWidget = CreateWidget<UUserWidget>(PlayerController, _PauseHideWidgetClass);

	//if (_PauseWidget)
	//{
	//	UButton* ButtonResume = Cast<UButton>(_PauseWidget->GetWidgetFromName(FName("Button_Resume")));
	//	if (ButtonResume)
	//	{
	//		ButtonResume->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedStartPause);
	//	}

	//	UButton* ButtonOption = Cast<UButton>(_PauseWidget->GetWidgetFromName(FName("Button_Option")));
	//	if (ButtonOption)
	//	{
	//		ButtonOption->OnClicked.AddDynamic(this, &AGameStateManager::OpenOptionMenu);
	//	}

	//	UButton* ButtonQuit = Cast<UButton>(_PauseWidget->GetWidgetFromName(FName("Button_Quit")));
	//	if (ButtonQuit)
	//	{
	//		ButtonQuit->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedQuitPause);
	//	}
	//}

	//if (_PauseHideWidget)
	//{
	//	UButton* ButtonPauseMenu = Cast<UButton>(_PauseHideWidget->GetWidgetFromName(FName("Button_PauseMenu")));
	//	if (ButtonPauseMenu)
	//	{
	//		ButtonPauseMenu->OnClicked.AddDynamic(this, &AGameStateManager::OnClickedPause);
	//	}
	//}
}

void AGameStateManager::ClosePauseWidget()
{
	//if (_PauseWidget && _PauseWidget->IsInViewport())
	//{
	//	_PauseWidget->RemoveFromParent();
	//}
	//if (_PauseBackgroundWidget && _PauseBackgroundWidget->IsInViewport())
	//{
	//	_PauseBackgroundWidget->RemoveFromParent();
	//}
	//if (_PauseHideWidget && _PauseHideWidget->IsInViewport())
	//{
	//	_PauseHideWidget->RemoveFromParent();
	//}

	//AVisitorController* VisitorController = Cast<AVisitorController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (VisitorController)
	//{
	//	VisitorController->SetPauseState(false);
	//}
}

void AGameStateManager::OpenHidePauseMenu()
{
	//if (!_PauseHideWidget)
	//{
	//	InitPauseWidget();
	//}

	//ClosePauseMenu();

	//_PauseHideWidget->AddToViewport();
}

void AGameStateManager::CloseHidePauseMenu()
{
	//if (_PauseHideWidget && _PauseHideWidget->IsInViewport())
	//{
	//	_PauseHideWidget->RemoveFromParent();
	//}
}

void AGameStateManager::OpenPauseMenu()
{
	//if (!_PauseWidget || !_PauseBackgroundWidget)
	//{
	//	InitPauseWidget();
	//}

	//CloseHidePauseMenu();

	//_PauseBackgroundWidget->AddToViewport();
	//_PauseWidget->AddToViewport();

	//AVisitorController* VisitorController = Cast<AVisitorController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (VisitorController)
	//{
	//	VisitorController->SetPauseState(true);
	//}
}

void AGameStateManager::ClosePauseMenu()
{
	//if (_PauseWidget && _PauseWidget->IsInViewport())
	//{
	//	_PauseWidget->RemoveFromParent();
	//}
	//if (_PauseBackgroundWidget && _PauseBackgroundWidget->IsInViewport())
	//{
	//	_PauseBackgroundWidget->RemoveFromParent();
	//}

	//AVisitorController* VisitorController = Cast<AVisitorController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (VisitorController)
	//{
	//	VisitorController->SetPauseState(false);
	//}
}

void AGameStateManager::OnClickedPause()
{
	/*CloseMainWidget();
	OpenPauseMenu();*/
}

void AGameStateManager::OnClickedStartPause()
{
	//InitMainWidget();
	//OpenHidePauseMenu();
}

void AGameStateManager::OnClickedOptionPause()
{

}

void AGameStateManager::OnClickedQuitPause()
{
	//UWorld* World = GetWorld();
	//if (!World) return;
	//APlayerController* CurrPlayerController = World->GetFirstPlayerController();
	//if (!CurrPlayerController) return;
	//APawn* CurrPawn = CurrPlayerController->GetPawn();
	//if (!CurrPawn) return;

	//if (_MainWidget)
	//{
	//	UFunction* Event = _MainWidget->FindFunction(FName("HideAll"));
	//	if (Event)
	//	{
	//		FHideAllParams Params = FHideAllParams();
	//		Params.bHideInstante = true;

	//		_MainWidget->ProcessEvent(Event, &Params);
	//	}
	//}

	//ClosePauseWidget();
	//CloseMainWidget();
	//
	//APawn* NewPawn = World->SpawnActor<APawn>(_MenuPawnClass, CurrPawn->GetActorLocation(), CurrPawn->GetActorRotation());
	//if (!NewPawn) return;

	//CurrPlayerController->UnPossess();
	//CurrPlayerController->Possess(NewPawn);

	//CurrPawn->Destroy();

	//_CurrTransitionTime = 0.0f;
	//FTimerDelegate TimerDelegate;
	//TimerDelegate.BindUFunction(this, FName("CameraTransitionCoroutine"), CurrPlayerController, NewPawn, World, NewPawn->GetActorLocation(), _MenuCameraPosition, NewPawn->GetActorRotation(), _MenuCameraRotation, false);
	//World->GetTimerManager().SetTimer(_TimerHandle, TimerDelegate, 1 / _TransitionFrameRate, true);

	//AVisitorController* VisitorController = Cast<AVisitorController>(UGameplayStatics::GetPlayerController(World, 0));
	//if (VisitorController)
	//{
	//	VisitorController->OnExitGame.Broadcast();
	//}
}

//Pause widget -----------------------------------------------

//Option widget ----------------------------------------------

//------------------------------------------------------------

//Getter, Setter ---------------------------------------------

EGameState AGameStateManager::GetGameState()
{
	return _GameState;
}