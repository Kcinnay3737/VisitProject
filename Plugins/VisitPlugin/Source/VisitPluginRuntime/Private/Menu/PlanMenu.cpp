#include "Menu/PlanMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Menu/BotMenu.h"

APlanMenu::APlanMenu()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APlanMenu::BeginPlay()
{
	Super::BeginPlay();
	
	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &APlanMenu::OnBotMenuStateChange);
	}

	InitWidget();
}

void APlanMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlanMenu::InitWidget()
{
	if (!_PlanWidgetClass) return;
	
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;
	
	_PlanWidget = CreateWidget<UUserWidget>(PlayerController, _PlanWidgetClass);
	if (!_PlanWidget) return;


}

void APlanMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (BotMenuState == EBotMenuState::Plan)
	{
		OpenPlanMenu();
	}
	else
	{
		ClosePlanMenu();
	}
}

void APlanMenu::OpenPlanMenu()
{
	if (_PlanWidget && !_PlanWidget->IsInViewport())
	{
		_PlanWidget->AddToViewport();
	}
}

void APlanMenu::ClosePlanMenu()
{
	if (_PlanWidget && _PlanWidget->IsInViewport())
	{
		_PlanWidget->RemoveFromParent();
	}
}
