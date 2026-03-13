// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TitanCharacter.generated.h"

class UTectonicMovementComponent;

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
