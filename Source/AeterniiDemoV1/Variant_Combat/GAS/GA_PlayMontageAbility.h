// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_PlayMontageAbility.generated.h"

class UAnimMontage;

/**
 *  Shared montage ability: PlayMontageAndWait, EndAbility on complete / interrupt.
 *  Subclasses own ActivationOwnedTags (Ability.Attack or Ability.Dodge).
 */
UCLASS(Abstract)
class UGA_PlayMontageAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_PlayMontageAbility();

protected:

	/** Montage to play. Assign in the editor (or on a BP child). Missing asset ends the ability immediately. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> MontageToPlay;

	/** Optional section to start from. Leave None to play from the beginning. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	FName StartSection = NAME_None;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void FinishAbility(bool bWasCancelled);
};
