// Copyright Epic Games, Inc. All Rights Reserved.


#include "CombatLifeBar.h"

void UCombatLifeBar::SetLifePercentage_Implementation(float /*Percent*/)
{
	// World-space life bar Blueprints override this. UCombatHUD fills LifeMeter.
}

void UCombatLifeBar::SetBarColor_Implementation(FLinearColor /*Color*/)
{
	// World-space life bar Blueprints override this. UCombatHUD fills LifeMeter.
}
