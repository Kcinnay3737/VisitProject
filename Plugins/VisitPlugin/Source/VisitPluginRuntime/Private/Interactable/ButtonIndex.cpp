#include "Interactable/ButtonIndex.h"

void UButtonIndex::Initialise(IButtonIndexReceiver* Receiver, int Index, FString Type)
{
	_ButtonIndexReceiver = Receiver;
	_Index = Index;
	_Type = Type;
}

void UButtonIndex::OnClick()
{
	if (!_ButtonIndexReceiver) return;
	_ButtonIndexReceiver->OnButtonIndexClick(_Type, _Index);
}