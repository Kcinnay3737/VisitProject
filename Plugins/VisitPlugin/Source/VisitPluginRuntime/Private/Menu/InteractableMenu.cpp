#include "Menu/InteractableMenu.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Menu/BotMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Interactable/InteractableMM.h"
#include "Components/ScrollBoxSlot.h"

AInteractableMenu::AInteractableMenu()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AInteractableMenu::BeginPlay()
{
	Super::BeginPlay();
	
	_BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (_BotMenu)
	{
		_BotMenu->OnBotMenuStateChange.AddDynamic(this, &AInteractableMenu::OnBotMenuStateChanged);
	}

	InitWidget();
}

void AInteractableMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableMenu::InitWidget()
{
	if (!_ObjectSelectionWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_ObjectSelectionWidget = CreateWidget<UUserWidget>(PlayerController, _ObjectSelectionWidgetClass);
	if (!_ObjectSelectionWidget) return;

	UButton* ButtonMesh = Cast<UButton>(_ObjectSelectionWidget->GetWidgetFromName(FName("Button_Mesh")));
	if (ButtonMesh)
	{
		ButtonMesh->OnClicked.AddDynamic(this, &AInteractableMenu::OnButtonMeshClicked);
	}

	UButton* ButtonMaterial = Cast<UButton>(_ObjectSelectionWidget->GetWidgetFromName(FName("Button_Material")));
	if (ButtonMaterial)
	{
		ButtonMaterial->OnClicked.AddDynamic(this, &AInteractableMenu::OnButtonMaterialClicked);
	}

	UButton* ButtonCloseView = Cast<UButton>(_ObjectSelectionWidget->GetWidgetFromName(FName("Button_CloseView")));
	if (ButtonCloseView)
	{
		ButtonCloseView->OnClicked.AddDynamic(this, &AInteractableMenu::OnButtonCloseViewClicked);
	}

	UButton* ButtonApplyView = Cast<UButton>(_ObjectSelectionWidget->GetWidgetFromName(FName("Button_ApplyView")));
	if (ButtonApplyView)
	{
		ButtonApplyView->OnClicked.AddDynamic(this, &AInteractableMenu::OnButtonApplyViewClicked);
	}

	_TextBlockViewName = Cast<UTextBlock>(_ObjectSelectionWidget->GetWidgetFromName(FName("TextBlock_ViewName")));
	_TextBlockViewDescription = Cast<UTextBlock>(_ObjectSelectionWidget->GetWidgetFromName(FName("TextBlock_ViewDescription")));
	_ImageView = Cast<UImage>(_ObjectSelectionWidget->GetWidgetFromName(FName("Image_View")));
	_ScrollBoxSelection = Cast<UScrollBox>(_ObjectSelectionWidget->GetWidgetFromName(FName("ScrollBox_Selection")));
}

void AInteractableMenu::ApplyCurrentSettings()
{
	if (_CurrInteractableMM)
	{
		_CurrInteractableMM->ApplySetting(_IndexMeshSelected, _IndexMaterialSelected);
	}
}

void AInteractableMenu::ViewCurrentSettings()
{
	if (_CurrInteractableMM)
	{
		const TArray<FDataPreset>& DataPresets = _CurrInteractableMM->GetDataPreset();

		const FDataSlot& DataSlot = _CurrentSelectionState == ESelectionState::Mesh ? DataPresets[_IndexMeshSelected].MeshDataSlot : DataPresets[_IndexMeshSelected].MaterialDataSlot[_IndexMaterialSelected];

		FParamShowViewPart ParamViewPart = FParamShowViewPart();
		ParamViewPart.Image = DataSlot.Thumbnail;
		ParamViewPart.Name = DataSlot.ProductName;
		ParamViewPart.Description = DataSlot.Description;

		CallShowViewPart(ParamViewPart);
	}
}

//Button view selector ---------------------------------------

void AInteractableMenu::OnButtonMeshClicked()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!_CurrInteractableMM || !PlayerController) return;

	_CurrentSelectionState = ESelectionState::Mesh;

	_ButtonIndex.Empty();

	CallHideViewPart();
	if (_ScrollBoxSelection)
	{
		_ScrollBoxSelection->ClearChildren();

		int Index = 0;
		const TArray<FDataPreset>&DataPresets = _CurrInteractableMM->GetDataPreset();
		for (const FDataPreset& DataPreset : DataPresets)
		{
			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _ObjectSelectionSlotWidgetClass);
			if (!Slot) continue;

			UButton* ButtonView = Cast<UButton>(Slot->GetWidgetFromName(FName("Button_Show")));
			if (ButtonView)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>();
				ButtonIndex->Initialise(this, Index, "View");
				ButtonView->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				_ButtonIndex.Add(ButtonIndex);
			}

			UButton* ButtonApply = Cast<UButton>(Slot->GetWidgetFromName(FName("Button_Apply")));
			if (ButtonApply)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>();
				ButtonIndex->Initialise(this, Index, "Apply");
				ButtonApply->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				_ButtonIndex.Add(ButtonIndex);
			}

			UTextBlock* TextBlockProductName = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_ProductName")));
			if (TextBlockProductName)
			{
				TextBlockProductName->SetText(DataPreset.MeshDataSlot.ProductName);
			}

			UTextBlock* TextBlockCompanyName = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_CompanyName")));
			if (TextBlockCompanyName)
			{
				TextBlockCompanyName->SetText(DataPreset.MeshDataSlot.CompanyName);
			}

			UImage* ImagePreview = Cast<UImage>(Slot->GetWidgetFromName(FName("Image_Preview")));
			if (ImagePreview)
			{
				ImagePreview->SetBrushFromTexture(DataPreset.MeshDataSlot.Thumbnail);
			}

			UPanelSlot* PanelSlot = _ScrollBoxSelection->AddChild(Slot);
			UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
			if (ScrollBoxSlot)
			{
				
			}

			Index++;
		}
	}
}

void AInteractableMenu::OnButtonMaterialClicked()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!_CurrInteractableMM || !PlayerController) return;

	_CurrentSelectionState = ESelectionState::Material;

	_ButtonIndex.Empty();

	CallHideViewPart();
	if (_ScrollBoxSelection)
	{
		_ScrollBoxSelection->ClearChildren();

		int Index = 0;
		const TArray<FDataPreset>& DataPresets = _CurrInteractableMM->GetDataPreset();
		for (const FDataSlot& DataSlot : DataPresets[_IndexMeshSelected].MaterialDataSlot)
		{
			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _ObjectSelectionSlotWidgetClass);
			if (!Slot) continue;

			UButton* ButtonView = Cast<UButton>(Slot->GetWidgetFromName(FName("Button_Show")));
			if (ButtonView)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>();
				ButtonIndex->Initialise(this, Index, "View");
				ButtonView->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				_ButtonIndex.Add(ButtonIndex);
			}

			UButton* ButtonApply = Cast<UButton>(Slot->GetWidgetFromName(FName("Button_Apply")));
			if (ButtonApply)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>();
				ButtonIndex->Initialise(this, Index, "Apply");
				ButtonApply->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				_ButtonIndex.Add(ButtonIndex);
			}

			UTextBlock* TextBlockProductName = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_ProductName")));
			if (TextBlockProductName)
			{
				TextBlockProductName->SetText(DataSlot.ProductName);
			}

			UTextBlock* TextBlockCompanyName = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_CompanyName")));
			if (TextBlockCompanyName)
			{
				TextBlockCompanyName->SetText(DataSlot.CompanyName);
			}

			UImage* ImagePreview = Cast<UImage>(Slot->GetWidgetFromName(FName("Image_Preview")));
			if (ImagePreview)
			{
				ImagePreview->SetBrushFromTexture(DataSlot.Thumbnail);
			}

			UPanelSlot* PanelSlot = _ScrollBoxSelection->AddChild(Slot);
			UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
			if (ScrollBoxSlot)
			{

			}

			Index++;
		}
	}
}

//------------------------------------------------------------

//View section button ----------------------------------------

void AInteractableMenu::OnButtonCloseViewClicked()
{
	CallHideViewPart();
}

void AInteractableMenu::OnButtonApplyViewClicked()
{
	ApplyCurrentSettings();
}

//------------------------------------------------------------

//Menu change state ------------------------------------------

void AInteractableMenu::OpenMenu()
{
	//Add the widget in the viewport
	if (_ObjectSelectionWidget && !_ObjectSelectionWidget->IsInViewport())
	{
		CallHideViewPart();
		_ObjectSelectionWidget->AddToViewport();
	}
}

void AInteractableMenu::CloseMenu()
{
	//Remove thw widget from the viewport
	if (_ObjectSelectionWidget && _ObjectSelectionWidget->IsInViewport())
	{
		CallHideViewPart();
		_ObjectSelectionWidget->RemoveFromViewport();
	}
}

void AInteractableMenu::OnBotMenuStateChanged(EBotMenuState BotMenuState)
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

void AInteractableMenu::CallHideViewPart()
{
	UFunction* Function = _ObjectSelectionWidget->FindFunction(FName("HideViewPart"));
	if (Function)
	{
		_ObjectSelectionWidget->ProcessEvent(Function, nullptr);
	}
	_bViewPartIsOpen = false;
}

void AInteractableMenu::CallShowViewPart(FParamShowViewPart& Params)
{
	UFunction* Function = _ObjectSelectionWidget->FindFunction(FName("ShowViewPart"));
	if (Function)
	{
		_ObjectSelectionWidget->ProcessEvent(Function, &Params);
	}
	_bViewPartIsOpen = true;
}

//------------------------------------------------------------

void AInteractableMenu::OnButtonIndexClick(FString Type, int Index)
{
	switch (_CurrentSelectionState)
	{
	case ESelectionState::Mesh:
		_IndexMeshSelected = Index;
		_IndexMaterialSelected = 0;
		break;
	case ESelectionState::Material:
		_IndexMaterialSelected = Index;
		break;
	}

	if (Type.Equals("View"))
	{
		ViewCurrentSettings();
	}
	if (Type.Equals("Apply"))
	{
		ApplyCurrentSettings();
		if (_bViewPartIsOpen)
		{
			ViewCurrentSettings();
		}
	}
}

void AInteractableMenu::SetInteractableMM(AInteractableMM* InteractableMM)
{
	if (!InteractableMM) return;

	if (_CurrInteractableMM != InteractableMM)
	{
		_CurrInteractableMM = InteractableMM;

		_IndexMeshSelected = _CurrInteractableMM->GetIndexMeshSelected();
		_IndexMaterialSelected = _CurrInteractableMM->GetIndexMaterialSelected();

		OnButtonMeshClicked();
	}

	if (_BotMenu)
	{
		_BotMenu->SetBotMenuState(EBotMenuState::Interactable);
	}
}