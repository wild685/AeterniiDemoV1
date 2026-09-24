// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatLifeBar.h"
#include "CombatAttributeSet.h"

void UCombatLifeBar::SetLifePercentage_Implementation(float /*Percent*/)
{
	// World-space life bar Blueprints override this. UCombatHUD fills LifeMeter.
}

void UCombatLifeBar::SetBarColor_Implementation(FLinearColor /*Color*/)
{
	// World-space life bar Blueprints override this. UCombatHUD fills LifeMeter.
}

void UCombatLifeBar::SetLifeFromAttributeSet(UCombatAttributeSet* AttributeSet)
{
	if (!AttributeSet)
	{
		return;
	}

	const float MaxHealth = AttributeSet->GetMaxHealth();
	SetLifePercentage(MaxHealth > 0.0f ? AttributeSet->GetHealth() / MaxHealth : 0.0f);
}
