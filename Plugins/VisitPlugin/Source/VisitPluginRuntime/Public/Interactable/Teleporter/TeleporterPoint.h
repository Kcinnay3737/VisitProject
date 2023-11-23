//#pragma once
//
//#include "CoreMinimal.h"
//#include "Interactable/Teleporter/Teleporter.h"
//#include "TeleporterPoint.generated.h"
//
//class UBoxComponent;
//
//UCLASS()
//class VISITPLUGINRUNTIME_API ATeleporterPoint : public ATeleporter
//{
//	GENERATED_BODY()
//	
//public:
//	ATeleporterPoint();
//	virtual void Tick(float DeltaTime) override;
//
//protected:
//	virtual void BeginPlay() override;
//
//private:
//	UPROPERTY(EditAnywhere, Category = "VisitData/BoxCollider")
//	TArray<UBoxComponent*> _BoxColliderComponents;
//	UFUNCTION(CallInEditor, Category = "VisitData/BoxCollider")
//	void AddBoxColliderComponent();
//
//	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
//	TSubclassOf<UUserWidget> _TeleportPointsWidgetClass;
//	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
//	TSubclassOf<UUserWidget> _SlotTeleportPointWidgetClass;
//
//	UPROPERTY()
//	UUserWidget* _TeleportPointWidget;
//
//	UFUNCTION()
//	void OnEnterTeleportPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
//	UFUNCTION()
//	void OnExitTeleportPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
//
//	void OpenWidget();
//	void CloseWidget();
//};
