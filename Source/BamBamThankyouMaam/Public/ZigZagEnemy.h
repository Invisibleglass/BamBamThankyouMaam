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

private:
	AActor* PlayerCharacter;
	float ZigZagSpeed;
	float ZigZagAmplitude;
	float TimeSinceLastChange;
	float RotationInterpSpeed;
	bool bIsMovingRight;

	void MoveZigZag(float DeltaTime);
	bool CheckForObstacle();
};
