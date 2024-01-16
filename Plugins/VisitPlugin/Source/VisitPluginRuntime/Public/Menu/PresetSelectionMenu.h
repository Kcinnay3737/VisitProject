#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PresetSelectionMenu.generated.h"

class UUserWidget;
class ABotMenu;
class UDataAssetProduct;
class UDataAssetProvider;
class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FPresetParams
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FString ProductName = "";
	UPROPERTY(EditAnywhere)
	UTexture2D* ProductImage = nullptr;
	UPROPERTY(EditAnywhere)
	FString ProductDescription = "";
	UPROPERTY(EditAnywhere)
	FString ProviderName = "";
	UPROPERTY(EditAnywhere)
	UTexture2D* ProviderImage = nullptr;
	UPROPERTY(EditAnywhere)
	FString ProviderDescription = "";
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FHightlightPresetButtonParam
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int Index = 0;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FHidePresetButtonParam
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bHide = false;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FActorPreset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<AActor*> ActorInPreset;
	UPROPERTY(EditAnywhere)
	FPresetParams PresetData = FPresetParams();
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FPreset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FActorPreset> Presets;
	
	int CurrPreset = 0;
};

UCLASS()
class VISITPLUGINRUNTIME_API APresetSelectionMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _PresetSelectionWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _PresetSelectionWidget = nullptr;

	UPROPERTY()
	ABotMenu* _BotMenu = nullptr;

	UPROPERTY(EditAnywhere, Category = "VisitData/Preset")
	TArray<FPreset> _AllPreset;

	UPROPERTY()
	TMap<AActor*, int> _AllActorPreset;

	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UMaterialInterface* _OverlayMaterial = nullptr;

	int _CurrentPresetIndex = 0;

public:	
	APresetSelectionMenu();

	virtual void Tick(float DeltaTime) override;

	void OnTouchActor(AActor* Actor);

	void ShowPreset(int PresetIndex, int ActorPresetIndex);

	void SetPresetSelected(int PresetIndex);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBotMenuStateChanged(EBotMenuState BotMenuState);

	void InitWidget();
	void OpenMenu();
	void CloseMenu();

	void UpdateUI();
	void UpdateSelectedPresetButton();

	UFUNCTION()
	void OnSelectPresetOne();
	UFUNCTION()
	void OnSelectPresetTwo();
};
