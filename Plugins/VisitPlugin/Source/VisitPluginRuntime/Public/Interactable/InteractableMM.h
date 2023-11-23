#pragma once

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "InteractableMM.generated.h"

class AStaticMeshActor;
class UDataAssetCompanyProduct;
class UMaterial;
class UStaticMesh;
class UStaticMeshComponent;
class AInteractableMenu;
class ABotMenu;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataLocation
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FVector BaseLocation = FVector::ZeroVector;
	UPROPERTY()
	FRotator BaseRotation = FRotator::ZeroRotator;
	UPROPERTY()
	FVector BaseScale = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FVector OffsetLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FRotator OffsetRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere)
	FVector OffsetScale = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataMPreset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataSMPreset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh = nullptr;
	UPROPERTY(EditAnywhere)
	FDataLocation DataLocation;
	UPROPERTY(EditAnywhere, EditFixedSize)
	TArray<FDataMPreset> DataMPreset;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataSM
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	AStaticMeshActor* StaticMeshActor = nullptr;
	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	UPROPERTY(EditAnywhere, EditFixedSize)
	TArray<FDataSMPreset> DataSMPreset;
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataSlot
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* Thumbnail = nullptr;
	UPROPERTY(EditAnywhere)
	FText CompanyName = FText::GetEmpty();
	UPROPERTY(EditAnywhere)
	FText ProductName = FText::GetEmpty();
	UPROPERTY(EditAnywhere)
	FText Description = FText::GetEmpty();
};

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataPreset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int NumberOfMaterial = 0;
	UPROPERTY(EditAnywhere)
	FDataSlot MeshDataSlot = FDataSlot();
	UPROPERTY(EditAnywhere, EditFixedSize)
	TArray<FDataSlot> MaterialDataSlot;
};

UCLASS()
class VISITPLUGINRUNTIME_API AInteractableMM : public AInteractable
{
	GENERATED_BODY()

public:
	AInteractableMM();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact() override;

	UFUNCTION()
	void ApplySetting(int meshIndex, int materialIndex);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "VisitData")
	UDataAssetCompanyProduct* _DataAssetCompanyProduct = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData")
	int _NumberOfPreset = 0;
	UPROPERTY(EditAnywhere, EditFixedSize, Category = "VisitData")
	TArray<FDataPreset> _DataPreset;
	UPROPERTY(EditAnywhere, Category = "VisitData")
	TArray<FDataSM> _DataStaticMesh;

	UPROPERTY(EditAnywhere, Category = "VisitData/OverlayMaterial")
	UMaterialInterface* _OverlayMaterial = nullptr;

	UPROPERTY()
	AInteractableMenu* _InteractableMenu = nullptr;

	UPROPERTY(EditAnywhere, Category = "VisitData/EditMaterialMesh")
	bool _bEditableMaterialMesh = true;

	int _IndexMeshSelected = 0;
	int _IndexMaterialSelected = 0;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedChainEvent) override;
	void OnNumberOfPresetChange();
	void OnDataStaticMeshArraySizeChange();
	void OnNumberOfMaterialChange(int index);
#endif

public:
	int GetIndexMeshSelected() const { return _IndexMeshSelected; }
	int GetIndexMaterialSelected() const { return _IndexMaterialSelected; }

	const TArray<FDataPreset>& GetDataPreset() const { return _DataPreset; }
};
