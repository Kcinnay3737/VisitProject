#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataVisitorControl.generated.h"

UENUM(BlueprintType)
enum class EInputType : uint8
{
	IT_MouseAndKeyboard		UMETA(DisplayName = "MouseAndKeyboard"),
	IT_Touch				UMETA(DisplayName = "Touch")
};

UCLASS()
class VISITPLUGINRUNTIME_API UDataVisitorControl : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	EInputType inputType;

	UPROPERTY(EditAnywhere)
	FVector2D lookSpeed = FVector2D(1.0f, 1.0f);
	UPROPERTY(EditAnywhere)
	FVector2D mapLookSpeed = FVector2D(3.0f, 3.0f);

	UPROPERTY(EditAnywhere)
	float minFOV = 20.0f;
	UPROPERTY(EditAnywhere)
	float zoomSpeed = 5.0f;
	UPROPERTY(EditAnywhere)
	float mapZoomSpeed = 50.0f;

	UPROPERTY(EditAnywhere)
	float touchDistance = 1000.0f;
	UPROPERTY(EditAnywhere)
	float baseOrthoSizeForMovementScale = 1000.0f;
};
