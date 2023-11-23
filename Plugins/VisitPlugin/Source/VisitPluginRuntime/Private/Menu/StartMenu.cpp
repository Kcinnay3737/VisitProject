#include "Menu/StartMenu.h"

#include "Menu/OptionMenu.h"
#include "Manager/GameStateManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

AStartMenu::AStartMenu()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AStartMenu::BeginPlay()
{
	Super::BeginPlay();
	
	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
	if (_GameStateManager)
	{
		_GameStateManager->OnEnterStartMenuStat.AddDynamic(this, &AStartMenu::OpenStartMenu);
		_GameStateManager->OnExitStartMenuState.AddDynamic(this, &AStartMenu::CloseStartMenu);
	}

	_OptionMenu = Cast<AOptionMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), AOptionMenu::StaticClass()));
	if (_OptionMenu)
	{
		_OptionMenu->OnOpenOption.AddDynamic(this, &AStartMenu::HideViewWidget);
		_OptionMenu->OnCloseOption.AddDynamic(this, &AStartMenu::ShowViewWidget);
	}
}

void AStartMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStartMenu::OpenStartMenu()
{
	if (_StartMenuWidget)
	{
		if (!_StartMenuWidget->IsInViewport())
		{
			_StartMenuWidget->AddToViewport(-1);
		}
		return;
	}

	if (!_StartMenuWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_StartMenuWidget = CreateWidget<UUserWidget>(PlayerController, _StartMenuWidgetClass);
	if (!_StartMenuWidget) return;
	_StartMenuWidget->AddToViewport(-1);

	if (!_GameStateManager)
	{
		_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
		if (!_GameStateManager) return;
	}

	_ButtonStart = Cast<UButton>(_StartMenuWidget->GetWidgetFromName("Button_Start"));
	if (_ButtonStart)
	{
		_ButtonStart->OnClicked.AddDynamic(_GameStateManager, &AGameStateManager::StartGame);
	}

	_TextBlockProjectName = Cast<UTextBlock>(_StartMenuWidget->GetWidgetFromName("TextBlock_ProjectName"));
	if (_TextBlockProjectName)
	{
		_TextBlockProjectName->SetText(FText::FromString(_ProjectName));
	}
}

void AStartMenu::CloseStartMenu()
{
	if (_StartMenuWidget && _StartMenuWidget->IsInViewport())
	{
		_StartMenuWidget->RemoveFromParent();
	}
}

void AStartMenu::HideViewWidget()
{
	if (!_StartMenuWidget) return;
	UFunction* Function = _StartMenuWidget->FindFunction(FName("HideViewWidget"));
	if (Function)
	{
		_StartMenuWidget->ProcessEvent(Function, nullptr);
	}
}

void AStartMenu::ShowViewWidget()
{
	if (!_StartMenuWidget) return;
	UFunction* Function = _StartMenuWidget->FindFunction(FName("ShowViewWidget"));
	if (Function)
	{
		_StartMenuWidget->ProcessEvent(Function, nullptr);
	}
}