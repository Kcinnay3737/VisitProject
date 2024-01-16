#include "Menu/PresetSelectionMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Menu/BotMenu.h"
#include "Data/DataAssetProvider.h"
#include "Data/DataAssetProduct.h"

APresetSelectionMenu::APresetSelectionMenu()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APresetSelectionMenu::BeginPlay()
{
	Super::BeginPlay();

	_BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (_BotMenu)
	{
		_BotMenu->OnBotMenuStateChange.AddDynamic(this, &APresetSelectionMenu::OnBotMenuStateChanged);
	}

	int Count = 0;
	for (FPreset& Preset : _AllPreset)
	{
		for (FActorPreset& ActorPreset : Preset.Presets)
		{
			for (AActor* Actor : ActorPreset.ActorInPreset)
			{
				_AllActorPreset.Add(Actor, Count);
				
				TArray<UActorComponent*> StaticMeshComponents = Actor->GetComponentsByClass(UStaticMeshComponent::StaticClass());
				for (UActorComponent* ActorComponent : StaticMeshComponents)
				{
					UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent);
					if (StaticMeshComponent)
					{
						StaticMeshComponent->SetOverlayMaterial(_OverlayMaterial);
					}
				}
			}
		}
		Count++;
	}

	for (int i = 0; i < _AllPreset.Num(); i++)
	{
		ShowPreset(i, 0);
	}

	InitWidget();
}

void APresetSelectionMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APresetSelectionMenu::OnBotMenuStateChanged(EBotMenuState BotMenuState)
{
	if (BotMenuState == EBotMenuState::Interactable)
	{
		OpenMenu();
	}
	else
	{
		CloseMenu();
	}
}

void APresetSelectionMenu::InitWidget()
{
	if (!_PresetSelectionWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_PresetSelectionWidget = CreateWidget<UUserWidget>(PlayerController, _PresetSelectionWidgetClass);
	if (!_PresetSelectionWidget) return;

	UButton* ButtonOne = Cast<UButton>(_PresetSelectionWidget->GetWidgetFromName(FName("Button_Preset1")));
	if (ButtonOne)
	{
		ButtonOne->OnClicked.AddDynamic(this, &APresetSelectionMenu::OnSelectPresetOne);
	}

	UButton* ButtonTwo = Cast<UButton>(_PresetSelectionWidget->GetWidgetFromName(FName("Button_Preset2")));
	if (ButtonTwo)
	{
		ButtonTwo->OnClicked.AddDynamic(this, &APresetSelectionMenu::OnSelectPresetTwo);
	}
}

void APresetSelectionMenu::OpenMenu()
{
	if (_PresetSelectionWidget && !_PresetSelectionWidget->IsInViewport())
	{
		_PresetSelectionWidget->AddToViewport();
	}
}

void APresetSelectionMenu::CloseMenu()
{
	if (_PresetSelectionWidget && _PresetSelectionWidget->IsInViewport())
	{
		_PresetSelectionWidget->RemoveFromViewport();
	}
}

void APresetSelectionMenu::UpdateUI()
{
	if (!_PresetSelectionWidget) return;

	UFunction* Function = _PresetSelectionWidget->FindFunction(FName("SetUIValue"));
	if (Function)
	{
		_PresetSelectionWidget->ProcessEvent(Function, &_AllPreset[_CurrentPresetIndex].Presets[_AllPreset[_CurrentPresetIndex].CurrPreset].PresetData);
	}
}

void APresetSelectionMenu::UpdateSelectedPresetButton()
{
	if (!_PresetSelectionWidget) return;

	UFunction* Function = _PresetSelectionWidget->FindFunction(FName("HightlightPreset"));
	if (Function)
	{
		FHightlightPresetButtonParam Param = FHightlightPresetButtonParam();
		Param.Index = _AllPreset[_CurrentPresetIndex].CurrPreset;
		_PresetSelectionWidget->ProcessEvent(Function, &Param);
	}
}

void APresetSelectionMenu::OnSelectPresetOne()
{
	ShowPreset(_CurrentPresetIndex, 0);
}

void APresetSelectionMenu::OnSelectPresetTwo()
{
	ShowPreset(_CurrentPresetIndex, 1);
}

void APresetSelectionMenu::OnTouchActor(AActor* Actor)
{
	if (_AllActorPreset.Contains(Actor))
	{
		SetPresetSelected(_AllActorPreset[Actor]);

		if (_BotMenu)
		{
			_BotMenu->SetBotMenuState(EBotMenuState::Interactable);
		}
	}
}

void APresetSelectionMenu::ShowPreset(int PresetIndex, int ActorPresetIndex)
{
	FPreset& Preset = _AllPreset[PresetIndex];

	int Count = 0;
	for (FActorPreset& ActorPreset : Preset.Presets)
	{
		if (ActorPresetIndex == Count)
		{
			Preset.CurrPreset = ActorPresetIndex;
			for (AActor* Actor : ActorPreset.ActorInPreset)
			{
				Actor->SetActorHiddenInGame(false);
				Actor->SetActorEnableCollision(true);
			}
		}
		else
		{
			for (AActor* Actor : ActorPreset.ActorInPreset)
			{
				Actor->SetActorHiddenInGame(true);
				Actor->SetActorEnableCollision(false);
			}
		}

		Count++;
	}
	UpdateSelectedPresetButton();

	UpdateUI();
}

void APresetSelectionMenu::SetPresetSelected(int PresetIndex)
{
	_CurrentPresetIndex = PresetIndex;
	UpdateSelectedPresetButton();

	UFunction* Function = _PresetSelectionWidget->FindFunction(FName("HidePresetButton"));
	if (Function)
	{
		FHidePresetButtonParam Param = FHidePresetButtonParam();
		if (_AllPreset[PresetIndex].Presets.Num() < 2)
		{
			Param.bHide = true;
		}
		else
		{
			Param.bHide = false;
		}
		_PresetSelectionWidget->ProcessEvent(Function, &Param);
	}
	
	UpdateUI();
}
