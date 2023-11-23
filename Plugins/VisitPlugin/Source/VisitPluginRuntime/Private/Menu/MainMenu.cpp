#include "Menu/MainMenu.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Kismet/GameplayStatics.h"

AMainMenu::AMainMenu()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainMenu::BeginPlay()
{
	Super::BeginPlay();

	InitOption();
	
	SetBackground();

	OpenMenu();
}

void AMainMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UGameInstanceSubsystemVisit* AMainMenu::GetGameInstanceSubsystemVisit()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (!gameInstance) return nullptr;
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = gameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
	if (!gameInstanceSubsystemVisit) return nullptr;

	return gameInstanceSubsystemVisit;
}

void AMainMenu::InitOption()
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->InitOption();
}

void AMainMenu::SetBackground()
{
	if (!_BackgroundClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	_BackgroundWidget = CreateWidget<UUserWidget>(playerController, _BackgroundClass);
	if (!_BackgroundWidget) return;
	_BackgroundWidget->AddToViewport();

	if (_ImageBackground)
	{
		UImage* BackgroundImage = Cast<UImage>(_BackgroundWidget->GetWidgetFromName(FName("Image_Background")));
		if (BackgroundImage)
		{
			BackgroundImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			BackgroundImage->SetBrushFromTexture(_ImageBackground);
		}
	}
}

void AMainMenu::OpenMenu()
{
	CloseOption();

	if (!_MenuWidgetClass) return;

	if (_MenuWidget)
	{
		_MenuWidget->AddToViewport();
		return;
	}

	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	playerController->SetShowMouseCursor(_bShowMouseCursor);

	_MenuWidget = CreateWidget<UUserWidget>(playerController, _MenuWidgetClass);
	if (!_MenuWidget) return;
	_MenuWidget->AddToViewport();

	UButton* buttonStart = Cast<UButton>(_MenuWidget->GetWidgetFromName("Button_Start"));
	if (buttonStart)
	{
		buttonStart->OnClicked.AddDynamic(this, &AMainMenu::StartLevel);
	}

	UButton* buttonQuit = Cast<UButton>(_MenuWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &AMainMenu::QuitApplication);
	}

	UButton* buttonOption = Cast<UButton>(_MenuWidget->GetWidgetFromName("Button_Option"));
	if (buttonOption)
	{
		buttonOption->OnClicked.AddDynamic(this, &AMainMenu::OpenOption);
	}
}

void AMainMenu::CloseMenu()
{
	if (!_MenuWidget) return;

	_MenuWidget->RemoveFromParent();
}

void AMainMenu::StartLevel()
{
	UGameplayStatics::OpenLevel(this, _StartLevelName);
}

void AMainMenu::QuitApplication()
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	UKismetSystemLibrary::QuitGame(world, playerController, EQuitPreference::Quit, true);
}

void AMainMenu::OpenOption()
{
	CloseMenu();

	if (!_OptionWidgetClass) return;

	if (_OptionWidget)
	{
		_OptionWidget->AddToViewport();
		return;
	}

	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	FDataPerformance dataPerformance = gameInstanceSubsystemVisit->GetDataPerformance();

	_OptionWidget = CreateWidget<UUserWidget>(playerController, _OptionWidgetClass);
	if (!_OptionWidget) return;
	_OptionWidget->AddToViewport();

	UButton* buttonQuit = Cast<UButton>(_OptionWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &AMainMenu::OpenMenu);
	}

	UComboBoxString* comboBoxResolution = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_Resolution"));
	if (comboBoxResolution)
	{
		comboBoxResolution->SetSelectedOption(dataPerformance.resolution);

		comboBoxResolution->OnSelectionChanged.AddDynamic(this, &AMainMenu::OnComboBoxResolutionChanged);
	}

	UComboBoxString* comboBoxWindowMode = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_WindowMode"));
	if (comboBoxWindowMode)
	{
		comboBoxWindowMode->SetSelectedOption(dataPerformance.windowMode);

		comboBoxWindowMode->OnSelectionChanged.AddDynamic(this, &AMainMenu::OnComboBoxWindowModeChanged);
	}

	USlider* sliderScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (sliderScreenPercent)
	{
		sliderScreenPercent->SetValue(dataPerformance.screenPercent / 100);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(sliderScreenPercent->GetValue() * 100))));
		}

		sliderScreenPercent->SetIsEnabled(dataPerformance.useTSR);
		sliderScreenPercent->OnValueChanged.AddDynamic(this, &AMainMenu::OnSliderScreenPercentChanged);
	}

	USlider* sliderSecondaryScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (sliderSecondaryScreenPercent)
	{
		sliderSecondaryScreenPercent->SetValue(dataPerformance.secondaryScreenPercent / 100);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(sliderSecondaryScreenPercent->GetValue() * 100))));
		}

		sliderSecondaryScreenPercent->SetIsEnabled(dataPerformance.useTSR);
		sliderSecondaryScreenPercent->OnValueChanged.AddDynamic(this, &AMainMenu::OnSliderSecondaryScreenPercentChanged);
	}

	UComboBoxString* comboBoxFPS = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_FPS"));
	if (comboBoxFPS)
	{
		comboBoxFPS->SetSelectedOption(dataPerformance.FPS);

		comboBoxFPS->OnSelectionChanged.AddDynamic(this, &AMainMenu::OnComboBoxFPSChanged);
	}

	UCheckBox* checkBoxTSR = Cast<UCheckBox>(_OptionWidget->GetWidgetFromName("CheckBox_TSR"));
	if (checkBoxTSR)
	{
		checkBoxTSR->SetIsChecked(dataPerformance.useTSR);

		checkBoxTSR->OnCheckStateChanged.AddDynamic(this, &AMainMenu::OnCheckBoxTSRChanged);
	}

	UCheckBox* checkBoxVSYNC = Cast<UCheckBox>(_OptionWidget->GetWidgetFromName("CheckBox_VSYNC"));
	if (checkBoxVSYNC)
	{
		checkBoxVSYNC->SetIsChecked(dataPerformance.useVSYNC);

		checkBoxVSYNC->OnCheckStateChanged.AddDynamic(this, &AMainMenu::OnCheckBoxVSYNCChanged);
	}
}

void AMainMenu::CloseOption()
{
	if (!_OptionWidget) return;

	_OptionWidget->RemoveFromParent();
}

void AMainMenu::OnComboBoxResolutionChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;
	
	gameInstanceSubsystemVisit->SetResolution(selectedItem);
}

void AMainMenu::OnSliderScreenPercentChanged(float value)
{
	USlider* slider = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (slider)
	{
		slider->SetValue(value);
	}

	UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
	if (textBlockSliderValue)
	{
		textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value * 100))));
	}

	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetScreenPercent(value * 100);
}

void AMainMenu::OnSliderSecondaryScreenPercentChanged(float value)
{
	USlider* slider = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (slider)
	{
		slider->SetValue(value);
	}

	UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
	if (textBlockSliderValue)
	{
		textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value * 100))));
	}

	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetSecondaryScreenPercent(value * 100);
}

void AMainMenu::OnComboBoxWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetWindowMode(SelectedItem);
}

void AMainMenu::OnComboBoxFPSChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetFPS(selectedItem);
}

void AMainMenu::OnCheckBoxTSRChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	USlider* sliderScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (sliderScreenPercent)
	{
		if (!bIsChecked)
		{
			sliderScreenPercent->SetValue(1.0f);
			OnSliderScreenPercentChanged(1.0f);
		}
		else
		{
			sliderScreenPercent->SetValue(0.8f);
			OnSliderScreenPercentChanged(0.8f);
		}
		sliderScreenPercent->SetIsEnabled(bIsChecked);
	}
	USlider* sliderSecondaryScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (sliderSecondaryScreenPercent)
	{
		if (!bIsChecked)
		{
			sliderSecondaryScreenPercent->SetValue(1.0f);
			OnSliderSecondaryScreenPercentChanged(1.0f);
		}
		else
		{
			sliderSecondaryScreenPercent->SetValue(0.8f);
			OnSliderSecondaryScreenPercentChanged(0.8f);
		}
		sliderSecondaryScreenPercent->SetIsEnabled(bIsChecked);
	}

	gameInstanceSubsystemVisit->SetTSR(bIsChecked);
}

void AMainMenu::OnCheckBoxVSYNCChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetVSYNC(bIsChecked);
}
