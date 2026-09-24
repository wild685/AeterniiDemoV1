// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CombatAbilitySystemComponent.generated.h"

class UGameplayAbility;
class UGameplayEffect;

/**
 *  Thin ASC wrapper used by the player, enemies, and the boss.
 *  InitAbilityActorInfo must be called after possess (player and AI).
 *  Player replication is Mixed. AI replication is Minimal.
 */
UCLASS()
class UCombatAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UCombatAbilitySystemComponent();

	/** Grants the ability once if it is not already on this ASC. */
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void GrantAbilityIfMissing(TSubclassOf<UGameplayAbility> AbilityClass);

	/** Applies GE_Damage_Melee (or a substitute) to Target. Skips Status.Dodge.IFrames. */
	UFUNCTION(BlueprintCallable, Category = "GAS")
	bool ApplyMeleeDamageTo(AActor* Target, float Damage, TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(BlueprintPure, Category = "GAS")
	bool HasCombatTag(FGameplayTag Tag) const;
};
