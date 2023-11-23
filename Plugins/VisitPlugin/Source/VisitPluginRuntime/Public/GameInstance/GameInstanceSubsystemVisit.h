#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameInstanceSubsystemVisit.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateCloseOtherEditMeshMaterial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateCloseOtherStateChanger);

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FDataPerformance
{
	GENERATED_BODY()
public:
	FString resolution = "Auto";
	float screenPercent = 100.0f;
	float secondaryScreenPercent = 100.0f;
	FString windowMode = "Fullscreen";
	FString FPS = "Unlimited";
	bool useTSR = false;
	bool useVSYNC = true;
};

UCLASS()
class VISITPLUGINRUNTIME_API UGameInstanceSubsystemVisit : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	//Dict
	void AddInDictObject(FString key, UObject* value);
	void RemoveInDictObject(FString key);
	UObject* GetInDictObject(FString key);

	//Performance
	void SetResolution(FString value);
	void SetScreenPercent(float value);
	void SetSecondaryScreenPercent(float value);
	void SetWindowMode(FString value);
	void SetFPS(FString value);
	void SetTSR(bool value);
	void SetVSYNC(bool value);
	FDataPerformance GetDataPerformance();
	void InitOption();
	bool _bOptionIsInit;

	FDelegateCloseOtherEditMeshMaterial _DelegateCloseOtherMeshMaterial;
	FDelegateCloseOtherStateChanger _DelegateCloseOtherStateChanger;

	UFUNCTION(BlueprintCallable)
	void SetCurrentAgent(UObject* agent);
	UFUNCTION(BlueprintCallable)
	UObject* GetCurrentAgent();

protected:

private:
	TMap<FString, UObject*> _DictObject;
	FDataPerformance _DataPerformance;

	UPROPERTY()
	UObject* _CurrentAgent = nullptr;
};
