// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_AeterniiCorruption.h"
#include "AeterniiAttributeSet.h"
#include "CombatGameplayTags.h"

UGE_AeterniiCorruption::UGE_AeterniiCorruption()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo CorruptionModifier;
	CorruptionModifier.Attribute = UAeterniiAttributeSet::GetCorruptionAttribute();
	CorruptionModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = TAG_Data_AeterniiCorruption;
	CorruptionModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(CorruptionModifier);
}
