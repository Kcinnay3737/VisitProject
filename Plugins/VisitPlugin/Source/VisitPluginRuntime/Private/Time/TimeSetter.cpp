#include "Time/TimeSetter.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/SceneComponent.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

ATimeSetter::ATimeSetter()
{
	PrimaryActorTick.bCanEverTick = true;

	_RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(_RootSceneComponent);
}

void ATimeSetter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATimeSetter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!_TimeWidget) InitWidget();
}

void ATimeSetter::InitWidget()
{
	if (!_TimeWidgetClass) return;
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, _TimeWidgetClass, false);
	if (FoundWidgets.Num() != 0)
	{
		_TimeWidget = FoundWidgets[0];
	}
	if (!_TimeWidget) return;

	_TextBlockTime = Cast<UTextBlock>(_TimeWidget->GetWidgetFromName(FName("TextBlock_Time")));

	_SliderTime = Cast<USlider>(_TimeWidget->GetWidgetFromName(FName("Slider_Time")));
	if (_SliderTime)
	{
		_SliderTime->SetMinValue(_MinimumTime);
		_SliderTime->SetMaxValue(_MaximumTime);
		_SliderTime->OnValueChanged.AddDynamic(this, &ATimeSetter::OnSliderTimeValueChange);

		SetInitialTime();
		SetTimeWithSliderValue();
	}
}

void ATimeSetter::OnSliderTimeValueChange(float Value)
{
	SetTime(Value);
	SetTimeWithSliderValue();
}

void ATimeSetter::SetTimeWithSliderValue()
{
	if (!_SliderTime || !_TextBlockTime) return;
	float SliderValue = _SliderTime->GetValue();
	
	int Hours = static_cast<int>(SliderValue);
	int Minutes = static_cast<int>((SliderValue - Hours) * 60);

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);

	_TextBlockTime->SetText(FText::FromString(TimeString));
}