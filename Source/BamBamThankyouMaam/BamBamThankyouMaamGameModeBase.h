// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BamBamThankyouMaamGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BAMBAMTHANKYOUMAAM_API ABamBamThankyouMaamGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	virtual void StartPlay();

public:
	UFUNCTION()
	void CreateActor();

	UFUNCTION()
	void SpawnWave();

	UFUNCTION()
	void SpawnEnemyWithDelay();

	int EnemysInWave;
	int EnemysKilled;
	int CurrentWave;

private:
	FTimerHandle TimerHandle;
};
