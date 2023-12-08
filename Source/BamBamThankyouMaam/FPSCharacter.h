// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include <PhysicsEngine/PhysicsHandleComponent.h>
#include "FPSCharacter.generated.h"

UCLASS()
class BAMBAMTHANKYOUMAAM_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float health;

	// Function to apply damage to the character
	UFUNCTION(BlueprintCallable, Category = "Player")
	void MyTakeDamage(float DamageAmount);

	// Function to update the health debug display
	void UpdateHealthDebugDisplay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles input for moving forward and backward.
	UFUNCTION()
	void MoveForward(float Value);

	// Handles input for moving right and left.
	UFUNCTION()
	void MoveRight(float Value);

	// Sets jump flag when key is pressed.
	UFUNCTION()
	void StartJump();

	// Clears jump flag when key is released.
	UFUNCTION()
	void StopJump();

	// Allows Player to interact with objects (gravity gun)
	UFUNCTION()
	void Interact();

	// Function that handles firing projectiles.
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void HideGravityGun(USceneComponent* SceneComponent);

	UFUNCTION()
	void Release();
	void Release(float Force);

	UFUNCTION()
	void SwitchWeapon();

	// FPS camera.
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	// First-person mesh (arms), visible only to the owning player.
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;

	bool bGravityGun = false;
	bool bHasAttachment = false;

private:
	// Other functions and properties as needed

	UPROPERTY(EditAnywhere)
	float ReachDistance;

	UPROPERTY()
	UPrimitiveComponent* CurrentlyGrabbedComponent;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle;
};
