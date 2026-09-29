// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayAbility;

/**
 *  Isometric-demo Noetic Art kits from Aeternii_Isometric_Demo.html (ABIL + castAbility).
 *  One canon art, class-specific kit. Prototype HTML numbers are not in this header.
 *
 *  Ranges, radii, and steps are demo world units. Multiply by
 *  ACombatCharacter::DemoUnitsToCentimeters at the trace boundary.
 *  The HTML files do not define centimeters per unit; the character default is 1.
 */
enum class ENoeticCanonArt : uint8
{
	ExistenceShift,
	MemoryBurn,
	TemporalEcho,
	AetherBlight
};

struct FNoeticKitSpec
{
	FName Id;
	FName ClassId;
	ENoeticCanonArt Art = ENoeticCanonArt::ExistenceShift;
	float CooldownSeconds = 0.0f;
	/** ABIL.rng. Zero when the cast is not aimed (ashen recitation, temporal echo). */
	float Range = 0.0f;
};

struct FNoeticPalindromeSample
{
	float TimeSeconds = 0.0f;
	FVector Location = FVector::ZeroVector;
	float IsometricHealth = 0.0f;
	float Corruption = 0.0f;
};

namespace AeterniiNoeticKits
{
	/** Three keyed slots in the isometric HUD (1/Q, 2/E, 3/R). The fourth player-class slot stays empty. */
	inline constexpr int32 SlotsPerClass = 3;

	inline constexpr float BlinkStep = 0.2f;
	inline constexpr float CenserLungeDamage = 26.0f;
	inline constexpr float CenserLungeSeamRadius = 0.85f;
	inline constexpr float VigilWardDuration = 1.5f;
	/** hurtPlayer: a *= (1 - amt) with amt 0.40. */
	inline constexpr float VigilWardHarmRefused = 0.40f;
	inline constexpr float VigilStepCooldown = 7.0f;
	inline constexpr float VigilStepRange = 3.4f;
	inline constexpr float CenserLungeCooldown = 6.0f;
	inline constexpr float CenserLungeRange = 4.6f;
	inline constexpr float FoldCooldown = 5.0f;
	inline constexpr float FoldRange = 5.6f;

	inline constexpr float AshenRecitationCooldown = 6.5f;
	inline constexpr float AshenRecitationRadius = 2.9f;
	inline constexpr float AshenRecitationDamage = 38.0f;
	inline constexpr float AshenSlowDuration = 2.2f;
	/** Foe speed is multiplied by (1 - this). */
	inline constexpr float AshenSlowAmount = 0.45f;
	inline constexpr float CensureLanceCooldown = 5.0f;
	inline constexpr float CensureLanceRange = 7.2f;
	inline constexpr float CensureLanceWidth = 0.72f;
	inline constexpr float CensureLanceDamage = 46.0f;
	inline constexpr float LanceStep = 0.25f;
	/** fits(..., 0.1) while walking the lance. */
	inline constexpr float LanceProbeRadius = 0.1f;
	inline constexpr float UnwritingCooldown = 4.2f;
	inline constexpr float UnwritingRange = 7.0f;
	inline constexpr float UnwritingDamageRadius = 2.0f;
	inline constexpr float UnwritingDamage = 44.0f;
	/** fx t >= 0.36 applies the unwriting harm. */
	inline constexpr float UnwritingDelay = 0.36f;

	inline constexpr float SecondBreathCooldown = 18.0f;
	inline constexpr float SecondBreathDuration = 5.0f;
	inline constexpr float SecondBreathSpeedMultiplier = 1.45f;
	inline constexpr float SecondBreathAttackCooldownMultiplier = 0.55f;
	inline constexpr float PalindromeCooldown = 16.0f;
	inline constexpr float PalindromeSampleInterval = 0.12f;
	inline constexpr int32 PalindromeHistoryCap = 60;
	inline constexpr float PalindromeRewindSeconds = 3.0f;
	/** Direct corruption write: min(current, recorded) - this. Not UnmakeCorruptionRefund. */
	inline constexpr float PalindromeCorruptionDrop = 14.0f;

	inline constexpr float EffigyVowCooldown = 17.0f;
	inline constexpr float EffigyVowRange = 5.0f;
	inline constexpr float EffigyHitPoints = 95.0f;
	inline constexpr float EffigyLifeSeconds = 7.5f;
	/** Foe attends the effigy when distance(effigy) * this < distance(player). */
	inline constexpr float EffigyAggroDistanceScale = 0.38f;
	/** Shared entity radius (PR) used as the effigy footprint. */
	inline constexpr float EntityRadius = 0.30f;
	inline constexpr float ClearSpotRingStep = 0.5f;
	inline constexpr float ClearSpotMaxRadius = 4.0f;
	inline constexpr int32 ClearSpotSpokes = 12;

	const FNoeticKitSpec* FindKit(FName ClassId, ENoeticCanonArt Art);
	const FNoeticKitSpec* FindKitBySlot(FName ClassId, int32 SlotIndex);
	FGameplayTag CooldownTagForKit(FName KitId);
	UClass* AbilityClassForArt(ENoeticCanonArt Art);
}
