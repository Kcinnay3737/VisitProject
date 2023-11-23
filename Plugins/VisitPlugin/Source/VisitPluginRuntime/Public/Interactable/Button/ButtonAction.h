#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ButtonAction.generated.h"

class AMeshMaterialChanger;

UCLASS()
class VISITPLUGINRUNTIME_API UButtonAction : public UObject
{
	GENERATED_BODY()
public:
	UButtonAction();

	void Initialise(AMeshMaterialChanger* Interactable, int InCount);

	UFUNCTION()
	void ExecuteMesh();
	UFUNCTION()
	void ExecuteMaterial();

private:
	//Current index in array
	int _Count = 0;
	UPROPERTY()
	AMeshMaterialChanger* _InteractableObject;
};
