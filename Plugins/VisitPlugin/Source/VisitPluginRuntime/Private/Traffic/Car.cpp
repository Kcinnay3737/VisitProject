#include "Traffic/Car.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Traffic/DataCar.h"
#include "Traffic/Intersection.h"
#include "Traffic/Road.h"
#include "Traffic/Traffic.h"

ACar::ACar()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponenent"));
	SetRootComponent(_RootSceneComponent);

	_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	_StaticMeshComponent->SetupAttachment(_RootSceneComponent);
	_StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_BoxComponent->SetupAttachment(_StaticMeshComponent);
	_BoxComponent->InitBoxExtent(FVector(0.5f, 0.5f, 0.5f));
	_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	_BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void ACar::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACar::SpawnCar(ARoad* road, float distance, UDataCar* carData)
{
	if (!road || !carData) return;

	_IsDestroy = false;

	_CurrRoad = road;
	_CurrDistance = distance;
	_CarData = carData;

	if (_StaticMeshComponent)
	{
		_StaticMeshComponent->SetStaticMesh(_CarData->dataStaticMesh.staticMesh);

		_StaticMeshComponent->SetActive(true);
		_StaticMeshComponent->SetVisibility(true);
	}

	if (_BoxComponent)
	{
		_BoxComponent->SetRelativeLocation(carData->dataBox.midOffset);
		_BoxComponent->SetWorldScale3D(carData->dataBox.boxScale);
		_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	FVector currSplineLocation = _CurrRoad->GetSplineLocationAtValue(_CurrDistance);
	FRotator currSplineRotation = _CurrRoad->GetSplineRotationAtValue(_CurrDistance);

	FVector localOffsetPostion = currSplineRotation.RotateVector(_CarData->dataStaticMesh.offsetPosition);
	SetActorLocation(currSplineLocation + localOffsetPostion);
	SetActorRotation(currSplineRotation + _CarData->dataStaticMesh.offsetRotation);
}

void ACar::DestroyCar()
{
	if (_CurrIntersection)
	{
		AIntersectionBox* intersectionBox = _CurrIntersection->GetInCarIntersection(this);
		_CurrIntersection->RemoveInCarIntersection(this);
		_CurrIntersection->RemoveAtIntersectionCar(intersectionBox, this);
		_CurrIntersection->RemoveEngagedCar(this);

		ExitIntersection();
	}

	_CurrRoad = nullptr;
	_CurrDistance = 0.0f;
	_CurrLerpSpeedValue = 0.0f;

	if (_StaticMeshComponent)
	{
		_StaticMeshComponent->SetActive(false);
		_StaticMeshComponent->SetVisibility(false);
	}

	if (_BoxComponent)
	{
		_BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	_IsDestroy = true;
}

void ACar::Move(ATraffic* traffic, float deltaTime)
{
	if (!_CurrRoad)
	{
		DestroyCar();
		return;
	}

	if (_bStop)
	{
		Decelerate(deltaTime);
		_CurrStopTime += deltaTime;
		return;
	}
	_CurrStopTime = 0.0f;

	_CurrTimeWait += deltaTime;
	if (_CurrTimeWait < traffic->GetTrafficData().carsAttribut.timeWaitAfterCollision)
	{
		Decelerate(deltaTime);
		return;
	}

	//To optimize
	if (CheckCollision(traffic, deltaTime))
	{
		return;
	}
	
	//to optimize
	if (LookForward(traffic))
	{
		Decelerate(deltaTime);
		_CurrTimeWait = 0.0f;
		return;
	}

	//To optimize
	if (CheckHasOtherRoad(traffic, deltaTime))
	{
		return;
	}

	Accelerate(deltaTime);

	if (traffic->GetTrafficData().drawDebugLine)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			DrawDebugPoint(world, _CurrRoad->GetSplineLocationAtValue(_CurrDistance), 10.0f, FColor::Red);
			if (_BoxComponent)
			{
				DrawDebugBox(world, _BoxComponent->GetCenterOfMass(), _BoxComponent->GetScaledBoxExtent(), _BoxComponent->GetComponentRotation().Quaternion(), FColor::Blue);
			}
		}
	}

	if (_CurrDistance >= _CurrRoad->GetSplineLenght())
	{
		if (EndRoad(traffic))
		{
			return;
		}
	}
}

void ACar::EnterIntersection(AIntersection* intersection)
{
	SetIntersection(intersection);

	_bIsInIntersection = true;
	_bIsChangeRoadDuringIntersection = false;

	SetStop(true);
}

void ACar::ExitIntersection()
{
	SetIntersection(nullptr);

	_bIsInIntersection = false;
	_bIsChangeRoadDuringIntersection = false;
}

void ACar::Accelerate(float deltaTime)
{
	if (!_CurrRoad) return;

	if (!_bAccelerateLastFrame)
	{
		float tempLerp = _CurrLerpSpeedValue / _CurrRoad->GetDataRoad().decelerationTime;
		_CurrLerpSpeedValue = tempLerp * _CurrRoad->GetDataRoad().accelerationTime;
	}
	_bAccelerateLastFrame = true;

	_CurrLerpSpeedValue += deltaTime;
	if (_CurrLerpSpeedValue < 0.0f)
	{
		_CurrLerpSpeedValue = 0.0f;
	}
	if (_CurrLerpSpeedValue > _CurrRoad->GetDataRoad().accelerationTime)
	{
		_CurrLerpSpeedValue = _CurrRoad->GetDataRoad().accelerationTime;
	}

	float tempLerp = _CurrLerpSpeedValue / _CurrRoad->GetDataRoad().accelerationTime;

	ApplyMovement(deltaTime, tempLerp);
}

void ACar::Decelerate(float deltaTime)
{
	if (!_CurrRoad) return;

	if (_bAccelerateLastFrame)
	{
		float tempLerp = _CurrLerpSpeedValue / _CurrRoad->GetDataRoad().accelerationTime;
		_CurrLerpSpeedValue = tempLerp * _CurrRoad->GetDataRoad().decelerationTime;
	}
	_bAccelerateLastFrame = false;

	_CurrLerpSpeedValue -= deltaTime;
	if (_CurrLerpSpeedValue < 0.0f)
	{
		_CurrLerpSpeedValue = 0.0f;
	}
	if (_CurrLerpSpeedValue > _CurrRoad->GetDataRoad().decelerationTime)
	{
		_CurrLerpSpeedValue = _CurrRoad->GetDataRoad().decelerationTime;
	}

	float tempLerp = _CurrLerpSpeedValue / _CurrRoad->GetDataRoad().decelerationTime;

	ApplyMovement(deltaTime, tempLerp);
}

void ACar::ApplyMovement(float deltaTime, float lerpValue)
{
	_CurrDistance += FMath::Lerp<float>(0.0f, _CurrRoad->GetDataRoad().speed, lerpValue) * deltaTime;

	FVector currSplineLocation = _CurrRoad->GetSplineLocationAtValue(_CurrDistance);
	FRotator currSplineRotation = _CurrRoad->GetSplineRotationAtValue(_CurrDistance);

	FVector localOffsetPostion = currSplineRotation.RotateVector(_CarData->dataStaticMesh.offsetPosition);
	SetActorLocation(currSplineLocation + localOffsetPostion);
	SetActorRotation(currSplineRotation + _CarData->dataStaticMesh.offsetRotation);
}

bool ACar::LookForward(ATraffic* traffic)
{
	if (!_CurrRoad) return false;
	UWorld* world = GetWorld();
	if (!world) return false;

	_BlockCar.Empty();

	FVector initialPoint = _CurrRoad->GetSplineLocationAtValue(_CurrDistance);
	FRotator initialRotation = _CurrRoad->GetSplineRotationAtValue(_CurrDistance);
	FVector dir = initialRotation.Vector();
	FVector localOffset;

	TArray<FRayStartEndPoint> rayPositions;

	FRayStartEndPoint rightRay = FRayStartEndPoint();
	localOffset = initialRotation.RotateVector(_CarData->dataRay.rightSideOffsetStartPointLocation);
	rightRay.startPoint = initialPoint + localOffset;
	rightRay.endPoint = rightRay.startPoint + (dir * _CarData->dataRay.rightSideDistanceLooking);

	FRayStartEndPoint leftRay = FRayStartEndPoint();
	FVector leftSideOffsetStartPointLocation = _CarData->dataRay.rightSideOffsetStartPointLocation;
	leftSideOffsetStartPointLocation.Y = -leftSideOffsetStartPointLocation.Y;
	localOffset = initialRotation.RotateVector(leftSideOffsetStartPointLocation);
	leftRay.startPoint = initialPoint + localOffset;
	leftRay.endPoint = leftRay.startPoint + (dir * _CarData->dataRay.rightSideDistanceLooking);

	FRayStartEndPoint forwardRay = FRayStartEndPoint();
	localOffset = initialRotation.RotateVector(_CarData->dataRay.rightSideForwardOffsetStartPointLocation);
	forwardRay.startPoint = initialPoint + localOffset;
	FRotator forwardRotation = initialRotation;
	forwardRotation.Yaw -= 90.0f;
	dir = forwardRotation.Vector();
	forwardRay.endPoint = forwardRay.startPoint + (dir * _CarData->dataRay.rightSideForwardDistanceLooking);

	FRayStartEndPoint cornerRight = FRayStartEndPoint();
	cornerRight.startPoint = rightRay.endPoint;
	cornerRight.endPoint = forwardRay.startPoint;

	FRayStartEndPoint cornerLeft = FRayStartEndPoint();
	cornerLeft.startPoint = leftRay.endPoint;
	cornerLeft.endPoint = forwardRay.endPoint;

	rayPositions.Add(forwardRay);
	if (!rightRay.endPoint.Equals(forwardRay.startPoint))
	{
		rayPositions.Add(cornerRight);
	}
	if (!leftRay.endPoint.Equals(forwardRay.endPoint))
	{
		rayPositions.Add(cornerLeft);
	}
	rayPositions.Add(rightRay);
	rayPositions.Add(leftRay);

	for (const FRayStartEndPoint& rayPosition : rayPositions)
	{
		if (traffic->GetTrafficData().drawDebugLine)
		{
			DrawDebugLine(world, rayPosition.startPoint, rayPosition.endPoint, FColor::Green, false);
		}

		FHitResult hitResult;
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);
		if (world->LineTraceSingleByChannel(hitResult, rayPosition.startPoint, rayPosition.endPoint, ECC_Visibility, traceParams))
		{
			ACar* otherCar = Cast<ACar>(hitResult.GetActor());
			if (otherCar)
			{
				if (!otherCar->CheckIsBlockingByThisCar(this))
				{
					ARoad* road = otherCar->GetCurrRoad();
					if (road)
					{
						if(_CurrRoad->GetDataRoad().roadPriority > road->GetDataRoad().roadPriority) continue;
					}
					_BlockCar.Add(otherCar, otherCar);
				}
			}
		}
	}

	return !_BlockCar.IsEmpty();
}

bool ACar::CheckCollision(ATraffic* traffic, float deltaTime)
{
	if (!_BoxComponent) return false;

	bool collision = false;

	TArray<AActor*> overlappingActor;
	_BoxComponent->GetOverlappingActors(overlappingActor, ACar::StaticClass());
	for (AActor* actor : overlappingActor)
	{
		ACar* car = Cast<ACar>(actor);
		if (!car) continue;

		if (car->GetCurrRoad() == _CurrRoad && FMath::Abs(car->GetCurrDistance() - _CurrDistance) <= traffic->GetTrafficData().carsAttribut.carDistanceOffsetToDelete)
		{
			collision = true;
			DestroyCar();
			return collision;
		}

		if (car->GetCurrRoad() == _CurrRoad && car->GetCurrDistance() > _CurrDistance)
		{
			collision = true;

			if (traffic->GetTrafficData().carsAttribut.carOverlappeBehavior == ECarOverlappeBehavior::destroy)
			{
				_CurrTimeCollision += deltaTime;
				if (_CurrTimeCollision >= traffic->GetTrafficData().carsAttribut.timeOverlappeToDestroyCar)
				{
					_CurrTimeCollision = 0.0f;
					DestroyCar();
				}
			}
			else if (traffic->GetTrafficData().carsAttribut.carOverlappeBehavior == ECarOverlappeBehavior::decelerate)
			{
				Decelerate(_CurrRoad->GetDataRoad().decelerationTime);
			}

			return collision;
		}
	}

	_CurrTimeCollision = 0.0f;

	return collision;
}

bool ACar::CheckHasOtherRoad(ATraffic* traffic, float deltaTime)
{
	if (!_CurrRoad) return false;

	_DelayChangeRoadAgain += deltaTime;
	if (_DelayChangeRoadAgain < 0.3f) return false;

	if (_bIsChangeRoadDuringIntersection) return false;

	int totalRarity = 0.0f;
	TMap<ARoad*, FMaxMinRarity> roadRarity;

	TArray<FVector> splinePoints = _CurrRoad->GetSplinePointBetweenDistance(_CurrDistance, _CurrDistance + _CurrRoad->GetDataRoad().speed * deltaTime);
	for (FVector splinePoint : splinePoints)
	{
		TArray<ARoad*> roads = traffic->GetRoadsAtThisLocation(splinePoint);
		if (roads.Num() == 0) continue;

		for (ARoad* road : roads)
		{
			if (roadRarity.Contains(road)) continue;

			FMaxMinRarity maxMinRarity;
			maxMinRarity.min = totalRarity;
			totalRarity += road->GetDataRoad().rarity;
			maxMinRarity.max = totalRarity;

			maxMinRarity.splinePoint = splinePoint;

			roadRarity.Add(road, maxMinRarity);
		}
	}

	ARoad* roadToGo = nullptr;
	int randRarity = FMath::RandRange(0, totalRarity);
	for (auto& elem : roadRarity)
	{
		ARoad* road = elem.Key;
		FMaxMinRarity rarity = elem.Value;

		if (randRarity >= rarity.min && randRarity <= rarity.max)
		{
			roadToGo = road;
			break;
		}
	}

	if (roadToGo == nullptr || roadToGo == _CurrRoad) return false;

	int splinePointIndex = roadToGo->GetPointIndexAtLocation(roadRarity[roadToGo].splinePoint);
	float distance = roadToGo->GetSplineLenghtAtPoint(splinePointIndex);
	SpawnCar(roadToGo, distance, _CarData);

	_DelayChangeRoadAgain = 0.0f;

	if (_bIsInIntersection)
	{
		_bIsChangeRoadDuringIntersection = true;
	}

	return true;
}

bool ACar::EndRoad(ATraffic* traffic)
{
	if (!_CurrRoad)
	{
		DestroyCar();
		return true;
	}

	FVector endPoint = _CurrRoad->GetEndSplinePoint();
	TArray<ARoad*> roads = traffic->GetRoadsAtThisLocation(endPoint);
	if (roads.Num() == 0)
	{
		DestroyCar();
		return true;
	}

	int totalRarity = 0.0f;
	TMap<ARoad*, FMaxMinRarity> roadRarity;

	for (ARoad* road : roads)
	{
		if (roadRarity.Contains(road) || road == _CurrRoad) continue;

		FMaxMinRarity maxMinRarity;
		maxMinRarity.min = totalRarity;
		totalRarity += road->GetDataRoad().rarity;
		maxMinRarity.max = totalRarity;

		maxMinRarity.splinePoint = endPoint;

		roadRarity.Add(road, maxMinRarity);
	}

	ARoad* roadToGo = nullptr;
	int randRarity = FMath::RandRange(0, totalRarity);
	for (auto& elem : roadRarity)
	{
		ARoad* road = elem.Key;
		FMaxMinRarity rarity = elem.Value;

		if (randRarity >= rarity.min && randRarity <= rarity.max)
		{
			roadToGo = road;
			break;
		}
	}

	if (roadToGo == nullptr || roadToGo == _CurrRoad)
	{
		DestroyCar();
		return true;
	}

	int splinePointIndex = roadToGo->GetPointIndexAtLocation(roadRarity[roadToGo].splinePoint);
	float distance = roadToGo->GetSplineLenghtAtPoint(splinePointIndex);
	SpawnCar(roadToGo, distance, _CarData);

	return false;
}

bool ACar::GetIsDestroy()
{
	return _IsDestroy;
}

ARoad* ACar::GetCurrRoad()
{
	return _CurrRoad;
}

float ACar::GetCurrDistance()
{
	return _CurrDistance;
}

bool ACar::CheckIsBlockingByThisCar(ACar* car)
{
	return _BlockCar.Contains(car);
}

void ACar::SetStop(bool bValue)
{
	_bStop = bValue;
}

bool ACar::GetStop()
{
	return _bStop;
}

float ACar::GetCurrStopTime()
{
	return _CurrStopTime;
}

void ACar::SetIntersection(AIntersection* intersection)
{
	_CurrIntersection = intersection;
}

void ACar::InitCollision(ATraffic* traffic)
{
	if (traffic->GetTrafficData().carsAttribut.bUseContinuousCollisionDetection)
	{
		_BoxComponent->SetUseCCD(true);
	}
	if (traffic->GetTrafficData().carsAttribut.bTraceComplexOnMove)
	{
		_BoxComponent->bTraceComplexOnMove = true;
	}
}
