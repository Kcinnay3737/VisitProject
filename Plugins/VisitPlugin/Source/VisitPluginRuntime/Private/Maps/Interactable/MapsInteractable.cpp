#include "Maps/Interactable/MapsInteractable.h"

#include "Components/SceneComponent.h"

AMapsInteractable::AMapsInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);
}

void AMapsInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapsInteractable::Interact()
{
	
}