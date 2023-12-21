#include "Menu/RoomsMenu.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Interactable/ButtonIndex.h"
#include "Kismet/GameplayStatics.h"

ARoomsMenu::ARoomsMenu() : Super::ATeleporter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARoomsMenu::BeginPlay()
{
	Super::BeginPlay();

	InitWidget();
	
	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &ARoomsMenu::OnBotMenuStateChange);
	}
}

void ARoomsMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoomsMenu::OnClickTeleport(int index)
{
	Super::OnClickTeleport(index);
	if (_bShowRoom)
	{
		ViewRoom(index);
	}
}

void ARoomsMenu::OnClickView(int index)
{
	ViewRoom(index);
}

void ARoomsMenu::OnClickFloor(int index)
{
	HideRoom();
	SelectFloor(_Floors[index]);
}

void ARoomsMenu::InitWidget()
{
	if (!_RoomsWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_RoomsWidget = CreateWidget<UUserWidget>(PlayerController, _RoomsWidgetClass);
	if (!_RoomsWidget) return;

	UScrollBox* ScrollBoxSlotFloor = Cast<UScrollBox>(_RoomsWidget->GetWidgetFromName("ScrollBox_Floor"));
	if (ScrollBoxSlotFloor)
	{
		ScrollBoxSlotFloor->ClearChildren();
		_ButtonFloor.Empty();
		int Count = 0;
		for (FString& Floor : _Floors)
		{
			int CurrentCount = Count;
			Count++;

			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _FloorSlotWidgetClass);
			if (!Slot) continue;

			UTextBlock* TextBlock = Cast<UTextBlock>(Slot->GetWidgetFromName("TextBlock_Floor"));
			if (TextBlock)
			{
				TextBlock->SetText(FText::FromString(Floor));
			}

			UButton* ButtonFloor = Cast<UButton>(Slot->GetWidgetFromName("Button_Floor"));
			if (ButtonFloor)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>(Slot);
				if (ButtonIndex)
				{
					ButtonIndex->Initialise(this, CurrentCount, "Floor");
					_ButtonFloor.Add(ButtonIndex);

					ButtonFloor->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				}
			}

			UPanelSlot* PanelSlot = ScrollBoxSlotFloor->AddChild(Slot);
		}
	}
}

void ARoomsMenu::OpenMenu()
{
	if (!_RoomsWidget || _RoomsWidget->IsInViewport()) return;
	_RoomsWidget->AddToViewport();
	if (_Floors.IsEmpty()) return;

	HideRoom();
	SelectFloor(_Floors[0]);
}

void ARoomsMenu::CloseMenu()
{
	if (!_RoomsWidget || !_RoomsWidget->IsInViewport()) return;
	HideRoom();
	_RoomsWidget->RemoveFromParent();
	_ButtonTeleport.Empty();
}

void ARoomsMenu::SelectFloor(FString Floor)
{
	if (!_ScrollBoxSlot)
	{
		_ScrollBoxSlot = Cast<UScrollBox>(_RoomsWidget->GetWidgetFromName("ScrollBox_Slot"));
		if (!_ScrollBoxSlot) return;
	}

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (_ScrollBoxSlot && PlayerController)
	{
		_ScrollBoxSlot->ClearChildren();
		_ButtonTeleport.Empty();
		int Count = 0;
		for (FTeleportData& TeleportData : _TeleportData)
		{
			int CurrCount = Count;
			Count++;

			if (!TeleportData.Floor.Equals(Floor)) continue;

			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _RoomsSlotWidgetClass);
			if (!Slot) continue;
			
			UTextBlock* TextBlock = Cast<UTextBlock>(Slot->GetWidgetFromName("TextBlock_Room"));
			if (TextBlock)
			{
				TextBlock->SetText(TeleportData.PlaceName);
			}

			UButton* ButtonTeleportOnPlace = Cast<UButton>(Slot->GetWidgetFromName("Button_Teleport"));
			if (ButtonTeleportOnPlace)
			{
				UButtonIndex* ButtonTeleport = NewObject<UButtonIndex>(Slot);
				if (ButtonTeleport)
				{
					ButtonTeleport->Initialise(this, CurrCount, "Teleport");
					_ButtonTeleport.Add(ButtonTeleport);

					ButtonTeleportOnPlace->OnClicked.AddDynamic(ButtonTeleport, &UButtonIndex::OnClick);
				}
			}

			UButton* ButtonView = Cast<UButton>(Slot->GetWidgetFromName("Button_Show"));
			if (ButtonView)
			{
				UButtonIndex* ButtonTeleport = NewObject<UButtonIndex>(Slot);
				if (ButtonTeleport)
				{
					ButtonTeleport->Initialise(this, CurrCount, "View");
					_ButtonTeleport.Add(ButtonTeleport);
					ButtonView->OnClicked.AddDynamic(ButtonTeleport, &UButtonIndex::OnClick);
				}
			}

			UPanelSlot* PanelSlot = _ScrollBoxSlot->AddChild(Slot);
		}
	}
}

void ARoomsMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (BotMenuState == EBotMenuState::Rooms)
	{
		OpenMenu();
	}
	else
	{
		CloseMenu();
	}
}

void ARoomsMenu::ViewRoom(int Index)
{
	_bShowRoom = true;
	UFunction* Function = _RoomsWidget->FindFunction(FName("ViewRoom"));
	if (Function)
	{
		FParamsViewRoom Param = FParamsViewRoom();
		Param.RoomName = _TeleportData[Index].PlaceName;
		Param.Texture = _TeleportData[Index].Thumbnail;
		_RoomsWidget->ProcessEvent(Function, &Param);
	}
}

void ARoomsMenu::HideRoom()
{
	_bShowRoom = false;
	UFunction* Function = _RoomsWidget->FindFunction(FName("HideRoom"));
	if (Function)
	{
		_RoomsWidget->ProcessEvent(Function, nullptr);
	}
}
