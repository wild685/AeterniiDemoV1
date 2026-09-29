// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_NoeticArtBase.h"
#include "GA_MemoryBurn.generated.h"

/**
 *  Memory Burn. Isometric kits:
 *  ashen_recitation (ordo, 6.5s, 38 harm inside 2.9, slow 2.2s at 0.45),
 *  censure_lance (ignivarum, 5s, range 7.2, 46 harm within 0.72 of the line),
 *  unwriting (luminarch, 4.2s, range 7, 44 harm inside 2.0 after 0.36s).
 *  Harm is ICombatDamageable::ApplyDamage. No corruption spend.
 */
UCLASS()
class UGA_MemoryBurn : public UGA_NoeticArtBase
{
	GENERATED_BODY()

public:

	UGA_MemoryBurn();

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:

	virtual ENoeticCanonArt GetArt() const override { return ENoeticCanonArt::MemoryBurn; }
	virtual bool ExecuteKit(const FNoeticKitSpec& Kit) override;

	UFUNCTION()
	void ResolveUnwriting();

	FVector PendingUnwriteLocation = FVector::ZeroVector;
	FTimerHandle UnwriteTimer;
};
