// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityGun.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AGravityGun::AGravityGun()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAbilitiesEnabled = false;
}

// Called when the game starts or when spawned
void AGravityGun::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
}

// Called every frame
void AGravityGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGravityGun::OnInteract()
{

}

