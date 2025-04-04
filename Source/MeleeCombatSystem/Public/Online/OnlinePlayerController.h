// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Online/OnlineGameInstance.h"
#include "OnlinePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlinePlayerController, All, All);

class UInputAction;
class UInputMappingContext;

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

  private:
	void OnStateUpdate(EPlayerState State);

	void InvokeSettings();

  protected:
	/******************************************************/
	/*                  INPUT                             */
	/******************************************************/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SettingsAction;

	virtual void SetupInputComponent() override;
};
