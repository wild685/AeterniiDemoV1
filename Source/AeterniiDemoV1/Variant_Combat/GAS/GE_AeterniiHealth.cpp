// Copyright Epic Games, Inc. All Rights Reserved.

#include "GE_AeterniiHealth.h"
#include "AeterniiAttributeSet.h"
#include "CombatGameplayTags.h"

UGE_AeterniiHealth::UGE_AeterniiHealth()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = UAeterniiAttributeSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = TAG_Data_AeterniiHealth;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(HealthModifier);
}
