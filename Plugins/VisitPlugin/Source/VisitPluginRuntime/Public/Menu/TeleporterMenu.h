//#pragma once
//
//#include "CoreMinimal.h"
//#include "Interactable/Teleporter/Teleporter.h"
//#include "TeleporterMenu.generated.h"
//
//UCLASS()
//class VISITPLUGINRUNTIME_API ATeleporterMenu : public ATeleporter
//{
//	GENERATED_BODY()
//	
//public:
//	ATeleporterMenu();
//	virtual void Tick(float DeltaTime) override;
//
//protected:
//	virtual void BeginPlay() override;
//
//private:
//	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
//	TSubclassOf<UUserWidget> _TeleportMenuWidgetClass = nullptr;
//	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
//	TSubclassOf<UUserWidget> _SlotTeleportWidgetClass = nullptr;
//
//	UPROPERTY()
//	UUserWidget* _CurrWidget = nullptr;
//
//	UFUNCTION()
//	void InitUI();
//};
