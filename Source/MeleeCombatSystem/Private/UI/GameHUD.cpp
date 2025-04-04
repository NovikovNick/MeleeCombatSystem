// Melee Combat System. All Rights Reserved.

#include "UI/GameHUD.h"
#include "Engine/Canvas.h"
#include "Blueprint/UserWidget.h"
#include "UI/NetworkMenuUserWidget.h"
#include "Online/OnlineSubsystem.h"

AGameHUD::AGameHUD()
{
	NetworkMenuUserWidgetClass = UNetworkMenuUserWidget::StaticClass();
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
	DrawCross();
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetWorld()->GetGameInstance());
	GameInstance->UpdateState(EPlayerState::IN_PLAY);
	GameInstance->FOnStateChangedDelegate.AddUObject(this, &ThisClass::OnStateUpdate);

	NetworkMenuUserWidget = CreateWidget<UNetworkMenuUserWidget>(GetWorld(), NetworkMenuUserWidgetClass);
	if (NetworkMenuUserWidget)
	{
		NetworkMenuUserWidget->SetVisibility(ESlateVisibility::Hidden);
		NetworkMenuUserWidget->AddToViewport();
		UOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<UOnlineSubsystem>();
		if (OnlineSubsystem)
		{
			OnlineSubsystem->OnLoginDelegate.AddUObject(NetworkMenuUserWidget, &UNetworkMenuUserWidget::OnLoginComplete);
		}
	}
}

void AGameHUD::DrawCross()
{
	const int32 CenterX		 = Canvas->SizeX * 0.5;
	const int32 CenterY		 = Canvas->SizeY * 0.5;
	const float Size		 = 10.0f;
	const float Thickness	 = 2.0f;
	const FLinearColor Color = FLinearColor::Black;

	DrawLine(CenterX - Size, CenterY, CenterX + Size, CenterY, Color, Thickness);
	DrawLine(CenterX, CenterY - Size, CenterX, CenterY + Size, Color, Thickness);
}

void AGameHUD::OnStateUpdate(EPlayerState State)
{
	if (State == EPlayerState::IN_SETTINGS)
	{
		NetworkMenuUserWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		NetworkMenuUserWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
