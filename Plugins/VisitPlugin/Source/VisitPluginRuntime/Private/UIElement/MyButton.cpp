#include "UIElement/MyButton.h"

bool UMyButton::_bAButtonCurrentlyPressed = false;

void UMyButton::OnButtonPressed()
{
	UMyButton::_bAButtonCurrentlyPressed = true;
}

void UMyButton::OnButtonReleased()
{
	UMyButton::_bAButtonCurrentlyPressed = false;
}

bool UMyButton::GetIsButtonCurrentlyPressed()
{
	return UMyButton::_bAButtonCurrentlyPressed;
}

void UMyButton::Initialise()
{
	OnPressed.AddDynamic(this, &UMyButton::OnButtonPressed);
	OnReleased.AddDynamic(this, &UMyButton::OnButtonReleased);
}
