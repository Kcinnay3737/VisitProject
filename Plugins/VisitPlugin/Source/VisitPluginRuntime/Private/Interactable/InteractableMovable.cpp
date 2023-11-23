#include "Interactable/InteractableMovable.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AInteractableMovable::AInteractableMovable() : Super::AInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	_SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	_SceneComponent->SetupAttachment(_SceneRootComponent);

	_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	_StaticMeshComponent->SetupAttachment(_SceneComponent);
}

void AInteractableMovable::BeginPlay()
{
	Super::BeginPlay();

	_bIsInteract = false;
}

void AInteractableMovable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_bChangeInteractByDistanceToPlayer)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (PlayerController)
		{
			APawn* Pawn = PlayerController->GetPawn();
			if (Pawn)
			{
				FVector CurrLocation = GetActorLocation();
				FVector PlayerLocation = Pawn->GetActorLocation();

				float Distance = FVector::Distance(CurrLocation, PlayerLocation);

				if (Distance > _InteractDistance)
				{
					if (_bApplyTransform && !_bMoving && _bPlayerInRange)
					{
						_bPlayerInRange = false;
						StartMovement();
						_bIsInteract = !_bIsInteract;
					}
				}
				else if (Distance <= _InteractDistance)
				{
					if (_bApplyTransform && !_bMoving && !_bPlayerInRange)
					{
						_bPlayerInRange = true;
						StartMovement();
						_bIsInteract = !_bIsInteract;
					}
				}
			}
		}
	}

	if (_bMoving)
	{
		ApplyMovement(DeltaTime);
	}
}

void AInteractableMovable::Interact()
{
	Super::Interact();

	if (_bApplyTransform && !_bMoving && !_bChangeInteractByDistanceToPlayer)
	{
		StartMovement();
		_bIsInteract = !_bIsInteract;
	}
}

void AInteractableMovable::StartMovement()
{
	//_StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	_bMoving = true;
	
	_CurrTime = 0.0f;

	if (_bIsInteract)
	{
		_StartLocation = _SceneComponent->GetRelativeLocation();
		_StartRotation = _SceneComponent->GetRelativeRotation();

		_LocationToApply = _InteractMovement * -1;
		_RotationToApply = _InteractRotation * -1;
	}
	else
	{
		_StartLocation = _SceneComponent->GetRelativeLocation();
		_StartRotation = _SceneComponent->GetRelativeRotation();

		_LocationToApply = _InteractMovement;
		_RotationToApply = _InteractRotation;
	}
}

void AInteractableMovable::ApplyMovement(float deltaTime)
{
	_CurrTime += deltaTime;
	if (_CurrTime >= _TimeToApplyTransform)
	{
		_SceneComponent->SetRelativeLocation(_StartLocation + _LocationToApply);
		_SceneComponent->SetRelativeRotation(_StartRotation + _RotationToApply);

		_bMoving = false;

		//_StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		return;
	}
	else
	{
		float ratio = 1 - ((_TimeToApplyTransform - _CurrTime) / _TimeToApplyTransform);
		
		_SceneComponent->SetRelativeLocation(_StartLocation + (_LocationToApply * ratio));
		_SceneComponent->SetRelativeRotation(_StartRotation + (_RotationToApply * ratio));
	}
}
