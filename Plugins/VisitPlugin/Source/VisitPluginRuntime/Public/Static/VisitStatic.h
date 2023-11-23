#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VisitStatic.generated.h"

class AActorHidden;

UCLASS()
class VISITPLUGINRUNTIME_API UVisitStatic : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static float GetLowerBoundValue(FFloatRange floatRange);
	UFUNCTION(BlueprintCallable)
	static float GetUpperBoundValue(FFloatRange floatRange);

	UFUNCTION(BlueprintCallable)
	static void UpdateRotation(APlayerController* playerController, float deltaTime);
};