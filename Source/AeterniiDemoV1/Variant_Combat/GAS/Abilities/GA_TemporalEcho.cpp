// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_TemporalEcho.h"
#include "AeterniiAttributeSet.h"
#include "CombatCharacter.h"
#include "CombatGameplayTags.h"
#include "GE_AeterniiCorruption.h"
#include "GE_AeterniiHealth.h"
#include "GE_NoeticSecondBreath.h"

UGA_TemporalEcho::UGA_TemporalEcho()
{
	AbilityTags.AddTag(TAG_Ability_Noetic_TemporalEcho);
	ActivationOwnedTags.AddTag(TAG_Ability_Noetic_TemporalEcho);
	ActivationBlockedTags.AddTag(TAG_Ability_Noetic_TemporalEcho);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Noetic_TemporalEcho);

	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_SecondBreath);
	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_Palindrome);
}

bool UGA_TemporalEcho::ExecuteKit(const FNoeticKitSpec& Kit)
{
	ACombatCharacter* Character = GetCombatAvatar();
	UAeterniiAttributeSet* Attributes = Character ? Character->GetAeterniiAttributeSet() : nullptr;
	if (!Character || !Attributes)
	{
		return false;
	}

	if (Kit.Id == FName(TEXT("second_breath")))
	{
		FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(UGE_NoeticSecondBreath::StaticClass(), 1.0f);
		if (Spec.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec);
		}

		HoldLooseTag(TAG_Status_Noetic_SecondBreath, AeterniiNoeticKits::SecondBreathDuration);
		HoldLooseTag(TAG_Status_Noetic_Purge, AeterniiNoeticKits::SecondBreathDuration);
		Character->ApplySecondBreathMoveMultiplier(AeterniiNoeticKits::SecondBreathSpeedMultiplier, AeterniiNoeticKits::SecondBreathDuration);
		return true;
	}

	if (Kit.Id == FName(TEXT("palindrome")))
	{
		FNoeticPalindromeSample Sample;
		if (!Character->FindPalindromeSample(Sample))
		{
			// castAbility still starts the cooldown when history is empty.
			return true;
		}

		const FVector Destination = ClearSpot(Sample.Location);
		Character->SetActorLocation(Destination, false, nullptr, ETeleportType::TeleportPhysics);

		const float RestoredHealth = FMath::Clamp(
			FMath::Max(Attributes->GetHealth(), Sample.IsometricHealth),
			0.0f,
			Attributes->GetMaxHealth());
		AddAeterniiAttribute(
			UGE_AeterniiHealth::StaticClass(),
			TAG_Data_AeterniiHealth,
			RestoredHealth - Attributes->GetHealth());

		const float RestoredCorruption = UAeterniiAttributeSet::CorruptionAfterPalindrome(
			Attributes->GetCorruption(),
			Sample.Corruption,
			AeterniiNoeticKits::PalindromeCorruptionDrop);
		AddAeterniiAttribute(
			UGE_AeterniiCorruption::StaticClass(),
			TAG_Data_AeterniiCorruption,
			RestoredCorruption - Attributes->GetCorruption());
		return true;
	}

	return false;
}
