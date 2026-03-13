// Copyright Orogeny. All Rights Reserved.

#include "OrogenyGameModeBase.h"
#include "TitanCharacter.h"
#include "Orogeny.h"

AOrogenyGameModeBase::AOrogenyGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn to our Titan character
	DefaultPawnClass = ATitanCharacter::StaticClass();

	UE_LOG(LogOrogeny, Log, TEXT("AOrogenyGameModeBase initialized. DefaultPawn: ATitanCharacter"));
}

void AOrogenyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ElapsedGameTime = 0.0f;
	SupercellOverlapDuration = 0.0f;

	UE_LOG(LogOrogeny, Log, TEXT("Orogeny Game Mode: BeginPlay. Win condition: survive %.0f seconds. Loss: Supercell overlap > %.0f seconds."),
		WinSurvivalTime, LossOverlapThreshold);
}

void AOrogenyGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedGameTime += DeltaTime;

	// Day 12: Full game loop logic will be implemented here.
	// - Check Supercell overlap with player capsule
	// - Track overlap duration
	// - Trigger Win/Loss sequences
}
