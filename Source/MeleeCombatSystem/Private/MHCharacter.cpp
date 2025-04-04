// Melee Combat System. All Rights Reserved.

#include "MHCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "MHProjectile.h"
#include "TimerManager.h"

AMHCharacter::AMHCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch						   = false;
	bUseControllerRotationYaw						   = false;
	bUseControllerRotationRoll						   = false;

	GetCharacterMovement()->bOrientRotationToMovement  = true;							// Character moves in the direction of input...
	GetCharacterMovement()->RotationRate			   = FRotator(0.0f, 500.0f, 0.0f);	// ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity			   = 700.f;
	GetCharacterMovement()->AirControl				   = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed			   = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed		   = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom										   = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->TargetArmLength						   = 400.0f;  // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation				   = true;	  // Rotate the arm based on the controller
	FollowCamera									   = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->bUsePawnControlRotation			   = false;

	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Initialize the player's Health
	MaxHealth	  = 100.0f;
	CurrentHealth = MaxHealth;

	// Initialize projectile class
	ProjectileClass = AMHProjectile::StaticClass();
	// Initialize fire rate
	FireRate		= 0.25f;
	bIsFiringWeapon = false;
}

void AMHCharacter::BeginPlay()
{
	Super::BeginPlay();

	FString Msg = FString::Printf(TEXT("%s's role is %d"), *GetName(), GetLocalRole());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Msg);
}

void AMHCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMHCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMHCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate current health.
	DOREPLIFETIME(AMHCharacter, CurrentHealth);
}

void AMHCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AMHCharacter::OnHealthUpdate()
{
	// Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	// Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	// Functions that occur on all machines.
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void AMHCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AMHCharacter::TakeDamage(float DamageTaken,
							   struct FDamageEvent const& DamageEvent,
							   AController* EventInstigator,
							   AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	return damageApplied;
}

void AMHCharacter::StartFire()
{
	if (!bIsFiringWeapon)
	{
		bIsFiringWeapon = true;
		UWorld* World	= GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AMHCharacter::StopFire, FireRate, false);
		HandleFire();
	}
}

void AMHCharacter::StopFire()
{
	bIsFiringWeapon = false;
}

void AMHCharacter::HandleFire_Implementation()
{
	FTransform Transform = GetActorTransform();
	Transform.SetLocation(GetActorLocation() + (GetActorRotation().Vector() * 100.0f) + (GetActorUpVector() * 50.0f));
	// todo rotation
	AMHProjectile* Spawned = GetWorld()->SpawnActorDeferred<AMHProjectile>(ProjectileClass, Transform, this, GetInstigator());
	if (Spawned)
	{
		Spawned->FinishSpawning(Transform);
	}
}