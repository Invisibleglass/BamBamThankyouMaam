// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BoomerEnemy.generated.h"

UCLASS()
class BAMBAMTHANKYOUMAAM_API ABoomerEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABoomerEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void Explode();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void MyTakeDamage(float Damage);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void Wander();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void MoveRandomly();

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ResetMovementFlag();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float PatrolRadius;


private:
	AActor* PlayerCharacter;
	AActor* PatrolCube;
	FTimerHandle WaitTimerHandle;
	float WaitTime;
	bool bIsMoving;
};
