#include "UIElement/MyScrollBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/SWidget.h"
#include "Components/ScrollBoxSlot.h"

void UMyScrollBox::UpdateScrollBox(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = Cast<APlayerController>(World->GetFirstPlayerController());
	if (!PlayerController) return;

	FVector2D MouseLocation = FVector2D::ZeroVector;
	bool bMousePressed = false;

	/*bool bButtonPressed = UMyButton::GetIsButtonCurrentlyPressed();*/

	if (PlayerController->IsInputKeyDown(EKeys::LeftMouseButton)/* || bButtonPressed*/)
	{
		MouseLocation = UWidgetLayoutLibrary::GetMousePositionOnPlatform();
		bMousePressed = true;
	}
	else
	{
		for (int32 i = 0; i < 1; i++)
		{
			PlayerController->GetInputTouchState(static_cast<ETouchIndex::Type>(i), MouseLocation.X, MouseLocation.Y, bMousePressed);
		}
	}

	if (bMousePressed && GetCachedGeometry().IsUnderLocation(MouseLocation))
	{
		if (!_bLastMousePositionIsSet)
		{
			_LastMousePosition = MouseLocation;
			_bLastMousePositionIsSet = true;
		}

		float Distance = 0;
		if (Orientation == EOrientation::Orient_Horizontal)
		{
			Distance = _LastMousePosition.X - MouseLocation.X;
		}
		else if (Orientation == EOrientation::Orient_Vertical)
		{
			Distance = _LastMousePosition.Y - MouseLocation.Y;
		}

		float CurrentOffset = GetScrollOffset();
		CurrentOffset += Distance * _ScrollSpeed * DeltaTime;
		SetScrollOffset(FMath::Clamp(CurrentOffset, 0.0f, GetScrollOffsetOfEnd()));

		_LastMousePosition = MouseLocation;
	}
	else
	{
		_bLastMousePositionIsSet = false;
	}
}

void UMyScrollBox::UpdateSpacing()
{
	int32 NumChildren = GetChildrenCount();
	for (int32 i = 1; i < NumChildren; i++)
	{
		UWidget* ChildWidget = GetChildAt(i);
		if (ChildWidget)
		{
			UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(ChildWidget->Slot);
			if (ScrollBoxSlot)
			{
				FMargin NewPadding = Orientation == EOrientation::Orient_Horizontal ? FMargin(_SpaceBetweenElement, 0.0f, 0.0f, 0.0f) : FMargin(0.0f, _SpaceBetweenElement, 0.0f, 0.0f);
				ScrollBoxSlot->SetPadding(NewPadding);
			}
		}
	}
}

void UMyScrollBox::OnSlotAdded(UPanelSlot* AddedSlot)
{
	Super::OnSlotAdded(AddedSlot);
	UpdateSpacing();
}

void UMyScrollBox::OnSlotRemoved(UPanelSlot* RemovedSlot)
{
	Super::OnSlotRemoved(RemovedSlot);
	UpdateSpacing();
}

#if WITH_EDITOR
void UMyScrollBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FProperty* Property = PropertyChangedEvent.Property;
	if (Property)
	{
		if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMyScrollBox, _SpaceBetweenElement)
			|| Property->GetFName() == GET_MEMBER_NAME_CHECKED(UMyScrollBox, Orientation))
		{
			UpdateSpacing();
		}	
	}
}
#endif