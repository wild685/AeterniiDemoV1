// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatGameplayTags.h"

DEFINE_LOG_CATEGORY(LogCombatGAS);

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Attack, "Ability.Attack", "Owned while an attack Gameplay Ability is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Dodge, "Ability.Dodge", "Owned while the dodge Gameplay Ability is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Status_Dodge_IFrames, "Status.Dodge.IFrames", "Invulnerability window during dodge");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Data_Damage, "Data.Damage", "SetByCaller magnitude key for GE_Damage_Melee");
