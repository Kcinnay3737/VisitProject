#include "GameInstance/GameInstanceSubsystemVisit.h"

#include "Engine/GameViewportClient.h"
#include "GameFramework/GameUserSettings.h"
#include "Misc/DefaultValueHelper.h"
#include "Misc/Parse.h"
#include "Widgets/SViewport.h"
#include "Widgets/SWindow.h"

void UGameInstanceSubsystemVisit::AddInDictObject(FString key, UObject* value)
{
    if (!_DictObject.Contains(key))
    {
        _DictObject.Add(key, value);
    }
}

void UGameInstanceSubsystemVisit::RemoveInDictObject(FString key)
{
    if (_DictObject.Contains(key))
    {
        _DictObject.Remove(key);
    }
}

UObject* UGameInstanceSubsystemVisit::GetInDictObject(FString key)
{
    if (_DictObject.Contains(key))
    {
        return _DictObject[key];
    }
    return nullptr;
}

void UGameInstanceSubsystemVisit::SetResolution(FString value)
{
    UGameUserSettings* gameUserSettings = GEngine->GetGameUserSettings();
    if (gameUserSettings)
    {
        FString cmd;
        FIntPoint resolution = FIntPoint::ZeroValue;
     
        if (value.Equals("Auto"))
        {
            resolution = gameUserSettings->GetDesktopResolution();
            cmd = FString::Printf(TEXT("%dx%d"), resolution.X, resolution.Y);
        }
        else
        {
            TArray<FString> resolutionArray;
            value.ParseIntoArray(resolutionArray, TEXT("x"), true);
            if (resolutionArray.Num() == 2)
            {
                resolution.X = FCString::Atoi(*resolutionArray[0]);
                resolution.Y = FCString::Atoi(*resolutionArray[1]);
            }
            cmd = value;
        }

        if (GEngine)
        {
            TSharedPtr<SWindow> window = nullptr;
            UGameViewportClient* gameViewport = GEngine->GameViewport;
            if (gameViewport)
            {
                TSharedPtr<SViewport> viewport = gameViewport->GetGameViewportWidget();
                if (viewport.IsValid())
                {
                    window = FSlateApplication::Get().FindWidgetWindow(viewport.ToSharedRef());
                }
            }

            FWindowSizeLimits windowSizeLimits;
            windowSizeLimits.SetMaxWidth(resolution.X);
            windowSizeLimits.SetMaxHeight(resolution.Y);
            windowSizeLimits.SetMinWidth(resolution.X);
            windowSizeLimits.SetMinHeight(resolution.Y);
            
            FString command;
            if (_DataPerformance.windowMode.Equals("Fullscreen"))
            {      
                if (window)
                {
                    FIntPoint desktopResolution = gameUserSettings->GetDesktopResolution();
                    windowSizeLimits.SetMaxWidth(desktopResolution.X);
                    windowSizeLimits.SetMaxHeight(desktopResolution.Y);

                    
                }

                gameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
                command = FString::Printf(TEXT("r.SetRes %sf"), *cmd);

                if (window)
                {
                    window->Maximize();
                    window->SetSizeLimits(windowSizeLimits);
                    window->SetSizingRule(ESizingRule::FixedSize);
                }
            }
            else if (_DataPerformance.windowMode.Equals("Windowed"))
            {         
                if (window)
                {
                    window->SetSizeLimits(windowSizeLimits);
                    window->SetSizingRule(ESizingRule::FixedSize);
                }

                gameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
                command = FString::Printf(TEXT("r.SetRes %sw"), *cmd);
            }
                        
            gameUserSettings->ApplySettings(false);
            GEngine->Exec(NULL, *command);

            _DataPerformance.resolution = value;
        }
    }
}

void UGameInstanceSubsystemVisit::SetScreenPercent(float value)
{
    if (GEngine)
    {
        FString command = FString::Printf(TEXT("r.ScreenPercentage %f"), value);
        GEngine->Exec(NULL, *command);

        _DataPerformance.screenPercent = value;
    }
}

void UGameInstanceSubsystemVisit::SetSecondaryScreenPercent(float value)
{
    if (GEngine)
    {
        FString command = FString::Printf(TEXT("r.SecondaryScreenPercentage.GameViewport %f"), value);
        GEngine->Exec(NULL, *command);

        _DataPerformance.secondaryScreenPercent = value;
    }
}

void UGameInstanceSubsystemVisit::SetWindowMode(FString value)
{
    if (value.Equals("Fullscreen") || value.Equals("Windowed"))
    {
        _DataPerformance.windowMode = value;
        SetResolution(_DataPerformance.resolution);
    }
}

void UGameInstanceSubsystemVisit::SetFPS(FString value)
{
    UGameUserSettings* gameUserSettings = GEngine->GetGameUserSettings();
    if (gameUserSettings)
    {
        int32 FPSValue = 0;
        if (!value.Equals("Unlimited") && !FDefaultValueHelper::ParseInt(value, FPSValue))
        {
            
        }

        gameUserSettings->SetFrameRateLimit(FPSValue);
        gameUserSettings->ApplySettings(false);

        _DataPerformance.FPS = value;

        SetResolution(_DataPerformance.resolution);
    }
}

void UGameInstanceSubsystemVisit::SetTSR(bool value)
{
    if (value)
    {
            
    }
    else
    {
            
    }

    _DataPerformance.useTSR = value;
}

void UGameInstanceSubsystemVisit::SetVSYNC(bool value)
{
    UGameUserSettings* gameUserSettings = GEngine->GetGameUserSettings();
    if (gameUserSettings)
    {
        gameUserSettings->SetVSyncEnabled(value);
        gameUserSettings->ApplySettings(false);
        _DataPerformance.useVSYNC = value;

        SetResolution(_DataPerformance.resolution);
    }
}

FDataPerformance UGameInstanceSubsystemVisit::GetDataPerformance()
{
    return _DataPerformance;
}

void UGameInstanceSubsystemVisit::InitOption()
{
    if (!_bOptionIsInit)
    {
        SetWindowMode(_DataPerformance.windowMode);
        SetScreenPercent(_DataPerformance.screenPercent);
        SetSecondaryScreenPercent(_DataPerformance.secondaryScreenPercent);
        SetFPS(_DataPerformance.FPS);
        SetTSR(_DataPerformance.useTSR);
        SetVSYNC(_DataPerformance.useVSYNC);
        _bOptionIsInit = true;
    }
}

void UGameInstanceSubsystemVisit::SetCurrentAgent(UObject* agent)
{
    _CurrentAgent = agent;
}

UObject* UGameInstanceSubsystemVisit::GetCurrentAgent()
{
    return _CurrentAgent;
}
