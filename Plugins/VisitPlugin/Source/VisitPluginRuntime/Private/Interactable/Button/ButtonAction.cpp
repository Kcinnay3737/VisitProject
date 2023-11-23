#include "Interactable/Button/ButtonAction.h"

#include "Interactable/MeshMaterialChanger.h"

UButtonAction::UButtonAction()
{
}

void UButtonAction::Initialise(AMeshMaterialChanger* interactable, int inCount)
{
	_Count = inCount;
	_InteractableObject = interactable;
}

void UButtonAction::ExecuteMesh()
{
	_InteractableObject->OnSelectMesh(_Count);
}

void UButtonAction::ExecuteMaterial()
{
	_InteractableObject->OnSelectMaterial(_Count);
}