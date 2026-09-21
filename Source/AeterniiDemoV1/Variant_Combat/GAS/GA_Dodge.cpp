// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_Dodge.h"
#include "CombatGameplayTags.h"

UGA_Dodge::UGA_Dodge()
{
	AbilityTags.AddTag(TAG_Ability_Dodge);
	ActivationOwnedTags.AddTag(TAG_Ability_Dodge);
	ActivationBlockedTags.AddTag(TAG_Ability_Dodge);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Dodge);
}
