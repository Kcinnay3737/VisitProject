#include "Traffic/Traffic.h"

#include "Components/SceneComponent.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Math/UnrealMathUtility.h"
#include "Traffic/Car.h"
#include "Traffic/DeadSpawnZone.h"
#include "Traffic/Intersection.h"
#include "Traffic/Road.h"
#include "Traffic/TrafficParent.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

ATraffic::ATraffic()
{
	PrimaryActorTick.bCanEverTick = true;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);
}

void ATraffic::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.TickInterval = _TrafficData.carMoveTimeInterval;
	
	InitDeadSpawnZone();
	InitRoad();
	InitCar();
}

void ATraffic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (ACar* car : _Cars)
	{
		if (car->GetIsDestroy())
		{
			if (_TrafficData.startRoad.Num() == 0 || _TrafficData.carsData.Num() == 0) continue;

			int randIndexRoad = FMath::RandRange(0, _TrafficData.startRoad.Num() - 1);
			int randIndexCar = FMath::RandRange(0, _TrafficData.carsData.Num() - 1);

			FVector spawnPoint = _TrafficData.startRoad[randIndexRoad]->GetSplineLocationAtValue(0.0f);
			bool bOverlapping = false;
			for (ADeadSpawnZone* deadSpawnZone : _DeadSpawnZones)
			{
				if (deadSpawnZone->CheckPointIsOverlapping(spawnPoint))
				{
					bOverlapping = true;
					break;
				}
			}
			if (bOverlapping)
			{
				car->DestroyCar();
				continue;
			}

			car->SpawnCar(_TrafficData.startRoad[randIndexRoad], 0.0f, _TrafficData.carsData[randIndexCar]);
		}

		car->Move(this, DeltaTime);
	}
}

void ATraffic::InitDeadSpawnZone()
{
	if (!_DeadSpawnZoneParent) return;

	TArray<AActor*> attachedActor;
	_DeadSpawnZoneParent->GetAttachedActors(attachedActor);
	for (AActor* actor : attachedActor)
	{
		ADeadSpawnZone* deadSpawnZone = Cast<ADeadSpawnZone>(actor);
		if(!deadSpawnZone) continue;
		_DeadSpawnZones.Add(deadSpawnZone);
	}
}

void ATraffic::InitRoad()
{
	if (!_RoadParent) return;
	TArray<AActor*> attachedActor;
	_RoadParent->GetAttachedActors(attachedActor);
	for (AActor* actor : attachedActor)
	{
		ARoad* road = Cast<ARoad>(actor);
		if (!road) continue;
		_Roads.Add(road);
	}

	for (ARoad* road : _Roads)
	{
		TArray<FVector> allPoint = road->GetAllSplinePoint();
		if(allPoint.Num() == 0) continue;

		FVector endPoint = allPoint[allPoint.Num() - 1];

		for (FVector point : allPoint)
		{	
			if (point.Equals(endPoint)) continue;

			if (!_RoadsAtPoint.Contains(point))
			{
				_RoadsAtPoint.Add(point);
			}
			_RoadsAtPoint[point].Add(road);
		}
	}
}

void ATraffic::InitCar()
{
	UWorld* world = GetWorld();
	if (!world) return;

	if (!_CarParent)
	{
		_CarParent = world->SpawnActor<ATrafficParent>();
		if (!_CarParent) return;
		_CarParent->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	for (int i = 0; i < _TrafficData.trafficDensity; i++)
	{
		ACar* car = world->SpawnActor<ACar>();
		if(!car) continue;
		car->AttachToActor(_CarParent, FAttachmentTransformRules::KeepRelativeTransform);
		_Cars.Add(car);
	}

	for (ACar* car : _Cars)
	{
		if (_Roads.Num() == 0 || _TrafficData.carsData.Num() == 0) continue;

		int randIndexRoad = FMath::RandRange(0, _Roads.Num() - 1);
		float randDistance = FMath::RandRange(0.0f, _Roads[randIndexRoad]->GetSplineLenght());
		
		FVector spawnPoint = _Roads[randIndexRoad]->GetSplineLocationAtValue(randDistance);

		bool bOverlapping = false;
		for (ADeadSpawnZone* deadSpawnZone : _DeadSpawnZones)
		{
			if (deadSpawnZone->CheckPointIsOverlapping(spawnPoint))
			{
				bOverlapping = true;
				break;
			}
		}
		if (bOverlapping)
		{
			car->DestroyCar();
			continue;
		}

		int randIndexCar = FMath::RandRange(0, _TrafficData.carsData.Num() - 1);

		car->SpawnCar(_Roads[randIndexRoad], randDistance, _TrafficData.carsData[randIndexCar]);
	}
}

TArray<ARoad*> ATraffic::GetRoadsAtThisLocation(FVector location)
{
	if(!_RoadsAtPoint.Contains(location)) return TArray<ARoad*>();
	return _RoadsAtPoint[location];
}

FTrafficData ATraffic::GetTrafficData()
{
	return _TrafficData;
}

#if WITH_EDITOR
void ATraffic::AddRoad()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add a road")));
	this->Modify();
	UWorld* world = GetWorld();
	if (!world) return;
	//--------------------------------------------------------------

	if (!_RoadParent)
	{
		_RoadParent = world->SpawnActor<ATrafficParent>();
		if (!_RoadParent) return;
		_RoadParent->SetActorLabel(TEXT("RoadParent"));
		_RoadParent->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	ARoad* road = world->SpawnActor<ARoad>();
	if (!road) return;
	road->AttachToActor(_RoadParent, FAttachmentTransformRules::KeepRelativeTransform);

	//--------------------------------------------------------------
	GEditor->EndTransaction();
}

void ATraffic::AddIntersection()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add an intersection")));
	this->Modify();
	UWorld* world = GetWorld();
	if (!world) return;
	//--------------------------------------------------------------

	if (!_IntersectionParent)
	{
		_IntersectionParent = world->SpawnActor<ATrafficParent>();
		if (!_IntersectionParent) return;
		_IntersectionParent->SetActorLabel(TEXT("IntersectionParent"));
		_IntersectionParent->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	AIntersection* intersection = world->SpawnActor<AIntersection>();
	if (!intersection) return;
	intersection->AttachToActor(_IntersectionParent, FAttachmentTransformRules::KeepRelativeTransform);

	//--------------------------------------------------------------
	GEditor->EndTransaction();
}

void ATraffic::AddDeadSpawnZone()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add a dead spawn zone")));
	this->Modify();
	UWorld* world = GetWorld();
	if (!world) return;
	//--------------------------------------------------------------

	if (!_DeadSpawnZoneParent)
	{
		_DeadSpawnZoneParent = world->SpawnActor<ATrafficParent>();
		if (!_DeadSpawnZoneParent) return;
		_DeadSpawnZoneParent->SetActorLabel(TEXT("DeadSpawnZoneParent"));
		_DeadSpawnZoneParent->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}

	ADeadSpawnZone* deadSpawnZone = world->SpawnActor<ADeadSpawnZone>();
	if (!deadSpawnZone) return;
	deadSpawnZone->AttachToActor(_DeadSpawnZoneParent, FAttachmentTransformRules::KeepRelativeTransform);

	//--------------------------------------------------------------
	GEditor->EndTransaction();
}
#endif