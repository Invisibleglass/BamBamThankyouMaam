#include "BoomerEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BamBamThankyouMaam\FPSCharacter.h"
#include <Kismet/GameplayStatics.h>

ABoomerEnemy::ABoomerEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set this character to call Tick() every frame.
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	
	health = 3.0f;
	ExplosionDamage = 2.0f;
	ExplosionRadius = 200.0f;
	SightRadius = 2000.0f;
	PatrolRadius = 1000.0f;
}

void ABoomerEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

} 

void ABoomerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter)
	{
		if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= SightRadius)
		{
			// Move towards the player
			FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			AddMovementInput(Direction);

			// Calculate rotation towards the target
			FRotator DesiredRotation = (PlayerCharacter->GetActorLocation() - this->GetActorLocation()).Rotation();
			// Set the enemy's rotation to face the target
			this->SetActorRotation(DesiredRotation);

			// Check if the BoomerEnemy is close enough to the player to explode
			if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= ExplosionRadius)
			{
				Explode();
			}
		}
		else
		{
			Wander();
		}
	}
}

void ABoomerEnemy::Explode()
{
	if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= ExplosionRadius+100)
	{
		// Deal damage to the player
		AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerCharacter);
		if (Player)
		{
			Player->MyTakeDamage(ExplosionDamage);
		}
	}

	// Destroy the BoomerEnemy
	Destroy();
}

void ABoomerEnemy::MyTakeDamage(float Damage)
{
	health -= Damage;

	// Check if the health goes below zero and handle death or other logic if needed
	if (health <= 0.0f)
	{
		Explode();
	}
}

void ABoomerEnemy::Wander()
{
	if (!bIsMoving) // Add a condition to prevent overlapping movements
	{
		bIsMoving = true; // Set a flag to indicate the AI is currently moving

		// Move the AI a couple of steps
		MoveRandomly();

		// Wait before moving again
		GetWorldTimerManager().SetTimer(WaitTimerHandle, this, &ABoomerEnemy::ResetMovementFlag, 2.0f, false); // Adjust the delay as needed
	}
}


void ABoomerEnemy::MoveRandomly()
{
	// Generate a random direction for the AI to move
	FVector RandomDirection = FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.0f);
	RandomDirection.Normalize();

	// Calculate rotation towards the target
	FRotator DesiredRotation = (RandomDirection).Rotation();

	// Set the enemy's rotation to face the target
	SetActorRotation(DesiredRotation);
}


void ABoomerEnemy::ResetMovementFlag()
{
	bIsMoving = false; // Reset the movement flag to allow the AI to move again
}
