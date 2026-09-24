// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatAbilityCooldownSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UCombatAbilityCooldownSlot::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UCombatAbilityCooldownSlot::SetCooldownTag(FGameplayTag InCooldownTag)
{
	CooldownTag = InCooldownTag;
}

void UCombatAbilityCooldownSlot::SetPlaceholderCooldownTagName(FName InTagName)
{
	PlaceholderCooldownTagName = InTagName;
}

void UCombatAbilityCooldownSlot::SetIcon(UTexture2D* Texture)
{
	if (Texture)
	{
		IconTexture = Texture;
		ApplyIconTexture();
		return;
	}

	IconTexture.Reset();

	if (AbilityIcon)
	{
		AbilityIcon->SetBrushFromTexture(nullptr);
	}
}

void UCombatAbilityCooldownSlot::SetCooldown_Implementation(float TimeRemaining, float Duration)
{
	if (Duration <= 0.0f)
	{
		CooldownPercent = 0.0f;
	}
	else
	{
		CooldownPercent = FMath::Clamp(TimeRemaining / Duration, 0.0f, 1.0f);
	}

	if (CooldownFill)
	{
		CooldownFill->SetPercent(CooldownPercent);
	}

	if (CooldownText)
	{
		if (CooldownPercent <= KINDA_SMALL_NUMBER)
		{
			CooldownText->SetText(FText::GetEmpty());
		}
		else
		{
			CooldownText->SetText(FText::AsNumber(FMath::CeilToInt(TimeRemaining)));
		}
	}
}

void UCombatAbilityCooldownSlot::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyIconTexture();
}

void UCombatAbilityCooldownSlot::ApplyIconTexture()
{
	if (!AbilityIcon)
	{
		return;
	}

	if (IconTexture.IsNull())
	{
		return;
	}

	if (UTexture2D* Texture = IconTexture.LoadSynchronous())
	{
		AbilityIcon->SetBrushFromTexture(Texture);
	}
}
