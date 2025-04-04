// Melee Combat System. All Rights Reserved.

#include "UI/NetworkMenuUserWidget.h"
#include "Online/OnlineGameInstance.h"
#include "Online/OnlineSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Components/Button.h"

void UNetworkMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (LoginBtn)
	{
		LoginBtn->OnClicked.AddDynamic(this, &ThisClass::OnLoginClicked);
		LoginBtn->SetIsEnabled(true);
	}

	if (HostBtn)
	{
		HostBtn->OnClicked.AddDynamic(this, &ThisClass::OnHostClicked);
		HostBtn->SetIsEnabled(false);
	}

	if (JoinBtn)
	{
		JoinBtn->OnClicked.AddDynamic(this, &ThisClass::OnJoinClicked);
		JoinBtn->SetIsEnabled(false);
	}
}

void UNetworkMenuUserWidget::OnLoginClicked()
{
	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetWorld()->GetGameInstance());
	UOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<UOnlineSubsystem>();
	ULocalPlayer* LocalPlayer		  = GetOwningLocalPlayer();
	if (LocalPlayer && OnlineSubsystem)
	{
		OnlineSubsystem->Login(LocalPlayer->GetPlatformUserId());

		LoginBtn->SetIsEnabled(false);
	}
}

void UNetworkMenuUserWidget::OnLoginComplete()
{
	LoginBtn->SetIsEnabled(false);
	HostBtn->SetIsEnabled(true);
	JoinBtn->SetIsEnabled(true);
}

void UNetworkMenuUserWidget::OnHostClicked()
{
	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetWorld()->GetGameInstance());
	UOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<UOnlineSubsystem>();
	ULocalPlayer* LocalPlayer		  = GetOwningLocalPlayer();
	if (LocalPlayer && OnlineSubsystem)
	{
		FPlatformUserId LocalPlayerPlatformUserId = LocalPlayer->GetPlatformUserId();
		FString TitleFileContent				  = OnlineSubsystem->ReadTitleFile(FString("test.json"), LocalPlayerPlatformUserId);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TitleFileContent);
	}
}

void UNetworkMenuUserWidget::OnJoinClicked() {}
