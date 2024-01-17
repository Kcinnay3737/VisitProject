#include "Menu/TopMenu.h"
#include "Manager/GameStateManager.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Data/DataAssetPartner.h"
#include "Components/ScrollBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

ATopMenu::ATopMenu()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATopMenu::BeginPlay()
{
	Super::BeginPlay();
	
	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
	if (_GameStateManager)
	{
		_GameStateManager->OnEnterGameState.AddDynamic(this, &ATopMenu::ShowWidget);
		_GameStateManager->OnExitGameState.AddDynamic(this, &ATopMenu::HideWidget);
	}

	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &ATopMenu::OnBotMenuStateChange);
	}

	InitWidget();
}

void ATopMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATopMenu::InitWidget()
{
	if (_TopWidget) return;

	if (!_TopWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_TopWidget = CreateWidget<UUserWidget>(PlayerController, _TopWidgetClass);
}

void ATopMenu::ShowWidget()
{
	if (_TopWidget)
	{
		if (!_TopWidget->IsInViewport())
		{
			_TopWidget->AddToViewport();

			//UFunction* Function = _TopWidget->FindFunction(FName("OnShowWidget"));
			//if (Function)
			//{
			//	_TopWidget->ProcessEvent(Function, nullptr);
			//}
		}
	}

	UFunction* Function = _TopWidget->FindFunction(FName("ResetAnimation"));
	if (Function)
	{
		_TopWidget->ProcessEvent(Function, nullptr);
	}

	//UFunction* Function = _TopWidget->FindFunction(FName("OnShowWidget"));
	//if (Function)
	//{
	//	_TopWidget->ProcessEvent(Function, nullptr);
	//}

	//_ButtonOpenClose = Cast<UButton>(_TopWidget->GetWidgetFromName(FName("Button_OpenClose")));
	//if (_ButtonOpenClose)
	//{
	//	_ButtonOpenClose->OnClicked.AddDynamic(this, &ATopMenu::OnOpenCloseMenuPressed);
	//}

	//UScrollBox* ScrollBoxPartner = Cast<UScrollBox>(_TopWidget->GetWidgetFromName(FName("ScrollBox_Partner")));
	//if (ScrollBoxPartner && _DataAssetPartner)
	//{
	//	for (auto& Elem : _DataAssetPartner->DataPartner)
	//	{
	//		FString& PartnerName = Elem.Key;
	//		FDataPartner& DataPartner = Elem.Value;
	//
	//		UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _SlotPartnerWidgetClass);
	//		if (!Slot) continue;
	//
	//		UImage* ImagePartnerLogo = Cast<UImage>(Slot->GetWidgetFromName(FName("Image_PartnerLogo")));
	//		if (ImagePartnerLogo)
	//		{
	//			ImagePartnerLogo->SetBrushFromTexture(DataPartner.PartnerLogo, false);
	//		}
	//
	//		UTextBlock* TextBlockPartner = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_Partner")));
	//		if (TextBlockPartner)
	//		{
	//			TextBlockPartner->SetText(FText::FromString(PartnerName));
	//		}
	//
	//		UPanelSlot* PanelSlot = ScrollBoxPartner->AddChild(Slot);
	//		UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
	//		if (ScrollBoxSlot)
	//		{
	//			
	//		}
	//	}
	//}
}

void ATopMenu::HideWidget()
{
	if (_TopWidget->IsInViewport())
	{
		_TopWidget->RemoveFromParent();
	}
	_bIsOpen = false;
}

void ATopMenu::OpenMenu()
{
	//if (!_TopWidget) return;
	//UFunction* Function = _TopWidget->FindFunction(FName("OpenMenu"));
	//if (Function)
	//{
	//	_TopWidget->ProcessEvent(Function, nullptr);
	//}
	//_bIsOpen = true;
}

void ATopMenu::CloseMenu()
{
	//if (!_TopWidget) return;
	//UFunction* Function = _TopWidget->FindFunction(FName("CloseMenu"));
	//if (Function)
	//{
	//	_TopWidget->ProcessEvent(Function, nullptr);
	//}
	//_bIsOpen = false;
}

void ATopMenu::OnOpenCloseMenuPressed()
{
	//if (_bIsOpen)
	//{
	//	CloseMenu();
	//}
	//else
	//{
	//	OpenMenu();
	//}
}

void ATopMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (!_GameStateManager || _GameStateManager->GetGameState() != EGameState::Game) return;
	if (BotMenuState == EBotMenuState::Home)
	{
		ShowWidget();
	}
	else
	{
		HideWidget();
	}
}