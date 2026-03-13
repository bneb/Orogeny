// Copyright Orogeny. All Rights Reserved.

#include "SaveLoadSubsystem.h"
#include "Orogeny.h"
#include "OrogenySaveGame.h"
#include "DeepTimeSubsystem.h"
#include "EcosystemArmorComponent.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// PackSaveData — Pure Data Transfer (TDD-testable)
// ============================================================================

void USaveLoadSubsystem::PackSaveData(
	UOrogenySaveGame* SaveObject,
	double CurrentDay, float EcoHealth,
	const FVector& TitanLoc,
	const TArray<FIntPoint>& BlightCells)
{
	if (!SaveObject)
	{
		return;
	}

	SaveObject->SavedDeepTimeDay = CurrentDay;
	SaveObject->SavedEcosystemHealth = EcoHealth;
	SaveObject->SavedTitanLocation = TitanLoc;
	SaveObject->SavedBlightGrid = BlightCells;
}

// ============================================================================
// ExtractBlightGrid — TMap Compression (TDD-testable)
// ============================================================================
// Only corrupted (true) cells are serialized.
// Clean cells are omitted — saves disk space and load time.
// ============================================================================

TArray<FIntPoint> USaveLoadSubsystem::ExtractBlightGrid(
	const TMap<FIntPoint, bool>& LiveGrid)
{
	TArray<FIntPoint> CorruptedCells;
	CorruptedCells.Reserve(LiveGrid.Num());

	for (const auto& Pair : LiveGrid)
	{
		if (Pair.Value)
		{
			CorruptedCells.Add(Pair.Key);
		}
	}

	return CorruptedCells;
}

// ============================================================================
// SaveGame — Gather State + Async Write
// ============================================================================

void USaveLoadSubsystem::SaveGame(FString SlotName)
{
	UOrogenySaveGame* SaveObject = NewObject<UOrogenySaveGame>();
	SaveObject->SaveSlotName = SlotName;
	SaveObject->UserIndex = 0;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogOrogeny, Error, TEXT("SaveLoad: No World — cannot save!"));
		return;
	}

	// Deep Time
	double CurrentDay = 0.0;
	if (UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>())
	{
		CurrentDay = DeepTime->CurrentDay;
	}

	// Ecosystem Health
	float EcoHealth = 1.0f;
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	FVector TitanLoc = FVector::ZeroVector;
	if (PlayerPawn)
	{
		TitanLoc = PlayerPawn->GetActorLocation();

		UEcosystemArmorComponent* Ecosystem =
			PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
		if (Ecosystem)
		{
			EcoHealth = Ecosystem->EcosystemHealth;
		}
	}

	// Blight Grid — future: gather from global blight manager
	TArray<FIntPoint> BlightCells;

	// Pack
	PackSaveData(SaveObject, CurrentDay, EcoHealth, TitanLoc, BlightCells);

	// Async write
	UGameplayStatics::AsyncSaveGameToSlot(
		SaveObject, SlotName, 0,
		FAsyncSaveGameToSlotDelegate::CreateLambda(
			[this, SlotName](const FString& InSlotName, const int32 InUserIndex, bool bSuccess)
			{
				if (bSuccess)
				{
					UE_LOG(LogOrogeny, Log,
						TEXT("SaveLoad: ✅ Saved to slot '%s'"), *InSlotName);
				}
				else
				{
					UE_LOG(LogOrogeny, Error,
						TEXT("SaveLoad: ❌ Failed to save slot '%s'"), *InSlotName);
				}
			}
		)
	);
}

// ============================================================================
// LoadGame — Async Read + Restore State
// ============================================================================

void USaveLoadSubsystem::LoadGame(FString SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("SaveLoad: Slot '%s' does not exist!"), *SlotName);
		return;
	}

	UGameplayStatics::AsyncLoadGameFromSlot(
		SlotName, 0,
		FAsyncLoadGameFromSlotDelegate::CreateUObject(
			this, &USaveLoadSubsystem::OnLoadComplete)
	);
}

void USaveLoadSubsystem::OnLoadComplete(
	const FString& SlotName, int32 UserIdx, USaveGame* LoadedSaveGame)
{
	UOrogenySaveGame* SaveData = Cast<UOrogenySaveGame>(LoadedSaveGame);
	if (!SaveData)
	{
		UE_LOG(LogOrogeny, Error,
			TEXT("SaveLoad: ❌ Failed to load slot '%s'"), *SlotName);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Restore Deep Time
	if (UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>())
	{
		DeepTime->CurrentDay = SaveData->SavedDeepTimeDay;
		UE_LOG(LogOrogeny, Log,
			TEXT("SaveLoad: Restored DeepTime to day %.1f"),
			SaveData->SavedDeepTimeDay);
	}

	// Restore Titan Position + Ecosystem
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (PlayerPawn)
	{
		PlayerPawn->SetActorLocation(SaveData->SavedTitanLocation);

		UEcosystemArmorComponent* Ecosystem =
			PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
		if (Ecosystem)
		{
			Ecosystem->EcosystemHealth = SaveData->SavedEcosystemHealth;
		}

		UE_LOG(LogOrogeny, Log,
			TEXT("SaveLoad: Restored Titan at (%.0f, %.0f, %.0f), Health=%.2f"),
			SaveData->SavedTitanLocation.X,
			SaveData->SavedTitanLocation.Y,
			SaveData->SavedTitanLocation.Z,
			SaveData->SavedEcosystemHealth);
	}

	// Blight Grid — future: push SaveData->SavedBlightGrid back into blight manager

	UE_LOG(LogOrogeny, Log,
		TEXT("SaveLoad: ✅ Loaded slot '%s' (Day %.1f, Health %.0f%%, %d blight cells)"),
		*SlotName,
		SaveData->SavedDeepTimeDay,
		SaveData->SavedEcosystemHealth * 100.0f,
		SaveData->SavedBlightGrid.Num());
}
