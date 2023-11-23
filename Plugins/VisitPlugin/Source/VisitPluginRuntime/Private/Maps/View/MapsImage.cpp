#include "Maps/View/MapsImage.h"

#include "Components/SceneComponent.h"

AMapsImage::AMapsImage()
{
	PrimaryActorTick.bCanEverTick = false;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);
}

void AMapsImage::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsImage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}