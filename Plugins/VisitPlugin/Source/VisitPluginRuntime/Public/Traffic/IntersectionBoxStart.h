#pragma once

#include "CoreMinimal.h"
#include "Traffic/IntersectionBox.h"
#include "IntersectionBoxStart.generated.h"

UENUM(BlueprintType)
enum class ETrafficLightColor : uint8
{
	red = 0 UMETA(DisplayName = "Red"),
	green = 1 UMETA(DisplayName = "Green")
};

UCLASS()
class VISITPLUGINRUNTIME_API AIntersectionBoxStart : public AIntersectionBox
{
	GENERATED_BODY()
public:
	AIntersectionBoxStart();

protected:
	virtual void OnCarBeginOverlappeStop(ACar* otherCar) override;
	virtual void OnCarBeginOverlappeTrafficLight(ACar* otherCar) override;

private:
	ETrafficLightColor _TrafficLightColor;

public:
	void SetTrafficLightColor(ETrafficLightColor trafficLightColor);
};
