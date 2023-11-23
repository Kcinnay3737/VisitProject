#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Visitor.generated.h"

class UCameraComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AVisitor : public ACharacter
{
	GENERATED_BODY()

public:
	AVisitor();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	//Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* _FollowCamera;

	UPROPERTY(EditAnywhere, Category = "VisitData/HiddenActor")
	TArray<TSubclassOf<AActor>> _HiddenActorsClass;
	UPROPERTY(EditInstanceOnly, Category = "VisitData/HiddenActor")
	TArray<AActor*> _HiddenActors;

	//Variable
	float _BaseFOV = 0.0f;
	bool _bEditWidgetIsOpen = false;

public:
	//Get and Set function
	UCameraComponent* GetFollowCamera();

	float GetBaseFov();

	void SetEditWidgetIsOpen(bool value);
	bool GetEditWidgetIsOpen();

	TArray<TSubclassOf<AActor>> GetHiddenActorsClass();
	TArray<AActor*> GetHiddenActors();

};