#include "Traffic/TrafficParent.h"

#include "Components/SceneComponent.h"

ATrafficParent::ATrafficParent()
{
	PrimaryActorTick.bCanEverTick = true;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);
}

void ATrafficParent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrafficParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

