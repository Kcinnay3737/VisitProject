#pragma once

#include "CoreMinimal.h"
#include "Maps/Interactable/MapsInteractableWidget.h"
#include "MapsInteractableWidgetTeleporter.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInteractableWidgetTeleporter : public AMapsInteractableWidget
{
	GENERATED_BODY()
	
public:	
	AMapsInteractableWidgetTeleporter();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "VisitData/Teleport")
	bool _bTeleportOnClick = true;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;
private:	
	void Teleport();

};