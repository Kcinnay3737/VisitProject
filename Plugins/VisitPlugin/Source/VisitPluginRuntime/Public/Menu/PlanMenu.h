#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Menu/BotMenu.h"
#include "Interactable/ButtonIndex.h"
#include "Interactable/IButtonIndexReceiver.h"
#include "PlanMenu.generated.h"

class UImage;
class UPaperSpriteComponent;
class USceneCaptureComponent2D;
class USceneComponent;
class UUserWidget;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FParamsFloorHightLight
{
	GENERATED_BODY()
public:
	UPROPERTY()
	int Index = 0;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FSpriteVisitor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float OffsetRotation = 0.0f;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataHiddenMap
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> HiddenActorsClass;
	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> HiddenActors;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataHeightCam
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FText FloorName = FText::GetEmpty();
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector PlayerHeight = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector CameraHeight = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FDataHiddenMap HiddenActor;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataMap
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float BaseViewSize = 1000.0f;
	//UPROPERTY(EditAnywhere)
	//float MinViewSize = 20.0f;
	//UPROPERTY(EditAnywhere)
	//float MaxViewSize = 3000.0f;
	//UPROPERTY(EditAnywhere)
	//float _BaseOrthoSizeForMovementScale = 1000.0f;
	UPROPERTY(EditAnywhere)
	bool bUseFixedLocation = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseFixedLocation"))
	FVector2D FixedPosition = FVector2D::ZeroVector;
	UPROPERTY(EditAnywhere)
	bool bCanClick = false;
};

UCLASS()
class VISITPLUGINRUNTIME_API APlanMenu : public AActor, public IButtonIndexReceiver
{
	GENERATED_BODY()
	
private:
	//Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _RootSceneComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneCaptureComponent2D* _MapsCameraView;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _SpriteComponentPlayerView;

	//Widget
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _PlanWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _PlanWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _FloorSlotWidgetClass = nullptr;
	UPROPERTY()
	TArray<UButtonIndex*> _ButtonIndex;

	//Player
	UPROPERTY()
	APlayerController* _PlayerController = nullptr;

	//Material
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UMaterialInterface* _MaterialPlan = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UTextureRenderTarget2D* _RenderTargetPlan = nullptr;

	//ParamsMaps
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataMap _DataPlan;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataHiddenMap _DataHiddenActor;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	TArray<FDataHeightCam> _DataHeightCam;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FSpriteVisitor _ViewVisitor;

	bool _bCaptureState = false;

	UImage* _ImagePlan = nullptr;

	float _CurrPosZ = 0.0f;
	int _CurrHeightCamIndex = -1;
	int _LastHeightCamIndex = -1;
	bool _FollowPlayer = false;
	bool _StepSetManually = false;

	FVector2D _LastImageSize = FVector2D::ZeroVector;

public:
	APlanMenu();
	virtual void Tick(float DeltaTime) override;

	//---------

	bool CheckPointIsOnTheMap(FVector2D Point);

	void Looking(FVector2D value);
	void Zooming(float value);
	void Taping(FVector2D textureClickPosition);

	UFUNCTION(BlueprintCallable)
	void SetCaptureState(bool State);

	virtual void OnButtonIndexClick(FString Type, int Index) override;

protected:
	virtual void BeginPlay() override;

private:	
	UFUNCTION()
	void OnBotMenuStateChange(EBotMenuState BotMenuState);

	void InitWidget();
	void OpenPlanMenu();
	void ClosePlanMenu();

	void HideActor();

	void UpdateTextureSize();

	void SetFollowPlayer(bool value);
	void FollowPlayer();
	void CheckCameraHeight();

	UFUNCTION()
	void StepUp();
	UFUNCTION()
	void StepDown();
	UFUNCTION()
	void Recenter();

public:
	FDataMap GetCurrentMapData();
};
