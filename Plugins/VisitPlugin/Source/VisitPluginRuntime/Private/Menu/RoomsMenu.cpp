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

	UButton* ButtonRender = Cast<UButton>(_RoomsWidget->GetWidgetFromName(FName("Button_Render")));
	if (ButtonRender)
	{
		ButtonRender->OnClicked.AddDynamic(this, &ARoomsMenu::OnClickRenderImage);
	}
}

void ARoomsMenu::OpenMenu()
{
	if (!_RoomsWidget || _RoomsWidget->IsInViewport()) return;
	_RoomsWidget->AddToViewport();
	if (_Floors.IsEmpty()) return;

	HideRoom();
	//SelectFloor(_Floors[0]);
	SelectFloor("");
}

void ARoomsMenu::CloseMenu()
{
	if (!_RoomsWidget || !_RoomsWidget->IsInViewport()) return;
	HideRoom();
	_RoomsWidget->RemoveFromParent();
	_ButtonTeleport.Empty();

	UFunction* Function = _RoomsWidget->FindFunction(FName("HightLightStep"));
	if (Function)
	{
		FParamsHightLight Param = FParamsHightLight();
		Param.Index = -1;
		_RoomsWidget->ProcessEvent(Function, &Param);
	}
}

void ARoomsMenu::SelectFloor(FString Floor)
{
	UFunction* Function = _RoomsWidget->FindFunction(FName("HightLightStep"));
	if (Function && Floor != "")
	{
		FParamsHightLight Param = FParamsHightLight();
		Param.Index = _Floors.Find(Floor);
		_RoomsWidget->ProcessEvent(Function, &Param);
	}

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
		_DictIndexToArrayIndex.Empty();
		int Count = 0;
		int ArrayCount = 0;
		for (FTeleportData& TeleportData : _TeleportData)
		{
			int CurrCount = Count;
			Count++;

			if (!TeleportData.Floor.Equals(Floor) && Floor != "") continue;

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

			_DictIndexToArrayIndex.Add(CurrCount, ArrayCount);

			ArrayCount++;
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
		Param.Texture = _TeleportData[Index].Thumbnail;
		_RoomsWidget->ProcessEvent(Function, &Param);
	}

	Function = _RoomsWidget->FindFunction(FName("HightLightSlot"));
	if (Function)
	{
		FParamsHightLight Param = FParamsHightLight();
		Param.Index = _DictIndexToArrayIndex[Index];
		_RoomsWidget->ProcessEvent(Function, &Param);
	}

	_CurrViewRoomIndex = Index;
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

void ARoomsMenu::OnClickRenderImage()
{
	if (_bShowRoom)
	{
		OnClickTeleport(_CurrViewRoomIndex);
	}
}