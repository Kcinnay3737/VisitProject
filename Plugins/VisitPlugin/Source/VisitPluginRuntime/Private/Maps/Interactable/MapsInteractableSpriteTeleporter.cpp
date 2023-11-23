#include "Maps/Interactable/MapsInteractableSpriteTeleporter.h"

#include "Kismet/GameplayStatics.h"

AMapsInteractableSpriteTeleporter::AMapsInteractableSpriteTeleporter() : Super::AMapsInteractableSprite()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMapsInteractableSpriteTeleporter::BeginPlay()
{
	Super::BeginPlay();

	_TeleportLocation = GetActorTransform().TransformPosition(_TeleportLocation);
}

void AMapsInteractableSpriteTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapsInteractableSpriteTeleporter::Interact()
{
	Super::Interact();

	if (_bTeleportOnClick)
	{
		Teleport();
	}
}

void AMapsInteractableSpriteTeleporter::Teleport()
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	APawn* pawn = playerController->GetPawn();
	if (!pawn) return;

	pawn->SetActorLocation(_TeleportLocation);
}