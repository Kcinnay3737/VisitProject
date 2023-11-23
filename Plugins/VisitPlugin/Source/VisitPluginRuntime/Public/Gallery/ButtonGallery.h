#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ButtonGallery.generated.h"

class AGallery;

UCLASS()
class VISITPLUGINRUNTIME_API UButtonGallery : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AGallery* _Gallery;

	int _Index = -1;

	void Initialise(AGallery* Gallery, int Index);

	UFUNCTION()
	void Execute();
};
