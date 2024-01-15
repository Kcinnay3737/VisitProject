#include "Menu/OptionMenu.h"
#include "GameInstance/GameInstanceSubsystemVisit.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "UMG/Public/Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/ComboBoxString.h"
#include "UMG/Public/Components/CheckBox.h"
#include "UMG/Public/Components/CanvasPanel.h"
#include "Manager/GameStateManager.h"

AOptionMenu::AOptionMenu()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AOptionMenu::BeginPlay()
{
	Super::BeginPlay();

	_GameStateManager = Cast<AGameStateManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameStateManager::StaticClass()));
	if (_GameStateManager)
	{
		_GameStateManager->OnEnterGameState.AddDynamic(this, &AOptionMenu::EnableOptionButton);
		_GameStateManager->OnEnterStartMenuStat.AddDynamic(this, &AOptionMenu::EnableOptionButton);
		_GameStateManager->OnExitGameState.AddDynamic(this, &AOptionMenu::DisableOptionButton);
		_GameStateManager->OnExitStartMenuState.AddDynamic(this, &AOptionMenu::DisableOptionButton);
	}

	ABotMenu* BotMenu = Cast<ABotMenu>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotMenu::StaticClass()));
	if (BotMenu)
	{
		BotMenu->OnBotMenuStateChange.AddDynamic(this, &AOptionMenu::OnBotMenuStateChange);
	}

	InitOption();
	InitOptionWidget();
}

void AOptionMenu::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOptionMenu::InitOption()
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->InitOption();
}

void AOptionMenu::InitOptionWidget()
{
	if (!_OptionWidgetClass) return;
	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	if (!PlayerController) return;
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	FDataPerformance DataPerformance = GameInstanceSubsystemVisit->GetDataPerformance();

	_OptionWidget = CreateWidget<UUserWidget>(PlayerController, _OptionWidgetClass);
	if (!_OptionWidget) return;
	_OptionWidget->AddToViewport();

	_ButtonOption = Cast<UButton>(_OptionWidget->GetWidgetFromName("Button_Option"));
	if (_ButtonOption)
	{
		_ButtonOption->OnClicked.AddDynamic(this, &AOptionMenu::OnClickOption);
	}

	UComboBoxString* ComboBoxResolution = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_Resolution"));
	if (ComboBoxResolution)
	{
		ComboBoxResolution->SetSelectedOption(DataPerformance.resolution);

		ComboBoxResolution->OnSelectionChanged.AddDynamic(this, &AOptionMenu::OnComboBoxResolutionChanged);
	}

	UComboBoxString* ComboBoxWindowMode = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_WindowMode"));
	if (ComboBoxWindowMode)
	{
		ComboBoxWindowMode->SetSelectedOption(DataPerformance.windowMode);

		ComboBoxWindowMode->OnSelectionChanged.AddDynamic(this, &AOptionMenu::OnComboBoxWindowModeChanged);
	}

	USlider* SliderScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (SliderScreenPercent)
	{
		SliderScreenPercent->SetValue(DataPerformance.screenPercent / 100.0f);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(SliderScreenPercent->GetValue() * 100))));
		}

		SliderScreenPercent->SetIsEnabled(DataPerformance.useTSR);
		SliderScreenPercent->OnValueChanged.AddDynamic(this, &AOptionMenu::OnSliderScreenPercentChanged);
	}

	USlider* SliderSecondaryScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (SliderSecondaryScreenPercent)
	{
		SliderSecondaryScreenPercent->SetValue(DataPerformance.secondaryScreenPercent / 100);
		UTextBlock* textBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
		if (textBlockSliderValue)
		{
			textBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(SliderSecondaryScreenPercent->GetValue() * 100))));
		}

		SliderSecondaryScreenPercent->SetIsEnabled(DataPerformance.useTSR);
		SliderSecondaryScreenPercent->OnValueChanged.AddDynamic(this, &AOptionMenu::OnSliderSecondaryScreenPercentChanged);
	}

	UComboBoxString* ComboBoxFPS = Cast<UComboBoxString>(_OptionWidget->GetWidgetFromName("ComboBoxString_FPS"));
	if (ComboBoxFPS)
	{
		ComboBoxFPS->SetSelectedOption(DataPerformance.FPS);

		ComboBoxFPS->OnSelectionChanged.AddDynamic(this, &AOptionMenu::OnComboBoxFPSChanged);
	}

	UCheckBox* CheckBoxDLSS = Cast<UCheckBox>(_OptionWidget->GetWidgetFromName("CheckBox_DLSS"));
	if (CheckBoxDLSS)
	{
		CheckBoxDLSS->SetIsChecked(DataPerformance.useDLSS);

		CheckBoxDLSS->OnCheckStateChanged.AddDynamic(this, &AOptionMenu::OnCheckBoxDLSSChanged);
	}

	UCheckBox* CheckBoxTSR = Cast<UCheckBox>(_OptionWidget->GetWidgetFromName("CheckBox_TSR"));
	if (CheckBoxTSR)
	{
		CheckBoxTSR->SetIsChecked(DataPerformance.useTSR);

		CheckBoxTSR->OnCheckStateChanged.AddDynamic(this, &AOptionMenu::OnCheckBoxTSRChanged);
	}

	UCheckBox* CheckBoxVSYNC = Cast<UCheckBox>(_OptionWidget->GetWidgetFromName("CheckBox_VSYNC"));
	if (CheckBoxVSYNC)
	{
		CheckBoxVSYNC->SetIsChecked(DataPerformance.useVSYNC);

		CheckBoxVSYNC->OnCheckStateChanged.AddDynamic(this, &AOptionMenu::OnCheckBoxVSYNCChanged);
	}

	CloseOption();
}

void AOptionMenu::OnBotMenuStateChange(EBotMenuState BotMenuState)
{
	if (!_GameStateManager || _GameStateManager->GetGameState() != EGameState::Game) return;
	if (BotMenuState == EBotMenuState::Home)
	{
		EnableOptionButton();
	}
	else
	{
		DisableOptionButton();
	}
}

void AOptionMenu::CloseOption()
{
	if (!_bIsOpen) return;
	if (!_OptionWidget) return;
	UFunction* Function = _OptionWidget->FindFunction(FName("CloseOption"));
	if (Function)
	{
		_OptionWidget->ProcessEvent(Function, nullptr);
	}

	_bIsOpen = false;
	OnCloseOption.Broadcast();
}

void AOptionMenu::OpenOption()
{
	if (_bIsOpen) return;
	if (!_OptionWidget) return;
	UFunction* Function = _OptionWidget->FindFunction(FName("OpenOption"));
	if (Function)
	{
		_OptionWidget->ProcessEvent(Function, nullptr);
	}

	_bIsOpen = true;
	OnOpenOption.Broadcast();
}

void AOptionMenu::OnClickOption()
{
	if (_bIsOpen)
	{
		CloseOption();
	}
	else
	{
		OpenOption();
	}
}

void AOptionMenu::DisableOptionButton()
{
	CloseOption();
	if (!_OptionWidget) return;
	UFunction* Function = _OptionWidget->FindFunction(FName("DisableOptionButton"));
	if (Function)
	{
		_OptionWidget->ProcessEvent(Function, nullptr);
	}
}

void AOptionMenu::EnableOptionButton()
{
	if (!_OptionWidget) return;
	UFunction* Function = _OptionWidget->FindFunction(FName("EnableOptionButton"));
	if (Function)
	{
		_OptionWidget->ProcessEvent(Function, nullptr);
	}
}

void AOptionMenu::OnComboBoxResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetResolution(SelectedItem);
}

void AOptionMenu::OnSliderScreenPercentChanged(float Value)
{
	USlider* Slider = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (Slider)
	{
		Slider->SetValue(Value);
	}

	UTextBlock* TextBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueScreenPercent"));
	if (TextBlockSliderValue)
	{
		TextBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(Value * 100))));
	}

	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetScreenPercent(Value * 100);
}

void AOptionMenu::OnSliderSecondaryScreenPercentChanged(float Value)
{
	USlider* Slider = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (Slider)
	{
		Slider->SetValue(Value);
	}

	UTextBlock* TextBlockSliderValue = Cast<UTextBlock>(_OptionWidget->GetWidgetFromName("TextBlock_SliderValueSecondaryScreenPercent"));
	if (TextBlockSliderValue)
	{
		TextBlockSliderValue->SetText(FText::FromString(FString::Printf(TEXT("%d"), static_cast<int>(Value * 100))));
	}

	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetSecondaryScreenPercent(Value * 100);
}

void AOptionMenu::OnComboBoxWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetWindowMode(SelectedItem);
}

void AOptionMenu::OnComboBoxFPSChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetFPS(SelectedItem);
}

void AOptionMenu::OnCheckBoxDLSSChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetDLSS(bIsChecked);
}

void AOptionMenu::OnCheckBoxTSRChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	USlider* SliderScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_ScreenPercent"));
	if (SliderScreenPercent)
	{
		if (!bIsChecked)
		{
			SliderScreenPercent->SetValue(1.0f);
			OnSliderScreenPercentChanged(1.0f);
		}
		else
		{
			SliderScreenPercent->SetValue(0.8f);
			OnSliderScreenPercentChanged(0.8f);
		}
		SliderScreenPercent->SetIsEnabled(bIsChecked);
	}
	USlider* SliderSecondaryScreenPercent = Cast<USlider>(_OptionWidget->GetWidgetFromName("Slider_SecondaryScreenPercent"));
	if (SliderSecondaryScreenPercent)
	{
		if (!bIsChecked)
		{
			SliderSecondaryScreenPercent->SetValue(1.0f);
			OnSliderSecondaryScreenPercentChanged(1.0f);
		}
		else
		{
			SliderSecondaryScreenPercent->SetValue(0.8f);
			OnSliderSecondaryScreenPercentChanged(0.8f);
		}
		SliderSecondaryScreenPercent->SetIsEnabled(bIsChecked);
	}

	GameInstanceSubsystemVisit->SetTSR(bIsChecked);
}

void AOptionMenu::OnCheckBoxVSYNCChanged(bool bIsChecked)
{
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GetGameInstanceSubsystemVisit();
	if (!GameInstanceSubsystemVisit) return;

	GameInstanceSubsystemVisit->SetVSYNC(bIsChecked);
}

UGameInstanceSubsystemVisit* AOptionMenu::GetGameInstanceSubsystemVisit()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance) return nullptr;
	UGameInstanceSubsystemVisit* GameInstanceSubsystemVisit = GameInstance->GetSubsystem<UGameInstanceSubsystemVisit>();
	if (!GameInstanceSubsystemVisit) return nullptr;

	return GameInstanceSubsystemVisit;
}

bool AOptionMenu::GetIsOpen()
{
	return _bIsOpen;
}
