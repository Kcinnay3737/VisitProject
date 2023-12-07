#include "UIElement/MyScrollBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
//#include "UIElement/MyButton.h"

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
