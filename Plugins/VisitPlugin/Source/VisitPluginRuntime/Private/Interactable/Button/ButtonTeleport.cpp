#include "Interactable/Button/ButtonTeleport.h"

#include "Interactable/Teleporter/Teleporter.h"

void UButtonTeleport::Initialise(ATeleporter* teleporter, int index)
{
	_Count = index;
	_Teleporter = teleporter;
}

void UButtonTeleport::OnClickTeleport()
{
	if (!_Teleporter) return;
	_Teleporter->OnClickTeleport(_Count);
}

void UButtonTeleport::OnClickView()
{
	if (!_Teleporter) return;
	_Teleporter->OnClickView(_Count);
}