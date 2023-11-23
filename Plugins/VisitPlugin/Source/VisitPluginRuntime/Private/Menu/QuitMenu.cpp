#include "Menu/QuitMenu.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Manager/GameStateManager.h"
#include "Kismet/GameplayStatics.h"

AQuitMenu::AQuitMenu()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AQuitMenu::BeginPlay()
{
	Super::BeginPlay();	

	InitWidget();

	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
	if (_GameStateManager)
	{
		_GameStateManager->OnEnterGameState.AddDynamic(this, &AQuitMenu::EnableQuitButton);
		_GameStateManager->OnEnterStartMenuStat.AddDynamic(this, &AQuitMenu::EnableQuitButton);
		_GameStateManager->OnExitGameState.AddDynamic(this, &AQuitMenu::DisableQuitButton);
		_GameStateManager->OnExitStartMenuState.AddDynamic(this, &AQuitMenu::DisableQuitButton);
	}
}

void AQuitMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuitMenu::InitWidget()
{
	if (!_QuitWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_QuitWidget = CreateWidget<UUserWidget>(PlayerController, _QuitWidgetClass);
	if (!_QuitWidget) return;
	_QuitWidget->AddToViewport();

	_ButtonQuit = Cast<UButton>(_QuitWidget->GetWidgetFromName("Button_Quit"));
	if (_ButtonQuit)
	{
		_ButtonQuit->OnClicked.AddDynamic(this, &AQuitMenu::Quit);
	}
}

void AQuitMenu::Quit()
{
	if (!_GameStateManager) return;

	EGameState CurrGameState = _GameStateManager->GetGameState();
	if (CurrGameState == EGameState::Game)
	{
		QuitGame();
	}
	else if (CurrGameState == EGameState::StartMenu)
	{
		QuitApplication();
	}
}

void AQuitMenu::QuitApplication()
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	UKismetSystemLibrary::QuitGame(world, playerController, EQuitPreference::Quit, true);
}

void AQuitMenu::QuitGame()
{
	if (!_GameStateManager) return;

	_GameStateManager->StopGame();
}

void AQuitMenu::DisableQuitButton()
{
	if (!_QuitWidget) return;
	UFunction* Function = _QuitWidget->FindFunction(FName("DisableQuitButton"));
	if (Function)
	{
		_QuitWidget->ProcessEvent(Function, nullptr);
	}
}

void AQuitMenu::EnableQuitButton()
{
	if (!_QuitWidget) return;
	UFunction* Function = _QuitWidget->FindFunction(FName("EnableQuitButton"));
	if (Function)
	{
		_QuitWidget->ProcessEvent(Function, nullptr);
	}
}