#include "Gallery/Gallery.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Gallery/ButtonGallery.h"
#include "Kismet/GameplayStatics.h"

AGallery::AGallery()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGallery::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGallery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_GalleryWidget)
	{
		InitWidget();
		if (!_GalleryWidget) return;
	}
}

void AGallery::InitWidget()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, _GalleryWidgetClass, false);
	if (FoundWidgets.Num() != 0)
	{
		_GalleryWidget = FoundWidgets[0];
	}
	if (!_GalleryWidget) return;

	_ImageView = Cast<UImage>(_GalleryWidget->GetWidgetFromName(TEXT("Image_View")));
	if (_ImageView)
	{
		UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(_ImageView->Slot);
		if (ImageSlot)
		{
			_BaseImageWidgetSize = ImageSlot->GetSize();

			_BaseImageWidgetOffsetY = FMath::Abs(ImageSlot->GetPosition().Y) - (ImageSlot->GetSize().Y / 2);
		}	
	}

	_TextBlockImageName = Cast<UTextBlock>(_GalleryWidget->GetWidgetFromName(TEXT("TextBlock_ImageName")));

	UScrollBox* ScrollBox = Cast<UScrollBox>(_GalleryWidget->GetWidgetFromName(TEXT("ScrollBox_Gallery")));
	if (ScrollBox)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PlayerController) return;

		int Count = 0;
		for (FImageValue& ImageValue : _GalleryImage)
		{
			if (!ImageValue.Image) continue;

			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _GallerySlotImageWidgetClass);
			if (!Slot) continue;

			UButton* SlotButton = Cast<UButton>(Slot->GetWidgetFromName(TEXT("Button_SlotGallery")));
			if(!SlotButton) continue;

			FSlateBrush Brush;
			Brush = SlotButton->WidgetStyle.Normal;
			Brush.SetResourceObject(ImageValue.Image);
			SlotButton->WidgetStyle.Normal = Brush;

			Brush = SlotButton->WidgetStyle.Hovered;
			Brush.SetResourceObject(ImageValue.Image);
			SlotButton->WidgetStyle.Hovered = Brush;

			Brush = SlotButton->WidgetStyle.Pressed;
			Brush.SetResourceObject(ImageValue.Image);
			SlotButton->WidgetStyle.Pressed = Brush;

			UButtonGallery* ButtonGallery = NewObject<UButtonGallery>();
			if(!ButtonGallery) continue;
			ButtonGallery->Initialise(this, Count);
			SlotButton->OnClicked.AddDynamic(ButtonGallery, &UButtonGallery::Execute);
			_ButtonGallery.Add(ButtonGallery);

			UPanelSlot* PanelSlot = ScrollBox->AddChild(Slot);
			UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
			if (ScrollBoxSlot)
			{
				
			}

			Count++;
		}
	}
	OnImageSelected(0);
}

void AGallery::OnImageSelected(int Index)
{
	if (!_ImageView || !_GalleryImage.IsValidIndex(Index) || !_GalleryImage[Index].Image) return;

	FVector2D BaseWidgetSize = _BaseImageWidgetSize;
	FVector2D TextureSize = FVector2D(_GalleryImage[Index].Image->GetSizeX(), _GalleryImage[Index].Image->GetSizeY());

	float TextureAspectRatio = TextureSize.X / TextureSize.Y;
	float WidgetAspectRatio = _BaseImageWidgetSize.X / BaseWidgetSize.Y;

	FVector2D CurrWidgetSize;

	if (TextureAspectRatio > WidgetAspectRatio) 
	{
		CurrWidgetSize.X = _BaseImageWidgetSize.X;
		CurrWidgetSize.Y = _BaseImageWidgetSize.X / TextureAspectRatio;
	}
	else
	{
		CurrWidgetSize.Y = _BaseImageWidgetSize.Y;
		CurrWidgetSize.X = _BaseImageWidgetSize.Y * TextureAspectRatio;
	}

	UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(_ImageView->Slot);
	if (ImageSlot)
	{
		ImageSlot->SetSize(CurrWidgetSize);
		ImageSlot->SetPosition(FVector2D(-(CurrWidgetSize.X / 2), -(CurrWidgetSize.Y / 2) - _BaseImageWidgetOffsetY));
	}

	_ImageView->SetBrushFromTexture(_GalleryImage[Index].Image, false);
	
	if (!_TextBlockImageName) return;
	_TextBlockImageName->SetText(FText::FromString(_GalleryImage[Index].ImageName));
}