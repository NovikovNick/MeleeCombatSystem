// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineGameInstance, All, All);

class AOnlinePlayerController;

UCLASS()
class MELEECOMBATSYSTEM_API UOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()

  public:
	/** Called to initialize game instance on game startup */
	virtual void Init() override;

	/** Called to shutdown game instance on game exit */
	virtual void Shutdown() override;

	/** Called to initialize game instance object */
	UOnlineGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Called to retrieve the primary player controller */
	AOnlinePlayerController* GetPrimaryPlayerController() const;
};
