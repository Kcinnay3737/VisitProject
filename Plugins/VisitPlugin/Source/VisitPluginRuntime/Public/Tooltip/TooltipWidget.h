#pragma once

#include "CoreMinimal.h"
#include "Tooltip/Tooltip.h"
#include "TooltipWidget.generated.h"

class UWidgetComponent;

UCLASS()
class VISITPLUGINRUNTIME_API ATooltipWidget : public ATooltip
{
	GENERATED_BODY()

public:
	ATooltipWidget();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* _WidgetComponentClose;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* _WidgetComponentOpen;
};
