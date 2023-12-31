#pragma once

#include "CoreMinimal.h"
#include "Interactable/Teleporter/Teleporter.h"
#include "Menu/BotMenu.h"
#include "RoomsMenu.generated.h"

class UUserWidget;
class UScrollBox;
class UTexture2D;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FParamsViewRoom
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FText RoomName = FText::GetEmpty();
	UPROPERTY()
	UTexture2D* Texture = nullptr;
};

UCLASS()
class VISITPLUGINRUNTIME_API ARoomsMenu : public ATeleporter
{
	GENERATED_BODY()
	
public:	
	ARoomsMenu();
	virtual void Tick(float DeltaTime) override;

	virtual void OnClickTeleport(int index) override;
	virtual void OnClickView(int index) override;
	virtual void OnClickFloor(int index) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _RoomsWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _RoomsWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _RoomsSlotWidgetClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _FloorSlotWidgetClass = nullptr;

	UPROPERTY()
	UScrollBox* _ScrollBoxSlot = nullptr;

	bool _bShowRoom = false;
	
	void InitWidget();

	UFUNCTION()
	void OpenMenu();
	UFUNCTION()
	void CloseMenu();

	UFUNCTION()
	void SelectFloor(FString Floor);

	UFUNCTION()
	void OnBotMenuStateChange(EBotMenuState BotMenuState);

	void ViewRoom(int Index);
	void HideRoom();
};
