#include "CollisionSetter/CollisionSetter.h"

#include "CollisionSetter/BoxCollisionSetter.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "MeshDescription.h"

#include "StaticMeshEditorSubsystem.h"

#define LOCTEXT_NAMESPACE "CollisionSetter"

ACollisionSetter::ACollisionSetter()
{
	PrimaryActorTick.bCanEverTick = false;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);
}

void ACollisionSetter::BeginPlay()
{
	Super::BeginPlay();
}

void ACollisionSetter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACollisionSetter::AddBoxCollider()
{
	if (!GEditor) return;
	GEditor->BeginTransaction(FText::FromString(TEXT("Add an collision setter box")));
	this->Modify();

	UWorld* World = GetWorld();
	if (!World) return;

	ABoxCollisionSetter* BoxColliderSetter = World->SpawnActor<ABoxCollisionSetter>();
	if (!BoxColliderSetter) return;
	BoxColliderSetter->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	GEditor->EndTransaction();
}

TArray<UBoxComponent*> ACollisionSetter::GetCollision()
{
	TArray<UBoxComponent*> BoxComponents;

	TArray<AActor*> AttachedActor;
	this->GetAttachedActors(AttachedActor);

	for (AActor* Actor : AttachedActor)
	{
		ABoxCollisionSetter* BoxCollisionSetter = Cast<ABoxCollisionSetter>(Actor);
		if (!BoxCollisionSetter) continue;
		BoxComponents.Add(BoxCollisionSetter->GetCollision());
	}

	return BoxComponents;
}

void ACollisionSetter::SetCollision()
{
	_PassedStaticMesh.Empty();
	for (TActorIterator<AActor> actorItr(GetWorld()); actorItr; ++actorItr)
	{
		//Get the current actor
		AActor* actor = *actorItr;
		if (actor)
		{
			//If ignore actor class continue
			for (TSubclassOf<AActor>& ActorClass : _IgnoreClass)
			{
				if (actor->IsA(ActorClass))
				{
					continue;
				}
			}

			//If ignore start label fit return
			bool matches = false;
			FString actorLabel = actor->GetActorLabel();
			for (FString ignoreStartLabel : _StartLabelIgnoreActor)
			{
				if (actorLabel.StartsWith(ignoreStartLabel))
				{
					matches = true;
					break;
				}
			}
			if (matches)
			{
				continue;
			}

			//If ignore actor continue
			if (_IgnoreActor.Contains(actor)) continue;

			//Get all static mesh component
			TArray<UStaticMeshComponent*> staticMeshComponents;
			actor->GetComponents<UStaticMeshComponent>(staticMeshComponents);
			//For all static mesh component
			for (UStaticMeshComponent* staticMeshComponent : staticMeshComponents)
			{
				if (staticMeshComponent)
				{
					//If no all scene
					if (!_AllScene)
					{
						//Check if the mesh is in a box
						bool bIsIntersect = false;
						TArray<UBoxComponent*> BoxColliders = GetCollision();
						for (UBoxComponent* boxCollider : BoxColliders)
						{
							if (boxCollider)
							{
								FBox meshComponentBox = staticMeshComponent->CalcBounds(staticMeshComponent->GetComponentTransform()).GetBox();
								FBox colliderBox = boxCollider->CalcBounds(boxCollider->GetComponentTransform()).GetBox();
								if (meshComponentBox.Intersect(colliderBox))
								{
									bIsIntersect = true;
									break;
								}
							}
						}
						if (!bIsIntersect)
						{
							continue;
						}
					}

					//Get static mesh
					UStaticMesh* staticMesh = staticMeshComponent->GetStaticMesh();
					if (staticMesh)
					{
						//If ignore static mesh continue
						if (_IgnoreStaticMesh.Contains(staticMesh)) continue;

						//If the static mesh is already passed continue
						if (_PassedStaticMesh.Contains(staticMesh)) continue;

						//Apply collision -----------------------------------------------------------------
						FScopedTransaction Transaction(LOCTEXT("UpdateCollision", "Update Collision"));

						staticMeshComponent->Modify();

						//Get number of polygon
						int NumberOfPolygons = 0;
						FMeshDescription* MeshDescription = staticMesh->GetMeshDescription(0);
						if (MeshDescription)
						{
							NumberOfPolygons = MeshDescription->Polygons().Num();
						}

						//Get current collision data
						FCollisionData& CollisionData = NumberOfPolygons <= _MaxPolygonNumber ? _CollisionDataLesserOrEqualMaxPolygon : _CollisionDataBiggerMaxPolygon;

						//Set collision parameter
						staticMeshComponent->SetCollisionEnabled(CollisionData.CollisionEnabled);
						staticMeshComponent->SetCollisionResponseToAllChannels(CollisionData.CollisionResponse);
						UBodySetup* bodySetup = staticMesh->GetBodySetup();
						if (!bodySetup)
						{
							staticMesh->CreateBodySetup();
							bodySetup = staticMesh->GetBodySetup();
						}
						if (bodySetup)
						{
							bodySetup->Modify();
							bodySetup->CollisionTraceFlag = CollisionData.CollisionTraceFlag;
						}
						staticMesh->Modify();

						//Generate or delete simple collision
						UStaticMeshEditorSubsystem* MeshEditorSubsystem = GEditor->GetEditorSubsystem<UStaticMeshEditorSubsystem>();
						if (MeshEditorSubsystem)
						{
							if (CollisionData.bUseSimpleCollision)
							{
								MeshEditorSubsystem->SetConvexDecompositionCollisions(staticMesh, CollisionData.HullCount, CollisionData.MaxHullVerts, CollisionData.HullPrecision);
							}
							else
							{
								MeshEditorSubsystem->RemoveCollisions(staticMesh);
							}
						}

						staticMesh->Build();

						_PassedStaticMesh.Add(staticMesh, staticMesh);
						//---------------------------------------------------------------------------------
					}
				}
			}
		}
	}
	_PassedStaticMesh.Empty();
}

#undef LOCTEXT_NAMESPACE