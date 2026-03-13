// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TitanCharacter.generated.h"

class UTectonicMovementComponent;
class UTectonicTrenchComponent;
class UTectonicAtmosphereComponent;
class UOrographicLiftComponent;
class UTectonicAudioComponent;
class UEcosystemArmorComponent;
class UTitanSubductionComponent;

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * ATitanCharacter
 *
 * The player-controlled tectonic titan. A colossal humanoid walking through
 * a Romantic-era landscape. Everything about this character conveys mass:
 *
 * - High momentum movement (Day 3: TectonicMovementComponent)
 * - Un-cancelable action states (Day 4: Animation State Machine)
 * - Procedural camera shake on footsteps (Day 5: Scale Camera)
 * - Terrain deformation under feet (Day 6: Render Target Trenching)
 * - Atmospheric wake displacing fog (Day 7: Niagara VFX)
 * - Sub-bass footstep audio (Day 10: MetaSounds)
 *
 * Day 1 establishes the skeleton: capsule, spring arm camera, input stubs.
 * Day 2 adds foot placement IK via UTitanAnimInstance.
 * Day 3 replaces default CMC with UTectonicMovementComponent.
 * Day 4 adds un-cancelable action states via bIsCommitted + AnimNotifyState.
 */
UCLASS(config = Game)
class OROGENY_API ATitanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATitanCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// -----------------------------------------------------------------------
	// Camera System (Day 5 will subclass USpringArmComponent)
	// -----------------------------------------------------------------------

	/** Camera boom — arm length 2500 for colossal scale framing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera attached to the boom */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// -----------------------------------------------------------------------
	// Terrain Deformation (Day 6)
	// -----------------------------------------------------------------------

	/** Persistent R16f render target component for terrain trenching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Trench", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTectonicTrenchComponent> TrenchComponent;

	// -----------------------------------------------------------------------
	// Atmospheric Wake (Day 7)
	// -----------------------------------------------------------------------

	/** Velocity-scaled atmospheric displacement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Atmosphere", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTectonicAtmosphereComponent> AtmosphereComponent;

	// -----------------------------------------------------------------------
	// Combat (Day 9)
	// -----------------------------------------------------------------------

	/** Orographic Lift AOE combat component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOrographicLiftComponent> LiftComponent;

	// -----------------------------------------------------------------------
	// Audio (Day 10)
	// -----------------------------------------------------------------------

	/** Velocity-to-audio parameter pipeline for MetaSound sub-bass */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Audio", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTectonicAudioComponent> AudioComponent;

	// -----------------------------------------------------------------------
	// Ecosystem Armor (Sprint 2)
	// -----------------------------------------------------------------------

	/** Procedural flora HISM — the Mountain's living health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Ecosystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEcosystemArmorComponent> EcosystemComponent;

	// -----------------------------------------------------------------------
	// Subduction (Sprint 4)
	// -----------------------------------------------------------------------

	/** Z-axis subduction mechanic — sink to crush blight */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Subduction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTitanSubductionComponent> SubductionComponent;

	// -----------------------------------------------------------------------
	// Input (Enhanced Input System)
	// -----------------------------------------------------------------------

	/** Input Mapping Context — assigned in Blueprint or via DefaultPawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orogeny|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Move input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orogeny|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look input action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Orogeny|Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	// -----------------------------------------------------------------------
	// Committed Action State (Day 4)
	// -----------------------------------------------------------------------
	// When committed, ALL player input (movement + look) is ignored until
	// the current montage completes. UAnimNotifyState_ActionCommit drives
	// this from animation montages. Direct C++ access via getter/setter.
	// -----------------------------------------------------------------------

	/** Returns true if the Titan is in an un-cancelable action state */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|State")
	bool GetIsCommitted() const { return bIsCommitted; }

	/** Sets the committed state. Called by UAnimNotifyState_ActionCommit. */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|State")
	void SetIsCommitted(bool bInCommitted);

	// -----------------------------------------------------------------------
	// IK Configuration (Day 2: Foot Placement)
	// -----------------------------------------------------------------------

	/** Left foot bone name — must match skeleton */
	static const FName LeftFootBoneName;

	/** Right foot bone name — must match skeleton */
	static const FName RightFootBoneName;

protected:
	/**
	 * When true, the titan is in an un-cancelable action state.
	 * All movement and action inputs are ignored until the montage completes.
	 * Driven by UAnimNotifyState_ActionCommit placed on animation montages.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|State")
	bool bIsCommitted = false;

	/** Called for movement input — blocked when bIsCommitted */
	void Move(const FInputActionValue& Value);

	/** Called for looking input — blocked when bIsCommitted */
	void Look(const FInputActionValue& Value);

public:
	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
