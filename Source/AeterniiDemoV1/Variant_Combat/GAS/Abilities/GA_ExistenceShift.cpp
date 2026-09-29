// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_ExistenceShift.h"
#include "CombatGameplayTags.h"

UGA_ExistenceShift::UGA_ExistenceShift()
{
	AbilityTags.AddTag(TAG_Ability_Noetic_ExistenceShift);
	ActivationOwnedTags.AddTag(TAG_Ability_Noetic_ExistenceShift);
	ActivationBlockedTags.AddTag(TAG_Ability_Noetic_ExistenceShift);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Noetic_ExistenceShift);

	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_VigilStep);
	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_CenserLunge);
	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_Fold);
}

bool UGA_ExistenceShift::ExecuteKit(const FNoeticKitSpec& Kit)
{
	FVector AimPoint = FVector::ZeroVector;
	if (!ResolveAimPoint(Kit.Range, AimPoint))
	{
		return false;
	}

	const FVector Start = GetCombatAvatar() ? GetCombatAvatar()->GetActorLocation() : AimPoint;
	const FVector End = BlinkTo(AimPoint, Kit.Range);

	if (Kit.Id == FName(TEXT("vigil_step")))
	{
		HoldLooseTag(TAG_Status_Noetic_VigilWard, AeterniiNoeticKits::VigilWardDuration);
	}
	else if (Kit.Id == FName(TEXT("censer_lunge")))
	{
		DamageEnemiesNearSegment(Start, End, AeterniiNoeticKits::CenserLungeSeamRadius, AeterniiNoeticKits::CenserLungeDamage);
	}

	return true;
}
