#include "Interactable/Teleporter/Teleporter.h"

#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"

ATeleporter::ATeleporter()
{
	PrimaryActorTick.bCanEverTick = true;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);
}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();
	
	for (FTeleportData& TeleportData : _TeleportData)
	{
		TeleportData.TeleportLocation = GetActorTransform().TransformPosition(TeleportData.TeleportLocation);
	}
}

void ATeleporter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATeleporter::OnClickTeleport(int index)
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = world->GetFirstPlayerController();
	if (!playerController) return;
	APawn* pawn = playerController->GetPawn();
	if (!pawn) return;

	FVector worldLocation = _TeleportData[index].TeleportLocation;
	pawn->SetActorLocation(worldLocation);
}

void ATeleporter::OnClickView(int index)
{

}

void ATeleporter::OnClickFloor(int index)
{
}

void ATeleporter::OnButtonIndexClick(FString Type, int Index)
{
	if (Type == "Teleport")
	{
		OnClickTeleport(Index);
	}
	if (Type == "View")
	{
		OnClickView(Index);
	}
	if (Type == "Floor")
	{
		OnClickFloor(Index);
	}
}
