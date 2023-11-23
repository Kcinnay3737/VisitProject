#include "Interactable/InteractableMM.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Interactable/Button/ButtonAction.h"
#include "Interactable/DataAssetCompanyProduct.h"
#include "Menu/InteractableMenu.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

AInteractableMM::AInteractableMM() : AInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableMM::BeginPlay()
{
	Super::BeginPlay();

	//Replace all static mesh actor with a static mesh component
	int count = 0;
	for (FDataSM& DataSM : _DataStaticMesh)
	{
		if(!DataSM.StaticMeshActor) continue;

		//Create staticMeshComponent
		UStaticMeshComponent* staticMeshComponent = NewObject<UStaticMeshComponent>(this, FName(FString::Printf(TEXT("StaticMeshComponent%d"), count)));
	
		if (staticMeshComponent)
		{
			DataSM.StaticMeshComponent = staticMeshComponent;

			DataSM.StaticMeshComponent->SetOverlayMaterial(_OverlayMaterial);

			//Assign the base transform
			for (FDataSMPreset& DataSMPreset : DataSM.DataSMPreset)
			{
				DataSMPreset.DataLocation.BaseLocation = DataSM.StaticMeshActor->GetActorLocation();
				DataSMPreset.DataLocation.BaseRotation = DataSM.StaticMeshActor->GetActorRotation();
				DataSMPreset.DataLocation.BaseScale = DataSM.StaticMeshActor->GetActorScale();
			}

			//Save the component
			staticMeshComponent->RegisterComponent();
		}

		DataSM.StaticMeshActor->Destroy();

		count++;
	}

	ApplySetting(0, 0);

	_InteractableMenu = Cast<AInteractableMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), AInteractableMenu::StaticClass()));
}

void AInteractableMM::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableMM::Interact()
{
	Super::Interact();

	if (_bEditableMaterialMesh)
	{
		if (!_InteractableMenu)
		{
			_InteractableMenu = Cast<AInteractableMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), AInteractableMenu::StaticClass()));
			if (!_InteractableMenu) return;
		}
		_InteractableMenu->SetInteractableMM(this);
	}
}

void AInteractableMM::ApplySetting(int meshIndex, int materialIndex)
{
	//For all data static mesh
	for (FDataSM& DataSM : _DataStaticMesh)
	{
		if (!DataSM.StaticMeshComponent) continue;
		if (DataSM.DataSMPreset.Num() == 0) continue;

		//Set the static mesh value
		DataSM.StaticMeshComponent->SetStaticMesh(DataSM.DataSMPreset[meshIndex].StaticMesh);
		FDataLocation& DataLocation = DataSM.DataSMPreset[meshIndex].DataLocation;
		DataSM.StaticMeshComponent->SetWorldLocation(DataLocation.BaseLocation + DataLocation.OffsetLocation);
		DataSM.StaticMeshComponent->SetWorldRotation(DataLocation.BaseRotation + DataLocation.OffsetRotation);
		DataSM.StaticMeshComponent->SetWorldScale3D(DataLocation.BaseScale + DataLocation.OffsetScale);

		if (DataSM.DataSMPreset[meshIndex].DataMPreset.Num() == 0) continue;
		//Set the material value
		for (int iMat = 0; iMat < DataSM.StaticMeshComponent->GetNumMaterials(); iMat++)
		{
			if(iMat >= DataSM.DataSMPreset[meshIndex].DataMPreset[materialIndex].Materials.Num()) continue;

			DataSM.StaticMeshComponent->SetMaterial(iMat, DataSM.DataSMPreset[meshIndex].DataMPreset[materialIndex].Materials[iMat]);
		}
	}

	_IndexMeshSelected = meshIndex;
	_IndexMaterialSelected = materialIndex;
}

//void AInteractableMM::OnSelectMesh(int index)
//{
	//UWorld* world = GetWorld();
	//if (!world) return;
	//APlayerController* playerController = world->GetFirstPlayerController();
	//if (!playerController) return;

	////Save the current mesh index
	//_CurrMeshIndex = index;

	//if (!_EditMaterialWidget) return;

	//UScrollBox* ScrollBoxMaterial = Cast<UScrollBox>(_EditMaterialWidget->GetWidgetFromName(TEXT("ScrollBox_Material")));
	//_MaterialButtonAction.Empty();
	//if (ScrollBoxMaterial)
	//{
	//	ScrollBoxMaterial->ClearChildren();
	//	for (int i = 0; i < _DataPreset[index].NumberOfMaterial; i++)
	//	{
	//		UUserWidget* Slot = CreateWidget<UUserWidget>(playerController, _EditSlotMaterialWidgetClass);
	//		if (Slot)
	//		{
	//			UButton* ButtonSlot = Cast<UButton>(Slot->GetWidgetFromName("Button_Material"));
	//			if (ButtonSlot)
	//			{
	//				ButtonSlot->WidgetStyle.Normal.SetResourceObject(_DataPreset[index].MaterialThumbnail[i]);
	//				ButtonSlot->WidgetStyle.Hovered.SetResourceObject(_DataPreset[index].MaterialThumbnail[i]);
	//				ButtonSlot->WidgetStyle.Pressed.SetResourceObject(_DataPreset[index].MaterialThumbnail[i]);

	//				UButtonAction* ButtonAction = NewObject<UButtonAction>();
	//				ButtonAction->Initialise(this, i);
	//				ButtonSlot->OnClicked.AddDynamic(ButtonAction, &UButtonAction::ExecuteMaterial);
	//				_MaterialButtonAction.Add(ButtonAction);

	//				UPanelSlot* PanelSlot = ScrollBoxMaterial->AddChild(Slot);
	//				UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
	//				if (ScrollBoxSlot)
	//				{
	//					ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	//				}
	//			}
	//		}
	//	}
	//}

	//OnSelectMaterial(0);

	//if (!_InfoWidget || !_DataAssetCompanyProduct) return;

	//UTextBlock* TextBlockCompanyName = Cast<UTextBlock>(_InfoWidget->GetWidgetFromName(FName("TextBlock_Name")));
	//if (TextBlockCompanyName)
	//{
	//	TextBlockCompanyName->SetText(FText::FromString(_DataPreset[_CurrMeshIndex].CompanyName));
	//}

	//UTextBlock* TextBlockCompanyInfo = Cast<UTextBlock>(_InfoWidget->GetWidgetFromName(FName("TextBlock_CompanyInfo")));
	//if (TextBlockCompanyInfo)
	//{
	//	if (_DataAssetCompanyProduct->CompanyNameText.Contains(_DataPreset[_CurrMeshIndex].CompanyName))
	//	{
	//		TextBlockCompanyInfo->SetText(_DataAssetCompanyProduct->CompanyNameText[_DataPreset[_CurrMeshIndex].CompanyName].CompanyInfo);
	//	}
	//	else
	//	{
	//		TextBlockCompanyInfo->SetText(FText::GetEmpty());
	//	}
	//}

	//UImage* ImageCompanyLogo = Cast<UImage>(_InfoWidget->GetWidgetFromName(FName("Image_Logo")));
	//if (ImageCompanyLogo)
	//{
	//	if (_DataAssetCompanyProduct->CompanyNameText.Contains(_DataPreset[_CurrMeshIndex].CompanyName))
	//	{
	//		ImageCompanyLogo->SetBrushFromTexture(_DataAssetCompanyProduct->CompanyNameText[_DataPreset[_CurrMeshIndex].CompanyName].CompanyLogo);
	//	}
	//	else
	//	{
	//		ImageCompanyLogo->SetBrushFromTexture(nullptr);
	//	}
	//}

	//UScrollBox* ScrollBoxProductInfo = Cast<UScrollBox>(_InfoWidget->GetWidgetFromName(FName("ScrollBox_ProductInfo")));
	//if (ScrollBoxProductInfo)
	//{
	//	ScrollBoxProductInfo->ClearChildren();

	//	UTextBlock* TextBlockProductName = NewObject<UTextBlock>(ScrollBoxProductInfo);
	//	if (TextBlockProductName)
	//	{
	//		TextBlockProductName->SetAutoWrapText(true);
	//		TextBlockProductName->SetFont(_FontInfo);

	//		FString NameString = "Name: " + _DataPreset[_CurrMeshIndex].ProductName;
	//		TextBlockProductName->SetText(FText::FromString(NameString));

	//		UPanelSlot* PanelSlotProductName = ScrollBoxProductInfo->AddChild(TextBlockProductName);
	//		UScrollBoxSlot* ScrollBoxSlotProductName = Cast<UScrollBoxSlot>(PanelSlotProductName);
	//		if (ScrollBoxSlotProductName)
	//		{
	//			ScrollBoxSlotProductName->SetPadding(FMargin(0.0f, 0.0f, 7.0f, 0.0f));
	//		}
	//	}

	//	if (_DataAssetCompanyProduct->ProductNameText.Contains(_DataPreset[_CurrMeshIndex].ProductName))
	//	{
	//		int Count = 1;
	//		for (FString& Text : _DataAssetCompanyProduct->ProductNameText[_DataPreset[_CurrMeshIndex].ProductName].ProductInfo)
	//		{
	//			UTextBlock* TextBlock = NewObject<UTextBlock>(ScrollBoxProductInfo);
	//			if (TextBlock)
	//			{
	//				TextBlock->SetAutoWrapText(true);
	//				TextBlock->SetFont(_FontInfo);

	//				FString InfoString = FString::Printf(TEXT("Info %d: "), Count);
	//				InfoString += Text;
	//				TextBlock->SetText(FText::FromString(InfoString));

	//				UPanelSlot* PanelSlot = ScrollBoxProductInfo->AddChild(TextBlock);
	//				UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
	//				if (ScrollBoxSlot)
	//				{
	//					ScrollBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 7.0f, 0.0f));
	//				}

	//				Count++;
	//			}
	//		}
	//	}
	//}
//}

//void AInteractableMM::InitEditUI()
//{
	//if (_MainWidget)
	//{
	//	UFunction* Event = _MainWidget->FindFunction(FName("EventOpenEditMM"));
	//	if (Event)
	//	{
	//		_MainWidget->ProcessEvent(Event, nullptr);
	//	}
	//}

	//if (_bIsOpen) return;

	//_bIsOpen = true;
	//UGameInstance* GameInstance = GetGameInstance();
	//if (GameInstance)
	//{
	//	UGameInstanceSubsystemVisit* GameInsanceSubsystemVisit = GameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
	//	if (GameInsanceSubsystemVisit)
	//	{
	//		GameInsanceSubsystemVisit->_DelegateCloseOtherMeshMaterial.Broadcast();
	//		GameInsanceSubsystemVisit->_DelegateCloseOtherMeshMaterial.AddDynamic(this, &AInteractableMM::DestroyUI);
	//	}
	//}

	//_MeshButtonAction.Empty();
	//_MaterialButtonAction.Empty();

	//if (_MenuMeshMaterialInfoWidget)
	//{
	//	UFunction* Event = _MenuMeshMaterialInfoWidget->FindFunction(FName("SetActiveWidgetIndex"));
	//	if (Event)
	//	{
	//		_MenuMeshMaterialInfoWidget->ProcessEvent(Event, &_InitialWidgetIndex);
	//	}
	//}

	//UWorld* World = GetWorld();
	//if (!World) return;
	//APlayerController* PlayerController = World->GetFirstPlayerController();
	//if (!PlayerController) return;

	//if (_EditMeshWidget)
	//{
	//	UScrollBox* ScrollBoxMesh = Cast<UScrollBox>(_EditMeshWidget->GetWidgetFromName(TEXT("ScrollBox_Mesh")));
	//	if (ScrollBoxMesh)
	//	{
	//		ScrollBoxMesh->ClearChildren();
	//	}

	//	for (int i = 0; i < _NumberOfPreset; i++)
	//	{
	//		//Create the slot widget
	//		UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _EditSlotMeshWidgetClass);
	//		if (Slot)
	//		{
	//			UButton* ButtonSlot = Cast<UButton>(Slot->GetWidgetFromName("Button_Mesh"));
	//			if (ButtonSlot)
	//			{
	//				ButtonSlot->WidgetStyle.Normal.SetResourceObject(_DataPreset[i].MeshThumbnail);
	//				ButtonSlot->WidgetStyle.Hovered.SetResourceObject(_DataPreset[i].MeshThumbnail);
	//				ButtonSlot->WidgetStyle.Pressed.SetResourceObject(_DataPreset[i].MeshThumbnail);

	//				UButtonAction* ButtonAction = NewObject<UButtonAction>();
	//				ButtonAction->Initialise(this, i);
	//				ButtonSlot->OnClicked.AddDynamic(ButtonAction, &UButtonAction::ExecuteMesh);
	//				_MeshButtonAction.Add(ButtonAction);

	//				//Add the slot widget to scroll box and get the value in to the scroll box and center it
	//				if (ScrollBoxMesh)
	//				{
	//					UPanelSlot* PanelSlot = ScrollBoxMesh->AddChild(Slot);
	//					UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
	//					if (ScrollBoxSlot)
	//					{
	//						ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//OnSelectMesh(_CurrMeshIndex);
	//OnSelectMaterial(_CurrMaterialIndex);
//}

#if WITH_EDITOR
void AInteractableMM::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* Property = PropertyChangedEvent.Property;
	FProperty* MemberProperty = PropertyChangedEvent.MemberProperty;

	if (Property)
	{
		if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(AInteractableMM, _NumberOfPreset))
		{
			OnNumberOfPresetChange();
		}
		else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(AInteractableMM, _DataStaticMesh))
		{
			OnDataStaticMeshArraySizeChange();
		}
	}
}

void AInteractableMM::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedChainEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedChainEvent);

	FProperty* Property = PropertyChangedChainEvent.Property;

	if (Property)
	{
		if (Property->GetName() == TEXT("NumberOfMaterial"))
		{
			int32 index = PropertyChangedChainEvent.GetArrayIndex(PropertyChangedChainEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetName());
			if (index != INDEX_NONE)
			{
				OnNumberOfMaterialChange(index);
			}
		}
	}
}

void AInteractableMM::OnNumberOfPresetChange()
{
	if (!GEditor) return;

	GEditor->BeginTransaction(FText::FromString(TEXT("Number of preset change")));
	this->Modify();

	_DataPreset.SetNum(_NumberOfPreset, true);

	for (FDataSM& DataSM : _DataStaticMesh)
	{
		DataSM.DataSMPreset.SetNum(_NumberOfPreset, true);
	}

	GEditor->EndTransaction();
}

void AInteractableMM::OnDataStaticMeshArraySizeChange()
{
	if (!GEditor) return;

	GEditor->BeginTransaction(FText::FromString(TEXT("Data static mesh array size change")));
	this->Modify();

	for (FDataSM& DataSM : _DataStaticMesh)
	{
		DataSM.DataSMPreset.SetNum(_NumberOfPreset, true);

		int Index = 0;
		for (FDataSMPreset& DataSMPreset : DataSM.DataSMPreset)
		{
			DataSMPreset.DataMPreset.SetNum(_DataPreset[Index].NumberOfMaterial, true);
			Index++;
		}
	}

	GEditor->EndTransaction();
}

void AInteractableMM::OnNumberOfMaterialChange(int index)
{
	if (!GEditor) return;

	GEditor->BeginTransaction(FText::FromString(TEXT("Number of material in preset change")));
	this->Modify();

	for (FDataSM& DataSM : _DataStaticMesh)
	{
		DataSM.DataSMPreset[index].DataMPreset.SetNum(_DataPreset[index].NumberOfMaterial, true);
	}

	_DataPreset[index].MaterialDataSlot.SetNum(_DataPreset[index].NumberOfMaterial, true);

	GEditor->EndTransaction();
}
#endif