// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_AeterniiCorruption.generated.h"

/**
 *  Instant additive Corruption on UAeterniiAttributeSet.
 *  Magnitude is SetByCaller Data.Aeternii.Corruption (negative subtracts).
 *  Does not touch pack CurrentHP or UCombatAttributeSet.
 */
UCLASS()
class UGE_AeterniiCorruption : public UGameplayEffect
{
	GENERATED_BODY()

public:

	UGE_AeterniiCorruption();
};
