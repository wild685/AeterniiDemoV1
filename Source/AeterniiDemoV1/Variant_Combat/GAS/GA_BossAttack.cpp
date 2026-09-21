// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_BossAttack.h"
#include "CombatGameplayTags.h"

UGA_BossAttackBase::UGA_BossAttackBase()
{
	AbilityTags.AddTag(TAG_Ability_Attack);
	ActivationOwnedTags.AddTag(TAG_Ability_Attack);
	ActivationBlockedTags.AddTag(TAG_Ability_Attack);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Attack);
}

UGA_Boss_Sweep::UGA_Boss_Sweep()
{
	AttackType = EBossAttack::Sweep;
}

UGA_Boss_Slam::UGA_Boss_Slam()
{
	AttackType = EBossAttack::Slam;
}

UGA_Boss_Closer::UGA_Boss_Closer()
{
	AttackType = EBossAttack::Closer;
}
