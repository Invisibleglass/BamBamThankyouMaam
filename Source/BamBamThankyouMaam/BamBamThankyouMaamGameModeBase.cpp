// Copyright Epic Games, Inc. All Rights Reserved.


#include "BamBamThankyouMaamGameModeBase.h"
#include "FPSCharacter.h"
#include "Public/BoomerEnemy.h"
#include "Public/ShootingEnemy.h"
#include "Public/ZigZagEnemy.h"
#include "Math/UnrealMathUtility.h"

void ABamBamThankyouMaamGameModeBase::StartPlay()
{
	Super::StartPlay();
	//Log to Screen
	// C Assert
	check(GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Starting FPS Map")));

	UE_LOG(LogTemp, Warning, TEXT("Starting FPS Map"));
}

void ABamBamThankyouMaamGameModeBase::CreateActor()
{
    // Define the size of the box where enemies will spawn
    FVector SpawnBoxExtent(2000.0f, 2000.0f, 100.0f);
    // Get a random edge index (0-3)
    int32 RandomEdgeIndex = FMath::RandRange(0, 3);
    int32 RandomEnemyIndex = FMath::RandRange(0, 2);

    // Calculate a random point along the selected edge
    FVector SpawnLocation;
    switch (RandomEdgeIndex)
    {
    case 0:
        SpawnLocation = FVector(FMath::RandRange(-SpawnBoxExtent.X, SpawnBoxExtent.X), SpawnBoxExtent.Y, SpawnBoxExtent.Z);
        break;
    case 1:
        SpawnLocation = FVector(FMath::RandRange(-SpawnBoxExtent.X, SpawnBoxExtent.X), -SpawnBoxExtent.Y, SpawnBoxExtent.Z);
        break;
    case 2:
        SpawnLocation = FVector(SpawnBoxExtent.X, FMath::RandRange(-SpawnBoxExtent.Y, SpawnBoxExtent.Y), SpawnBoxExtent.Z);
        break;
    case 3:
        SpawnLocation = FVector(-SpawnBoxExtent.X, FMath::RandRange(-SpawnBoxExtent.Y, SpawnBoxExtent.Y), SpawnBoxExtent.Z);
        break;
    }

    // Choose a random enemy class to spawn
    TSubclassOf<ACharacter> EnemyClass;

    switch (RandomEnemyIndex)
    {
    case 0:
        EnemyClass = ABoomerEnemy::StaticClass();
        break;
    case 1:
        EnemyClass = AShootingEnemy::StaticClass();
        break;
    case 2:
        EnemyClass = AZigZagEnemy::StaticClass();
        break;
    }

    // Spawn the chosen enemy class
    ACharacter* Enemy = GetWorld()->SpawnActor<ACharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
}

void ABamBamThankyouMaamGameModeBase::SpawnWave()
{
    // Define the size of the box where enemies will spawn
    FVector SpawnBoxExtent(2000.0f, 2000.0f, 100.0f);

    // Define the number of enemies to spawn
    EnemysInWave = 12;

    // Spawn enemies with a delay
    for (int32 i = 0; i < EnemysInWave; ++i)
    {
        // Set up a timer to spawn the enemy after a delay
        float Delay = i * 1.0f; // 1.0f second delay between each enemy
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABamBamThankyouMaamGameModeBase::SpawnEnemyWithDelay, Delay, false);
    }
}

void ABamBamThankyouMaamGameModeBase::SpawnEnemyWithDelay()
{
    // Define the size of the box where enemies will spawn
    FVector SpawnBoxExtent(2000.0f, 2000.0f, 100.0f);
    // Get a random edge index (0-3)
    int32 RandomEdgeIndex = FMath::RandRange(0, 3);

    // Calculate a random point along the selected edge
    FVector SpawnLocation;
    switch (RandomEdgeIndex)
    {
    case 0:
        SpawnLocation = FVector(FMath::RandRange(-SpawnBoxExtent.X, SpawnBoxExtent.X), SpawnBoxExtent.Y, SpawnBoxExtent.Z);
        break;
    case 1:
        SpawnLocation = FVector(FMath::RandRange(-SpawnBoxExtent.X, SpawnBoxExtent.X), -SpawnBoxExtent.Y, SpawnBoxExtent.Z);
        break;
    case 2:
        SpawnLocation = FVector(SpawnBoxExtent.X, FMath::RandRange(-SpawnBoxExtent.Y, SpawnBoxExtent.Y), SpawnBoxExtent.Z);
        break;
    case 3:
        SpawnLocation = FVector(-SpawnBoxExtent.X, FMath::RandRange(-SpawnBoxExtent.Y, SpawnBoxExtent.Y), SpawnBoxExtent.Z);
        break;
    }

    TSoftClassPtr<ACharacter> ActorBpClassBoomer = TSoftClassPtr<ACharacter>(FSoftObjectPath(TEXT("Blueprint'/BamBamThankyouMaam/Content/Blueprints/BP_BoomerEnemy.uasset'")));
    TSoftClassPtr<ACharacter> ActorBpClassShooter = TSoftClassPtr<ACharacter>(FSoftObjectPath(TEXT("Blueprint'/BamBamThankyouMaam/Content/Blueprints/BP_BoomerEnemy.uasset'")));
    TSoftClassPtr<ACharacter> ActorBpClassZigZag = TSoftClassPtr<ACharacter>(FSoftObjectPath(TEXT("Blueprint'/BamBamThankyouMaam/Content/Blueprints/BP_BoomerEnemy.uasset'")));

    UClass* LoadedBPAsset = nullptr;

    int32 RandomEnemyIndex = FMath::RandRange(0, 2);

    switch (RandomEnemyIndex)
    {
    case 0:
        LoadedBPAsset = ActorBpClassBoomer.LoadSynchronous();
        break;
    case 1:
        LoadedBPAsset = ActorBpClassShooter.LoadSynchronous();
        break;
    case 2:
        LoadedBPAsset = ActorBpClassZigZag.LoadSynchronous();
        break;
    }

    if (LoadedBPAsset)
    {
        // Spawn the chosen enemy class at the specified location
        ACharacter* SpawnedEnemy = GetWorld()->SpawnActor<ACharacter>(LoadedBPAsset, SpawnLocation, FRotator::ZeroRotator); 

        if (SpawnedEnemy)
        {
            // Optionally, you can perform additional configuration or initialization for each spawned enemy here.
        }
    }
    else
    {
        // Handle the case where LoadedBPAsset is not initialized (you might want to log an error or take appropriate action)
        UE_LOG(LogTemp, Error, TEXT("LoadedBPAsset is not initialized"));
    }
}