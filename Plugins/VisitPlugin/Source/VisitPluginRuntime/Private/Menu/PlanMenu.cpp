#include "Menu/PlanMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Menu/BotMenu.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Interactable/Button/ButtonTeleport.h"
#include "Maps/Interactable/MapsInteractable.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Engine/TextureRenderTarget2D.h"

APlanMenu::APlanMenu()
{
	PrimaryActorTick.bCanEverTick = true;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);

	_MapsCameraView = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MapsCameraView"));
	_MapsCameraView->SetupAttachment(_RootSceneComponent);
	_MapsCameraView->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	_MapsCameraView->ProjectionType = ECameraProjectionMode::Orthographic;

	// Disable lighting
	_MapsCameraView->bCaptureEveryFrame = true;
	_MapsCameraView->bCaptureOnMovement = true;
	_MapsCameraView->ShowFlags.SetLighting(false);

	// Set Exposure settings to fixed
	_MapsCameraView->PostProcessSettings.bOverride_AutoExposureMethod = true;
	_MapsCameraView->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;

	// Set fixed exposure value
	_MapsCameraView->PostProcessSettings.bOverride_AutoExposureBias = true;
	_MapsCameraView->PostProcessSettings.AutoExposureBias = 0.0f;

	// Set Capture Source to Final Color (LDR)
	_MapsCameraView->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	_SpriteComponentPlayerView = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponentPlayerView"));
	_SpriteComponentPlayerView->SetupAttachment(_RootSceneComponent);
	_SpriteComponentPlayerView->SetWorldRotation(FRotator(0.0f, 0.0f, -90.0f));
	_SpriteComponentPlayerView->SetRelativeLocation(FVector(0.0f, 0.0f, -10.0f));
	_SpriteComponentPlayerView->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	_SpriteComponentPlayerView->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void APlanMenu::BeginPlay()
{
	Super::BeginPlay();
	
	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &APlanMenu::OnBotMenuStateChange);
	}

	InitWidget();

	HideActor();
	SetFollowPlayer(true);

	SetCaptureState(false);
}

void APlanMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTextureSize();
	CheckCameraHeight();
	FollowPlayer();
}

//Initialise ---------------------------------------

void APlanMenu::InitWidget()
{
	if (!_PlanWidgetClass) return;

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController) return;

	_PlanWidget = CreateWidget<UUserWidget>(PlayerController, _PlanWidgetClass);
	if (!_PlanWidget) return;

	_ImagePlan = Cast<UImage>(_PlanWidget->GetWidgetFromName(FName("Image_PlanRender")));
	if (_ImagePlan && _MaterialPlan && _RenderTargetPlan)
	{
		_ImagePlan->SetBrushFromMaterial(_MaterialPlan);
	}

	if (_MapsCameraView)
	{
		_MapsCameraView->TextureTarget = _RenderTargetPlan;
		_MapsCameraView->OrthoWidth = _DataPlan.BaseViewSize;
		_MapsCameraView->CaptureScene();
	}

	UButton* ButtonRecenter = Cast<UButton>(_PlanWidget->GetWidgetFromName(FName("Button_Recenter")));
	if (ButtonRecenter)
	{
		ButtonRecenter->OnClicked.AddDynamic(this, &APlanMenu::Recenter);
	}

	UScrollBox* ScrollBoxFloor = Cast<UScrollBox>(_PlanWidget->GetWidgetFromName(FName("ScrollBox_Floor")));
	if (ScrollBoxFloor)
	{
		ScrollBoxFloor->ClearChildren();
		_ButtonIndex.Empty();

		for (int i = _DataHeightCam.Num() - 1; i >= 0 ; i--)
		{
			UUserWidget* Slot = CreateWidget<UUserWidget>(PlayerController, _FloorSlotWidgetClass);
			if (!Slot) continue;

			UButton* ButtonApply = Cast<UButton>(Slot->GetWidgetFromName(FName("Button_Floor")));
			if (ButtonApply)
			{
				UButtonIndex* ButtonIndex = NewObject<UButtonIndex>();
				ButtonIndex->Initialise(this, i, "");
				ButtonApply->OnClicked.AddDynamic(ButtonIndex, &UButtonIndex::OnClick);
				_ButtonIndex.Add(ButtonIndex);
			}

			UTextBlock* TextBlockFloor = Cast<UTextBlock>(Slot->GetWidgetFromName(FName("TextBlock_Floor")));
			if (TextBlockFloor)
			{
				TextBlockFloor->SetText(_DataHeightCam[i].FloorName);
			}

			UPanelSlot* PanelSlot = ScrollBoxFloor->AddChild(Slot);
			UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot);
			if (ScrollBoxSlot)
			{

			}
		}
	}
}

void APlanMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (BotMenuState == EBotMenuState::Plan)
	{
		OpenPlanMenu();
	}
	else
	{
		ClosePlanMenu();
	}
}

void APlanMenu::OpenPlanMenu()
{
	if (_PlanWidget && !_PlanWidget->IsInViewport())
	{
		SetCaptureState(true);

		_PlanWidget->AddToViewport();
		Recenter();
		_CurrHeightCamIndex = -1;
	}
}

void APlanMenu::ClosePlanMenu()
{
	if (_PlanWidget && _PlanWidget->IsInViewport())
	{
		SetCaptureState(false);

		_PlanWidget->RemoveFromParent();
	}
}

//Initialise ---------------------------------------

//Plan action --------------------------------------

bool APlanMenu::CheckPointIsOnTheMap(FVector2D Point)
{
	if (!_ImagePlan) return false;

	if (_ImagePlan->GetCachedGeometry().IsUnderLocation(Point))
	{
		return true;
	}
	return false;
}

void APlanMenu::Looking(FVector2D value)
{
	//To implement
}

void APlanMenu::Zooming(float value)
{
	//To implement
}

void APlanMenu::Taping(FVector2D textureClickPosition)
{
	if (!_ImagePlan || !_DataPlan.bCanClick) return;

	FGeometry imageGeometry = _ImagePlan->GetCachedGeometry();

	FVector2D imageClickPos = imageGeometry.AbsoluteToLocal(textureClickPosition);
	FVector2D imageCameraPos = FVector2D(imageGeometry.GetLocalSize().X / 2, imageGeometry.GetLocalSize().Y / 2);

	FVector2D imageDelta = imageClickPos - imageCameraPos;

	if (!_MapsCameraView) return;
	float orthoWidthX = _MapsCameraView->OrthoWidth * (imageGeometry.GetLocalSize().Y / imageGeometry.GetLocalSize().X);
	float orthoWidthY = _MapsCameraView->OrthoWidth;

	FVector2D worldDelta = FVector2D::ZeroVector;
	worldDelta.X = -(imageDelta.Y / imageGeometry.GetLocalSize().Y) * orthoWidthX;
	worldDelta.Y = (imageDelta.X / imageGeometry.GetLocalSize().X) * orthoWidthY;

	//worldDelta = worldDelta.GetRotated(_MapsCamera->GetComponentRotation().Yaw);

	worldDelta = worldDelta.GetRotated(GetActorRotation().Yaw);

	FVector cameraPos = _MapsCameraView->GetComponentLocation();

	FVector clickPos = FVector(cameraPos.X + worldDelta.X, cameraPos.Y + worldDelta.Y, cameraPos.Z);

	FVector end = clickPos + FVector(0, 0, -10000);

	UWorld* world = GetWorld();
	if (world)
	{
		FCollisionQueryParams queryParams;
		FCollisionResponseParams responseParam(ECollisionResponse::ECR_Overlap);
		TArray<FHitResult> hitResults;

		world->LineTraceMultiByChannel(hitResults, clickPos, end, ECC_Visibility, queryParams, responseParam);

		for (const FHitResult& hitResult : hitResults)
		{
			AMapsInteractable* mapInteractable = Cast<AMapsInteractable>(hitResult.GetActor());
			if (mapInteractable)
			{
				mapInteractable->Interact();
				return;
			}
		}
	}
}

//Plan action --------------------------------------

void APlanMenu::SetCaptureState(bool State)
{
	_bCaptureState = State;

	if (_MapsCameraView)
	{
		_MapsCameraView->SetComponentTickEnabled(_bCaptureState);
	}
}

void APlanMenu::OnButtonIndexClick(FString Type, int Index)
{
	_StepSetManually = true;
	_CurrHeightCamIndex = Index;
	HideActor();
}

void APlanMenu::HideActor()
{
	UWorld* World = GetWorld();
	if (!World || !_MapsCameraView) return;
	
	_MapsCameraView->ClearHiddenComponents();

	for (TSubclassOf<AActor> ActorClass : _DataHiddenActor.HiddenActorsClass)
	{
		TArray<AActor*> FoundActor;
		UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActor);

		for (AActor* Actor : FoundActor)
		{
			_MapsCameraView->HideActorComponents(Actor);
		}
	}

	for (AActor* Actor : _DataHiddenActor.HiddenActors)
	{
		_MapsCameraView->HideActorComponents(Actor);
	}

	if (_CurrHeightCamIndex >= 0 && _CurrHeightCamIndex < _DataHeightCam.Num())
	{
		for (TSubclassOf<AActor> ActorClass : _DataHeightCam[_CurrHeightCamIndex].HiddenActor.HiddenActorsClass)
		{
			TArray<AActor*> FoundActor;
			UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActor);

			for (AActor* Actor : FoundActor)
			{
				_MapsCameraView->HideActorComponents(Actor);
			}
		}

		for (AActor* Actor : _DataHeightCam[_CurrHeightCamIndex].HiddenActor.HiddenActors)
		{
			_MapsCameraView->HideActorComponents(Actor);
		}
	}
}

void APlanMenu::UpdateTextureSize()
{
	if (_ImagePlan && _RenderTargetPlan)
	{
		FVector2D ImageSize = _ImagePlan->GetCachedGeometry().GetLocalSize();

		if (ImageSize != _LastImageSize && ImageSize != FVector2D::ZeroVector)
		{
			_RenderTargetPlan->ResizeTarget(ImageSize.X, ImageSize.Y);
			_LastImageSize = ImageSize;
		}
	}
}

void APlanMenu::SetFollowPlayer(bool value)
{
	_FollowPlayer = value;
}

void APlanMenu::FollowPlayer()
{
	if (!_PlayerController)
	{
		_PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!_PlayerController) return;
	}

	APawn* Pawn = _PlayerController->GetPawn();
	if (!Pawn) return;

	FVector PlayerLocation = Pawn->GetActorLocation();
	PlayerLocation.Z = _CurrPosZ;

	if (_DataPlan.bUseFixedLocation)
	{
		FVector Location = FVector(_DataPlan.FixedPosition.X, _DataPlan.FixedPosition.Y, _CurrPosZ);
		SetActorLocation(Location);
	}
	else if (_FollowPlayer)
	{
		SetActorLocation(PlayerLocation);
	}
	else
	{
		FVector Location = GetActorLocation();
		Location.Z = _CurrPosZ;
		SetActorLocation(Location);
	}

	FRotator PawnRotation = Pawn->GetActorRotation();
	FRotator SpriteRotation = _SpriteComponentPlayerView->GetRelativeRotation();
	_SpriteComponentPlayerView->SetWorldRotation(FRotator(SpriteRotation.Pitch, PawnRotation.Yaw + _ViewVisitor.OffsetRotation, SpriteRotation.Roll));

	FVector SpriteLocation = _SpriteComponentPlayerView->GetComponentLocation();
	SpriteLocation.X = PlayerLocation.X;
	SpriteLocation.Y = PlayerLocation.Y;
	_SpriteComponentPlayerView->SetWorldLocation(SpriteLocation);
}

void APlanMenu::CheckCameraHeight()
{
	if (_LastHeightCamIndex != _CurrHeightCamIndex)
	{
		UFunction* Function = _PlanWidget->FindFunction(FName("SelectFloor"));
		if (Function)
		{
			FParamsFloorHightLight Param = FParamsFloorHightLight();
			Param.Index = (_DataHeightCam.Num() - 1) - _CurrHeightCamIndex;
			_PlanWidget->ProcessEvent(Function, &Param);
		}
	}
	_LastHeightCamIndex = _CurrHeightCamIndex;

	if (_DataHeightCam.Num() == 0)
	{
		_CurrPosZ = GetActorLocation().Z;
	}

	if (!_FollowPlayer || _StepSetManually)
	{
		if (_CurrHeightCamIndex >= 0 && _CurrHeightCamIndex < _DataHeightCam.Num())
		{
			_CurrPosZ = _DataHeightCam[_CurrHeightCamIndex].CameraHeight.Z;
		}
		return;
	}

	if (!_PlayerController)
	{
		_PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!_PlayerController) return;
	}
	APawn* Player = _PlayerController->GetPawn();
	if (!Player) return;

	int OldIndex = _CurrHeightCamIndex;

	int Count = 0;
	for (FDataHeightCam DataHeightCam : _DataHeightCam)
	{
		FVector CurrPlayerPos = Player->GetActorLocation();
		if (CurrPlayerPos.Z <= DataHeightCam.PlayerHeight.Z)
		{
			_CurrPosZ = DataHeightCam.CameraHeight.Z;
			_CurrHeightCamIndex = Count;
		}

		if (CurrPlayerPos.Z > DataHeightCam.PlayerHeight.Z)
		{
			if (Count == 0)
			{
				_CurrPosZ = DataHeightCam.CameraHeight.Z;
				_CurrHeightCamIndex = Count;
			}
			break;
		}

		Count++;
	}

	if (OldIndex != _CurrHeightCamIndex)
	{
		HideActor();
	}
}

void APlanMenu::StepUp()
{
	if (_CurrHeightCamIndex > 0)
	{
		_StepSetManually = true;
		_CurrHeightCamIndex--;
		HideActor();
	}
}

void APlanMenu::StepDown()
{
	if (_CurrHeightCamIndex < _DataHeightCam.Num() - 1)
	{
		_StepSetManually = true;
		_CurrHeightCamIndex++;
		HideActor();
	}
}

void APlanMenu::Recenter()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Test");
	SetFollowPlayer(true);
	_StepSetManually = false;
}

FDataMap APlanMenu::GetCurrentMapData()
{
	return _DataPlan;
}
