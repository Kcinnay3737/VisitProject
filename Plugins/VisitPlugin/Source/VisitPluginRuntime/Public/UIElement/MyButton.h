#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MyButton.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API UMyButton : public UButton
{
	GENERATED_BODY()
private:
	static bool _bAButtonCurrentlyPressed;

	UFUNCTION()
	void OnButtonPressed();
	UFUNCTION()
	void OnButtonReleased();

public:
	UFUNCTION(BlueprintCallable)
	static bool GetIsButtonCurrentlyPressed();

	UFUNCTION(BlueprintCallable)
	void Initialise();
};
