// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSProjectile.h"
#include "BoomerEnemy.h"
#include "ShootingEnemy.h"
#include "ZigZagEnemy.h"
#include "BamBamThankyouMaam/FPSCharacter.h"

// Sets default values
AFPSProjectile::AFPSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// Set the sphere's collision profile name to "Projectile".
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		// Event called when component hits something.
		CollisionComponent->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);
		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);
		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("/Script/Engine.StaticMesh'/Game/Model/Projectile/Sphere.Sphere'"));
		if (Mesh.Succeeded())
		{
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}

	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("/Script/Engine.Material'/Game/Model/Projectile/SphereMaterial.SphereMaterial'"));
	if (Material.Succeeded())
	{
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	// Delete the projectile after 3 seconds.
	BulletDamage = 1.0f;
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AFPSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSProjectile::OnProjectileOverlap);
}

// Called every frame
void AFPSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Function that initializes the projectile's velocity in the shoot direction.
void AFPSProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// Function that is called when the projectile hits something.
void AFPSProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}

	if (OtherActor->IsA(ABoomerEnemy::StaticClass()))
	{
		ABoomerEnemy* Enemy = Cast<ABoomerEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->MyTakeDamage(BulletDamage);
		}
	}
	if (OtherActor->IsA(AFPSCharacter::StaticClass()))
	{
		AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor);
		if (Player)
		{
			Player->MyTakeDamage(BulletDamage);
		}
	}
	if (OtherActor->IsA(AShootingEnemy::StaticClass()))
	{
		AShootingEnemy* Enemy = Cast<AShootingEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->MyTakeDamage(BulletDamage);
		}
	}
	if (OtherActor->IsA(AZigZagEnemy::StaticClass()))
	{
		AZigZagEnemy* Enemy = Cast<AZigZagEnemy>(OtherActor);
		if (Enemy)
		{
			Enemy->MyTakeDamage(BulletDamage);
		}
	}
	Destroy();
}

void AFPSProjectile::OnProjectileOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor->IsA(AFPSCharacter::StaticClass()))
	{
		AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor);
		if (Player)
		{
			Player->MyTakeDamage(BulletDamage);
		}
		Destroy();
	}
}