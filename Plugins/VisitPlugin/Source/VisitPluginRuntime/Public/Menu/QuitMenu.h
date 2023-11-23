#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuitMenu.generated.h"

class AGameStateManager;
class UUserWidget;
class UButton;

UCLASS()
class VISITPLUGINRUNTIME_API AQuitMenu : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _QuitWidgetClass = nullptr;
	UPROPERTY()
	UUserWidget* _QuitWidget = nullptr;

	UPROPERTY()
	UButton* _ButtonQuit = nullptr;

	UPROPERTY()
	AGameStateManager* _GameStateManager = nullptr;

public:	
	AQuitMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	void InitWidget();

	UFUNCTION()
	void Quit();

	UFUNCTION()
	void QuitApplication();
	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void DisableQuitButton();
	UFUNCTION()
	void EnableQuitButton();
};
