// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSProjectile.h"
#include "BamBamThankyouMaam\FPSCharacter.h"
#include "ShootingEnemy.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BAMBAMTHANKYOUMAAM_API AShootingEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AShootingEnemy();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float RotationInterpSpeed = 20.0f;

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void MyTakeDamage(float Damage);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float health;

private:
    // Projectile class to spawn.
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<class AFPSProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    TArray<AActor*> PatrolWaypointActors;

    AActor* PlayerCharacter;

    FVector NextWaypoint;

    FRotator DesiredRotation;

    FTimerHandle ShootTimerHandle;

    UPROPERTY(EditAnywhere, Category = "Patrol")
    float PatrolWaypointProximity;

    float ShootCooldownTime;
    float SightRadius;
    float PatrolSpeed;

    int32 CurrentWaypointIndex;

    void MoveToNextWaypoint();
    void StartShooting();
    void ResetShootCooldown();
};
