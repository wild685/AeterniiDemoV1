// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_NoeticSecondBreath.generated.h"

/**
 *  Second Breath attribute window (5s).
 *  MoveSpeed × 1.45, AttackCooldown × 0.55, on UAeterniiAttributeSet.
 *  MaxWalkSpeed is applied separately on the character. Pack HP is untouched.
 */
UCLASS()
class UGE_NoeticSecondBreath : public UGameplayEffect
{
	GENERATED_BODY()

public:

	UGE_NoeticSecondBreath();
};
