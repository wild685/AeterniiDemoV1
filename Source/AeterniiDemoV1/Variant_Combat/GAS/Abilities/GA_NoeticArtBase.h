// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/TimerHandle.h"
#include "NoeticArtKits.h"
#include "GA_NoeticArtBase.generated.h"

class ACombatCharacter;
class ACombatEnemy;
class UGameplayEffect;

/**
 *  Shared Noetic Art activation: class kit lookup, loose-tag cooldown, cursor aim.
 *  Subclasses execute one isometric kit of a single canon art.
 *  Foe harm goes through ICombatDamageable::ApplyDamage. Pack HP is not an attribute write.
 */
UCLASS(Abstract)
class UGA_NoeticArtBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_NoeticArtBase();

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:

	/** Every kit cooldown tag for this art. CheckCooldown blocks if any is present. */
	FGameplayTagContainer ArtCooldownTags;

	virtual ENoeticCanonArt GetArt() const;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Kit effect. Cooldown starts only when this returns true. */
	virtual bool ExecuteKit(const FNoeticKitSpec& Kit);

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	ACombatCharacter* GetCombatAvatar() const;
	float ToWorld(float DemoUnits) const;
	bool ResolveAimPoint(float DemoRange, FVector& OutPoint) const;
	bool ProbeClear(const FVector& From, const FVector& To, float DemoProbeRadius) const;

	/** Walk the pawn capsule forward. Stops at world static. Returns the accepted location. */
	FVector BlinkTo(const FVector& AimPoint, float DemoRange) const;

	FVector ClearSpot(const FVector& Desired) const;

	void DamageEnemiesNearSegment(const FVector& Start, const FVector& End, float DemoRadius, float Amount) const;
	void DamageEnemiesInRadius(const FVector& Center, float DemoRadius, float Amount) const;
	void SlowEnemiesInRadius(const FVector& Center, float DemoRadius, float Duration, float SlowAmount) const;

	void AddAeterniiAttribute(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude) const;
	void HoldLooseTag(FGameplayTag Tag, float Duration);

	void ForEachLivingEnemy(TFunctionRef<void(ACombatEnemy*)> Callback) const;

private:

	bool ResolveKit(const FGameplayAbilityActorInfo* ActorInfo, FNoeticKitSpec& OutKit) const;
	void StartKitCooldown(const FNoeticKitSpec& Kit);
	void ReleaseLooseTag(FGameplayTag Tag);

	TMap<FGameplayTag, FTimerHandle> LooseTagTimers;
};
