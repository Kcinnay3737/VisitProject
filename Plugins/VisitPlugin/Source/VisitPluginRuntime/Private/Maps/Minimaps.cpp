#include "Maps/Minimaps.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Interactable/Button/ButtonTeleport.h"
#include "Kismet/GameplayStatics.h"
#include "Maps/Interactable/MapsInteractable.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Engine/TextureRenderTarget2D.h"

AMinimaps::AMinimaps()
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

void AMinimaps::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetGameInstance())
	{
		UGameInstanceSubsystemVisit* gameInstanceVisit = GetGameInstance()->GetSubsystem<UGameInstanceSubsystemVisit>();
		if (gameInstanceVisit)
		{
			gameInstanceVisit->AddInDictObject("Maps", this);
		}
	}

	_DataHeightCamDict.Add(EMapState::Minimap);
	_DataHeightCamDict.Add(EMapState::Bigmap);

	for (FDataHeightCam1& DataHeightCam : _DataHeightCam)
	{
		DataHeightCam.CameraHeight = GetActorTransform().TransformPosition(DataHeightCam.CameraHeight);
		DataHeightCam.PlayerHeight = GetActorTransform().TransformPosition(DataHeightCam.PlayerHeight);

		if (DataHeightCam.bJustForMinimap)
		{
			_DataHeightCamDict[EMapState::Minimap].Add(DataHeightCam);
		}
		else if (DataHeightCam.bJustForBigmap)
		{
			_DataHeightCamDict[EMapState::Bigmap].Add(DataHeightCam);
		}
		else
		{
			_DataHeightCamDict[EMapState::Minimap].Add(DataHeightCam);
			_DataHeightCamDict[EMapState::Bigmap].Add(DataHeightCam);
		}
	}

	HideActor();
	SetFollowPlayer(true);

	OnOpenMinimap();

	SetCaptureState(false);
}

void AMinimaps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_CurrMinimapWidget)
	{
		InitMinimapWidget();
		if (!_CurrMinimapWidget) return;
	}
	if (!_CurrBigmapWidget)
	{
		InitBigmapWidget();
		if (!_CurrBigmapWidget) return;
	}

	CheckChangeResolution();
	
	CheckCameraHeight();
	FollowPlayer();

	UdpateTextCurrState();
}

void AMinimaps::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (GetGameInstance())
	{
		UGameInstanceSubsystemVisit* gameInstanceVisit = GetGameInstance()->GetSubsystem<UGameInstanceSubsystemVisit>();
		if (gameInstanceVisit)
		{
			gameInstanceVisit->RemoveInDictObject("Maps");
		}
	}
}

bool AMinimaps::CheckPointIsOnTheMap(FVector2D Point)
{
	UImage* CurrImage = nullptr;
	if (_MapState == EMapState::Minimap)
	{
		CurrImage = _MinimapImage;
	}
	else if (_MapState == EMapState::Bigmap)
	{
		CurrImage = _BigmapImage;
	}
	if (!CurrImage) return false;

	if (CurrImage->GetCachedGeometry().IsUnderLocation(Point))
	{
		return true;
	}

	return false;
}

void AMinimaps::Looking(FVector2D value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Looking");
}

void AMinimaps::Zooming(float value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, "Zooming");
}

void AMinimaps::Taping(FVector2D textureClickPosition)
{
	FDataMap1 currDataMap = GetCurrentMapData();
	if (!currDataMap.bCanClick) return;

	UImage* image = nullptr;
	if (_MapState == EMapState::Minimap)
	{
		image = _MinimapImage;
	}
	else if (_MapState == EMapState::Bigmap)
	{
		image = _BigmapImage;
	}
	if (!image) return;

	FGeometry imageGeometry = image->GetCachedGeometry();

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

		for (FHitResult hitResult : hitResults)
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

void AMinimaps::OnOpenMinimap()
{
	_MapState = EMapState::Minimap;

	if (_MapsCameraView)
	{
		_MapsCameraView->TextureTarget = _RenderTargetMinimap;
		_MapsCameraView->OrthoWidth = _DataMinimap.BaseViewSize;
		_MapsCameraView->CaptureScene();
	}

	_OldImageSize = FVector2D::ZeroVector;

	Recenter();
	_CurrHeightCamIndex = -1;
}

void AMinimaps::OnOpenBigmap()
{
	_MapState = EMapState::Bigmap;

	if (_MapsCameraView)
	{
		_MapsCameraView->TextureTarget = _RenderTargetBigmap;
		_MapsCameraView->OrthoWidth = _DataBigmap.BaseViewSize;
		_MapsCameraView->CaptureScene();
	}

	_OldImageSize = FVector2D::ZeroVector;

	Recenter();
	_CurrHeightCamIndex = -1;
}

void AMinimaps::SetCaptureState(bool State)
{
	_bCaptureState = State;

	if (_MapsCameraView)
	{
		_MapsCameraView->SetComponentTickEnabled(_bCaptureState);
	}
}

void AMinimaps::CheckChangeResolution()
{
	UImage* CurrImage = nullptr;
	UMaterialInterface* CurrMaterial = nullptr;
	UTextureRenderTarget2D* CurrTexture = nullptr;
	if (_MapState == EMapState::Minimap)
	{
		CurrImage = _MinimapImage;
		CurrMaterial = _MaterialMinimap;
		CurrTexture = _RenderTargetMinimap;
	}
	else if (_MapState == EMapState::Bigmap)
	{
		CurrImage = _BigmapImage;
		CurrMaterial = _MaterialBigmap;
		CurrTexture = _RenderTargetBigmap;
	}

	if (!CurrImage || !CurrMaterial || !CurrTexture) return;

	FVector2D ImageSize = CurrImage->GetCachedGeometry().GetLocalSize();
	if (ImageSize.Equals(_OldImageSize)) return;

	CurrTexture->ResizeTarget(ImageSize.X, ImageSize.Y);
	CurrImage->SetBrushFromMaterial(CurrMaterial);

	_OldImageSize = ImageSize;
}

void AMinimaps::InitMinimapWidget()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, _MinimapWidgetClass, false);
	if (FoundWidgets.Num() != 0)
	{
		_CurrMinimapWidget = FoundWidgets[0];
	}
	if (!_CurrMinimapWidget) return;

	_MinimapImage = Cast<UImage>(_CurrMinimapWidget->GetWidgetFromName("Image_Maps"));
	//if (Image)
	//{
	//	_bChangeResolution = true;
	//	_CurrImage = Image;
	//}

	UButton* ButtonStepUp = Cast<UButton>(_CurrMinimapWidget->GetWidgetFromName("Button_StepUp"));
	if (ButtonStepUp)
	{
		ButtonStepUp->OnClicked.AddDynamic(this, &AMinimaps::StepUp);
	}

	UButton* ButtonStepDown = Cast<UButton>(_CurrMinimapWidget->GetWidgetFromName("Button_StepDown"));
	if (ButtonStepDown)
	{
		ButtonStepDown->OnClicked.AddDynamic(this, &AMinimaps::StepDown);
	}

	UButton* ButtonCurrStep = Cast<UButton>(_CurrMinimapWidget->GetWidgetFromName("Button_CurrStep"));
	if (ButtonCurrStep)
	{
		ButtonCurrStep->OnClicked.AddDynamic(this, &AMinimaps::Recenter);
	}
}

void AMinimaps::InitBigmapWidget()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, _BigmapWidgetClass, false);
	if (FoundWidgets.Num() != 0)
	{
		_CurrBigmapWidget = FoundWidgets[0];
	}
	if (!_CurrBigmapWidget) return;

	_BigmapImage = Cast<UImage>(_CurrBigmapWidget->GetWidgetFromName("Image_Maps"));

	UButton* ButtonStepUp = Cast<UButton>(_CurrBigmapWidget->GetWidgetFromName("Button_StepUp"));
	if (ButtonStepUp)
	{
		ButtonStepUp->OnClicked.AddDynamic(this, &AMinimaps::StepUp);
	}

	UButton* ButtonStepDown = Cast<UButton>(_CurrBigmapWidget->GetWidgetFromName("Button_StepDown"));
	if (ButtonStepDown)
	{
		ButtonStepDown->OnClicked.AddDynamic(this, &AMinimaps::StepDown);
	}

	UButton* ButtonCurrStep = Cast<UButton>(_CurrBigmapWidget->GetWidgetFromName("Button_CurrStep"));
	if (ButtonCurrStep)
	{
		ButtonCurrStep->OnClicked.AddDynamic(this, &AMinimaps::Recenter);
	}
}

void AMinimaps::InitMainWidget()
{
}

void AMinimaps::HideActor()
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

	if (_MapState == EMapState::Minimap)
	{
		for (TSubclassOf<AActor> ActorClass : _DataHiddenActorMinimap.HiddenActorsClass)
		{
			TArray<AActor*> FoundActor;
			UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActor);

			for (AActor* Actor : FoundActor)
			{
				_MapsCameraView->HideActorComponents(Actor);
			}
		}

		for (AActor* Actor : _DataHiddenActorMinimap.HiddenActors)
		{
			_MapsCameraView->HideActorComponents(Actor);
		}
	}
	else if (_MapState == EMapState::Bigmap)
	{
		for (TSubclassOf<AActor> ActorClass : _DataHiddenActorBigmap.HiddenActorsClass)
		{
			TArray<AActor*> FoundActor;
			UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActor);

			for (AActor* Actor : FoundActor)
			{
				_MapsCameraView->HideActorComponents(Actor);
			}
		}

		for (AActor* Actor : _DataHiddenActorBigmap.HiddenActors)
		{
			_MapsCameraView->HideActorComponents(Actor);
		}
	}
}

void AMinimaps::SetFollowPlayer(bool value)
{
	_FollowPlayer = value;
}

void AMinimaps::FollowPlayer()
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

	FDataMap1 CurrMapData = GetCurrentMapData();

	if (CurrMapData.bUseFixedLocation)
	{
		FVector Location = FVector(CurrMapData.FixedPosition.X, CurrMapData.FixedPosition.Y, _CurrPosZ);
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

void AMinimaps::CheckCameraHeight()
{
	if (_DataHeightCamDict[_MapState].Num() == 0)
	{
		_CurrPosZ = GetActorLocation().Z;
	}

	if (!_FollowPlayer || _StepSetManually)
	{
		if (_CurrHeightCamIndex >= 0 && _CurrHeightCamIndex < _DataHeightCamDict[_MapState].Num())
		{
			_CurrPosZ = _DataHeightCamDict[_MapState][_CurrHeightCamIndex].CameraHeight.Z;
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
	for (FDataHeightCam1 DataHeightCam : _DataHeightCamDict[_MapState])
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

void AMinimaps::StepUp()
{
	if (_CurrHeightCamIndex > 0)
	{
		_StepSetManually = true;
		_CurrHeightCamIndex--;
		HideActor();
	}
}

void AMinimaps::StepDown()
{
	if (_CurrHeightCamIndex < _DataHeightCamDict[_MapState].Num() - 1)
	{
		_StepSetManually = true;
		_CurrHeightCamIndex++;
		HideActor();
	}
}

void AMinimaps::Recenter()
{
	SetFollowPlayer(true);
	_StepSetManually = false;
}

void AMinimaps::UdpateTextCurrState()
{
	UUserWidget* CurrWidget = nullptr;
	if (_MapState == EMapState::Minimap)
	{
		CurrWidget = _CurrMinimapWidget;
	}
	else if (_MapState == EMapState::Bigmap)
	{
		CurrWidget = _CurrBigmapWidget;
	}
	if (!CurrWidget) return;

	UTextBlock* TextBlockCurrStep = Cast<UTextBlock>(CurrWidget->GetWidgetFromName("TextBlock_CurrStep"));
	if (TextBlockCurrStep)
	{
		if (_DataHeightCamDict[_MapState].Num() == 0)
		{
			TextBlockCurrStep->SetText(FText::FromString("0"));
		}
		else
		{
			TextBlockCurrStep->SetText(FText::FromString(FString::Printf(TEXT("%d"), _DataHeightCamDict[_MapState].Num() - _CurrHeightCamIndex - 1)));
		}
	}

	UButton* ButtonStepUp = Cast<UButton>(CurrWidget->GetWidgetFromName("Button_StepUp"));
	UButton* ButtonStepDown = Cast<UButton>(CurrWidget->GetWidgetFromName("Button_StepDown"));

	if (ButtonStepUp)
	{
		if (_CurrHeightCamIndex == 0 || _DataHeightCamDict[_MapState].Num() == 0)
		{
			ButtonStepUp->SetIsEnabled(false);
		}
		else
		{
			ButtonStepUp->SetIsEnabled(true);
		}
	}
	if (ButtonStepDown)
	{
		if (_CurrHeightCamIndex == _DataHeightCamDict[_MapState].Num() - 1 || _DataHeightCamDict[_MapState].Num() == 0)
		{
			ButtonStepDown->SetIsEnabled(false);
		}
		else
		{
			ButtonStepDown->SetIsEnabled(true);
		}
	}
}

FDataMap1 AMinimaps::GetCurrentMapData()
{
	if (_MapState == EMapState::Minimap)
	{
		return _DataMinimap;
	}
	else if (_MapState == EMapState::Bigmap)
	{
		return _DataBigmap;
	}
	return FDataMap1();
}