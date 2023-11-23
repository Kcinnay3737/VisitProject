#include "Maps/View/MapsInfoSprite.h"

#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstance.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

AMapsInfoSprite::AMapsInfoSprite() : Super::AMapsInfo()
{
	PrimaryActorTick.bCanEverTick = false;

	_PaperSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PaperSpriteComponent"));
	_PaperSpriteComponent->SetupAttachment(_SceneRootComponent);
	_PaperSpriteComponent->AddWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	_PaperSpriteComponent->SetWorldScale3D(FVector(5.0f, 1.0f, 5.0f));
	_PaperSpriteComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_PaperSpriteComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	_TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	_TextRenderComponent->SetupAttachment(_SceneRootComponent);
	_TextRenderComponent->AddLocalRotation(FRotator(90.0f, 0.0f, -90.0f));
	_TextRenderComponent->AddLocalOffset(FVector(0.02f, 0.0f, 0.0f));
	_TextRenderComponent->SetTextRenderColor(FColor::Black);
	_TextRenderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_TextRenderComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AMapsInfoSprite::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsInfoSprite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}