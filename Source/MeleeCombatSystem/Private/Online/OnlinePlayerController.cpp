// Melee Combat System. All Rights Reserved.

#include "Online/OnlinePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "MHCharacter.h"

DEFINE_LOG_CATEGORY(LogOnlinePlayerController);

AOnlinePlayerController::AOnlinePlayerController() {}

void AOnlinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetGameInstance());
	GameInstance->FOnStateChangedDelegate.AddUObject(this, &ThisClass::OnStateUpdate);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	AMHCharacter* MHCharacter						= Cast<AMHCharacter>(GetCharacter());
	if (MHCharacter && EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, MHCharacter, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, MHCharacter, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, MHCharacter, &AMHCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, MHCharacter, &AMHCharacter::Look);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, MHCharacter, &AMHCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, MHCharacter, &AMHCharacter::StopFire);
	}
	else
	{
		UE_LOG(LogOnlinePlayerController, Error, TEXT("'%s' Failed to setup character's input"), *GetNameSafe(this));
	}
}

void AOnlinePlayerController::EndPlay(EEndPlayReason::Type EndReason)
{
	Super::EndPlay(EndReason);
}

void AOnlinePlayerController::OnStateUpdate(EPlayerState State)
{
	if (State == EPlayerState::IN_SETTINGS)
	{
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI{});
	}
	else
	{
		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly{});
	}
}

void AOnlinePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(SettingsAction, ETriggerEvent::Started, this, &AOnlinePlayerController::InvokeSettings);
	}
	else
	{
		UE_LOG(LogOnlinePlayerController, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void AOnlinePlayerController::InvokeSettings()
{
	UOnlineGameInstance* GameInstance = Cast<UOnlineGameInstance>(GetGameInstance());
	EPlayerState PrevState			  = GameInstance->GetState();
	EPlayerState NewState			  = PrevState == EPlayerState::IN_SETTINGS ? EPlayerState::IN_PLAY : EPlayerState::IN_SETTINGS;
	GameInstance->UpdateState(NewState);
}
