#include "Maps/View/MapsImageSprite.h"

#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

AMapsImageSprite::AMapsImageSprite() : Super::AMapsImage()
{
	PrimaryActorTick.bCanEverTick = false;

	_PaperSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
	_PaperSpriteComponent->SetupAttachment(_SceneRootComponent);
	_PaperSpriteComponent->AddWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	_PaperSpriteComponent->SetWorldScale3D(FVector(5.0f, 1.0f, 5.0f));
	_PaperSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_PaperSpriteComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AMapsImageSprite::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMapsImageSprite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

