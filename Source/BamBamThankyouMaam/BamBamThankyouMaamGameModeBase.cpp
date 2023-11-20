// Copyright Epic Games, Inc. All Rights Reserved.


#include "BamBamThankyouMaamGameModeBase.h"

void ABamBamThankyouMaamGameModeBase::StartPlay()
{
	Super::StartPlay();

	//Log to Screen
	// C Assert
	check(GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Starting FPS Map")));

	UE_LOG(LogTemp, Warning, TEXT("Starting FPS Map"));
}
