#pragma once

#include "CoreMinimal.h"
#include "Maps/View/MapsImage.h"
#include "MapsImageSprite.generated.h"

class UPaperSpriteComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsImageSprite : public AMapsImage
{
	GENERATED_BODY()
	
public:	
	AMapsImageSprite();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _PaperSpriteComponent;

};
