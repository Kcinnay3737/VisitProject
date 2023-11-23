#include "Traffic/IntersectionBox.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Traffic/Car.h"
#include "Traffic/Intersection.h"

AIntersectionBox::AIntersectionBox()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);

	_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_BoxComponent->SetupAttachment(_RootSceneComponent);
	_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	_BoxComponent->SetGenerateOverlapEvents(true);

	_BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AIntersectionBox::OnCarBeginOverlappe);
}

void AIntersectionBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIntersectionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIntersectionBox::OnCarBeginOverlappe(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (!otherActor || !_Intersection) return;

	ACar* car = Cast<ACar>(otherActor);
	if (!car) return;

	switch (_Intersection->GetDataIntersection().intersectionType)
	{
	case EIntersectionType::stop:
		OnCarBeginOverlappeStop(car);
		break;
	case EIntersectionType::trafficLight:
		OnCarBeginOverlappeTrafficLight(car);
		break;
	default:
		break;
	}
}

void AIntersectionBox::OnCarBeginOverlappeStop(ACar* otherCar)
{

}

void AIntersectionBox::OnCarBeginOverlappeTrafficLight(ACar* otherCar)
{
	
}

void AIntersectionBox::SetIntersection(AIntersection* intersection)
{
	_Intersection = intersection;
}

TArray<ACar*> AIntersectionBox::GetOverlappingCars()
{
	if(!_BoxComponent) return TArray<ACar*>();

	TArray<AActor*> overlappingActor;
	_BoxComponent->GetOverlappingActors(overlappingActor, ACar::StaticClass());

	TArray<ACar*> overlappingCars;
	for (AActor* actor : overlappingActor)
	{
		ACar* car = Cast<ACar>(actor);
		if (car)
		{
			overlappingCars.Add(car);
		}
	}

	return overlappingCars;
}

void AIntersectionBox::SetCCD(bool state)
{
	_BoxComponent->SetUseCCD(state);
}
