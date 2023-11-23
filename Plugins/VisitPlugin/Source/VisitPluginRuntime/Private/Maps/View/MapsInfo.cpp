#include "Maps/View/MapsInfo.h"

#include "Components/SceneComponent.h"

AMapsInfo::AMapsInfo()
{
	PrimaryActorTick.bCanEverTick = false;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);
}

void AMapsInfo::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

