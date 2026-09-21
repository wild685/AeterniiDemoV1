// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatHUD.h"
#include "CombatAbilityCooldownSlot.h"
#include "Components/ProgressBar.h"

UCombatHUD::UCombatHUD()
{
	AbilityCooldownTags.SetNum(AbilitySlotCount);
}

void UCombatHUD::SetLifePercentage_Implementation(float Percent)
{
	Super::SetLifePercentage_Implementation(Percent);

	if (LifeMeter)
	{
		LifeMeter->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
	}
}

void UCombatHUD::SetBarColor_Implementation(FLinearColor Color)
{
	Super::SetBarColor_Implementation(Color);

	if (LifeMeter)
	{
		LifeMeter->SetFillColorAndOpacity(Color);
	}
}

void UCombatHUD::SetCorruptionPercentage_Implementation(float Percent)
{
	if (CorruptionMeter)
	{
		CorruptionMeter->SetPercent(FMath::Clamp(Percent, 0.0f, 1.0f));
	}
}

void UCombatHUD::SetCorruptionBarColor_Implementation(FLinearColor Color)
{
	if (CorruptionMeter)
	{
		CorruptionMeter->SetFillColorAndOpacity(Color);
	}
}

void UCombatHUD::SetCorruptionFromCurrentMax(float Current, float MaxValue)
{
	const float Percent = (MaxValue > 0.0f) ? (Current / MaxValue) : 0.0f;
	SetCorruptionPercentage(Percent);
}

void UCombatHUD::SetAbilityCooldown(int32 SlotIndex, float TimeRemaining, float Duration)
{
	if (UCombatAbilityCooldownSlot* Slot = GetAbilitySlot(SlotIndex))
	{
		Slot->SetCooldown(TimeRemaining, Duration);
	}
}

void UCombatHUD::SetAbilityCooldownTag(int32 SlotIndex, FGameplayTag CooldownTag)
{
	if (!AbilityCooldownTags.IsValidIndex(SlotIndex))
	{
		return;
	}

	AbilityCooldownTags[SlotIndex] = CooldownTag;

	if (UCombatAbilityCooldownSlot* Slot = GetAbilitySlot(SlotIndex))
	{
		Slot->SetCooldownTag(CooldownTag);
	}
}

UCombatAbilityCooldownSlot* UCombatHUD::GetAbilitySlot(int32 SlotIndex) const
{
	switch (SlotIndex)
	{
	case 0: return AbilitySlot_0;
	case 1: return AbilitySlot_1;
	case 2: return AbilitySlot_2;
	case 3: return AbilitySlot_3;
	default: return nullptr;
	}
}

FGameplayTag UCombatHUD::GetAbilityCooldownTag(int32 SlotIndex) const
{
	if (AbilityCooldownTags.IsValidIndex(SlotIndex))
	{
		return AbilityCooldownTags[SlotIndex];
	}

	return FGameplayTag();
}

FName UCombatHUD::GetAbilityCooldownPlaceholderName(int32 SlotIndex) const
{
	if (const UCombatAbilityCooldownSlot* Slot = GetAbilitySlot(SlotIndex))
	{
		if (!Slot->PlaceholderCooldownTagName.IsNone())
		{
			return Slot->PlaceholderCooldownTagName;
		}
	}

	return CombatHUDCooldownTags::SlotName(SlotIndex);
}

void UCombatHUD::NativePreConstruct()
{
	Super::NativePreConstruct();
	InitializeAbilitySlots();
}

void UCombatHUD::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeAbilitySlots();
}

void UCombatHUD::InitializeAbilitySlots()
{
	if (AbilityCooldownTags.Num() < AbilitySlotCount)
	{
		AbilityCooldownTags.SetNum(AbilitySlotCount);
	}

	for (int32 SlotIndex = 0; SlotIndex < AbilitySlotCount; ++SlotIndex)
	{
		if (UCombatAbilityCooldownSlot* Slot = GetAbilitySlot(SlotIndex))
		{
			Slot->SetSlotIndex(SlotIndex);

			if (Slot->PlaceholderCooldownTagName.IsNone())
			{
				Slot->SetPlaceholderCooldownTagName(CombatHUDCooldownTags::SlotName(SlotIndex));
			}

			if (AbilityCooldownTags.IsValidIndex(SlotIndex) && AbilityCooldownTags[SlotIndex].IsValid())
			{
				Slot->SetCooldownTag(AbilityCooldownTags[SlotIndex]);
			}
		}
	}
}
