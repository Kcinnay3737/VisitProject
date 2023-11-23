#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Menu/BotMenu.h"
#include "PlanMenu.generated.h"

class UUserWidget;

UCLASS()
class VISITPLUGINRUNTIME_API APlanMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _PlanWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _PlanWidget = nullptr;


public:
	APlanMenu();
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
