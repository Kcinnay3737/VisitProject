#include "Maps/View/MapsImageWidget.h"

#include "Components/WidgetComponent.h"

AMapsImageWidget::AMapsImageWidget() : Super::AMapsImage()
{
	PrimaryActorTick.bCanEverTick = false;

	_WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	_WidgetComponent->SetupAttachment(_SceneRootComponent);
	_WidgetComponent->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	_WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AMapsImageWidget::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsImageWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

