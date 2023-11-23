#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StartMenu.generated.h"

class UUserWidget;
class AGameStateManager;
class AOptionMenu;
class UButton;
class UTextBlock;

UCLASS()
class VISITPLUGINRUNTIME_API AStartMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _StartMenuWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _StartMenuWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "VisitData/ProjectName")
	FString _ProjectName = "Project Name";

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;

	UPROPERTY()
	AOptionMenu* _OptionMenu = nullptr;

	UPROPERTY()
	UButton* _ButtonStart = nullptr;
	UPROPERTY()
	UTextBlock* _TextBlockProjectName = nullptr;

public:	
	AStartMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UFUNCTION()
	void OpenStartMenu();
	UFUNCTION()
	void CloseStartMenu();



	UFUNCTION()
	void HideViewWidget();
	UFUNCTION()
	void ShowViewWidget();
};
