// Melee Combat System. All Rights Reserved.

#include "Online/OnlinePlayerController.h"
#include "Online/OnlineGameInstance.h"
#include "Online/OnlineSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogOnlinePlayerController);

AOnlinePlayerController::AOnlinePlayerController() {}

void AOnlinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Register this player with the Online Services
	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetWorld()->GetGameInstance());
	UOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<UOnlineSubsystem>();
	ULocalPlayer* LocalPlayer		  = Super::GetLocalPlayer();
	if (LocalPlayer && OnlineSubsystem)
	{
		OnlineSubsystem->Login(LocalPlayer->GetPlatformUserId());
	}
}

void AOnlinePlayerController::EndPlay(EEndPlayReason::Type EndReason)
{
	Super::EndPlay(EndReason);
}

void AOnlinePlayerController::ReadFile()
{
	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetWorld()->GetGameInstance());
	UOnlineSubsystem* OnlineSubsystem = GameInstance->GetSubsystem<UOnlineSubsystem>();
	ULocalPlayer* LocalPlayer		  = Super::GetLocalPlayer();
	if (LocalPlayer && OnlineSubsystem)
	{
		FPlatformUserId LocalPlayerPlatformUserId = LocalPlayer->GetPlatformUserId();
		FString TitleFileContent				  = OnlineSubsystem->ReadTitleFile(FString("test.json"), LocalPlayerPlatformUserId);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, TitleFileContent);
	}
}
