// Copyright Epic Games, Inc. All Rights Reserved.


#include "BamBamThankyouMaamGameModeBase.h"
#include "FPSCharacter.h"

void ABamBamThankyouMaamGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Set default pawn class to your character
	//DefaultPawnClass = AFPSCharacter::StaticClass();

	//Log to Screen
	// C Assert
	check(GEngine != nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Starting FPS Map")));

	UE_LOG(LogTemp, Warning, TEXT("Starting FPS Map"));
}
