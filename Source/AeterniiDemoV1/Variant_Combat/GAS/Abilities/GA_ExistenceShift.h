// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_NoeticArtBase.h"
#include "GA_ExistenceShift.generated.h"

/**
 *  Existence Shift. Isometric kits, chosen from the applied player class:
 *  vigil_step (ordo, 7s, range 3.4, blink + 40% harm refused for 1.5s),
 *  censer_lunge (ignivarum, 6s, range 4.6, blink, 26 harm within 0.85 of the seam),
 *  fold (luminarch, 5s, range 5.6, blink).
 *  Cast at the cursor. No corruption spend.
 */
UCLASS()
class UGA_ExistenceShift : public UGA_NoeticArtBase
{
	GENERATED_BODY()

public:

	UGA_ExistenceShift();

protected:

	virtual ENoeticCanonArt GetArt() const override { return ENoeticCanonArt::ExistenceShift; }
	virtual bool ExecuteKit(const FNoeticKitSpec& Kit) override;
};
