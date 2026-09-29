// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_AeterniiHealth.generated.h"

/**
 *  Instant additive Health on UAeterniiAttributeSet (isometric hp).
 *  Magnitude is SetByCaller Data.Aeternii.Health.
 *  Pack CurrentHP stays on ICombatDamageable.
 */
UCLASS()
class UGE_AeterniiHealth : public UGameplayEffect
{
	GENERATED_BODY()

public:

	UGE_AeterniiHealth();
};
