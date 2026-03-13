// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TitanVFXComponent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

/**
 * UTitanVFXComponent
 *
 * Sprint 13: Niagara VFX Director — The Mountain's Breath
 *
 * Manages and scales Niagara particle systems based on the
 * Titan's physical state: footstep dust, subduction debris,
 * and blight smoke.
 *
 * ARCHITECTURE:
 *   - BeginPlay spawns looping systems (dust) attached to owner.
 *   - Tick reads velocity and drives spawn rate parameters.
 *   - TriggerSubductionBurst fires one-shot debris on demand.
 *   - FNiagaraMath handles all scaling (proven via TDD).
 *
 * BUDGET (780M iGPU):
 *   - Dust capped at 500/s, short lifetime → ~250 alive max
 *   - Debris burst is one-shot, self-destroying
 *   - Total GPU particles < 2K at any time
 */
UCLASS(ClassGroup = (Orogeny), meta = (BlueprintSpawnableComponent))
class OROGENY_API UTitanVFXComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTitanVFXComponent();

	// -----------------------------------------------------------------------
	// Niagara System Assets (assign in Editor)
	// -----------------------------------------------------------------------

	/** Looping footstep dust — driven by velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|VFX")
	TObjectPtr<UNiagaraSystem> FootstepDustSystem;

	/** One-shot subduction debris burst */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|VFX")
	TObjectPtr<UNiagaraSystem> SubductionDebrisSystem;

	/** Looping blight smoke — driven by corruption ratio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|VFX")
	TObjectPtr<UNiagaraSystem> BlightSmokeSystem;

	// -----------------------------------------------------------------------
	// Live Component References
	// -----------------------------------------------------------------------

	/** Active looping dust emitter */
	UPROPERTY(VisibleAnywhere, Transient, Category = "Orogeny|VFX|State")
	TObjectPtr<UNiagaraComponent> ActiveDustComponent;

	/** Active looping smoke emitter */
	UPROPERTY(VisibleAnywhere, Transient, Category = "Orogeny|VFX|State")
	TObjectPtr<UNiagaraComponent> ActiveSmokeComponent;

	// -----------------------------------------------------------------------
	// State (read-only, for debugging)
	// -----------------------------------------------------------------------

	/** Current dust spawn rate being sent to Niagara */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|VFX|State")
	float CurrentDustRate = 0.0f;

	/** Current smoke opacity being sent to Niagara */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|VFX|State")
	float CurrentSmokeOpacity = 0.0f;

	// -----------------------------------------------------------------------
	// Public Interface
	// -----------------------------------------------------------------------

	/**
	 * Trigger a one-shot debris burst at the Titan's feet.
	 * Called by UTitanSubductionComponent when subduction reaches depth.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|VFX")
	void TriggerSubductionBurst(float CurrentDepth);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
