#pragma once

#include "CoreMinimal.h"

class VISITPLUGINRUNTIME_API IButtonIndexReceiver
{
public:
	IButtonIndexReceiver();
	virtual ~IButtonIndexReceiver();

	virtual void OnButtonIndexClick(FString Type, int Index) = 0;
};
