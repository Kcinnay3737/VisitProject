#include "Traffic/IntersectionBoxStart.h"

#include "Traffic/Car.h"
#include "Traffic/Intersection.h"

AIntersectionBoxStart::AIntersectionBoxStart() : Super::AIntersectionBox()
{
}

void AIntersectionBoxStart::OnCarBeginOverlappeStop(ACar* otherCar)
{
	Super::OnCarBeginOverlappeStop(otherCar);

	if (!_Intersection->CheckCarIsEngagedInIntersection(otherCar))
	{
		otherCar->EnterIntersection(_Intersection);

		_Intersection->AddInQueueCar(otherCar);

		_Intersection->AddInCarIntersection(otherCar, this);
		_Intersection->AddInIntersectionCar(this, otherCar);
	}
}

void AIntersectionBoxStart::OnCarBeginOverlappeTrafficLight(ACar* otherCar)
{
	Super::OnCarBeginOverlappeTrafficLight(otherCar);

	if (!_Intersection->CheckCarIsEngagedInIntersection(otherCar))
	{
		otherCar->EnterIntersection(_Intersection);

		if (_TrafficLightColor == ETrafficLightColor::green)
		{
			_Intersection->AddEngagedCar(otherCar);

			otherCar->SetStop(false);
		}
		else if (_TrafficLightColor == ETrafficLightColor::red)
		{
			_Intersection->AddInCarIntersection(otherCar, this);
			_Intersection->AddInIntersectionCar(this, otherCar);
		}
	}
}

void AIntersectionBoxStart::SetTrafficLightColor(ETrafficLightColor trafficLightColor)
{
	_TrafficLightColor = trafficLightColor;

	if (_TrafficLightColor == ETrafficLightColor::green && _Intersection)
	{
		TArray<ACar*> cars = _Intersection->GetInInteractionCar(this);
		for (ACar* car : cars)
		{
			_Intersection->RemoveInCarIntersection(car);
			_Intersection->RemoveAtIntersectionCar(this, car);

			_Intersection->AddEngagedCar(car);

			car->SetStop(false);
		}
	}
}
