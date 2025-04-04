// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkMenuUserWidget.generated.h"

class UButton;

UCLASS()
class MELEECOMBATSYSTEM_API UNetworkMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
  public:
	virtual void NativeOnInitialized() override;

	void OnLoginComplete();

  protected:
	UPROPERTY(meta = (BindWidget))
	UButton* LoginBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* HostBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinBtn;

  private:
	UFUNCTION()
	void OnLoginClicked();

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();
};
