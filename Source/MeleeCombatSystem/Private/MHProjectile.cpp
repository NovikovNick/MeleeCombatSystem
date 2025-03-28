// Melee Combat System. All Rights Reserved.

#include "MHProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

// Sets default values
AMHProjectile::AMHProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates					  = true;

	// Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>("RootComponent");
	SphereComponent->InitSphereRadius(37.5f);
	SphereComponent->SetCollisionProfileName("BlockAllDynamic");
	RootComponent = SphereComponent;

	// Definition for the Mesh that will serve as your visual representation.
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	StaticMesh->SetupAttachment(RootComponent);

	// Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
	ProjectileMovementComponent->InitialSpeed			  = 1500.0f;
	ProjectileMovementComponent->MaxSpeed				  = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale	  = 0.0f;

	DamageType											  = UDamageType::StaticClass();
	Damage												  = 10.0f;
}

// Called when the game starts or when spawned
void AMHProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComponent->OnComponentHit.AddDynamic(this, &AMHProjectile::OnProjectileImpact);
	}
}

// Called every frame
void AMHProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMHProjectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this,	//
											 ExplosionEffect,
											 spawnLocation,
											 FRotator::ZeroRotator,
											 true,
											 EPSCPoolMethod::AutoRelease);
}

void AMHProjectile::OnProjectileImpact(UPrimitiveComponent* HitComponent,
									   AActor* OtherActor,
									   UPrimitiveComponent* OtherComp,
									   FVector NormalImpulse,
									   const FHitResult& Hit)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor,	//
										   Damage,
										   NormalImpulse,
										   Hit,
										   GetInstigatorController(),
										   this,
										   DamageType);
	}

	Destroy();
}