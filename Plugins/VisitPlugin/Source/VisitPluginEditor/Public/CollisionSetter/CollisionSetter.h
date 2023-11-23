#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionSetter.generated.h"

class UBoxComponent;
class USceneComponent;

USTRUCT(BlueprintType)
struct VISITPLUGINEDITOR_API FCollisionData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::QueryAndPhysics;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionResponse> CollisionResponse = ECollisionResponse::ECR_Block;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionTraceFlag> CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	UPROPERTY(EditAnywhere)
	bool bUseSimpleCollision = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseSimpleCollision"))
	int32 HullCount = 4;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseSimpleCollision"))
	int32 MaxHullVerts = 16;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseSimpleCollision"))
	int32 HullPrecision = 100000;
};

UCLASS()
class VISITPLUGINEDITOR_API ACollisionSetter : public AActor
{
	GENERATED_BODY()
	
public:	
	ACollisionSetter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* _SceneRootComponent;
	
	UPROPERTY(EditAnywhere, Category = "VisitData/Scope")
	bool _AllScene = false;

	UPROPERTY(EditAnywhere, Category = "VisitData/IgnoreData")
	TArray<TSubclassOf<AActor>> _IgnoreClass;
	UPROPERTY(EditAnywhere, Category = "VisitData/IgnoreData")
	TArray<AActor*> _IgnoreActor;
	UPROPERTY(EditAnywhere, Category = "VisitData/IgnoreData")
	TArray<UStaticMesh*> _IgnoreStaticMesh;
	UPROPERTY(EditAnywhere, Category = "VisitData/IgnoreData")
	TArray<FString> _StartLabelIgnoreActor;

	UPROPERTY()
	TMap<UStaticMesh*, UStaticMesh*> _PassedStaticMesh;

	UFUNCTION(CallInEditor, Category = "VisitData/BoxCollider")
	void AddBoxCollider();

	TArray<UBoxComponent*> GetCollision();

	UFUNCTION(CallInEditor, Category = "VisitData/Collision")
	void SetCollision();

	UPROPERTY(EditAnywhere, Category = "VisitData/Collision")
	int _MaxPolygonNumber = 10000;

	UPROPERTY(EditAnywhere, Category = "VisitData/Collision")
	FCollisionData _CollisionDataLesserOrEqualMaxPolygon = FCollisionData();
	UPROPERTY(EditAnywhere, Category = "VisitData/Collision")
	FCollisionData _CollisionDataBiggerMaxPolygon = FCollisionData();
};
