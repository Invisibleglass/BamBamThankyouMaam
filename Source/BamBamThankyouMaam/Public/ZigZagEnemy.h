// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZigZagEnemy.generated.h"

UCLASS()
class BAMBAMTHANKYOUMAAM_API AZigZagEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AZigZagEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void MyTakeDamage(float Damage);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float health;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AZigZagEnemy>ActorToSpawn;

private:
	AActor* PlayerCharacter;
	float ZigZagSpeed;
	float ZigZagAmplitude;
	float TimeSinceLastChange;
	float RotationInterpSpeed;
	float AttackDistance;
	float HitDistance;

	bool bIsMovingRight;
	bool bAttacked;

	void MoveZigZag(float DeltaTime);
	void Attack(float DeltaTime);
	void HitPlayer();
	bool CheckForObstacle();
};
