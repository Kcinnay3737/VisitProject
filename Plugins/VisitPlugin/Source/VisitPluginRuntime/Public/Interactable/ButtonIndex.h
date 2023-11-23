#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interactable/IButtonIndexReceiver.h"
#include "ButtonIndex.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API UButtonIndex : public UObject
{
	GENERATED_BODY()
private:
	IButtonIndexReceiver* _ButtonIndexReceiver = nullptr;
	int _Index = 0;
	FString _Type = "";

public:
	void Initialise(IButtonIndexReceiver* Receiver, int Index, FString Type);

	UFUNCTION()
	void OnClick();
};
