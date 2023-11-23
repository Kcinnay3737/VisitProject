#include "Tooltip/TooltipSprite.h"

#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstance.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"

ATooltipSprite::ATooltipSprite() : Super::ATooltip()
{
	PrimaryActorTick.bCanEverTick = true;

	_PaperSpriteComponentClose = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PaperSpriteComponentClose"));
	_PaperSpriteComponentClose->SetupAttachment(_SceneComponentTooltipClose);
	_PaperSpriteComponentClose->AddWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
	_PaperSpriteComponentClose->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_PaperSpriteComponentClose->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	_TextRenderComponentClose = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentClose"));
	_TextRenderComponentClose->SetupAttachment(_SceneComponentTooltipClose);
	_TextRenderComponentClose->SetTextRenderColor(FColor::Black);
	_TextRenderComponentClose->AddLocalOffset(FVector(0.02f, 0.0f, 0.0f));
	_TextRenderComponentClose->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_TextRenderComponentClose->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	_PaperSpriteComponentOpen = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("PaperSpriteComponentOpen"));
	_PaperSpriteComponentOpen->SetupAttachment(_SceneComponentTooltipOpen);
	_PaperSpriteComponentOpen->AddWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
	_PaperSpriteComponentOpen->SetWorldScale3D(FVector(5.0f, 1.0f, 5.0f));
	_PaperSpriteComponentOpen->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_PaperSpriteComponentOpen->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	_TextRenderComponentOpen = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponentOpen"));
	_TextRenderComponentOpen->SetupAttachment(_SceneComponentTooltipOpen);
	_TextRenderComponentOpen->SetTextRenderColor(FColor::Black);
	_TextRenderComponentOpen->AddLocalOffset(FVector(0.02f, 0.0f, 0.0f));
	_TextRenderComponentOpen->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_TextRenderComponentOpen->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ATooltipSprite::BeginPlay()
{
	Super::BeginPlay();
}

void ATooltipSprite::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
