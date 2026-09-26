// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayMontageAbility.h"
#include "GA_Dodge.generated.h"

/**
 *  Player dodge.
 *
 *  Expected montage length: ~0.55s.
 *  Place ANS_DodgeIFrames from ~0.05s to ~0.35s (≈0.30s of Status.Dodge.IFrames).
 *  Stamina exists on the legacy boss UCombatAttributeSet but is not spent in P1.
 *  Player Depth/Corruption attributes are UAeterniiAttributeSet, not this cost.
 *
 *  Ability.Dodge is owned while this GA is active. I-frames are notify-driven,
 *  not the full montage duration.
 */
UCLASS()
class UGA_Dodge : public UGA_PlayMontageAbility
{
	GENERATED_BODY()

public:

	UGA_Dodge();
};
