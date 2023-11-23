#pragma once

#include "CoreMinimal.h"
#include "Maps/Interactable/MapsInteractableSprite.h"
#include "MapsInteractableSpriteTeleporter.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInteractableSpriteTeleporter : public AMapsInteractableSprite
{
	GENERATED_BODY()
	
public:	
	AMapsInteractableSpriteTeleporter();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "VisitData/Teleport")
	bool _bTeleportOnClick = true;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "_bTeleportOnClick", MakeEditWidget = "true"), Category = "VisitData/Teleport")
	FVector _TeleportLocation = FVector::ZeroVector;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;
private:	
	void Teleport();

};