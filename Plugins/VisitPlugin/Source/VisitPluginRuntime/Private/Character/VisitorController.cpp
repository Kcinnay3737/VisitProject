#include "Character/VisitorController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Camera/CameraComponent.h"
#include "Character/DataVisitorControl.h"
#include "Character/Visitor.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Interactable/Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Maps/Minimaps.h"
#include "Tooltip/Tooltip.h"
#include "Manager/GameStateManager.h"
#include "Menu/BotMenu.h"


AVisitorController::AVisitorController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVisitorController::BeginPlay()
{
	Super::BeginPlay();

	InitGameStateManager();

	if (_GameStateManager)
	{
		_GameStateManager->OnEnterGameState.AddDynamic(this, &AVisitorController::OnEnterGame);
		_GameStateManager->OnExitGameState.AddDynamic(this, &AVisitorController::OnExitGame);
		_GameStateManager->OnPlayerCanInteract.AddDynamic(this, &AVisitorController::OnPlayerCanInteract);
		_GameStateManager->OnPlayerCannotInteract.AddDynamic(this, &AVisitorController::OnPlayerCannotInteract);
	}

	InitializeMappingContext();

	if (!_DataVisitorControl) return;

	switch (_DataVisitorControl->inputType)
	{
	case EInputType::IT_MouseAndKeyboard:
		SetShowMouseCursor(true);
		bEnableClickEvents = true;
		bEnableMouseOverEvents = true;

		bEnableTouchEvents = false;
		bEnableTouchOverEvents = false;
		break;
	case EInputType::IT_Touch:
		SetShowMouseCursor(false);
		bEnableClickEvents = false;
		bEnableMouseOverEvents = false;

		bEnableTouchEvents = true;
		bEnableTouchOverEvents = true;
		break;
	default:
		break;
	}
}

void AVisitorController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_DataVisitorControl) return;

	switch (_DataVisitorControl->inputType)
	{
	case EInputType::IT_MouseAndKeyboard:
		break;
	case EInputType::IT_Touch:
		CheckPressedTouchData();
		CheckHoldingTouchData(DeltaTime);
		CheckReleasedTouchData();

		CheckLooking(DeltaTime);
		CheckZooming();
		CheckMoving();
		break;
	default:
		break;
	}
}

void AVisitorController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (!_DataVisitorControl) return;

		switch (_DataVisitorControl->inputType)
		{
		case EInputType::IT_MouseAndKeyboard:
			enhancedInputComponent->BindAction(_MouseAndKeyBoardMovingAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyBoardMovingAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardZoomingAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyboardZoomingAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardTapingAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyboardTapingAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardPressedAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyboardPressedAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardHoldingAction, ETriggerEvent::Ongoing, this, &ThisClass::OnMouseAndKeyboardHoldingAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardHoldingAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyboardHoldingAction);
			enhancedInputComponent->BindAction(_MouseAndKeyboardReleasedAction, ETriggerEvent::Triggered, this, &ThisClass::OnMouseAndKeyboardReleasedAction);
			break;
		case EInputType::IT_Touch:
			break;
		default:
			break;
		}
	}
}

AVisitor* AVisitorController::GetVisitor()
{
	APawn* pawn = GetPawn();
	if (!pawn) return nullptr;
	AVisitor* visitor = Cast<AVisitor>(pawn);
	if (!visitor) return nullptr;
	return visitor;
}

void AVisitorController::AddInIgnoreWidget(UWidget* Widget, EIgnoreState IgnoreState)
{
	if (_IgnoreWidget.Contains(Widget)) return;
	_IgnoreWidget.Add(Widget, IgnoreState);
}

void AVisitorController::RemoveInIgnoreWidget(UWidget* Widget)
{
	if (!_IgnoreWidget.Contains(Widget)) return;
	_IgnoreWidget.Remove(Widget);
}

void AVisitorController::InitializeMappingContext()
{
	if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer()))
	{
		if (_InputMappingContext)
		{
			subsystem->ClearAllMappings();
			subsystem->AddMappingContext(_InputMappingContext, 0);
		}
	}
}

void AVisitorController::InitGameStateManager()
{
	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
}

void AVisitorController::OnEnterGame()
{
	_bIsInGame = true;

	HideActor();
}

void AVisitorController::OnExitGame()
{
	_bIsInGame = false;
}

void AVisitorController::OnPlayerCanInteract()
{
	_bCanInteract = true;

	if (_DataVisitorControl && _DataVisitorControl->inputType == EInputType::IT_Touch)
	{
		InitJoystickWidget();
	}
}

void AVisitorController::OnPlayerCannotInteract()
{
	_bCanInteract = false;

	if (_DataVisitorControl && _DataVisitorControl->inputType == EInputType::IT_Touch)
	{
		HideJoystickWidget();
	}
}

//Mouse and Keyboard -------------------------------------------------------------------------------------------

void AVisitorController::OnMouseAndKeyBoardMovingAction(const FInputActionValue& value)
{
	if (_bPause) return;

	FVector2D currentValue = value.Get<FVector2D>();
	Moving(currentValue);
}

void AVisitorController::OnMouseAndKeyboardZoomingAction(const FInputActionValue& value)
{
	if (_bPause) return;

	float currentValue = value.Get<float>();

	FVector2D platformMousePos = GetMousePlatformPos();

	if (CheckPointIsOnTheMap(platformMousePos))
	{
		Zooming(currentValue, true);
	}
	if (!CheckPointIsOnWidget(platformMousePos))
	{
		Zooming(currentValue, false);
	}
	
}

void AVisitorController::OnMouseAndKeyboardTapingAction(const FInputActionValue& value)
{
	if (_bPause) return;

	Taping(GetMousePos(), GetMousePlatformPos());
}

void AVisitorController::OnMouseAndKeyboardPressedAction(const FInputActionValue& value)
{
	if (_bPause) return;

	_OldMousePosition = GetMousePos();
}

void AVisitorController::OnMouseAndKeyboardHoldingAction(const FInputActionValue& value)
{
	if (_bPause) return;
	if (_OldMousePosition.Equals(FVector2D::ZeroVector)) return;

	FVector2D currPoint = GetMousePos();
	FVector2D lookingMovement = currPoint - _OldMousePosition;

	FVector2D mousePlatformPos = GetMousePlatformPos();
	if (CheckPointIsOnTheMap(mousePlatformPos))
	{
		Looking(lookingMovement, true);
	}
	else if (!CheckPointIsOnWidget(mousePlatformPos))
	{
		Looking(lookingMovement, false);
	}

	_OldMousePosition = currPoint;
}

void AVisitorController::OnMouseAndKeyboardReleasedAction(const FInputActionValue& value)
{
	_OldMousePosition = FVector2D::ZeroVector;
}

//--------------------------------------------------------------------------------------------------------------

//Touch --------------------------------------------------------------------------------------------------------

void AVisitorController::CheckPressedTouchData()
{
	FVector2D localPos = FVector2D::ZeroVector;
	FVector2D platformPos = FVector2D::ZeroVector;
	bool bIsCurrentlyPressed = false;

	for (int32 i = 0; i < 10; i++)
	{
		if (_AllTouchData.touchData.Contains(i)) continue;

		GetInputTouchState(static_cast<ETouchIndex::Type>(i), localPos.X, localPos.Y, bIsCurrentlyPressed);
		if (bIsCurrentlyPressed)
		{
			SetMouseLocation(localPos.X, localPos.Y);
			platformPos = UWidgetLayoutLibrary::GetMousePositionOnPlatform();


			FTouchData touchData;
			touchData.oldLocalPos = localPos;
			touchData.localPos = localPos;
			touchData.oldPlatformPos = platformPos;
			touchData.platformPos = platformPos;
			_AllTouchData.touchData.Add(i, touchData);

			if (CheckPointIsOnTheMap(touchData.platformPos))
			{
				_AllTouchData.touchOnMap.Add(i);
			}
			else if (CheckPointIsOnJoystick(touchData.platformPos))
			{
				_AllTouchData.touchOnJoystick.Add(i);
			}
			else if (!CheckPointIsOnWidget(touchData.platformPos))
			{
				_AllTouchData.touchOnViewport.Add(i);
			}
		}
	}
}

void AVisitorController::CheckHoldingTouchData(float deltaTime)
{
	FVector2D localPos = FVector2D::ZeroVector;
	FVector2D platformPos = FVector2D::ZeroVector;
	bool bIsCurrentlyPressed = false;

	for (auto& keyValue : _AllTouchData.touchData)
	{
		int32 key = keyValue.Key;
		FTouchData& value = keyValue.Value;

		GetInputTouchState(static_cast<ETouchIndex::Type>(key), localPos.X, localPos.Y, bIsCurrentlyPressed);
		if (bIsCurrentlyPressed)
		{
			SetMouseLocation(localPos.X, localPos.Y);
			platformPos = UWidgetLayoutLibrary::GetMousePositionOnPlatform();

			value.oldLocalPos = value.localPos;
			value.localPos = localPos;
			value.oldPlatformPos = value.platformPos;
			value.platformPos = platformPos;
			value.pressedTime += deltaTime;
			value.pressedDistance += FVector2D::Distance(value.oldLocalPos, value.localPos);
		}
	}
}

void AVisitorController::CheckReleasedTouchData()
{
	FVector2D localPos = FVector2D::ZeroVector;
	bool bIsCurrentlyPressed = false;

	TArray<int> indexToRemove;
	for (auto& keyValue : _AllTouchData.touchData)
	{
		int32 key = keyValue.Key;
		FTouchData& value = keyValue.Value;

		GetInputTouchState(static_cast<ETouchIndex::Type>(key), localPos.X, localPos.Y, bIsCurrentlyPressed);
		if (!bIsCurrentlyPressed)
		{
			indexToRemove.Add(key);

			if (_AllTouchData.touchData[key].pressedTime <= 0.2f && _AllTouchData.touchData[key].pressedDistance <= 10.0f)
			{
				Taping(_AllTouchData.touchData[key].localPos, _AllTouchData.touchData[key].platformPos);
			}
		}
	}

	for (int key : indexToRemove)
	{
		_AllTouchData.touchData.Remove(key);
		_AllTouchData.touchOnJoystick.Remove(key);
		_AllTouchData.touchOnMap.Remove(key);
		_AllTouchData.touchOnViewport.Remove(key);
	}
}

void AVisitorController::CheckLooking(float deltaTime)
{
	if (_AllTouchData.touchOnViewport.Num() == 1)
	{
		FVector2D oldPos = _AllTouchData.touchData[_AllTouchData.touchOnViewport[0]].oldLocalPos;
		FVector2D currPos = _AllTouchData.touchData[_AllTouchData.touchOnViewport[0]].localPos;
		FVector2D dir = currPos - oldPos;
		Looking(dir, false);
		UpdateRotation(deltaTime);
	}
	
	if (_AllTouchData.touchOnMap.Num() == 1)
	{
		FVector2D oldPos = _AllTouchData.touchData[_AllTouchData.touchOnMap[0]].oldLocalPos;
		FVector2D currPos = _AllTouchData.touchData[_AllTouchData.touchOnMap[0]].localPos;
		FVector2D dir = currPos - oldPos;
		Looking(dir, true);
	}
}

void AVisitorController::CheckZooming()
{
	if (_AllTouchData.touchOnViewport.Num() == 2)
	{
		FVector2D oldPos1 = _AllTouchData.touchData[_AllTouchData.touchOnViewport[0]].oldLocalPos;
		FVector2D currPos1 = _AllTouchData.touchData[_AllTouchData.touchOnViewport[0]].localPos;
		FVector2D oldPos2 = _AllTouchData.touchData[_AllTouchData.touchOnViewport[1]].oldLocalPos;
		FVector2D currPos2 = _AllTouchData.touchData[_AllTouchData.touchOnViewport[1]].localPos;

		float oldDist = FVector2D::Distance(oldPos1, oldPos2);
		float currDist = FVector2D::Distance(currPos1, currPos2);
		float difference = currDist - oldDist;
		Zooming(difference, false);
	}

	if (_AllTouchData.touchOnMap.Num() == 2)
	{
		FVector2D oldPos1 = _AllTouchData.touchData[_AllTouchData.touchOnMap[0]].oldLocalPos;
		FVector2D currPos1 = _AllTouchData.touchData[_AllTouchData.touchOnMap[0]].localPos;
		FVector2D oldPos2 = _AllTouchData.touchData[_AllTouchData.touchOnMap[1]].oldLocalPos;
		FVector2D currPos2 = _AllTouchData.touchData[_AllTouchData.touchOnMap[1]].localPos;

		float oldDist = FVector2D::Distance(oldPos1, oldPos2);
		float currDist = FVector2D::Distance(currPos1, currPos2);
		float difference = currDist - oldDist;
		Zooming(difference, true);
	}
}

void AVisitorController::CheckMoving()
{
	if (_AllTouchData.touchOnJoystick.Num() == 1)
	{
		MovingJoystick(_AllTouchData.touchData[_AllTouchData.touchOnJoystick[0]].platformPos);
	}
	else
	{
		ResetJoystickLocation();
	}
}

void AVisitorController::InitJoystickWidget()
{
	if (_JoystickWidget)
	{
		_JoystickWidget->AddToViewport();
		return;
	}
	if (!_JoystickWidgetClass) return;

	_JoystickWidget = CreateWidget<UUserWidget>(this, _JoystickWidgetClass);
	if (!_JoystickWidget) return;
	_JoystickWidget->AddToViewport();
}

void AVisitorController::HideJoystickWidget()
{
	if (_JoystickWidget && _JoystickWidget->IsInViewport())
	{
		_JoystickWidget->RemoveFromParent();
	}
}

void AVisitorController::MovingJoystick(FVector2D platformPos)
{
	if (!_JoystickWidget) return;

	UImage* imageLimit = Cast<UImage>(_JoystickWidget->GetWidgetFromName("Image_Limit"));
	UImage* imageJoystick = Cast<UImage>(_JoystickWidget->GetWidgetFromName("Image_Joystick"));
	if (!imageLimit || !imageJoystick) return;

	float rayImageLimit = imageLimit->GetCachedGeometry().GetAbsoluteSize().X / 2;
	float rayImageJoystick = imageJoystick->GetCachedGeometry().GetAbsoluteSize().X / 2;
	float maxDistance = rayImageLimit - rayImageJoystick;

	FVector2D midPos = imageLimit->GetCachedGeometry().GetAbsolutePosition() + rayImageLimit;

	//if (FVector2D::Distance(platformPos, midPos) > rayImageLimit)
	//{
	//	ResetJoystickLocation();
	//	return;
	//}

	FVector2D dir = platformPos - midPos;

	if (dir.Length() > maxDistance)
	{
		dir.Normalize();
		dir *= maxDistance;
	}

	imageJoystick->SetRenderTranslation(dir);

	dir.Y = -dir.Y;
	float lenght = dir.Length();
	dir.Normalize();
	dir *= (lenght / maxDistance);
	Moving(dir);
}

void AVisitorController::ResetJoystickLocation()
{
	if (!_JoystickWidget) return;

	UImage* imageJoystick = Cast<UImage>(_JoystickWidget->GetWidgetFromName("Image_Joystick"));
	if (imageJoystick)
	{
		FVector2D newPosition = FVector2D(0.0f, 0.0f);
		imageJoystick->SetRenderTranslation(newPosition);
	}
}

//--------------------------------------------------------------------------------------------------------------

void AVisitorController::Moving(FVector2D value)
{
	if (_bPause) return;

	AVisitor* visitor = GetVisitor();
	if (value.Equals(FVector2D::ZeroVector) || visitor == nullptr) return;

	//Move
	if (value.Y != 0.0f)
	{
		const FRotator yawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X));
		visitor->AddMovementInput(direction, value.Y);
	}
	if (value.X != 0.0f)
	{
		const FRotator yawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
		const FVector direction(FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y));
		visitor->AddMovementInput(direction, value.X);
	}
}

void AVisitorController::Zooming(float value, bool bZoomMap)
{
	if (_bPause) return;

	AVisitor* visitor = GetVisitor();
	if (value == 0.0f || visitor == nullptr || !_DataVisitorControl) return;

	if (bZoomMap)
	{
		AMinimaps* map = GetMap();
		if (map)
		{
			map->Zooming(value * _DataVisitorControl->mapZoomSpeed);
			return;
		}
	}
	else
	{
		UCameraComponent* followCamera = visitor->GetFollowCamera();
		if (!followCamera) return;

		//Zoom or unzoom the camera FOV
		float fov = followCamera->FieldOfView;
		fov += value * _DataVisitorControl->zoomSpeed;
		if (fov > visitor->GetBaseFov())
		{
			fov = visitor->GetBaseFov();
		}
		else if (fov < _DataVisitorControl->minFOV)
		{
			fov = _DataVisitorControl->minFOV;
		}

		followCamera->FieldOfView = fov;
	}
}

void AVisitorController::Taping(FVector2D localPos, FVector2D platformPos)
{
	if (_bPause) return;

	if (!_DataVisitorControl) return;

	//If the touch is on the map
	AMinimaps* map = GetMap();
	if (map)
	{
		if (map->CheckPointIsOnTheMap(platformPos))
		{
			map->Taping(platformPos);
			return;
		}
	}
	
	if (CheckPointIsOnWidget(platformPos)) return;

	//Trace a raycast in a click pos and dir
	UWorld* world = GetWorld();
	if (world)
	{
		FVector worldPos;
		FVector worldDir;
		DeprojectScreenPositionToWorld(localPos.X, localPos.Y, worldPos, worldDir);

		FVector startPos = worldPos;
		FVector endPos = worldPos + (worldDir * _DataVisitorControl->touchDistance);

		FHitResult hitResult;
		FCollisionQueryParams traceParams;
		bool bHit = world->LineTraceSingleByChannel(hitResult, startPos, endPos, ECC_Visibility, traceParams);
		if (bHit)
		{
			AInteractable* interactable = Cast<AInteractable>(hitResult.GetActor());
			if (interactable)
			{
				interactable->Interact();
			}

			//ATooltip* tooltip = Cast<ATooltip>(hitResult.GetActor());
			//if (tooltip)
			//{
			//	tooltip->Interact();
			//}
		}
	}
}

void AVisitorController::Looking(FVector2D value, bool bLookingMap)
{
	if (_bPause) return;

	AVisitor* visitor = GetVisitor();
	if (value.Equals(FVector2D::ZeroVector) || visitor == nullptr || !_DataVisitorControl) return;

	if (bLookingMap)
	{
		AMinimaps* map = GetMap();
		if (map)
		{
			map->Looking(value * _DataVisitorControl->mapLookSpeed);
		}
	}
	else
	{
		if (value.X != 0.0f)
		{
			float rotateValue = (value.X * _DataVisitorControl->lookSpeed.X) * (PlayerCameraManager->GetFOVAngle() / _DataVisitorControl->baseOrthoSizeForMovementScale);
			visitor->AddControllerYawInput(rotateValue);
		}
		if (value.Y != 0.0f)
		{
			float rotateValue = (value.Y * _DataVisitorControl->lookSpeed.Y) * (PlayerCameraManager->GetFOVAngle() / _DataVisitorControl->baseOrthoSizeForMovementScale);
			visitor->AddControllerPitchInput(rotateValue);
		}
	}
}

void AVisitorController::HideActor()
{
	UWorld* world = GetWorld();
	if (!world) return;

	HiddenActors.Empty();
	HiddenPrimitiveComponents.Empty();

	for (TSubclassOf<AActor> actorClass : _HiddenActorsClass)
	{
		TArray<AActor*> foundActor;
		UGameplayStatics::GetAllActorsOfClass(world, actorClass, foundActor);
		for (AActor* actor : foundActor)
		{
			HiddenActors.AddUnique(actor);

			TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
			actor->GetComponents(PrimitiveComponents);
			for (UPrimitiveComponent* Component : PrimitiveComponents)
			{
				HiddenPrimitiveComponents.AddUnique(Component);
			}
		}
	}

	AVisitor* visitor = GetVisitor();
	if (!visitor) return;

	TArray<TSubclassOf<AActor>> hiddenActorsClass = visitor->GetHiddenActorsClass();

	for (TSubclassOf<AActor> actorClass : hiddenActorsClass)
	{
		TArray<AActor*> foundActor;
		UGameplayStatics::GetAllActorsOfClass(world, actorClass, foundActor);
		for (AActor* actor : foundActor)
		{
			HiddenActors.AddUnique(actor);

			TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
			actor->GetComponents(PrimitiveComponents);
			for (UPrimitiveComponent* Component : PrimitiveComponents)
			{
				HiddenPrimitiveComponents.AddUnique(Component);
			}
		}
	}

	TArray<AActor*> hiddenActors = visitor->GetHiddenActors();
	for (AActor* actor : hiddenActors)
	{
		HiddenActors.AddUnique(actor);

		TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
		actor->GetComponents(PrimitiveComponents);
		for (UPrimitiveComponent* Component : PrimitiveComponents)
		{
			HiddenPrimitiveComponents.AddUnique(Component);
		}
	}
}

bool AVisitorController::CheckVisibility(UWidget* widget)
{
	if (!widget) return false;
	switch (widget->GetVisibility())
	{
	case ESlateVisibility::Collapsed:
		return false;
	case ESlateVisibility::Hidden:
		return false;
	case ESlateVisibility::HitTestInvisible:
		return false;
	case ESlateVisibility::SelfHitTestInvisible:
		return false;
	case ESlateVisibility::Visible:
		return true;
	default:
		return false;
	}
}

FVector2D AVisitorController::GetGameViewportSize()
{
	FVector2D Result = FVector2D(1, 1);

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize( /*out*/Result);
	}

	return Result;
}

FVector2D AVisitorController::GetGameResolution()
{
	FVector2D Result = FVector2D(1, 1);

	Result.X = GSystemResolution.ResX;
	Result.Y = GSystemResolution.ResY;

	return Result;
}

FVector2D AVisitorController::GetMousePos()
{
	FVector2D touchPos;
	GetMousePosition(touchPos.X, touchPos.Y);
	return touchPos;
}

FVector2D AVisitorController::GetMousePlatformPos()
{
	FVector2D touchPos = UWidgetLayoutLibrary::GetMousePositionOnPlatform();
	return touchPos;
}

bool AVisitorController::CheckPointIsOnWidget(FVector2D point)
{
	TArray<UUserWidget*> allWidgets;
	TSubclassOf<UUserWidget> widgetClass = UUserWidget::StaticClass();
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), allWidgets, widgetClass, false);

	for (UUserWidget* widget : allWidgets)
	{
		_TempIgnoreWidget.Empty();
		if (widget->IsInViewport() && CheckWidgetForPoint(widget, point))
		{
			return true;
		}
	}

	return false;
}

bool AVisitorController::CheckWidgetForPoint(UWidget* widget, FVector2D point)
{
	if (!widget) return false;

	if (_TempIgnoreWidget.Contains(widget)) return false;

	bool bIgnore = false;
	EIgnoreState IgnoreState = EIgnoreState::NotIngoreChild;

	if (_IgnoreWidget.Contains(widget))
	{
		bIgnore = true;
		IgnoreState = _IgnoreWidget[widget];
	}
	else if (_IgnoreWidgetClass.Contains(widget->GetClass()))
	{
		bIgnore = true;
		IgnoreState = _IgnoreWidgetClass[widget->GetClass()];
	}

	if (UUserWidget* userWidget = Cast<UUserWidget>(widget))
	{
		if (bIgnore && IgnoreState == EIgnoreState::IngoreChild) return false;

		TArray<UWidget*> childWidgets;
		userWidget->WidgetTree->GetAllWidgets(childWidgets);
		for (UWidget* child : childWidgets)
		{
			if (CheckWidgetForPoint(child, point))
			{
				return true;
			}
		}
		return false;
	}
	else if (UPanelWidget* panelWidget = Cast<UPanelWidget>(widget))
	{
		if (bIgnore && IgnoreState == EIgnoreState::IngoreChild)
		{
			TArray<UWidget*> ChildWidget = GetAllChildWidgetsRecursive(panelWidget);
			for (UWidget* Child : ChildWidget)
			{
				_TempIgnoreWidget.Add(Child, Child);
			}
			return false;
		}
	}

	if (bIgnore) return false;

	if (widget->GetCachedGeometry().IsUnderLocation(point))
	{
		return true;
	}

	return false;
}

bool AVisitorController::CheckPointIsOnTheMap(FVector2D point)
{
	AMinimaps* map = GetMap();
	if (map)
	{
		if (map->CheckPointIsOnTheMap(point))
		{
			return true;
		}
	}
	return false;
}

bool AVisitorController::CheckPointIsOnJoystick(FVector2D point)
{
	if (_JoystickWidget)
	{
		UImage* image = Cast<UImage>(_JoystickWidget->GetWidgetFromName("Image_Limit"));
		if (image)
		{
			float ray = image->GetCachedGeometry().GetAbsoluteSize().X / 2;
			FVector2D midPoint = image->GetCachedGeometry().GetAbsolutePosition() + ray;

			float dist = FVector2D::Distance(point, midPoint);

			if (FVector2D::Distance(point, midPoint) <= ray) return true;
		}
	}
	return false;
}

bool AVisitorController::CheckMapIsOpen()
{
	return true;
}

void AVisitorController::SetPauseState(bool value)
{
	_bPause = value;

	if (_DataVisitorControl && _DataVisitorControl->inputType == EInputType::IT_Touch)
	{
		if (value)
		{
			HideJoystickWidget();
		}
		else
		{
			InitJoystickWidget();
		}
	}
}

AMinimaps* AVisitorController::GetMap()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (!gameInstance) return nullptr;
	UGameInstanceSubsystemVisit* gameInstanceVisit =gameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
	if (!gameInstanceVisit) return nullptr;
	AMinimaps* map = Cast<AMinimaps>(gameInstanceVisit->GetInDictObject("Maps"));
	if (!map) return nullptr;
	return map;
}

TArray<UWidget*> AVisitorController::GetAllChildWidgetsRecursive(UPanelWidget* PanelWidget)
{
	TArray<UWidget*> Result;

	if (!PanelWidget)
	{
		return Result;
	}

	for (int32 i = 0; i < PanelWidget->GetChildrenCount(); i++)
	{
		UWidget* ChildWidget = PanelWidget->GetChildAt(i);
		Result.Add(ChildWidget);
		if (UPanelWidget* PW = Cast<UPanelWidget>(ChildWidget))
		{
			Result.Append(GetAllChildWidgetsRecursive(PW));
		}
	}

	return Result;
}
