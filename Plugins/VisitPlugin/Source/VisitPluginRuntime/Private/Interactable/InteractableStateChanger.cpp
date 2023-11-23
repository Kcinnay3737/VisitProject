#include "Interactable/InteractableStateChanger.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/LightComponent.h"
#include "Components/Slider.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Kismet/GameplayStatics.h"

AInteractableStateChanger::AInteractableStateChanger() : Super::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	_InteractableComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Interactable"));
	_InteractableComponent->SetupAttachment(_SceneRootComponent);

	_StateChangerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("StateChanger"));
	_StateChangerSceneComponent->SetupAttachment(_SceneRootComponent);
}

void AInteractableStateChanger::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> stateChangerSceneComponents;
	_StateChangerSceneComponent->GetChildrenComponents(true, stateChangerSceneComponents);
	for (USceneComponent* sceneComponent : stateChangerSceneComponents)
	{
		UPrimitiveComponent* primitiveComponent = Cast<UPrimitiveComponent>(sceneComponent);
		if (primitiveComponent)
		{
			primitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		}
	}

	if (_bActiveStateChanger && !_bWithIntensity)
	{
		SetEnableComponent(_bEnableAtStartup);
	}
	else if (_bActiveStateChanger && _bWithIntensity)
	{
		SetIntensity(_BaseIntensity);
	}
}

void AInteractableStateChanger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableStateChanger::Interact()
{
	if (_bActiveStateChanger && !_bWithIntensity)
	{
		SetEnableComponent(!_bCurrState);
	}
	else if (_bActiveStateChanger && _bWithIntensity && !_bWidgetIntensityIsOpen)
	{
		OpenIntensityWidget();
	}
}

void AInteractableStateChanger::OpenIntensityWidget()
{
	_bWidgetIntensityIsOpen = true;
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		UGameInstanceSubsystemVisit* gameInstanceVisit = gameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
		if (gameInstanceVisit)
		{
			gameInstanceVisit->_DelegateCloseOtherStateChanger.Broadcast();
			gameInstanceVisit->_DelegateCloseOtherStateChanger.AddDynamic(this, &AInteractableStateChanger::CloseIntensityWidget);
		}
	}

	if (_IntensityWidget)
	{
		_IntensityWidget->AddToViewport();
		return;
	}

	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = world->GetFirstPlayerController();
	if (!playerController) return;
	if (!_IntensityWidgetClass) return;

	_IntensityWidget = CreateWidget<UUserWidget>(playerController, _IntensityWidgetClass);
	if (!_IntensityWidget) return;
	_IntensityWidget->AddToViewport();

	UButton* buttonQuit = Cast<UButton>(_IntensityWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &AInteractableStateChanger::CloseIntensityWidget);
	}

	USlider* sliderIntensity = Cast<USlider>(_IntensityWidget->GetWidgetFromName("Slider_Intensity"));
	if (sliderIntensity)
	{
		sliderIntensity->SetValue((_BaseIntensity - _MinIntensity) / (_MaxIntensity - _MinIntensity));
		sliderIntensity->OnValueChanged.AddDynamic(this, &AInteractableStateChanger::OnSliderIntensityChanged);
	}
}

void AInteractableStateChanger::CloseIntensityWidget()
{
	if (_IntensityWidget && _IntensityWidget->IsInViewport())
	{
		_IntensityWidget->RemoveFromParent();
	}

	_bWidgetIntensityIsOpen = false;
	UGameInstance* gameInstance = GetGameInstance();
	if (gameInstance)
	{
		UGameInstanceSubsystemVisit* gameInstanceVisit = gameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
		if (gameInstanceVisit)
		{
			gameInstanceVisit->_DelegateCloseOtherStateChanger.RemoveDynamic(this, &AInteractableStateChanger::CloseIntensityWidget);
		}
	}
}

void AInteractableStateChanger::AddStartLabelInChangeStateActor()
{
#if WITH_EDITOR
	if (_bActiveStateChanger)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			TArray<AActor*> foundActors;
			UGameplayStatics::GetAllActorsOfClass(world, AActor::StaticClass(), foundActors);
			for (AActor* actor : foundActors)
			{
				FString actorLabel = actor->GetActorLabel();
				for (FString startLabel : _ChangeStateStartLabel)
				{
					if (actorLabel.StartsWith(startLabel))
					{
						if (!_ChangeStateActor.Contains(actor))
						{
							_ChangeStateActor.Add(actor);
						}
					}
				}
			}
		}
	}
#endif
}

void AInteractableStateChanger::SetEnableComponent(bool state)
{
	TArray<USceneComponent*> children;
	_StateChangerSceneComponent->GetChildrenComponents(true, children);
	for (USceneComponent* child : children)
	{
		if (!child) continue;
		child->SetActive(state);
		child->SetVisibility(state);
	}

	for (AActor* actor : _ChangeStateActor)
	{
		if (!actor) continue;
		TArray<USceneComponent*> sceneComponent;
		actor->GetComponents<USceneComponent>(sceneComponent);
		for (USceneComponent* component : sceneComponent)
		{
			if (!component) continue;
			component->SetActive(state);
			component->SetVisibility(state);
		}
	}

	_bCurrState = state;
}

void AInteractableStateChanger::OnSliderIntensityChanged(float value)
{
	if (!_IntensityWidget) return;
	USlider* sliderIntensity = Cast<USlider>(_IntensityWidget->GetWidgetFromName("Slider_Intensity"));
	if (sliderIntensity)
	{
		sliderIntensity->SetValue(value);
	}

	SetIntensity((value * (_MaxIntensity - _MinIntensity)) + _MinIntensity);
}

void AInteractableStateChanger::SetIntensity(float value)
{
	TArray<USceneComponent*> children;
	_StateChangerSceneComponent->GetChildrenComponents(true, children);
	for (USceneComponent* child : children)
	{
		ULightComponent* lightComponent = Cast<ULightComponent>(child);
		if (lightComponent)
		{
			lightComponent->SetIntensity(value);
		}
	}

	for (AActor* actor : _ChangeStateActor)
	{
		TArray<USceneComponent*> sceneComponent;
		actor->GetComponents<USceneComponent>(sceneComponent);
		for (USceneComponent* component : sceneComponent)
		{
			ULightComponent* lightComponent = Cast<ULightComponent>(component);
			if (lightComponent)
			{
				lightComponent->SetIntensity(value);
			}
		}
	}
}