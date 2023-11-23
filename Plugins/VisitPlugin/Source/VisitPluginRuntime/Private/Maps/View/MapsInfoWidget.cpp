#include "Maps/View/MapsInfoWidget.h"

#include "Components/WidgetComponent.h"

AMapsInfoWidget::AMapsInfoWidget() : Super::AMapsInfo()
{
	PrimaryActorTick.bCanEverTick = false;

	_WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	_WidgetComponent->SetupAttachment(_SceneRootComponent);
	_WidgetComponent->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	_WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AMapsInfoWidget::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsInfoWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}