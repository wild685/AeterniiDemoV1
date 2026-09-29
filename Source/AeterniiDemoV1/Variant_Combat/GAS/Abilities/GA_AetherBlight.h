// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_NoeticArtBase.h"
#include "GA_AetherBlight.generated.h"

/**
 *  Aether Blight. Isometric kit effigy_vow only (ordo):
 *  cooldown 17s, range 5, decoy 95 HP, life 7.5s.
 *  Foes attend it when distance(effigy) * 0.38 < distance(player).
 *  Display name in the HTML is Effigy of the Vow / False Reality.
 *  The locked art name is Aether Blight. Not the prototype false_reality kit.
 */
UCLASS()
class UGA_AetherBlight : public UGA_NoeticArtBase
{
	GENERATED_BODY()

public:

	UGA_AetherBlight();

protected:

	virtual ENoeticCanonArt GetArt() const override { return ENoeticCanonArt::AetherBlight; }
	virtual bool ExecuteKit(const FNoeticKitSpec& Kit) override;
};
