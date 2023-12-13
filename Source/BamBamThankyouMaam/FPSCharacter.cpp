// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "GravityGun.h"
#include "Kismet/GameplayStatics.h"
#include "HUDWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);

	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	// Enable the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;

	// Create a first person mesh component for the owning player.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// Only the owning player sees this mesh.
	FPSMesh->SetOnlyOwnerSee(true);

	// Attach the FPS mesh to the FPS camera.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// Disable some environmental shadows to preserve the illusion of having a single mesh.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// The owning player doesn't see the regular (third-person) body mesh.
	GetMesh()->SetOwnerNoSee(true);

	ReachDistance = 500.0f;
	CurrentlyGrabbedComponent = nullptr;
	PhysicsHandle = PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));

	MaxHealth = 3;
	Health = MaxHealth;

	HealthHUDClass = nullptr;
	PlayerHUD = nullptr;
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthHUDClass)
	{
		APlayerController* PlayerController = GetController<APlayerController>();
		PlayerHUD = CreateWidget<UHUDWidget>(PlayerController, HealthHUDClass);
		PlayerHUD->AddToViewport();
		PlayerHUD->SetHealth(Health, MaxHealth);
	}

	check(GEngine != nullptr);
	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	UpdateHealthDebugDisplay();

	EnemysKilled = 0;
	EnemysInWave = 12;
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		FVector CameraForwardVector = FPSCameraComponent->GetForwardVector();

		// Specify the distance you want to move the object
		float DistanceToMove = 500.0f;

		// Calculate the new location by adding the forward vector multiplied by the distance
		FVector NewLocation = GetActorLocation() + CameraForwardVector * DistanceToMove;

		// Perform a downward trace to find the ground 
		FVector DownwardTraceStart = NewLocation;
		FVector DownwardTraceEnd = DownwardTraceStart - FVector::UpVector * 200.0f; // Adjust the trace distance as needed

		FHitResult FloorHitResult;
		FCollisionQueryParams FloorCollisionParams;
		FloorCollisionParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(FloorHitResult, DownwardTraceStart, DownwardTraceEnd, ECC_Visibility, FloorCollisionParams))
		{
			// Adjust the new location to stay above the ground
			NewLocation.Z = FloorHitResult.Location.Z + 10.0f; // Adjust the offset as needed
		}

		PhysicsHandle->SetTargetRotation(this->GetActorRotation());

		// Set the target location for smooth interpolation
		PhysicsHandle->SetTargetLocation(NewLocation);
	}
	if (EnemysKilled >= EnemysInWave)
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("YouWon"), true);
	}
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up "movement" bindings.
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	// Set up "look" bindings.
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
	// Set up "action" bindings.
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
	// Set up "fire" bindings.
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
	// Set up "release" bindings
	PlayerInputComponent->BindAction("Release", IE_Pressed, this, &AFPSCharacter::Release);
	// Bind the "Interact" function to a custom input
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSCharacter::Interact);
	// Set up the "switchweapon" bindings
	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &AFPSCharacter::SwitchWeapon);
}

void AFPSCharacter::MoveForward(float Value)
{
	// Find out which way is "forward" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
	// Find out which way is "right" and record that the player wants to move that way.
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
	bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
	bPressedJump = false;
}

void AFPSCharacter::Fire()
{
	if (bGravityGun)
	{
		if (CurrentlyGrabbedComponent == nullptr)
		{
			FVector LineTraceStart = GetActorLocation();
			FVector LineTraceEnd = LineTraceStart + GetActorForwardVector() * ReachDistance;

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);

			// Perform a line trace to find a grabbable object
			if (GetWorld()->LineTraceSingleByObjectType(HitResult, LineTraceStart, LineTraceEnd, FCollisionObjectQueryParams::AllObjects, CollisionParams))
			{
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();

				// If the actor has physics, grab it
				if (HitComponent && HitComponent->IsSimulatingPhysics())
				{
					// Get the player's camera location and forward vector
					FVector CameraLocation;
					FRotator CameraRotation;
					GetActorEyesViewPoint(CameraLocation, CameraRotation);

					// Calculate the target location in front of the player
					float DesiredDistance = 500.0f; // Adjust this value as needed
					FVector TargetLocation = CameraLocation + CameraRotation.Vector() * DesiredDistance;

					FHitResult FloorHitResult;
					FVector DownwardTraceStart = HitResult.ImpactPoint;
					FVector DownwardTraceEnd = DownwardTraceStart - FVector(0.0f, 0.0f, 100.0f); // Adjust the trace length as needed

					if (GetWorld()->LineTraceSingleByChannel(FloorHitResult, DownwardTraceStart, DownwardTraceEnd, ECC_Visibility, CollisionParams))
					{
						// Adjust the target location to prevent clipping through the floor during grabbing
						TargetLocation = FloorHitResult.ImpactPoint + FVector(0.0f, 0.0f, 10.0f); // Adjust the height as needed
					}

					// Grab the component at the modified target location
					PhysicsHandle->GrabComponentAtLocation(HitComponent, NAME_None, HitComponent->GetCenterOfMass());
					CurrentlyGrabbedComponent = HitComponent;
				}
			}
		}
		else
		{
			float Force = 3000.0f;
			Release(Force);
		}
	}
	else
	{
		// Attempt to fire a projectile.
		if (ProjectileClass)
		{
			// Get the camera transform.
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
			MuzzleOffset.Set(132.0f, 35.0f, -10.0f);

			// Transform MuzzleOffset from camera space to world space.
			FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

			// Skew the aim to be slightly upwards.
			FRotator MuzzleRotation = CameraRotation;
			MuzzleRotation.Pitch += 0.7f;
			MuzzleRotation.Yaw += -1.0f;

			UWorld* World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				// Spawn the projectile at the muzzle.
				AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
				if (Projectile)
				{
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = MuzzleRotation.Vector();
					Projectile->FireInDirection(LaunchDirection);
				}
			}
		}
	}
}

void AFPSCharacter::MyTakeDamage(float DamageAmount)
{
	Health -= DamageAmount;
	
	// Check if the health goes below zero and handle death or other logic if needed
	if (Health <= 0.0f)
	{
		// Handle death or other logic here
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("YouLost"), true);
	}

	// Update health debug display
	UpdateHealthDebugDisplay();
}

void AFPSCharacter::UpdateHealthDebugDisplay()
{
	PlayerHUD->SetHealth(Health, MaxHealth);
}

void AFPSCharacter::Interact()
{
	// Check for nearby Gravity Guns to collect
	FHitResult HitResult;
	FVector Start = FPSCameraComponent->GetComponentLocation();
	FVector ForwardVector = FPSCameraComponent->GetForwardVector();
	FVector End = Start + ForwardVector * 200.0f; 

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 1.0f);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		AGravityGun* HitGravityGun = Cast<AGravityGun>(HitResult.GetActor());

		if (HitGravityGun)
		{
			// Change the bGravityGun boolean to true
			bGravityGun = true;
			bHasAttachment = true;
			// Destroy the Gravity Gun
			HitGravityGun->Destroy();
		}
	}
}

void AFPSCharacter::Release()
{
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->ReleaseComponent();
		CurrentlyGrabbedComponent->SetAllPhysicsLinearVelocity(FVector());
		CurrentlyGrabbedComponent = nullptr;
	}
}
void AFPSCharacter::Release(float Force)
{
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		// Release the physics handle
		PhysicsHandle->ReleaseComponent();

		if (bGravityGun)
		{
			// Set a constant velocity to the released object
			FVector CameraForwardVector = FPSCameraComponent->GetForwardVector();
			CurrentlyGrabbedComponent->SetAllPhysicsLinearVelocity(CameraForwardVector * Force);
		}

		// Reset the grabbed component
		CurrentlyGrabbedComponent = nullptr;
	}
}

void AFPSCharacter::SwitchWeapon()
{
	if (bHasAttachment)
	{
		if (!bGravityGun)
		{
			bGravityGun = true;
		}
		else
		{
			bGravityGun = false;
		}
	}
}
void AFPSCharacter::HideGravityGun(USceneComponent* SceneComponent)
{

}
