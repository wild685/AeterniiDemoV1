// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_Damage_Melee.h"
#include "CombatAttributeSet.h"
#include "CombatGameplayTags.h"

UGE_Damage_Melee::UGE_Damage_Melee()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = UCombatAttributeSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = TAG_Data_Damage;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(HealthModifier);
}
