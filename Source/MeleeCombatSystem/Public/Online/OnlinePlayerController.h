// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OnlinePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlinePlayerController, All, All);

UCLASS()
class MELEECOMBATSYSTEM_API AOnlinePlayerController : public APlayerController
{
	GENERATED_BODY()

  public:
	AOnlinePlayerController();

  protected:
	/** Called once play begins */
	virtual void BeginPlay();

	/** Called once play ends */
	virtual void EndPlay(EEndPlayReason::Type EndReason);

	UFUNCTION(BlueprintCallable)
	void ReadFile();
};
