#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Minimaps.generated.h"

class UImage;
class UPaperSpriteComponent;
class USceneCaptureComponent2D;
class USceneComponent;
class UUserWidget;

UENUM(BlueprintType)
enum class EMapState : uint8 
{	
	Minimap UMETA(DisplayName = "Minimap"),
	Bigmap UMETA(DisplayName = "Bigmap")
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FSpriteVisitor1
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float OffsetRotation = 0.0f;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataHiddenMap1
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> HiddenActorsClass;
	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> HiddenActors;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataHeightCam1
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector PlayerHeight = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector CameraHeight = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FDataHiddenMap1 HiddenActor;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bJustForBigmap"))
	bool bJustForMinimap = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bJustForMinimap"))
	bool bJustForBigmap = false;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataMap1
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
class VISITPLUGINRUNTIME_API AMinimaps : public AActor
{
	GENERATED_BODY()
	
public:	
	AMinimaps();
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool CheckPointIsOnTheMap(FVector2D Point);

	void Looking(FVector2D value);
	void Zooming(float value);
	void Taping(FVector2D textureClickPosition);

	UFUNCTION(BlueprintCallable)
	void OnOpenMinimap();
	UFUNCTION(BlueprintCallable)
	void OnOpenBigmap();
	UFUNCTION(BlueprintCallable)
	void SetCaptureState(bool State);

protected:
	virtual void BeginPlay() override;

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
	TSubclassOf<UUserWidget> _MinimapWidgetClass;
	UPROPERTY()
	UUserWidget* _CurrMinimapWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _BigmapWidgetClass;
	UPROPERTY()
	UUserWidget* _CurrBigmapWidget = nullptr;

	//Player
	UPROPERTY()
	APlayerController* _PlayerController = nullptr;

	//Material
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UMaterialInterface* _MaterialMinimap = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UTextureRenderTarget2D* _RenderTargetMinimap = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UMaterialInterface* _MaterialBigmap = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Material")
	UTextureRenderTarget2D* _RenderTargetBigmap = nullptr;

	//ParamsMaps
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataMap1 _DataMinimap;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataMap1 _DataBigmap;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataHiddenMap1 _DataHiddenActor;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataHiddenMap1 _DataHiddenActorMinimap;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FDataHiddenMap1 _DataHiddenActorBigmap;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	TArray<FDataHeightCam1> _DataHeightCam;
	TMap<EMapState, TArray<FDataHeightCam1>> _DataHeightCamDict;
	UPROPERTY(EditAnywhere, Category = "VisitData/MapsParams")
	FSpriteVisitor1 _ViewVisitor;

	bool _bCaptureState = false;

	void CheckChangeResolution();

	EMapState _MapState = EMapState::Minimap;
	FVector2D _OldImageSize = FVector2D::ZeroVector;
	UImage* _MinimapImage = nullptr;
	UImage* _BigmapImage = nullptr;


	void InitMinimapWidget();
	void InitBigmapWidget();
	void InitMainWidget();

	void HideActor();

	float _CurrPosZ = 0.0f;
	int _CurrHeightCamIndex = -1;
	bool _FollowPlayer = false;
	bool _StepSetManually = false;

	void SetFollowPlayer(bool value);
	void FollowPlayer();
	void CheckCameraHeight();

	UFUNCTION()
	void StepUp();
	UFUNCTION()
	void StepDown();
	UFUNCTION()
	void Recenter();
	
	void UdpateTextCurrState();

public:
	FDataMap1 GetCurrentMapData();

};
