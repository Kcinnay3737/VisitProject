#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable/ButtonIndex.h"
#include "Teleporter.generated.h"

class USceneComponent;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FTeleportData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* Thumbnail = nullptr;
	UPROPERTY(EditAnywhere)
	FText PlaceName = FText::GetEmpty();
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector TeleportLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FString Floor = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API ATeleporter : public AActor, public IButtonIndexReceiver
{
	GENERATED_BODY()
	
public:	
	ATeleporter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnClickTeleport(int index);
	UFUNCTION()
	virtual void OnClickView(int index);
	UFUNCTION()
	virtual void OnClickFloor(int index);

	virtual void OnButtonIndexClick(FString Type, int Index) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;

	UPROPERTY(EditAnywhere, Category = "VisitData/TeleportPoint")
	TArray<FTeleportData> _TeleportData;
	UPROPERTY(EditAnywhere, Category = "VisitData/Floor")
	TArray<FString> _Floors;

	UPROPERTY()
	TArray<UButtonIndex*> _ButtonFloor;
	UPROPERTY()
	TArray<UButtonIndex*> _ButtonTeleport;

private:	
	

};
