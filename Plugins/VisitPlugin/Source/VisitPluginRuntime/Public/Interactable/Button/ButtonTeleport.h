#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ButtonTeleport.generated.h"

class ATeleporter;

UCLASS()
class VISITPLUGINRUNTIME_API UButtonTeleport : public UObject
{
	GENERATED_BODY()
public:
	void Initialise(ATeleporter* teleporter, int index);

	UFUNCTION()
	void OnClickTeleport();
	UFUNCTION()
	void OnClickView();

private:
	int _Count;
	UPROPERTY()
	ATeleporter* _Teleporter;
};
