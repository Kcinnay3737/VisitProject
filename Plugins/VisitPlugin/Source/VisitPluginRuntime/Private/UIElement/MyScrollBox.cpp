#include "UIElement/MyScrollBox.h"
#include "Character/VisitorController.h"

void UMyScrollBox::UpdateScrollBox(float DeltaTime)
{
	UWorld* World = GetWorld();
	if (!World) return;
	AVisitorController* PlayerController = Cast<AVisitorController>(World->GetFirstPlayerController());
	if (!PlayerController) return;

	if (!PlayerController->GetScrollBoxIsInteractable()) return;

	FVector2D CurrLocation = PlayerController->GetMousePlatformPos();
	
	if (GetCachedGeometry().IsUnderLocation(CurrLocation))
	{
		GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Blue, CurrLocation.ToString());

		if (Orientation == EOrientation::Orient_Horizontal)
		{

		}
		else if (Orientation == EOrientation::Orient_Vertical)
		{

		}
	}
}
