#pragma once

#include "CoreMinimal.h"
#include "Tooltip/Tooltip.h"
#include "TooltipSprite.generated.h"

class UPaperSpriteComponent;
class UTextRenderComponent;

UCLASS()
class VISITPLUGINRUNTIME_API ATooltipSprite : public ATooltip
{
	GENERATED_BODY()
	
public:	
	ATooltipSprite();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _PaperSpriteComponentClose;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* _TextRenderComponentClose;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPaperSpriteComponent* _PaperSpriteComponentOpen;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UTextRenderComponent* _TextRenderComponentOpen;
};
