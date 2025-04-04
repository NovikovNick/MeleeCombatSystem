// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Online/OnlineGameInstance.h"
#include "GameHUD.generated.h"

class UNetworkMenuUserWidget;

UCLASS()
class MELEECOMBATSYSTEM_API AGameHUD : public AHUD
{
	GENERATED_BODY()
  public:
	AGameHUD();

	virtual void DrawHUD() override;

  protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UNetworkMenuUserWidget> NetworkMenuUserWidgetClass;

	virtual void BeginPlay() override;

  private:
	UNetworkMenuUserWidget* NetworkMenuUserWidget;

	void DrawCross();

	void OnStateUpdate(EPlayerState State);
};
