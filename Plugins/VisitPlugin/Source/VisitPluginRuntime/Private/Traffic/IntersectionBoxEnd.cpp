#include "Traffic/IntersectionBoxEnd.h"

#include "Traffic/Car.h"
#include "Traffic/Intersection.h"

AIntersectionBoxEnd::AIntersectionBoxEnd() : Super::AIntersectionBox()
{
}

void AIntersectionBoxEnd::OnCarBeginOverlappeStop(ACar* otherCar)
{
	Super::OnCarBeginOverlappeStop(otherCar);

	if (_Intersection->CheckCarIsEngagedInIntersection(otherCar))
	{
		otherCar->ExitIntersection();

		_Intersection->PopInQueueCar(otherCar);

		_Intersection->RemoveEngagedCar(otherCar);
	}
}

void AIntersectionBoxEnd::OnCarBeginOverlappeTrafficLight(ACar* otherCar)
{
	Super::OnCarBeginOverlappeTrafficLight(otherCar);

	if (_Intersection->CheckCarIsEngagedInIntersection(otherCar))
	{
		otherCar->ExitIntersection();

		_Intersection->RemoveEngagedCar(otherCar);
	}
}
