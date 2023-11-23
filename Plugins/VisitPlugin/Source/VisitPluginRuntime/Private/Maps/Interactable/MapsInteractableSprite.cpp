#include "Maps/Interactable/MapsInteractableSprite.h"

#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstance.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

AMapsInteractableSprite::AMapsInteractableSprite() : Super::AMapsInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	_PaperSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PaperSpriteComponent"));
	_PaperSpriteComponent->SetupAttachment(_SceneRootComponent);
	_PaperSpriteComponent->AddWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	_PaperSpriteComponent->SetWorldScale3D(FVector(5.0f, 1.0f, 5.0f));
	_PaperSpriteComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_PaperSpriteComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	UPrimitiveComponent* primitivePaperSpriteComponent = Cast<UPrimitiveComponent>(_PaperSpriteComponent);
	if (primitivePaperSpriteComponent)
	{
		primitivePaperSpriteComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void AMapsInteractableSprite::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsInteractableSprite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapsInteractableSprite::Interact()
{
	Super::Interact();
}

