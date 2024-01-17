#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Menu/BotMenu.h"
#include "TopMenu.generated.h"

class UUserWidget;
class AGameStateManager;
class UButton;
class UDataAssetPartner;

UCLASS()
class VISITPLUGINRUNTIME_API ATopMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _TopWidgetClass = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UUserWidget* _TopWidget = nullptr;
	//UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	//TSubclassOf<UUserWidget> _SlotPartnerWidgetClass = nullptr;
	//UPROPERTY()
	//TArray<UUserWidget*> _SlotWidget;
	//
	//UPROPERTY(EditAnywhere, Category = "VisitData/Partner")
	//UDataAssetPartner* _DataAssetPartner = nullptr;

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;

	//UPROPERTY()
	//UButton* _ButtonOpenClose = nullptr;

	bool _bIsOpen = false;

public:	
	ATopMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UFUNCTION(BlueprintCallable)
	void InitWidget();

	UFUNCTION()
	void ShowWidget();
	UFUNCTION()
	void HideWidget();
	UFUNCTION()
	void OpenMenu();
	UFUNCTION()
	void CloseMenu();
	UFUNCTION()
	void OnOpenCloseMenuPressed();

	UFUNCTION()
	void OnBotMenuStateChange(EBotMenuState BotMenuState);

};
