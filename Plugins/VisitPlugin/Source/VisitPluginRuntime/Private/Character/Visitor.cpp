#include "Character/Visitor.h"

#include "Camera/CameraComponent.h"

AVisitor::AVisitor()
{
	PrimaryActorTick.bCanEverTick = true;

	//Create camera
	_FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_FollowCamera->SetupAttachment(GetRootComponent());
	_FollowCamera->AddLocalOffset(FVector(0.0f, 0.0f, 50.0f));
	_FollowCamera->bUsePawnControlRotation = true;
	_BaseFOV = _FollowCamera->FieldOfView;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AVisitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisitor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVisitor::BeginPlay()
{
	Super::BeginPlay();
	_BaseFOV = _FollowCamera->FieldOfView;
}

UCameraComponent* AVisitor::GetFollowCamera()
{
	return _FollowCamera;
}

float AVisitor::GetBaseFov()
{
	return _BaseFOV;
}

void AVisitor::SetEditWidgetIsOpen(bool value)
{
	_bEditWidgetIsOpen = value;
}

bool AVisitor::GetEditWidgetIsOpen()
{
	return _bEditWidgetIsOpen;
}

TArray<TSubclassOf<AActor>> AVisitor::GetHiddenActorsClass()
{
	return _HiddenActorsClass;
}

TArray<AActor*> AVisitor::GetHiddenActors()
{
	return _HiddenActors;
}
