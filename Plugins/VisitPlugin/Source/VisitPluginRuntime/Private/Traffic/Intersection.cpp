#include "Traffic/Intersection.h"

#include "Components/SceneComponent.h"
#include "Traffic/Car.h"
#include "Traffic/IntersectionBox.h"
#include "Traffic/IntersectionBoxEnd.h"
#include "Traffic/IntersectionBoxStart.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

AIntersection::AIntersection()
{
	PrimaryActorTick.bCanEverTick = true;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);
}

void AIntersection::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.TickInterval = _DataIntersection.intersectionTickInterval;
	
	InitIntersectionsBox();
	InitTrafficLight();
}

void AIntersection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (_DataIntersection.intersectionType)
	{
	case EIntersectionType::stop:
		StopTick(DeltaTime);
		break;
	case EIntersectionType::trafficLight:
		TrafficLightTick(DeltaTime);
		break;
	default:
		break;
	}
}

void AIntersection::StopTick(float deltaTime)
{
	if (CheckIntersectionIsEngaged()) return;

	if (_StopCarQueue.Num() > 0)
	{
		ACar* firstCar = _StopCarQueue[0];
		if (!firstCar)
		{
			_StopCarQueue.RemoveAt(0);
			return;
		}

		AIntersectionBox* currIntersectionBox = _CarIntersection[firstCar];
		if (!currIntersectionBox)
		{
			_StopCarQueue.RemoveAt(0);
			return;
		}

		if (_DataIntersection.stopCirculationType == EStopCirculationType::oneByOne)
		{
			if (firstCar->GetCurrStopTime() >= _DataIntersection.stopTime)
			{
				RemoveInCarIntersection(firstCar);
				RemoveAtIntersectionCar(currIntersectionBox, firstCar);

				AddEngagedCar(firstCar);

				firstCar->SetStop(false);
			}
		}
		else if (_DataIntersection.stopCirculationType == EStopCirculationType::sideBySide)
		{
			TArray<ACar*> currCars = _IntersectionCar[currIntersectionBox];
			for (ACar* car : currCars)
			{
				if (car->GetCurrStopTime() >= _DataIntersection.stopTime)
				{
					RemoveInCarIntersection(car);
					RemoveAtIntersectionCar(currIntersectionBox, car);

					AddEngagedCar(car);

					car->SetStop(false);
				}
			}
		}
	}
}

void AIntersection::TrafficLightTick(float deltaTime)
{
	if (_IntersectionsBoxStart.Num() == 0) return;
	if (_CurrTrafficLightTime == 0.0f && CheckIntersectionIsEngaged()) return;

	if (_CurrTrafficLightTime == 0.0f)
	{
		_IntersectionsBoxStart[_CurrTrafficLightIndex]->SetTrafficLightColor(ETrafficLightColor::green);
	}

	_CurrTrafficLightTime += deltaTime;
	if (_CurrTrafficLightTime >= _DataIntersection.greenLightTime)
	{
		_CurrTrafficLightTime = 0.0f;

		_IntersectionsBoxStart[_CurrTrafficLightIndex]->SetTrafficLightColor(ETrafficLightColor::red);

		_CurrTrafficLightIndex++;
		if (_CurrTrafficLightIndex >= _IntersectionsBoxStart.Num())
		{
			_CurrTrafficLightIndex = 0;
		}
	}
}

void AIntersection::InitIntersectionsBox()
{
	TArray<AActor*> attachedActor;
	this->GetAttachedActors(attachedActor);
	for (AActor* actor : attachedActor)
	{
		AIntersectionBox* intersectionBox = Cast<AIntersectionBox>(actor);
		if (!intersectionBox) continue;
		intersectionBox->SetIntersection(this);
		intersectionBox->SetCCD(_DataIntersection.bUseContinuousCollisionDetection);

		AIntersectionBoxStart* intersectionBoxStart = Cast<AIntersectionBoxStart>(intersectionBox);
		if (intersectionBoxStart)
		{
			_IntersectionsBoxStart.Add(intersectionBoxStart);
		}
		AIntersectionBoxEnd* intersectionBoxEnd = Cast<AIntersectionBoxEnd>(intersectionBox);
		if (intersectionBoxEnd)
		{
			_IntersectionsBoxEnd.Add(intersectionBoxEnd);
		}
	}
}

void AIntersection::InitTrafficLight()
{
	if (_DataIntersection.intersectionType != EIntersectionType::trafficLight || _IntersectionsBoxStart.Num() == 0) return;

	for (AIntersectionBoxStart* intersectionBoxStart : _IntersectionsBoxStart)
	{
		intersectionBoxStart->SetTrafficLightColor(ETrafficLightColor::red);
	}

	_CurrTrafficLightIndex = FMath::RandRange(0, _IntersectionsBoxStart.Num() - 1);
	_CurrTrafficLightTime = 0.0f;
}

FDataIntersection AIntersection::GetDataIntersection()
{
	return _DataIntersection;
}

void AIntersection::AddInQueueCar(ACar* car)
{
	_StopCarQueue.Add(car);
}

void AIntersection::PopInQueueCar(ACar* car)
{
	if (_StopCarQueue.Num() == 0) return;
	_StopCarQueue.Remove(car);
}

bool AIntersection::CheckCarIsFirstOnQueue(ACar* car)
{
	if (_StopCarQueue.Num() == 0) return false;
	return _StopCarQueue[0] == car;
}

void AIntersection::AddInCarIntersection(ACar* car, AIntersectionBox* intersectionBox)
{
	if (_CarIntersection.Contains(car)) return;
	_CarIntersection.Add(car, intersectionBox);
}

void AIntersection::RemoveInCarIntersection(ACar* car)
{
	if (!_CarIntersection.Contains(car)) return;
	_CarIntersection.Remove(car);
}

AIntersectionBox* AIntersection::GetInCarIntersection(ACar* car)
{
	if (!_CarIntersection.Contains(car)) return nullptr;
	return _CarIntersection[car];
}

void AIntersection::AddInIntersectionCar(AIntersectionBox* intersection, ACar* car)
{
	if (_IntersectionCar.Contains(intersection))
	{
		if (_IntersectionCar[intersection].Contains(car)) return;
		_IntersectionCar[intersection].Add(car);
	}
	else
	{
		_IntersectionCar.Add(intersection);
		_IntersectionCar[intersection].Add(car);
	}
}

void AIntersection::RemoveInIntersectionCar(AIntersectionBox* intersection)
{
	if (!_IntersectionCar.Contains(intersection)) return;
	_IntersectionCar.Remove(intersection);
}

void AIntersection::RemoveAtIntersectionCar(AIntersectionBox* intersection, ACar* car)
{
	if (!_IntersectionCar.Contains(intersection)) return;
	_IntersectionCar[intersection].Remove(car);
}

TArray<ACar*> AIntersection::GetInInteractionCar(AIntersectionBox* intersection)
{
	if(!_IntersectionCar.Contains(intersection)) return TArray<ACar*>();
	return _IntersectionCar[intersection];
}

void AIntersection::AddEngagedCar(ACar* car)
{
	if (_EngagedCarInIntersection.Contains(car)) return;
	_EngagedCarInIntersection.Add(car, car);
}

void AIntersection::RemoveEngagedCar(ACar* car)
{
	if (!_EngagedCarInIntersection.Contains(car)) return;
	_EngagedCarInIntersection.Remove(car);
}

bool AIntersection::CheckCarIsEngagedInIntersection(ACar* car)
{
	return _EngagedCarInIntersection.Contains(car);
}

bool AIntersection::CheckIntersectionIsEngaged()
{
	return !_EngagedCarInIntersection.IsEmpty();
}

#if WITH_EDITOR
void AIntersection::AddStartIntersectionBox()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add an intersection box")));
	this->Modify();
	UWorld* world = GetWorld();
	if (!world) return;
	//-----------------------------------------------------------------

	AIntersectionBoxStart* intersectionBoxStart = world->SpawnActor<AIntersectionBoxStart>();
	if (!intersectionBoxStart) return;
	intersectionBoxStart->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	//-----------------------------------------------------------------
	GEditor->EndTransaction();
}

void AIntersection::AddEndIntersectionBox()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add an intersection box")));
	this->Modify();
	UWorld* world = GetWorld();
	if (!world) return;
	//-----------------------------------------------------------------

	AIntersectionBoxEnd* intersectionBoxEnd = world->SpawnActor<AIntersectionBoxEnd>();
	if (!intersectionBoxEnd) return;
	intersectionBoxEnd->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	//-----------------------------------------------------------------
	GEditor->EndTransaction();
}
#endif