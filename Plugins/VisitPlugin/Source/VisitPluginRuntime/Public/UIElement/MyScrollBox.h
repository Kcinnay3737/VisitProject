#pragma once

#include "CoreMinimal.h"
#include "Components/ScrollBox.h"
#include "MyScrollBox.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API UMyScrollBox : public UScrollBox
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FVector2D _LastMousePosition = FVector2D::ZeroVector;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateScrollBox(float DeltaTime);
};
