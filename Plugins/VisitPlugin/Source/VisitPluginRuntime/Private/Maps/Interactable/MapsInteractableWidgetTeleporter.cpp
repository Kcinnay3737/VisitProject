#include "Maps/Interactable/MapsInteractableWidgetTeleporter.h"

#include "Kismet/GameplayStatics.h"

AMapsInteractableWidgetTeleporter::AMapsInteractableWidgetTeleporter() : Super::AMapsInteractableWidget()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMapsInteractableWidgetTeleporter::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsInteractableWidgetTeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapsInteractableWidgetTeleporter::Interact()
{
	Super::Interact();

	if (_bTeleportOnClick)
	{
		Teleport();
	}
}

void AMapsInteractableWidgetTeleporter::Teleport()
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	APawn* pawn = playerController->GetPawn();
	if (!pawn) return;

	pawn->SetActorLocation(GetActorLocation());
}