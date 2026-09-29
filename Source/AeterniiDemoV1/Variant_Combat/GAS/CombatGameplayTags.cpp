// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatGameplayTags.h"

DEFINE_LOG_CATEGORY(LogCombatGAS);

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Attack, "Ability.Attack", "Owned while an attack Gameplay Ability is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Dodge, "Ability.Dodge", "Owned while the dodge Gameplay Ability is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Status_Dodge_IFrames, "Status.Dodge.IFrames", "Invulnerability window during dodge");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Data_Damage, "Data.Damage", "SetByCaller magnitude key for GE_Damage_Melee");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Noetic_ExistenceShift, "Ability.Noetic.ExistenceShift", "Existence Shift is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Noetic_MemoryBurn, "Ability.Noetic.MemoryBurn", "Memory Burn is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Noetic_TemporalEcho, "Ability.Noetic.TemporalEcho", "Temporal Echo is active");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Ability_Noetic_AetherBlight, "Ability.Noetic.AetherBlight", "Aether Blight is active");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_VigilStep, "Cooldown.Noetic.VigilStep", "Vigil Step cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_CenserLunge, "Cooldown.Noetic.CenserLunge", "Censer Lunge cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_Fold, "Cooldown.Noetic.Fold", "Fold cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_AshenRecitation, "Cooldown.Noetic.AshenRecitation", "Ashen Recitation cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_CensureLance, "Cooldown.Noetic.CensureLance", "Censure Lance cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_Unwriting, "Cooldown.Noetic.Unwriting", "Unwriting cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_SecondBreath, "Cooldown.Noetic.SecondBreath", "Second Breath cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_Palindrome, "Cooldown.Noetic.Palindrome", "Palindrome cooldown");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Cooldown_Noetic_EffigyVow, "Cooldown.Noetic.EffigyVow", "Effigy of the Vow cooldown");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Status_Noetic_VigilWard, "Status.Noetic.VigilWard", "Vigil Step 40% harm refusal");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Status_Noetic_SecondBreath, "Status.Noetic.SecondBreath", "Second Breath haste");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Status_Noetic_Purge, "Status.Noetic.Purge", "Second Breath purge window");

UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Data_AeterniiCorruption, "Data.Aeternii.Corruption", "SetByCaller magnitude for UAeterniiAttributeSet Corruption");
UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Data_AeterniiHealth, "Data.Aeternii.Health", "SetByCaller magnitude for UAeterniiAttributeSet Health");
