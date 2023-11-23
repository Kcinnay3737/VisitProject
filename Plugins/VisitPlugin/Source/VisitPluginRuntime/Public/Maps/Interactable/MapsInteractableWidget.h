#pragma once

#include "CoreMinimal.h"
#include "Maps/Interactable/MapsInteractable.h"
#include "MapsInteractableWidget.generated.h"

class UWidgetComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInteractableWidget : public AMapsInteractable
{
	GENERATED_BODY()

public:
	AMapsInteractableWidget();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* _WidgetComponent;
};
