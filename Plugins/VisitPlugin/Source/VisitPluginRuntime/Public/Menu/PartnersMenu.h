#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PartnersMenu.generated.h"

UCLASS()
class VISITPLUGINRUNTIME_API APartnersMenu : public AActor
{
	GENERATED_BODY()
	
public:	
	APartnersMenu();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	

};
