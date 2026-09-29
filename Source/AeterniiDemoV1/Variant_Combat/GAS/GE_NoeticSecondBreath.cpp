// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_NoeticSecondBreath.h"
#include "AeterniiAttributeSet.h"
#include "NoeticArtKits.h"

UGE_NoeticSecondBreath::UGE_NoeticSecondBreath()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AeterniiNoeticKits::SecondBreathDuration));

	FGameplayModifierInfo MoveSpeedModifier;
	MoveSpeedModifier.Attribute = UAeterniiAttributeSet::GetMoveSpeedAttribute();
	MoveSpeedModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	MoveSpeedModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AeterniiNoeticKits::SecondBreathSpeedMultiplier));
	Modifiers.Add(MoveSpeedModifier);

	FGameplayModifierInfo AttackCooldownModifier;
	AttackCooldownModifier.Attribute = UAeterniiAttributeSet::GetAttackCooldownAttribute();
	AttackCooldownModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	AttackCooldownModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(AeterniiNoeticKits::SecondBreathAttackCooldownMultiplier));
	Modifiers.Add(AttackCooldownModifier);
}
