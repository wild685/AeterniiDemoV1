// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatEnemy.h"
#include "Engine/TimerHandle.h"
#include "GhuraziHordeEnemy.generated.h"

/**
 *  Ghurazi Horde archetypes from the isometric vertical slice.
 *  Stats and telegraph timings are data on this class — the AI still runs through
 *  ACombatAIController + the existing Combat StateTree tasks (Combo / Charged /
 *  IsInDanger / EQS Danger). No parallel AI stack.
 */
UENUM(BlueprintType)
enum class EGhuraziHordeArchetype : uint8
{
	/** Fast swarm melee. Short, readable windup then a combo string. */
	Raider UMETA(DisplayName = "Raider"),

	/** Star-priest. Long chant telegraph, then a delayed strike at the location the target occupied when the windup began. */
	Chanter UMETA(DisplayName = "Chanter"),

	/** Heavy war-band brute. Medium telegraph, wide melee slam via charged attack. */
	Mauler UMETA(DisplayName = "Mauler")
};

/**
 *  Star-aligned Ghurazi Horde pawn. Subclasses ACombatEnemy so existing spawners,
 *  damage routing (ICombatDamageable::ApplyDamage), and StateTree tasks keep working.
 *
 *  Telegraph timing is expressed by driving the existing charged-attack loop count
 *  from TelegraphDuration, plus a C++ commit timer so the tell is readable even
 *  before montage notifies exist. Do not add new StateTree task types for this.
 *
 *  Damage (settled): pack/horde hits stay on ICombatDamageable::ApplyDamage /
 *  TakeDamage, same as ACombatEnemy. Do not route them through UAeterniiAttributeSet
 *  or the legacy boss UCombatAttributeSet. The enemy ASC is only for Noetic Arts
 *  and Depth/Corruption.
 */
UCLASS(abstract)
class AGhuraziHordeEnemy : public ACombatEnemy
{
	GENERATED_BODY()

public:

	AGhuraziHordeEnemy();

	/** Horde role. Changing this in the editor reapplies archetype combat defaults. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde")
	EGhuraziHordeArchetype Archetype = EGhuraziHordeArchetype::Raider;

	/** Windup duration copied from the isometric demo (Raider 0.34 / Mauler 0.72 / Chanter 1.35). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Telegraph", meta = (ClampMin = 0.05, ClampMax = 5, Units = "s"))
	float TelegraphDuration = 0.34f;

	/** Assumed length of ChargedAttackMontage's charge-loop section, used to map TelegraphDuration onto TargetChargeLoops. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Telegraph", meta = (ClampMin = 0.05, ClampMax = 2, Units = "s"))
	float ChargeLoopDuration = 0.35f;

	/** Delay after telegraph commit before a fallback DoAttackTrace, in case the montage has no AnimNotify_DoAttackTrace yet. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Telegraph", meta = (ClampMin = 0.05, ClampMax = 2, Units = "s"))
	float FallbackStrikeDelay = 0.12f;

	/**
	 *  Suggested StateTree Delay after Combo/Charged Attack (iso cooldowns:
	 *  Raider 1.35 / Mauler 2.3 / Chanter 3.1). Not gated in C++ — the tree owns pacing
	 *  so FStateTreeComboAttackTask / FStateTreeChargedAttackTask still complete via OnAttackCompleted.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|AI", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float AttackRecoveryTime = 1.35f;

	/** Suggested GetPlayerInfo / MoveTo engage distance (Raider ~105, Mauler ~145, Chanter ~640). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|AI", meta = (ClampMin = 0, ClampMax = 2000, Units = "cm"))
	float PreferredEngageRange = 105.0f;

	/** Soft separation radius so the pack does not collapse into one silhouette. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Pack", meta = (ClampMin = 0, ClampMax = 400, Units = "cm"))
	float HordeSeparationDistance = 90.0f;

	/** How hard each overlapping Ghurazi is pushed apart per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Pack", meta = (ClampMin = 0, ClampMax = 2000, Units = "cm/s"))
	float HordeSeparationSpeed = 180.0f;

	/** Chanter-only overlap radius for the delayed strike at the cached cast location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ghurazi Horde|Chanter", meta = (ClampMin = 0, ClampMax = 1000, Units = "cm", EditCondition = "Archetype == EGhuraziHordeArchetype::Chanter"))
	float ChantStrikeRadius = 190.0f;

	/** Copy isometric-demo stats for the current Archetype onto this enemy. */
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Ghurazi Horde")
	void ApplyArchetypeDefaults();

	UFUNCTION(BlueprintPure, Category="Ghurazi Horde|Telegraph")
	bool IsTelegraphing() const { return bIsTelegraphing; }

	/** 0 at windup start, 1 at commit. */
	UFUNCTION(BlueprintPure, Category="Ghurazi Horde|Telegraph")
	float GetTelegraphAlpha() const;

	UFUNCTION(BlueprintPure, Category="Ghurazi Horde|Telegraph")
	FVector GetCachedCastLocation() const { return CachedCastLocation; }

	// ~begin AI attack entry points used by FStateTreeComboAttackTask / FStateTreeChargedAttackTask
	virtual void DoAIComboAttack() override;
	virtual void DoAIChargedAttack() override;
	virtual void AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted) override;
	// ~end AI attack entry points

	virtual void DoAttackTrace(FName DamageSourceBone) override;
	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void HandleDeath() override;

protected:

	virtual int32 ChooseComboHitCount() const override;
	virtual int32 ChooseChargeLoopCount() const override;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Readable windup: freeze, face the player, cache the chant location, notify danger. */
	void BeginTelegraph(bool bChargedAttack);

	void CommitTelegraph();
	void TryFallbackStrike();
	void ClearTelegraphState();
	void RestoreMovementFromTelegraph();
	void CacheCastLocationFromPlayer();
	void NotifyPlayerOfTelegraph();
	void ApplyHordeSeparation(float DeltaSeconds);
	void ApplyChantStrike();
	void DealHordeDamage(AActor* HitActor, const FVector& DamageLocation, const FVector& Impulse);
	void PlayComboMontage();
	void PlayChargedMontage();

	UFUNCTION(BlueprintImplementableEvent, Category="Ghurazi Horde|Telegraph")
	void BP_OnTelegraphStarted(EGhuraziHordeArchetype InArchetype, float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category="Ghurazi Horde|Telegraph")
	void BP_OnTelegraphCommitted(EGhuraziHordeArchetype InArchetype);

	bool bIsTelegraphing = false;
	bool bPendingChargedAttack = false;
	bool bDidStrikeThisAttack = false;
	bool bMovementFrozenForTelegraph = false;

	float WalkSpeedBeforeTelegraph = 0.0f;
	float TelegraphStartTime = 0.0f;

	FVector CachedCastLocation = FVector::ZeroVector;

	FTimerHandle TelegraphTimer;
	FTimerHandle FallbackStrikeTimer;
};
