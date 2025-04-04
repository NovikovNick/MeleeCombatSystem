// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineGameInstance, All, All);

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	INITIALIZATION,
	IN_PLAY,
	IN_SETTINGS,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EPlayerState);

class AOnlinePlayerController;

UCLASS()
class MELEECOMBATSYSTEM_API UOnlineGameInstance : public UGameInstance
{
	GENERATED_BODY()

  public:
	FOnStateChanged FOnStateChangedDelegate;

	virtual void Init() override;

	/** Called to shutdown game instance on game exit */
	virtual void Shutdown() override;

	/** Called to initialize game instance object */
	UOnlineGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Called to retrieve the primary player controller */
	AOnlinePlayerController* GetPrimaryPlayerController() const;

	UFUNCTION(BlueprintCallable)
	void UpdateState(EPlayerState State);

	UFUNCTION(BlueprintCallable)
	EPlayerState GetState() const { return State; };

  private:
	EPlayerState State = EPlayerState::INITIALIZATION;
};
