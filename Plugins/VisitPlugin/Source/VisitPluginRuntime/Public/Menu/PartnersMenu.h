#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Menu/BotMenu.h"
#include "PartnersMenu.generated.h"

class UUserWidget;
class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FPartnerData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UTexture2D* PartnerImage = nullptr;
	UPROPERTY(EditAnywhere)
	FString ParterURL = "";
	UPROPERTY(EditAnywhere)
	FString PartnerDescription = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API APartnersMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _PartnerWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _PartnerWidget = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "VisitData/Partner")
	TArray<FPartnerData> _DataPartners;

public:	
	APartnersMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UFUNCTION()
	void OnBotMenuStateChange(EBotMenuState BotMenuState);

	void InitWidget();
	void OpenPlanMenu();
	void ClosePlanMenu();

};
