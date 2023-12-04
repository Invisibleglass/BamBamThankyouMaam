// Fill out your copyright notice in the Description page of Project Settings.
#include "ZigZagEnemy.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "BamBamThankyouMaam/FPSCharacter.h"

AZigZagEnemy::AZigZagEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	ZigZagSpeed = 300.0f;
	ZigZagAmplitude = 100.0f;
	TimeSinceLastChange = 0.0f;
	bIsMovingRight = true;
	health = 5.0f;
	RotationInterpSpeed = 20.0f;
	AttackDistance = 600.0f;
	HitDistance = 200.0f;
}

void AZigZagEnemy::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
}

void AZigZagEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter)
	{
		if (!CheckForObstacle())
		{
			if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= AttackDistance)
			{
				Attack(DeltaTime);
			}
			else
			{
				bAttacked = false;
				MoveZigZag(DeltaTime);
			}
		}
	}
}

void AZigZagEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AZigZagEnemy::Attack(float DeltaTime)
{
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Normalize();

	// Rotate smoothly to face the player
	if (!DirectionToPlayer.IsNearlyZero())
	{
		FRotator NewRotation = DirectionToPlayer.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, RotationInterpSpeed));
	}

	if (!bAttacked)
	{
		AddMovementInput(DirectionToPlayer, 1.0f);
	}
	else
	{
		AddMovementInput(-DirectionToPlayer, 1.0f);
	}

	if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= HitDistance)
	{
		HitPlayer();
	}

}

void AZigZagEnemy::MoveZigZag(float DeltaTime)
{
	FVector DirectionToPlayer = PlayerCharacter->GetActorLocation() - GetActorLocation();
	DirectionToPlayer.Normalize();

	// Move towards the player
	AddMovementInput(DirectionToPlayer, 0.5f);

	// Oscillate left and right
	FVector OscillationDirection = bIsMovingRight ? GetActorRightVector() : -GetActorRightVector();
	AddMovementInput(OscillationDirection, 1.0f); // Adjust the value for the desired oscillation speed

	TimeSinceLastChange += DeltaTime;

	// Change direction every 2 seconds
	if (TimeSinceLastChange >= 2.0f)
	{
		bIsMovingRight = !bIsMovingRight;
		TimeSinceLastChange = 0.0f;
	}

	// Rotate smoothly to face the player
	if (!DirectionToPlayer.IsNearlyZero())
	{
		FRotator NewRotation = DirectionToPlayer.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, RotationInterpSpeed));
	}
}

bool AZigZagEnemy::CheckForObstacle()
{
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = PlayerCharacter->GetActorLocation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	// Debug: Draw the raycast line
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f, 0, 1.0f);

	return bHit && HitResult.GetActor() != PlayerCharacter;
}

void AZigZagEnemy::MyTakeDamage(float Damage)
{
	health -= Damage;

	// Check if the health goes below zero and handle death or other logic if needed
	if (health <= 0.0f)
	{
		Destroy();
	}
}

void AZigZagEnemy::HitPlayer()
{
	if (!bAttacked)
	{
		// Player takes damage
		AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerCharacter);
		if (Player)
		{
			Player->MyTakeDamage(1.0f);
		}
	}
	bAttacked = true;
}