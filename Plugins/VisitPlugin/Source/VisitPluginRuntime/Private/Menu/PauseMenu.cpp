#include "Menu/PauseMenu.h"

#include "Blueprint/UserWidget.h"
#include "Character/VisitorController.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Kismet/GameplayStatics.h"

APauseMenu::APauseMenu()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APauseMenu::BeginPlay()
{
	Super::BeginPlay();

	InitOption();

	OpenHideMenu();
}

void APauseMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APauseMenu::InitOption()
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->InitOption();
}

void APauseMenu::OpenHideMenu()
{
	SetPauseState(false);
	CloseBackground();

	CloseOption();
	CloseWorldSetting();
	CloseMenu();
	
	if (_HideMenuWidget)
	{
		_HideMenuWidget->AddToViewport();
		return;
	}

	if (!_HideMenuWidgetClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	_HideMenuWidget = CreateWidget<UUserWidget>(playerController, _HideMenuWidgetClass);
	if (!_HideMenuWidget) return;
	_HideMenuWidget->AddToViewport();

	UButton* buttonPauseMenu = Cast<UButton>(_HideMenuWidget->GetWidgetFromName("Button_PauseMenu"));
	if (buttonPauseMenu)
	{
		buttonPauseMenu->OnClicked.AddDynamic(this, &APauseMenu::OpenMenu);
	}
}

void APauseMenu::CloseHideMenu()
{
	if (_HideMenuWidget && _HideMenuWidget->IsInViewport())
	{
		_HideMenuWidget->RemoveFromParent();
	}
}

void APauseMenu::OpenMenu()
{
	SetPauseState(true);
	OpenBackgound();

	CloseHideMenu();
	CloseOption();
	CloseWorldSetting();

	if (_PauseMenuWidget)
	{
		_PauseMenuWidget->AddToViewport();
		return;
	}

	if (!_PauseMenuWidgetClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	_PauseMenuWidget = CreateWidget<UUserWidget>(playerController, _PauseMenuWidgetClass);
	if (!_PauseMenuWidget) return;
	_PauseMenuWidget->AddToViewport();

	UButton* buttonResume = Cast<UButton>(_PauseMenuWidget->GetWidgetFromName("Button_Resume"));
	if (buttonResume)
	{
		buttonResume->OnClicked.AddDynamic(this, &APauseMenu::OpenHideMenu);
	}

	UButton* buttonOption = Cast<UButton>(_PauseMenuWidget->GetWidgetFromName("Button_Option"));
	if (buttonOption)
	{
		buttonOption->OnClicked.AddDynamic(this, &APauseMenu::OpenOption);
	}

	UButton* buttonQuit = Cast<UButton>(_PauseMenuWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &APauseMenu::Quit);
	}

	UButton* buttonWorldSetting = Cast<UButton>(_PauseMenuWidget->GetWidgetFromName("Button_WorldSettings"));
	if (buttonWorldSetting)
	{
		buttonWorldSetting->OnClicked.AddDynamic(this, &APauseMenu::OpenWorldSetting);
	}
}

void APauseMenu::CloseMenu()
{
	if (_PauseMenuWidget && _PauseMenuWidget->IsInViewport())
	{
		_PauseMenuWidget->RemoveFromParent();
	}
}

void APauseMenu::OpenOption()
{
	CloseMenu();
	CloseHideMenu();
	CloseWorldSetting();

	if (_OptionMenuWidget)
	{
		_OptionMenuWidget->AddToViewport();
		return;
	}

	if (!_OptionMenuWidgetClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	FDataPerformance dataPerformance = gameInstanceSubsystemVisit->GetDataPerformance();

	_OptionMenuWidget = CreateWidget<UUserWidget>(playerController, _OptionMenuWidgetClass);
	if (!_OptionMenuWidget) return;
	_OptionMenuWidget->AddToViewport();

	UButton* buttonQuit = Cast<UButton>(_OptionMenuWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &APauseMenu::OpenMenu);
	}

	UComboBoxString* comboBoxResolution = Cast<UComboBoxString>(_OptionMenuWidget->GetWidgetFromName("ComboBoxString_Resolution"));
	if (comboBoxResolution)
	{
		comboBoxResolution->SetSelectedOption(dataPerformance.resolution);

		comboBoxResolution->OnSelectionChanged.AddDynamic(this, &APauseMenu::OnComboBoxResolutionChanged);
	}

	UComboBoxString* comboBoxWindowMode = Cast<UComboBoxString>(_OptionMenuWidget->GetWidgetFromName("ComboBoxString_WindowMode"));
	if (comboBoxWindowMode)
	{
		comboBoxWindowMode->SetSelectedOption(dataPerformance.windowMode);

		comboBoxWindowMode->OnSelectionChanged.AddDynamic(this, &APauseMenu::OnComboBoxWindowModeChanged);
	}

	USlider* sliderScreenPercent = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (sliderScreenPercent)
	{
		sliderScreenPercent->SetValue(dataPerformance.screenPercent / 100.0f);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionMenuWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(sliderScreenPercent->GetValue() * 100))));
		}

		sliderScreenPercent->SetIsEnabled(dataPerformance.useTSR);
		sliderScreenPercent->OnValueChanged.AddDynamic(this, &APauseMenu::OnSliderScreenPercentChanged);
	}

	USlider* sliderSecondaryScreenPercent = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (sliderSecondaryScreenPercent)
	{
		sliderSecondaryScreenPercent->SetValue(dataPerformance.secondaryScreenPercent / 100);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionMenuWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(sliderSecondaryScreenPercent->GetValue() * 100))));
		}

		sliderSecondaryScreenPercent->SetIsEnabled(dataPerformance.useTSR);
		sliderSecondaryScreenPercent->OnValueChanged.AddDynamic(this, &APauseMenu::OnSliderSecondaryScreenPercentChanged);
	}

	UComboBoxString* comboBoxFPS = Cast<UComboBoxString>(_OptionMenuWidget->GetWidgetFromName("ComboBoxString_FPS"));
	if (comboBoxFPS)
	{
		comboBoxFPS->SetSelectedOption(dataPerformance.FPS);

		comboBoxFPS->OnSelectionChanged.AddDynamic(this, &APauseMenu::OnComboBoxFPSChanged);
	}

	UCheckBox* checkBoxTSR = Cast<UCheckBox>(_OptionMenuWidget->GetWidgetFromName("CheckBox_TSR"));
	if (checkBoxTSR)
	{
		checkBoxTSR->SetIsChecked(dataPerformance.useTSR);

		checkBoxTSR->OnCheckStateChanged.AddDynamic(this, &APauseMenu::OnCheckBoxTSRChanged);
	}

	UCheckBox* checkBoxVSYNC = Cast<UCheckBox>(_OptionMenuWidget->GetWidgetFromName("CheckBox_VSYNC"));
	if (checkBoxVSYNC)
	{
		checkBoxVSYNC->SetIsChecked(dataPerformance.useVSYNC);

		checkBoxVSYNC->OnCheckStateChanged.AddDynamic(this, &APauseMenu::OnCheckBoxVSYNCChanged);
	}
}

void APauseMenu::CloseOption()
{
	if (_OptionMenuWidget && _OptionMenuWidget->IsInViewport())
	{
		_OptionMenuWidget->RemoveFromParent();
	}
}

void APauseMenu::OpenWorldSetting()
{
	CloseMenu();
	CloseHideMenu();
	CloseOption();

	if (_WorldSettingWidget)
	{
		_WorldSettingWidget->AddToViewport();
		return;
	}
	
	if (!_WorldSettingWidgetClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	
	_WorldSettingWidget = CreateWidget<UUserWidget>(playerController, _WorldSettingWidgetClass);
	if (!_WorldSettingWidget) return;
	_WorldSettingWidget->AddToViewport();
	
	UButton* buttonQuit = Cast<UButton>(_WorldSettingWidget->GetWidgetFromName("Button_Quit"));
	if (buttonQuit)
	{
		buttonQuit->OnClicked.AddDynamic(this, &APauseMenu::OpenMenu);
	}

	USlider* sliderSolarTime = Cast<USlider>(_WorldSettingWidget->GetWidgetFromName("Slider_SolarTime"));
	if (sliderSolarTime)
	{
		GetSolarTime();
		sliderSolarTime->OnValueChanged.AddDynamic(this, &APauseMenu::OnSliderSolarTimeChanged);
	}
}

void APauseMenu::CloseWorldSetting()
{
	if (_WorldSettingWidget && _WorldSettingWidget->IsInViewport())
	{
		_WorldSettingWidget->RemoveFromParent();
	}
}

void APauseMenu::OpenBackgound()
{
	if (_BackgroundWidget)
	{
		_BackgroundWidget->AddToViewport();
		return;
	}

	if (!_BackgroudPauseWidgetClass) return;
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;

	_BackgroundWidget = CreateWidget<UUserWidget>(playerController, _BackgroudPauseWidgetClass);
	if (!_BackgroundWidget) return;
	_BackgroundWidget->AddToViewport();
}

void APauseMenu::CloseBackground()
{
	if (_BackgroundWidget && _BackgroundWidget->IsInViewport())
	{
		_BackgroundWidget->RemoveFromParent();
	}
}

void APauseMenu::Quit()
{
	UGameplayStatics::OpenLevel(this, _QuitLevelName);
}

UGameInstanceSubsystemVisit* APauseMenu::GetGameInstanceSubsystemVisit()
{
	UGameInstance* gameInstance = GetGameInstance();
	if (!gameInstance) return nullptr;
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = gameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
	if (!gameInstanceSubsystemVisit) return nullptr;

	return gameInstanceSubsystemVisit;
}

void APauseMenu::OnComboBoxResolutionChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetResolution(selectedItem);
}

void APauseMenu::OnSliderScreenPercentChanged(float value)
{
	USlider* slider = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (slider)
	{
		slider->SetValue(value);
	}

	UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionMenuWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
	if (textBlockSliderValue)
	{
		textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value * 100))));
	}

	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetScreenPercent(value * 100);
}

void APauseMenu::OnSliderSecondaryScreenPercentChanged(float value)
{
	USlider* slider = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (slider)
	{
		slider->SetValue(value);
	}

	UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionMenuWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
	if (textBlockSliderValue)
	{
		textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value * 100))));
	}

	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetSecondaryScreenPercent(value * 100);
}

void APauseMenu::OnComboBoxWindowModeChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetWindowMode(selectedItem);
}

void APauseMenu::OnComboBoxFPSChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetFPS(selectedItem);
}

void APauseMenu::OnCheckBoxTSRChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	USlider* sliderScreenPercent = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_ScreenPercent"));
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
	USlider* sliderSecondaryScreenPercent = Cast<USlider>(_OptionMenuWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
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

void APauseMenu::OnCheckBoxVSYNCChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* gameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceSubsystemVisit) return;

	gameInstanceSubsystemVisit->SetVSYNC(bIsChecked);
}

void APauseMenu::OnSliderSolarTimeChanged(float value)
{
	SetSolarTime(value);
	USlider* sliderSolarTime = Cast<USlider>(_WorldSettingWidget->GetWidgetFromName("Slider_SolarTime"));
	if (!sliderSolarTime) return;
	sliderSolarTime->SetValue(value);
	UTextBlock* textBlockSolarTimeValue = Cast<UTextBlock>(_WorldSettingWidget->GetWidgetFromName("TextBlock_SolarTimeValue"));
	if (!textBlockSolarTimeValue) return;
	textBlockSolarTimeValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value))));
}

void APauseMenu::ReceiveSolarTime(float value)
{
	if (!_WorldSettingWidget) return;
	USlider* sliderSolarTime = Cast<USlider>(_WorldSettingWidget->GetWidgetFromName("Slider_SolarTime"));
	if (!sliderSolarTime) return;
	sliderSolarTime->SetValue(value);
	UTextBlock* textBlockSolarTimeValue = Cast<UTextBlock>(_WorldSettingWidget->GetWidgetFromName("TextBlock_SolarTimeValue"));
	if (!textBlockSolarTimeValue) return;
	textBlockSolarTimeValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(value))));
}

void APauseMenu::SetPauseState(bool value)
{
	UWorld* world = GetWorld();
	if (!world) return;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(world, 0);
	if (!playerController) return;
	AVisitorController* visitorController = Cast<AVisitorController>(playerController);
	if (!visitorController) return;

	visitorController->SetPauseState(value);

	UGameInstanceSubsystemVisit* gameInstanceVisit = GetGameInstanceSubsystemVisit();
	if (!gameInstanceVisit) return;
	/*AMaps* maps = Cast<AMaps>(gameInstanceVisit->GetInDictObject("Map"));
	if (!maps) return;

	maps->SetPause(value);*/
}