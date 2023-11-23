#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Interactable/ButtonIndex.h"
#include "MeshMaterialChanger.generated.h"

class UUserWidget;

UCLASS()
class VISITPLUGINRUNTIME_API AMeshMaterialChanger : public AInteractable, public IButtonIndexReceiver
{
	GENERATED_BODY()
public:
	AMeshMaterialChanger();

	virtual void OnSelectMesh(int index);
	virtual void OnSelectMaterial(int index);

	virtual void OnButtonIndexClick(FString Type, int Index) override;

protected:
	UPROPERTY()
	TArray<UButtonIndex*> _MeshButtonAction;
	UPROPERTY()
	TArray<UButtonIndex*> _MaterialButtonAction;

	UPROPERTY(EditAnywhere, Category = "VisitData/EditMaterialMesh")
	bool _bEditableMaterialMesh = true;

	int _CurrMeshIndex = 0;
	int _CurrMaterialIndex = 0;
	bool _bIsOpen = false;
};
