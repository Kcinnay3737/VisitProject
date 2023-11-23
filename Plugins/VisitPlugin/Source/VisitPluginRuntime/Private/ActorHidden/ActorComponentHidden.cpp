#include "ActorHidden/ActorComponentHidden.h"

#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

AActorComponentHidden::AActorComponentHidden()
{
	PrimaryActorTick.bCanEverTick = false;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);
}

void AActorComponentHidden::BeginPlay()
{
	Super::BeginPlay();
}

void AActorComponentHidden::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorComponentHidden::HideAll()
{
	for (AActor* actor : _Actors)
	{
		for (FName tag : _HiddenComponentsTag)
		{
			TArray<UActorComponent*> actorComponents = actor->GetComponentsByTag(UActorComponent::StaticClass(), tag);
			for (UActorComponent* actorComponent : actorComponents)
			{
				UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(actorComponent);
				if (primitiveComponent)
				{
					primitiveComponent->SetActive(false);
					primitiveComponent->SetVisibility(false);
				}
			}
		}
	}
}

void AActorComponentHidden::UnhideAll()
{
	for (AActor* actor : _Actors)
	{
		for (FName tag : _HiddenComponentsTag)
		{
			TArray<UActorComponent*> actorComponents = actor->GetComponentsByTag(UActorComponent::StaticClass(), tag);
			for (UActorComponent* actorComponent : actorComponents)
			{
				UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(actorComponent);
				if (primitiveComponent)
				{
					primitiveComponent->SetActive(true);
					primitiveComponent->SetVisibility(true);
				}
			}
		}
	}
}