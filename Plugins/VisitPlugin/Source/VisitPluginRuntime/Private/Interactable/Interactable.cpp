#include "Interactable/Interactable.h"

#include "Components/SceneComponent.h"

AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(_SceneRootComponent);
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractable::Interact()
{

}