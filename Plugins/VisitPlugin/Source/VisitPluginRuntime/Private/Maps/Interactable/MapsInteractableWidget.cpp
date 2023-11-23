#include "Maps/Interactable/MapsInteractableWidget.h"

#include "Components/WidgetComponent.h"

AMapsInteractableWidget::AMapsInteractableWidget() : Super::AMapsInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	_WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	_WidgetComponent->SetupAttachment(_SceneRootComponent);
	_WidgetComponent->SetWorldRotation(FRotator(90.0f, 0.0f, 0.0f));
	_WidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	_WidgetComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	_WidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AMapsInteractableWidget::BeginPlay()
{
	Super::BeginPlay();

}

void AMapsInteractableWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapsInteractableWidget::Interact()
{
	Super::Interact();
}

