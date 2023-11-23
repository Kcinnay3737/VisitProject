#include "Character/MenuPawn.h"

AMenuPawn::AMenuPawn()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}