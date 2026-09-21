// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_Damage_Melee.generated.h"

/**
 *  Instant Health modifier. Magnitude comes from SetByCaller tag Data.Damage
 *  (pass a negative value, e.g. -1, to subtract Health).
 *
 *  Editor: you can also create a Blueprint GE parented to this class.
 */
UCLASS()
class UGE_Damage_Melee : public UGameplayEffect
{
	GENERATED_BODY()

public:

	UGE_Damage_Melee();
};
