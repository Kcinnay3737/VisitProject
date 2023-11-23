#include "Menu/BotMenu.h"

#include "Manager/GameStateManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UMG/Public/Components/Button.h"

ABotMenu::ABotMenu()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABotMenu::BeginPlay()
{
	Super::BeginPlay();
	
	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
	if (_GameStateManager)
	{
		_GameStateManager->OnEnterGameState.AddDynamic(this, &ABotMenu::ShowBotWidget);
		_GameStateManager->OnExitGameState.AddDynamic(this, &ABotMenu::HideBotWidget);
	}
}

void ABotMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABotMenu::ShowBotWidget()
{
	if (_BotWidget)
	{
		if (!_BotWidget->IsInViewport())
		{
			_BotWidget->AddToViewport();
		}
		return;
	}

	if (!_BotWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_BotWidget = CreateWidget<UUserWidget>(PlayerController, _BotWidgetClass);
	if (!_BotWidget) return;
	_BotWidget->AddToViewport();

	_HomeButton = Cast<UButton>(_BotWidget->GetWidgetFromName(FName("Button_Home")));
	if (_HomeButton)
	{
		_HomeButton->OnClicked.AddDynamic(this, &ABotMenu::OnHomeButtonClick);
	}

	_RoomsButton = Cast<UButton>(_BotWidget->GetWidgetFromName(FName("Button_Rooms")));
	if (_RoomsButton)
	{
		_RoomsButton->OnClicked.AddDynamic(this, &ABotMenu::OnRoomsButtonClick);
	}

	_PlanButton = Cast<UButton>(_BotWidget->GetWidgetFromName(FName("Button_Plan")));
	if (_PlanButton)
	{
		_PlanButton->OnClicked.AddDynamic(this, &ABotMenu::OnPlanButtonClick);
	}

	_PartnersButton = Cast<UButton>(_BotWidget->GetWidgetFromName(FName("Button_Partners")));
	if (_PartnersButton)
	{
		_PartnersButton->OnClicked.AddDynamic(this, &ABotMenu::OnPartnersButtonClick);
	}

	SetBotMenuState(EBotMenuState::Home);
}

void ABotMenu::HideBotWidget()
{
	if (_BotWidget && _BotWidget->IsInViewport())
	{
		SetBotMenuState(EBotMenuState::Home);
		_BotWidget->RemoveFromParent();
	}
}

void ABotMenu::OnHomeButtonClick()
{
	SetBotMenuState(EBotMenuState::Home);
}

void ABotMenu::OnRoomsButtonClick()
{
	SetBotMenuState(EBotMenuState::Rooms);
}

void ABotMenu::OnPlanButtonClick()
{
	SetBotMenuState(EBotMenuState::Plan);
}

void ABotMenu::OnPartnersButtonClick()
{
	SetBotMenuState(EBotMenuState::Partners);
}

void ABotMenu::SetBotMenuState(EBotMenuState BotMenuState)
{
	_BotMenuState = BotMenuState;

	if (_BotWidget)
	{
		UFunction* Function = _BotWidget->FindFunction(FName("OnButtonClicked"));
		if (Function)
		{
			FParamOnClickedButton ParamOnClickedButton = FParamOnClickedButton();
			ParamOnClickedButton.BotMenuState = BotMenuState;
			_BotWidget->ProcessEvent(Function, &ParamOnClickedButton);
		}
	}
	
	if (_GameStateManager)
	{
		if (_BotMenuState == EBotMenuState::Home)
		{
			_GameStateManager->OnPlayerCanInteract.Broadcast();
		}
		else
		{
			_GameStateManager->OnPlayerCannotInteract.Broadcast();
		}
	}

	OnBotMenuStateChange.Broadcast(_BotMenuState);
}

EBotMenuState ABotMenu::GetBotMenuState()
{
	return _BotMenuState;
}
