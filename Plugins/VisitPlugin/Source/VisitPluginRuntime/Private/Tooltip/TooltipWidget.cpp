#include "Tooltip/TooltipWidget.h"

#include "Components/WidgetComponent.h"

ATooltipWidget::ATooltipWidget() : Super::ATooltip()
{
	PrimaryActorTick.bCanEverTick = true;

	_WidgetComponentClose = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponentClose"));
	_WidgetComponentClose->SetupAttachment(_SceneComponentTooltipClose);
	_WidgetComponentClose->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_WidgetComponentClose->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	_WidgetComponentOpen = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponentOpen"));
	_WidgetComponentOpen->SetupAttachment(_SceneComponentTooltipOpen);
	_WidgetComponentOpen->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_WidgetComponentOpen->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void ATooltipWidget::BeginPlay()
{
	Super::BeginPlay();
}

void ATooltipWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
