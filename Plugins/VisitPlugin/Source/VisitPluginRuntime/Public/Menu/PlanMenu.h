#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlanMenu.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API APlanMenu : public AActor
{
	GENERATED_BODY()
	
public:
	APlanMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	

};
