#pragma once

#include "CoreMinimal.h"
#include "Traffic/IntersectionBox.h"
#include "IntersectionBoxEnd.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API AIntersectionBoxEnd : public AIntersectionBox
{
	GENERATED_BODY()
public:
	AIntersectionBoxEnd();

protected:
	virtual void OnCarBeginOverlappeStop(ACar* otherCar) override;
	virtual void OnCarBeginOverlappeTrafficLight(ACar* otherCar) override;

private:
	
};
