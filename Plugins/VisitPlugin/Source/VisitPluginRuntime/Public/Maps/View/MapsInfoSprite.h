#pragma once

#include "CoreMinimal.h"
#include "Maps/View/MapsInfo.h"
#include "MapsInfoSprite.generated.h"

class UPaperSpriteComponent;
class UTextRenderComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInfoSprite : public AMapsInfo
{
	GENERATED_BODY()
	
public:	
	AMapsInfoSprite();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	//Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _PaperSpriteComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* _TextRenderComponent;
};
