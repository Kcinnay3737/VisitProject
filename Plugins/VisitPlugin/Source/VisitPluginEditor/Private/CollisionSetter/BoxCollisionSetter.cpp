#include "CollisionSetter/BoxCollisionSetter.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

ABoxCollisionSetter::ABoxCollisionSetter()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);

	_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	_BoxComponent->SetupAttachment(_RootSceneComponent);
	_BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
}

void ABoxCollisionSetter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoxCollisionSetter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UBoxComponent* ABoxCollisionSetter::GetCollision()
{
	return _BoxComponent;
}

