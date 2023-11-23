//#include "Interactable/Teleporter/TeleporterPoint.h"
//
//#include "Blueprint/UserWidget.h"
//#include "Character/Visitor.h"
//#include "Components/BoxComponent.h"
//#include "Components/Button.h"
//#include "Components/ScrollBox.h"
//#include "Components/ScrollBoxSlot.h"
//#include "Components/TextBlock.h"
//#include "Interactable/Button/ButtonTeleport.h"
//
//ATeleporterPoint::ATeleporterPoint() : Super::ATeleporter()
//{
//	PrimaryActorTick.bCanEverTick = true;
//}
//
//void ATeleporterPoint::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//void ATeleporterPoint::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//void ATeleporterPoint::AddBoxColliderComponent()
//{
//	FString name = FString::Printf(TEXT("BoxCollider%d"), _BoxColliderComponents.Num());
//	_BoxColliderComponents.Add(NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName(name)));
//	int lastIndex = _BoxColliderComponents.Num() - 1;
//
//	_BoxColliderComponents[lastIndex]->SetupAttachment(_SceneRootComponent);
//	_BoxColliderComponents[lastIndex]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//	_BoxColliderComponents[lastIndex]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
//	_BoxColliderComponents[lastIndex]->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
//	_BoxColliderComponents[lastIndex]->SetGenerateOverlapEvents(true);
//	_BoxColliderComponents[lastIndex]->OnComponentBeginOverlap.AddDynamic(this, &ATeleporterPoint::OnEnterTeleportPoint);
//	_BoxColliderComponents[lastIndex]->OnComponentEndOverlap.AddDynamic(this, &ATeleporterPoint::OnExitTeleportPoint);
//
//	_BoxColliderComponents[lastIndex]->RegisterComponent();
//
//	AddOwnedComponent(_BoxColliderComponents[lastIndex]);
//}
//
//void ATeleporterPoint::OnEnterTeleportPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	AVisitor* visitor = Cast<AVisitor>(OtherActor);
//	if (!visitor) return;
//
//	OpenWidget();
//}
//
//void ATeleporterPoint::OnExitTeleportPoint(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//{
//	AVisitor* visitor = Cast<AVisitor>(OtherActor);
//	if (!visitor) return;
//
//	CloseWidget();
//}
//
//void ATeleporterPoint::OpenWidget()
//{
//	if (_TeleportPointWidget)
//	{
//		_TeleportPointWidget->AddToViewport();
//		return;
//	}
//
//	UWorld* world = GetWorld();
//	if (!world) return;
//	APlayerController* playerController = world->GetFirstPlayerController();
//	if (!playerController) return;
//	if (!_TeleportPointsWidgetClass) return;
//
//	_TeleportPointWidget = CreateWidget<UUserWidget>(playerController, _TeleportPointsWidgetClass);
//	if (!_TeleportPointWidget) return;
//	_TeleportPointWidget->AddToViewport();
//
//	UScrollBox* scrollBox = Cast<UScrollBox>(_TeleportPointWidget->GetWidgetFromName("ScrollBox_TeleportPoint"));
//	if (!scrollBox) return;
//
//	int count = 0;
//	for (FTeleportData teleportData : _TeleportData)
//	{
//		UUserWidget* slot = CreateWidget<UUserWidget>(playerController, _SlotTeleportPointWidgetClass);
//		if (slot)
//		{
//			UTextBlock* textBlock = Cast<UTextBlock>(slot->GetWidgetFromName("TextBlock_Location"));
//			if (textBlock)
//			{
//				textBlock->SetText(teleportData.PlaceName);
//			}
//
//			UButton* button = Cast<UButton>(slot->GetWidgetFromName("Button_Slot"));
//			if (button)
//			{
//				FSlateBrush brush;
//				brush.SetResourceObject(teleportData.Thumbnail);
//				button->WidgetStyle.Normal = brush;
//				button->WidgetStyle.Hovered = brush;
//				button->WidgetStyle.Pressed = brush;
//
//				UButtonTeleport* buttonTeleport = NewObject<UButtonTeleport>(button);
//				if (buttonTeleport)
//				{
//					buttonTeleport->Initialise(this, count);
//					_ButtonTeleport.Add(buttonTeleport);
//					button->OnClicked.AddDynamic(buttonTeleport, &UButtonTeleport::OnClickTeleport);
//				}
//			}
//
//			UPanelSlot* panelSlot = scrollBox->AddChild(slot);
//			UScrollBoxSlot* scrollBoxSlot = Cast<UScrollBoxSlot>(panelSlot);
//			if (scrollBoxSlot)
//			{
//				scrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
//			}
//
//			count++;
//		}
//	}
//}
//
//void ATeleporterPoint::CloseWidget()
//{
//	if (_TeleportPointWidget && _TeleportPointWidget->IsInViewport())
//	{
//		_TeleportPointWidget->RemoveFromParent();
//	}
//}