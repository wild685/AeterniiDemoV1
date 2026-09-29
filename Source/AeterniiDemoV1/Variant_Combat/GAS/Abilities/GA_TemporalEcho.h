// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_NoeticArtBase.h"
#include "GA_TemporalEcho.generated.h"

/**
 *  Temporal Echo. Isometric kits (ordo has none):
 *  second_breath (ignivarum, 18s) — 5s haste (speed 1.45, attack cooldown 0.55) and purge,
 *  palindrome (luminarch, 16s) — return to the sample at least 3s old, restore isometric
 *  health upward, corruption = max(0, min(current, recorded) - 14).
 */
UCLASS()
class UGA_TemporalEcho : public UGA_NoeticArtBase
{
	GENERATED_BODY()

public:

	UGA_TemporalEcho();

protected:

	virtual ENoeticCanonArt GetArt() const override { return ENoeticCanonArt::TemporalEcho; }
	virtual bool ExecuteKit(const FNoeticKitSpec& Kit) override;
};
