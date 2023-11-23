#pragma once

#include "CoreMinimal.h"
#include "Maps/View/MapsInfo.h"
#include "MapsInfoWidget.generated.h"

class UWidgetComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsInfoWidget : public AMapsInfo
{
	GENERATED_BODY()

public:
	AMapsInfoWidget();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* _WidgetComponent;
};
