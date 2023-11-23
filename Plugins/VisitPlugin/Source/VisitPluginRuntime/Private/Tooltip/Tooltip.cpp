#include "Tooltip/Tooltip.h"

#include "Components/SceneComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

ATooltip::ATooltip()
{
	PrimaryActorTick.bCanEverTick = true;

	_SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	SetRootComponent(_SceneRootComponent);

	_SceneComponentTooltipClose = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponentTooltipClose"));
	_SceneComponentTooltipClose->SetupAttachment(_SceneRootComponent);

	_SceneComponentTooltipOpen = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponentTooltipOpen"));
	_SceneComponentTooltipOpen->SetupAttachment(_SceneRootComponent);
}

void ATooltip::BeginPlay()
{
	Super::BeginPlay();
	
	ShowTooltipClose();
}

void ATooltip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTooltip();
}

void ATooltip::Interact()
{
	if (_IsOpen)
	{
		ShowTooltipClose();
	}
	else
	{
		ShowTooltipOpen();
	}
}

void ATooltip::ShowTooltipClose()
{
	HideTooltipOpen();
	_SceneComponentTooltipClose->SetActive(true);
	_SceneComponentTooltipClose->SetVisibility(true);
	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(_SceneComponentTooltipClose);
	if (primitiveComponent)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
	for (USceneComponent* child : _SceneComponentTooltipClose->GetAttachChildren())
	{
		child->SetActive(true);
		child->SetVisibility(true);

		UPrimitiveComponent* primitiveChild = Cast<UPrimitiveComponent>(child);
		if (primitiveChild)
		{
			primitiveChild->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}
	_IsOpen = false;
}

void ATooltip::HideTooltipClose()
{
	_SceneComponentTooltipClose->SetActive(false);
	_SceneComponentTooltipClose->SetVisibility(false);
	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(_SceneComponentTooltipClose);
	if (primitiveComponent)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	for (USceneComponent* child : _SceneComponentTooltipClose->GetAttachChildren())
	{
		child->SetActive(false);
		child->SetVisibility(false);
		UPrimitiveComponent* primitiveChild = Cast<UPrimitiveComponent>(child);
		if (primitiveChild)
		{
			primitiveChild->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		}
	}
}

void ATooltip::ShowTooltipOpen()
{
	HideTooltipClose();
	_SceneComponentTooltipOpen->SetActive(true);
	_SceneComponentTooltipOpen->SetVisibility(true);
	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(_SceneComponentTooltipOpen);
	if (primitiveComponent)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
	for (USceneComponent* child : _SceneComponentTooltipOpen->GetAttachChildren())
	{
		child->SetActive(true);
		child->SetVisibility(true);
		UPrimitiveComponent* primitiveChild = Cast<UPrimitiveComponent>(child);
		if (primitiveChild)
		{
			primitiveChild->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}
	_IsOpen = true;
}

void ATooltip::HideTooltipOpen()
{
	_SceneComponentTooltipOpen->SetActive(false);
	_SceneComponentTooltipOpen->SetVisibility(false);
	UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(_SceneComponentTooltipOpen);
	if (primitiveComponent)
	{
		primitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	for (USceneComponent* child : _SceneComponentTooltipOpen->GetAttachChildren())
	{
		child->SetActive(false);
		child->SetVisibility(false);

		UPrimitiveComponent* primitiveChild = Cast<UPrimitiveComponent>(child);
		if (primitiveChild)
		{
			primitiveChild->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		}
	}
}

void ATooltip::RotateTooltip()
{
	UWorld* world = GetWorld();
	if (!world) return;

	APlayerController* playerController = world->GetFirstPlayerController();
	if (playerController)
	{
		APlayerCameraManager* cameraManager = playerController->PlayerCameraManager;
		if (cameraManager)
		{
			USceneComponent* currentComponent;
			FDataAlignmentTooltip* currentAlignment;
			if (_IsOpen)
			{
				currentComponent = _SceneComponentTooltipOpen;
				currentAlignment = &_AlignmentTooltipOpen;
			}
			else
			{
				currentComponent = _SceneComponentTooltipClose;
				currentAlignment = &_AlignmentTooltipClose;
			}

			FVector currLocation = currentComponent->GetComponentLocation();
			FRotator currRotation = currentComponent->GetComponentRotation();

			FVector cameraLocation = cameraManager->GetCameraLocation();
			FVector lookAtDirection = (cameraLocation - currLocation).GetSafeNormal();
			FRotator lookAtRotation = lookAtDirection.Rotation();

			if (!currentAlignment->alignsPitch)
			{
				lookAtRotation.Pitch = currRotation.Pitch;
			}
			if (!currentAlignment->alignsRoll)
			{
				lookAtRotation.Roll = currRotation.Roll;
			}
			if (!currentAlignment->alignsYaw)
			{
				lookAtRotation.Yaw = currRotation.Yaw;
			}

			currentComponent->SetRelativeRotation(lookAtRotation);
		}
	}
}