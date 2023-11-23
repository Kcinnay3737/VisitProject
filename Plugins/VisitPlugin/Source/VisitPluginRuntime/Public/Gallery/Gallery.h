#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gallery.generated.h"

class UButtonGallery;
class UImage;
class UTextBlock;
class UTexture2D;
class UUserWidget;

USTRUCT(BlueprintType)
struct VISITPLUGINRUNTIME_API FImageValue
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UTexture2D* Image = nullptr;
	UPROPERTY(EditAnywhere)
	FString ImageName = "";
};

UCLASS()
class VISITPLUGINRUNTIME_API AGallery : public AActor
{
	GENERATED_BODY()
	
public:	
	AGallery();
	virtual void Tick(float DeltaTime) override;

	void OnImageSelected(int Index);

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _GalleryWidgetClass;
	UPROPERTY()
	UUserWidget* _GalleryWidget = nullptr;
	UPROPERTY(EditAnywhere, Category = "VisitData/Widget")
	TSubclassOf<UUserWidget> _GallerySlotImageWidgetClass;

	UPROPERTY(EditAnywhere, Category = "VisitData/GalleryImage")
	TArray<FImageValue> _GalleryImage;

	UPROPERTY()
	UImage* _ImageView = nullptr;
	UPROPERTY()
	UTextBlock* _TextBlockImageName = nullptr;

	UPROPERTY()
	TArray<UButtonGallery*> _ButtonGallery;

	FVector2D _BaseImageWidgetSize = FVector2D::ZeroVector;
	float _BaseImageWidgetOffsetY = 0.0f;

	void InitWidget();
};
