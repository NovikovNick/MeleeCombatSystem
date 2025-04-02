// Melee Combat System. All Rights Reserved.

#include "Online/OnlineGameInstance.h"
#include "Online/OnlinePlayerController.h"

DEFINE_LOG_CATEGORY(LogOnlineGameInstance);

void UOnlineGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogOnlineGameInstance, Display, TEXT("'%s' initialized"), *GetNameSafe(this));
}

void UOnlineGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogOnlineGameInstance, Display, TEXT("'%s' shutdown"), *GetNameSafe(this));
}

UOnlineGameInstance::UOnlineGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

AOnlinePlayerController* UOnlineGameInstance::GetPrimaryPlayerController() const
{
	return Cast<AOnlinePlayerController>(Super::GetPrimaryPlayerController(false));
}
