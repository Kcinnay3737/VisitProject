#pragma once

#include "CoreMinimal.h"
#include "Maps/View/MapsImage.h"
#include "MapsImageWidget.generated.h"

class UWidgetComponent;

UCLASS()
class VISITPLUGINRUNTIME_API AMapsImageWidget : public AMapsImage
{
	GENERATED_BODY()

public:
	AMapsImageWidget();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* _WidgetComponent;
};
