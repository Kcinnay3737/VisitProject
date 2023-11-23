#include "Traffic/DeadSpawnZone.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

ADeadSpawnZone::ADeadSpawnZone()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);

	_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_BoxComponent->SetupAttachment(_RootSceneComponent);
	_BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ADeadSpawnZone::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADeadSpawnZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ADeadSpawnZone::CheckPointIsOverlapping(FVector point)
{
	if (!_BoxComponent) return false;

	FTransform boxTransform = _BoxComponent->GetComponentTransform();
	FVector boxExtent = _BoxComponent->GetScaledBoxExtent();

	FBox boxBounds = FBox(boxTransform.GetLocation() - boxExtent, boxTransform.GetLocation() + boxExtent);
	return boxBounds.IsInside(point);
}