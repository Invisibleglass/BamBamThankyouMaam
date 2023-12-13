// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingEnemy.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AShootingEnemy::AShootingEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    PatrolSpeed = 300.0f;
    PatrolWaypointProximity = 100.0f;
    SightRadius = 2000.0f;
    health = 2.0f;
}

// Called when the game starts or when spawned
void AShootingEnemy::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
    ShootCooldownTime = 3.0f;
    CurrentWaypointIndex = 0;
}

// Called every frame
void AShootingEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter)
    {
        if (FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation()) <= SightRadius)
        {
            StartShooting();
        }
        else
        {
            MoveToNextWaypoint();
        }
    }
}

void AShootingEnemy::MoveToNextWaypoint()
{
    // Check if there are waypoints in the array
    if (PatrolWaypointActors.Num() > 0)
    {
        // Get the current waypoint
        AActor* CurrentWaypoint = PatrolWaypointActors[CurrentWaypointIndex];

        // Check if the current waypoint is valid
        if (CurrentWaypoint)
        {
            // Calculate the direction to the current waypoint
            FVector Direction = CurrentWaypoint->GetActorLocation() - GetActorLocation();
            Direction.Normalize();

            // Calculate the rotation to face the current waypoint
            DesiredRotation = Direction.Rotation();

            // Smoothly interpolate towards the desired rotation
            FRotator CurrentRotation = GetActorRotation();
            FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);

            // Manually interpolate the rotation
            FRotator DeltaRotation = NewRotation - CurrentRotation;
            DeltaRotation.Normalize();
            SetActorRotation(CurrentRotation + DeltaRotation * RotationInterpSpeed * GetWorld()->GetDeltaSeconds());

            // Move towards the current waypoint
            FVector NewLocation = GetActorLocation() + GetActorForwardVector() * PatrolSpeed * GetWorld()->GetDeltaSeconds();
            SetActorLocation(NewLocation);

            // Check if the enemy is close to the current waypoint
            float DistanceToWaypoint = FVector::Dist(CurrentWaypoint->GetActorLocation(), GetActorLocation());
            if (DistanceToWaypoint < PatrolWaypointProximity)
            {
                // Move to the next waypoint
                CurrentWaypointIndex = (CurrentWaypointIndex + 1) % PatrolWaypointActors.Num();
            }
        }
    }
}

void AShootingEnemy::StartShooting()
{
    // Logic to rotate towards the player
    if (PlayerCharacter)
    {
        FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
        Direction.Normalize();
        DesiredRotation = Direction.Rotation();

        // Smoothly interpolate towards the desired rotation
        FRotator CurrentRotation = GetActorRotation();
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);

        // Manually interpolate the rotation
        FRotator DeltaRotation = NewRotation - CurrentRotation;
        DeltaRotation.Normalize();
        SetActorRotation(CurrentRotation + DeltaRotation * RotationInterpSpeed * GetWorld()->GetDeltaSeconds());

        // Check if the enemy is aligned with the player
        if (FMath::Abs(FVector::DotProduct(GetActorForwardVector(), Direction)) > 0.995f)
        {
            // Only shoot when aligned with the player
            if (!GetWorldTimerManager().IsTimerActive(ShootTimerHandle))
            {
                // Spawn the projectile
                if (ProjectileClass)
                {
                    UWorld* World = GetWorld();
                    if (World)
                    {
                        FActorSpawnParameters SpawnParams;
                        SpawnParams.Owner = this;
                        SpawnParams.Instigator = GetInstigator();

                        // Spawn the projectile at the muzzle.
                        FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f); // Adjust the offset values accordingly
                        MuzzleOffset = DesiredRotation.RotateVector(MuzzleOffset);
                        MuzzleOffset = GetActorLocation() + MuzzleOffset;
                        AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleOffset, DesiredRotation, SpawnParams);
                        if (Projectile)
                        {
                            // Set the projectile's initial trajectory.
                            FVector LaunchDirection = this->GetActorRotation().Vector();
                            Projectile->FireInDirection(LaunchDirection);
                        }
                    }
                }

                // Set the cooldown timer
                GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AShootingEnemy::ResetShootCooldown, ShootCooldownTime, false);
            }
        }
    }
}

void AShootingEnemy::ResetShootCooldown()
{
    GetWorldTimerManager().ClearTimer(ShootTimerHandle);
    ShootTimerHandle.Invalidate();
}

void AShootingEnemy::MyTakeDamage(float Damage)
{
    health -= Damage;

    // Check if the health goes below zero and handle death or other logic if needed
    if (health <= 0.0f)
    {
        AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerCharacter);
        if (Player)
        {
            Player->EnemysKilled++;
        }
        Destroy();
    }
}