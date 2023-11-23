//#include "Menu/TeleporterMenu.h"
//
//#include "Blueprint/UserWidget.h"
//#include "Blueprint/WidgetBlueprintLibrary.h"
//#include "Components/Button.h"
//#include "Components/ScrollBox.h"
//#include "Components/ScrollBoxSlot.h"
//#include "Components/TextBlock.h"
//#include "Interactable/Button/ButtonTeleport.h"
//#include "Kismet/GameplayStatics.h"
//
//ATeleporterMenu::ATeleporterMenu() : Super::ATeleporter()
//{
//	PrimaryActorTick.bCanEverTick = true;
//}
//
//void ATeleporterMenu::BeginPlay()
//{
//	Super::BeginPlay();
//
//}
//
//void ATeleporterMenu::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	
//	if (!_CurrWidget)
//	{
//		InitUI();
//		if (!_CurrWidget) return;
//	}
//}
//
//void ATeleporterMenu::InitUI()
//{
//	TArray<UUserWidget*> FoundWidgets;
//	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, _TeleportMenuWidgetClass, false);
//	if (FoundWidgets.Num() != 0)
//	{
//		_CurrWidget = FoundWidgets[0];
//	}
//	if (!_CurrWidget) return;
//
//	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//	UScrollBox* ScrollBoxTeleportPoint = Cast<UScrollBox>(_CurrWidget->GetWidgetFromName("ScrollBox_TeleportPoint"));
//	if (ScrollBoxTeleportPoint && PlayerController)
//	{
//		int Count = 0;
//		for (FTeleportData TeleportData : _TeleportData)
//		{
//			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _SlotTeleportWidgetClass);
//			if (Slot)
//			{
//				UTextBlock* TextBlock = Cast<UTextBlock>(Slot->GetWidgetFromName("TextBlock_Location"));
//				if (TextBlock)
//				{
//					TextBlock->SetText(TeleportData.PlaceName);
//				}
//
//				UButton* Button = Cast<UButton>(Slot->GetWidgetFromName("Button_Teleport"));
//				if (Button)
//				{
//					FSlateBrush Brush;
//					Brush = Button->WidgetStyle.Normal;
//					Brush.SetResourceObject(TeleportData.Thumbnail);
//					Button->WidgetStyle.Normal = Brush;
//
//					Brush = Button->WidgetStyle.Hovered;
//					Brush.SetResourceObject(TeleportData.Thumbnail);
//					Button->WidgetStyle.Hovered = Brush;
//
//					Brush = Button->WidgetStyle.Pressed;
//					Brush.SetResourceObject(TeleportData.Thumbnail);
//					Button->WidgetStyle.Pressed = Brush;
//
//					UButtonTeleport* ButtonTeleport = NewObject<UButtonTeleport>(Button);
//					if (ButtonTeleport)
//					{
//						ButtonTeleport->Initialise(this, Count);
//						_ButtonTeleport.Add(ButtonTeleport);
//						Button->OnClicked.AddDynamic(ButtonTeleport, &UButtonTeleport::OnClickTeleport);
//					}
//				}
//
//				UPanelSlot* PanelSlot = ScrollBoxTeleportPoint->AddChild(Slot);
//				UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
//				if (ScrollBoxSlot)
//				{
//					//ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
//				}
//
//				Count++;
//			}
//		}
//	}
//}