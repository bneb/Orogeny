// Copyright Orogeny. All Rights Reserved.

#include "OrogenySaveGame.h"

UOrogenySaveGame::UOrogenySaveGame()
	: SaveSlotName(TEXT("OrogenySlot0"))
	, UserIndex(0)
	, SavedDeepTimeDay(0.0)
	, SavedEcosystemHealth(1.0f)
	, SavedTitanLocation(FVector::ZeroVector)
{
}
