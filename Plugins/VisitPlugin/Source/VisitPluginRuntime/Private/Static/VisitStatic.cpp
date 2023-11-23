#include "Static/VisitStatic.h"

#include "GameFramework/PlayerController.h"

float UVisitStatic::GetLowerBoundValue(FFloatRange floatRange)
{
	return floatRange.GetLowerBoundValue();
}

float UVisitStatic::GetUpperBoundValue(FFloatRange floatRange)
{
	return floatRange.GetUpperBoundValue();
}

void UVisitStatic::UpdateRotation(APlayerController* playerController, float deltaTime)
{
	playerController->UpdateRotation(deltaTime);
}
