#include "Traffic/Road.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"

ARoad::ARoad()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);

	_SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	_SplineComponent->SetupAttachment(_RootSceneComponent);
}

void ARoad::BeginPlay()
{
	Super::BeginPlay();
}

void ARoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ARoad::GetStartSplinePoint()
{
	if (!_SplineComponent) return FVector();

	if (_DataRoad.reverseRoad)
	{
		int nbSpline = _SplineComponent->GetNumberOfSplinePoints();
		FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(nbSpline - 1, ESplineCoordinateSpace::World);
		splinePoint = RoundThisVector(splinePoint);
		return splinePoint;
	}
	else
	{
		FVector startSplinePoint = _SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		startSplinePoint = RoundThisVector(startSplinePoint);
		return startSplinePoint;
	}
}

FVector ARoad::GetEndSplinePoint()
{
	if(!_SplineComponent) return FVector();

	if (_DataRoad.reverseRoad)
	{
		FVector startSplinePoint = _SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		startSplinePoint = RoundThisVector(startSplinePoint);
		return startSplinePoint;
	}
	else
	{
		int nbSpline = _SplineComponent->GetNumberOfSplinePoints();
		FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(nbSpline - 1, ESplineCoordinateSpace::World);
		splinePoint = RoundThisVector(splinePoint);
		return splinePoint;
	}
}

FVector ARoad::GetSplineLocationAtValue(float value)
{
	if (!_SplineComponent) return FVector();
	
	if (_DataRoad.reverseRoad)
	{
		float reverseValue = _SplineComponent->GetSplineLength() - value;
		return _SplineComponent->GetLocationAtDistanceAlongSpline(reverseValue, ESplineCoordinateSpace::World);
	}
	else
	{
		return _SplineComponent->GetLocationAtDistanceAlongSpline(value, ESplineCoordinateSpace::World);
	}
}

FRotator ARoad::GetSplineRotationAtValue(float value)
{
	if (!_SplineComponent) return FRotator();
	
	FRotator rotation;

	if (_DataRoad.reverseRoad)
	{
		float reverseValue = _SplineComponent->GetSplineLength() - value;
		rotation = _SplineComponent->GetRotationAtDistanceAlongSpline(reverseValue, ESplineCoordinateSpace::World);
		rotation.Yaw += 180.0f;
		if (rotation.Yaw > 360.0f)
		{
			rotation.Yaw -= 360.0f;
		}

		float temp = rotation.Roll;
		rotation.Roll = rotation.Pitch;
		rotation.Pitch = temp;
	}
	else
	{
		rotation = _SplineComponent->GetRotationAtDistanceAlongSpline(value, ESplineCoordinateSpace::World);
		float temp = rotation.Roll;
		rotation.Roll = -rotation.Pitch;
		rotation.Pitch = temp;
	}

	return rotation;
}

TArray<FVector> ARoad::GetAllSplinePoint()
{
	if (!_SplineComponent) return TArray<FVector>();

	TArray<FVector> splinePoints;

	if (_DataRoad.reverseRoad)
	{
		for (int i = _SplineComponent->GetNumberOfSplinePoints() - 1; i >= 0; i--)
		{
			FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			splinePoint = RoundThisVector(splinePoint);

			splinePoints.Add(splinePoint);
		}
	}
	else
	{
		for (int i = 0; i < _SplineComponent->GetNumberOfSplinePoints(); i++)
		{
			FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
			splinePoint = RoundThisVector(splinePoint);

			splinePoints.Add(splinePoint);
		}
	}

	return splinePoints;
}

TArray<FVector> ARoad::GetSplinePointBetweenDistance(float distance1, float distance2)
{
	if (!_SplineComponent) return TArray<FVector>();

	TArray<FVector> splinePoints;

	if (_DataRoad.reverseRoad)
	{
		for (int i = _SplineComponent->GetNumberOfSplinePoints() - 1; i >= 0 ; i--)
		{
			float distanceAtThisPoint = _SplineComponent->GetDistanceAlongSplineAtSplinePoint(i);
			distanceAtThisPoint = _SplineComponent->GetSplineLength() - distanceAtThisPoint;
			if (distanceAtThisPoint >= distance1 && distanceAtThisPoint <= distance2)
			{
				FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				splinePoint = RoundThisVector(splinePoint);

				splinePoints.Add(splinePoint);
			}
		}
	}
	else
	{
		for (int i = 0; i < _SplineComponent->GetNumberOfSplinePoints(); i++)
		{
			float distanceAtThisPoint = _SplineComponent->GetDistanceAlongSplineAtSplinePoint(i);
			if (distanceAtThisPoint >= distance1 && distanceAtThisPoint <= distance2)
			{
				FVector splinePoint = _SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				splinePoint = RoundThisVector(splinePoint);

				splinePoints.Add(splinePoint);
			}
		}
	}
	
	return splinePoints;
}

float ARoad::GetSplineLenght()
{
	if (!_SplineComponent) return 0.0f;
	return _SplineComponent->GetSplineLength();
}

float ARoad::GetSplineLenghtAtPoint(int pointIndex)
{
	if (!_SplineComponent) return 0.0f;

	if (_DataRoad.reverseRoad)
	{
		int reverseIndex = _SplineComponent->GetNumberOfSplinePoints() - 1;
		reverseIndex -= pointIndex;
		return _SplineComponent->GetSplineLength() - _SplineComponent->GetDistanceAlongSplineAtSplinePoint(reverseIndex);
	}
	else
	{
		return _SplineComponent->GetDistanceAlongSplineAtSplinePoint(pointIndex);
	}
}

int ARoad::GetPointIndexAtLocation(FVector location)
{
	if (!_SplineComponent) return -1;

	TArray<FVector> splinePoints = GetAllSplinePoint();
	int index = 0;
	for (FVector point : splinePoints)
	{
		if (point.Equals(location))
		{
			return index;
		}
		index++;
	}

	return -1;
}

FDataRoad ARoad::GetDataRoad()
{
	return _DataRoad;
}

FVector ARoad::RoundThisVector(FVector vector)
{
	FVector roundVector;
	roundVector.X = FMath::RoundToFloat(vector.X);
	roundVector.Y = FMath::RoundToFloat(vector.Y);
	roundVector.Z = FMath::RoundToFloat(vector.Z);
	return roundVector;
}
