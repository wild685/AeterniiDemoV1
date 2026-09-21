// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayMontageAbility.h"
#include "CombatBossTypes.h"
#include "GA_BossAttack.generated.h"

/**
 *  Base boss attack GA. Adds Ability.Attack while active.
 *
 *  Expected montage sections: Tell → Commit → Active → Recover → End
 *  Notifies: AN_TellStart @ Tell, ANS_DamageWindow spanning Active,
 *  AN_RecoverStart @ Recover, AN_AttackEnd @ End (montage complete also clears).
 */
UCLASS(Abstract)
class UGA_BossAttackBase : public UGA_PlayMontageAbility
{
	GENERATED_BODY()

public:

	UGA_BossAttackBase();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss Attack")
	EBossAttack AttackType = EBossAttack::None;
};

/**
 *  Sweep (A)
 *  Tell 0.70, Commit 0.15, Active 0.25, Recover 0.90 (~2.0s total)
 *  Use Hitbox_Melee during Active.
 */
UCLASS()
class UGA_Boss_Sweep : public UGA_BossAttackBase
{
	GENERATED_BODY()

public:

	UGA_Boss_Sweep();
};

/**
 *  Slam (B)
 *  Tell 0.90, Commit 0.20, Active 0.20, Recover 1.20 (~2.5s total)
 *  Use Hitbox_AOE during Active (optional slam impact capsule).
 */
UCLASS()
class UGA_Boss_Slam : public UGA_BossAttackBase
{
	GENERATED_BODY()

public:

	UGA_Boss_Slam();
};

/**
 *  Closer (C)
 *  Tell 0.35, Commit 0.10, Active 0.30, Recover 0.55 (~1.3s total)
 *  Use Hitbox_Melee during Active.
 */
UCLASS()
class UGA_Boss_Closer : public UGA_BossAttackBase
{
	GENERATED_BODY()

public:

	UGA_Boss_Closer();
};
