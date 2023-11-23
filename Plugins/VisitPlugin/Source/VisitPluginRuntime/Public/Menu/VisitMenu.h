#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisitMenu.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API AVisitMenu : public AActor
{
	GENERATED_BODY()
	
public:	
	AVisitMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	
};
