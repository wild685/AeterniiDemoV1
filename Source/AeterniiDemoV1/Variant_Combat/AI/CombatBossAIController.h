// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatAIController.h"
#include "CombatBossAIController.generated.h"

/**
 *  AIC_Boss parent. Same StateTree AI component as ACombatAIController.
 *  Assign ST_BossCombat on the Blueprint (BP_AIC_Boss / AIC_Boss).
 */
UCLASS(abstract)
class ACombatBossAIController : public ACombatAIController
{
	GENERATED_BODY()

public:

	ACombatBossAIController();
};
