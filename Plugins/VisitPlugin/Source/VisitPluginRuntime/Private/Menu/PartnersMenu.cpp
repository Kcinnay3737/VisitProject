#include "Menu/PartnersMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Menu/BotMenu.h"

APartnersMenu::APartnersMenu()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APartnersMenu::BeginPlay()
{
	Super::BeginPlay();

	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &APartnersMenu::OnBotMenuStateChange);
	}

	InitWidget();
}

void APartnersMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APartnersMenu::InitWidget()
{
	if (!_PartnerWidgetClass) return;

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_PartnerWidget = CreateWidget<UUserWidget>(PlayerController, _PartnerWidgetClass);
	if (!_PartnerWidget) return;

	UFunction* Function = _PartnerWidget->FindFunction(FName("AddPartner"));
	if (Function)
	{
		for (FPartnerData& DataPartner : _DataPartners)
		{
			_PartnerWidget->ProcessEvent(Function, &DataPartner);
		}		
	}
}

void APartnersMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (BotMenuState == EBotMenuState::Partners)
	{
		OpenPlanMenu();
	}
	else
	{
		ClosePlanMenu();
	}
}

void APartnersMenu::OpenPlanMenu()
{
	if (_PartnerWidget && !_PartnerWidget->IsInViewport())
	{
		_PartnerWidget->AddToViewport();
	}
}

void APartnersMenu::ClosePlanMenu()
{
	if (_PartnerWidget && _PartnerWidget->IsInViewport())
	{
		_PartnerWidget->RemoveFromParent();
	}
}