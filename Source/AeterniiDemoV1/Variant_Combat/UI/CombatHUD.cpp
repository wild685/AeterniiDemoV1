// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatHUD.h"
#include "AbilitySystemComponent.h"
#include "AeterniiAttributeSet.h"
#include "CombatAbilityCooldownSlot.h"
#include "Components/ProgressBar.h"

namespace
{
	UAeterniiAttributeSet* FindAeterniiAttributeSet(UAbilitySystemComponent* InAbilitySystem)
	{
		if (!IsValid(InAbilitySystem))
		{
			return nullptr;
		}

		// GetSet is const-only. The set itself is not const.
		if (const UAeterniiAttributeSet* FromASC = InAbilitySystem->GetSet<UAeterniiAttributeSet>())
		{
			return const_cast<UAeterniiAttributeSet*>(FromASC);
		}

		// CreateDefaultSubobject sets are not always listed in SpawnedAttributes.
		// ACombatCharacter names its set AttributeSet. ACombatEnemy names it AeterniiAttributeSet.
		AActor* Actor = InAbilitySystem->GetOwner();
		if (!IsValid(Actor))
		{
			return nullptr;
		}

		static const FName SubobjectNames[] = {
			TEXT("AttributeSet"),
			TEXT("AeterniiAttributeSet")
		};

		for (const FName& SubobjectName : SubobjectNames)
		{
			if (UAeterniiAttributeSet* Set = Cast<UAeterniiAttributeSet>(Actor->GetDefaultSubobjectByName(SubobjectName)))
			{
				return Set;
			}
		}

		return nullptr;
	}
}

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

void UCombatHUD::SetCorruptionFromAttributeSet(UAeterniiAttributeSet* InAttributeSet)
{
	if (!IsValid(InAttributeSet))
	{
		UnbindAeterniiAttributes();
		Depth = 0;
		SetCorruptionPercentage(0.0f);
		return;
	}

	if (BoundAeterniiAttributes.Get() != InAttributeSet)
	{
		UnbindAeterniiAttributes();
		BoundAeterniiAttributes = InAttributeSet;
		InAttributeSet->OnCorruptionChanged.AddDynamic(this, &UCombatHUD::HandleAeterniiCorruptionChanged);
		InAttributeSet->OnDepthChanged.AddDynamic(this, &UCombatHUD::HandleAeterniiDepthChanged);
	}

	SetCorruptionFromCurrentMax(InAttributeSet->GetCorruption(), InAttributeSet->GetMaxCorruption());
	ApplyDepthIndex(InAttributeSet->GetDepthIndex());
}

void UCombatHUD::BindToAbilitySystem(UAbilitySystemComponent* InAbilitySystem)
{
	SetCorruptionFromAttributeSet(FindAeterniiAttributeSet(InAbilitySystem));
}

void UCombatHUD::HandleAeterniiCorruptionChanged(float NewCorruption, float NewMaxCorruption)
{
	SetCorruptionFromCurrentMax(NewCorruption, NewMaxCorruption);
}

void UCombatHUD::HandleAeterniiDepthChanged(float NewDepth)
{
	const int32 Index = FMath::Clamp(
		FMath::RoundToInt(NewDepth),
		0,
		AeterniiCorruptionMeter::StratumCount - 1);
	ApplyDepthIndex(Index);
}

void UCombatHUD::UnbindAeterniiAttributes()
{
	if (UAeterniiAttributeSet* Set = BoundAeterniiAttributes.Get())
	{
		Set->OnCorruptionChanged.RemoveDynamic(this, &UCombatHUD::HandleAeterniiCorruptionChanged);
		Set->OnDepthChanged.RemoveDynamic(this, &UCombatHUD::HandleAeterniiDepthChanged);
	}

	BoundAeterniiAttributes.Reset();
}

void UCombatHUD::ApplyDepthIndex(int32 Index)
{
	Depth = Index;
	OnDepthChanged(Index);
}

void UCombatHUD::SetAbilityCooldown(int32 SlotIndex, float TimeRemaining, float Duration)
{
	if (UCombatAbilityCooldownSlot* CooldownSlot = GetAbilitySlot(SlotIndex))
	{
		CooldownSlot->SetCooldown(TimeRemaining, Duration);
	}
}

void UCombatHUD::SetAbilityCooldownTag(int32 SlotIndex, FGameplayTag CooldownTag)
{
	if (!AbilityCooldownTags.IsValidIndex(SlotIndex))
	{
		return;
	}

	AbilityCooldownTags[SlotIndex] = CooldownTag;

	if (UCombatAbilityCooldownSlot* CooldownSlot = GetAbilitySlot(SlotIndex))
	{
		CooldownSlot->SetCooldownTag(CooldownTag);
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
	if (const UCombatAbilityCooldownSlot* CooldownSlot = GetAbilitySlot(SlotIndex))
	{
		if (!CooldownSlot->PlaceholderCooldownTagName.IsNone())
		{
			return CooldownSlot->PlaceholderCooldownTagName;
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

void UCombatHUD::NativeDestruct()
{
	UnbindAeterniiAttributes();
	Super::NativeDestruct();
}

void UCombatHUD::InitializeAbilitySlots()
{
	if (AbilityCooldownTags.Num() < AbilitySlotCount)
	{
		AbilityCooldownTags.SetNum(AbilitySlotCount);
	}

	for (int32 SlotIndex = 0; SlotIndex < AbilitySlotCount; ++SlotIndex)
	{
		if (UCombatAbilityCooldownSlot* CooldownSlot = GetAbilitySlot(SlotIndex))
		{
			CooldownSlot->SetSlotIndex(SlotIndex);

			if (CooldownSlot->PlaceholderCooldownTagName.IsNone())
			{
				CooldownSlot->SetPlaceholderCooldownTagName(CombatHUDCooldownTags::SlotName(SlotIndex));
			}

			if (AbilityCooldownTags.IsValidIndex(SlotIndex) && AbilityCooldownTags[SlotIndex].IsValid())
			{
				CooldownSlot->SetCooldownTag(AbilityCooldownTags[SlotIndex]);
			}
		}
	}
}
