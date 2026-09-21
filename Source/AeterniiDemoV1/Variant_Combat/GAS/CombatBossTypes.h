// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatBossTypes.generated.h"

/** Boss attack kit. Sweep=A, Slam=B, Closer=C. */
UENUM(BlueprintType)
enum class EBossAttack : uint8
{
	None UMETA(DisplayName = "None"),
	Sweep UMETA(DisplayName = "Sweep (A)"),
	Slam UMETA(DisplayName = "Slam (B)"),
	Closer UMETA(DisplayName = "Closer (C)")
};

/** Which capsule the Active-window notify should open. */
UENUM(BlueprintType)
enum class EBossHitbox : uint8
{
	Melee UMETA(DisplayName = "Hitbox_Melee"),
	AOE UMETA(DisplayName = "Hitbox_AOE")
};

/** Expected montage section names: Tell → Commit → Active → Recover → End */
namespace CombatMontageSections
{
	inline const FName Tell(TEXT("Tell"));
	inline const FName Commit(TEXT("Commit"));
	inline const FName Active(TEXT("Active"));
	inline const FName Recover(TEXT("Recover"));
	inline const FName End(TEXT("End"));
}
