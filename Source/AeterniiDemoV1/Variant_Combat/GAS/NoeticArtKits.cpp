// Copyright Epic Games, Inc. All Rights Reserved.

#include "NoeticArtKits.h"
#include "CombatGameplayTags.h"
#include "PlayerClassData.h"
#include "GA_ExistenceShift.h"
#include "GA_MemoryBurn.h"
#include "GA_TemporalEcho.h"
#include "GA_AetherBlight.h"

namespace AeterniiNoeticKits
{
	static const TArray<FNoeticKitSpec>& AllKits()
	{
		static const TArray<FNoeticKitSpec> Kits = {
			{ FName(TEXT("vigil_step")), PlayerClassIds::Ordo(), ENoeticCanonArt::ExistenceShift, VigilStepCooldown, VigilStepRange },
			{ FName(TEXT("ashen_recitation")), PlayerClassIds::Ordo(), ENoeticCanonArt::MemoryBurn, AshenRecitationCooldown, 0.0f },
			{ FName(TEXT("effigy_vow")), PlayerClassIds::Ordo(), ENoeticCanonArt::AetherBlight, EffigyVowCooldown, EffigyVowRange },

			{ FName(TEXT("censer_lunge")), PlayerClassIds::Ignivarum(), ENoeticCanonArt::ExistenceShift, CenserLungeCooldown, CenserLungeRange },
			{ FName(TEXT("censure_lance")), PlayerClassIds::Ignivarum(), ENoeticCanonArt::MemoryBurn, CensureLanceCooldown, CensureLanceRange },
			{ FName(TEXT("second_breath")), PlayerClassIds::Ignivarum(), ENoeticCanonArt::TemporalEcho, SecondBreathCooldown, 0.0f },

			{ FName(TEXT("fold")), PlayerClassIds::Luminarch(), ENoeticCanonArt::ExistenceShift, FoldCooldown, FoldRange },
			{ FName(TEXT("unwriting")), PlayerClassIds::Luminarch(), ENoeticCanonArt::MemoryBurn, UnwritingCooldown, UnwritingRange },
			{ FName(TEXT("palindrome")), PlayerClassIds::Luminarch(), ENoeticCanonArt::TemporalEcho, PalindromeCooldown, 0.0f },
		};
		return Kits;
	}

	const FNoeticKitSpec* FindKit(FName ClassId, ENoeticCanonArt Art)
	{
		for (const FNoeticKitSpec& Kit : AllKits())
		{
			if (Kit.ClassId == ClassId && Kit.Art == Art)
			{
				return &Kit;
			}
		}
		return nullptr;
	}

	const FNoeticKitSpec* FindKitBySlot(FName ClassId, int32 SlotIndex)
	{
		if (SlotIndex < 0 || SlotIndex >= SlotsPerClass)
		{
			return nullptr;
		}

		int32 Seen = 0;
		for (const FNoeticKitSpec& Kit : AllKits())
		{
			if (Kit.ClassId != ClassId)
			{
				continue;
			}
			if (Seen == SlotIndex)
			{
				return &Kit;
			}
			++Seen;
		}
		return nullptr;
	}

	FGameplayTag CooldownTagForKit(FName KitId)
	{
		if (KitId == FName(TEXT("vigil_step")))
		{
			return TAG_Cooldown_Noetic_VigilStep;
		}
		if (KitId == FName(TEXT("censer_lunge")))
		{
			return TAG_Cooldown_Noetic_CenserLunge;
		}
		if (KitId == FName(TEXT("fold")))
		{
			return TAG_Cooldown_Noetic_Fold;
		}
		if (KitId == FName(TEXT("ashen_recitation")))
		{
			return TAG_Cooldown_Noetic_AshenRecitation;
		}
		if (KitId == FName(TEXT("censure_lance")))
		{
			return TAG_Cooldown_Noetic_CensureLance;
		}
		if (KitId == FName(TEXT("unwriting")))
		{
			return TAG_Cooldown_Noetic_Unwriting;
		}
		if (KitId == FName(TEXT("second_breath")))
		{
			return TAG_Cooldown_Noetic_SecondBreath;
		}
		if (KitId == FName(TEXT("palindrome")))
		{
			return TAG_Cooldown_Noetic_Palindrome;
		}
		if (KitId == FName(TEXT("effigy_vow")))
		{
			return TAG_Cooldown_Noetic_EffigyVow;
		}
		return FGameplayTag();
	}

	UClass* AbilityClassForArt(ENoeticCanonArt Art)
	{
		switch (Art)
		{
		case ENoeticCanonArt::ExistenceShift:
			return UGA_ExistenceShift::StaticClass();
		case ENoeticCanonArt::MemoryBurn:
			return UGA_MemoryBurn::StaticClass();
		case ENoeticCanonArt::TemporalEcho:
			return UGA_TemporalEcho::StaticClass();
		case ENoeticCanonArt::AetherBlight:
			return UGA_AetherBlight::StaticClass();
		}
		return nullptr;
	}
}
