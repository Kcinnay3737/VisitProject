#pragma once

#include "CoreMinimal.h"
#include "Maps/Interactable/MapsInteractable.h"
#include "MapsInteractableSprite.generated.h"

class UPaperSpriteComponent;
class UTextRenderComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInteractableSprite : public AMapsInteractable
{
	GENERATED_BODY()
	
public:	
	AMapsInteractableSprite();
	virtual void Tick(float DeltaTime) override;

	virtual void Interact() override;

protected:
	virtual void BeginPlay() override;
private:	
	//Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _PaperSpriteComponent;


};
