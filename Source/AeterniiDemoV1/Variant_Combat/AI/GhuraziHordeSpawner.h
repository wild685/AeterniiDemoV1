// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatEnemySpawner.h"
#include "GhuraziHordeEnemy.h"
#include "GhuraziHordeSpawner.generated.h"

/**
 *  Weighted class entry for a Ghurazi pack spawn.
 */
USTRUCT(BlueprintType)
struct FGhuraziHordeSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Enemy Spawner")
	TSubclassOf<AGhuraziHordeEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, Category="Enemy Spawner", meta = (ClampMin = 0, ClampMax = 100))
	float Weight = 1.0f;
};

/**
 *  CombatEnemySpawner that drops a staggered pack instead of a single pawn.
 *  Reuses ICombatActivatable and OnEnemyDied; does not replace the base spawner.
 */
UCLASS(abstract)
class AGhuraziHordeSpawner : public ACombatEnemySpawner
{
	GENERATED_BODY()

protected:

	/** If non-empty, a weighted roll picks the class. Otherwise EnemyClass is used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner|Horde")
	TArray<FGhuraziHordeSpawnEntry> HordeClasses;

	/** How many Ghurazi to drop together (iso opening pack is 3 raiders). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner|Horde", meta = (ClampMin = 1, ClampMax = 12))
	int32 PackSize = 3;

	/** Radial offset around the spawn capsule so the pack does not stack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner|Horde", meta = (ClampMin = 0, ClampMax = 500, Units = "cm"))
	float PackSpreadRadius = 120.0f;

	int32 AliveInCurrentPack = 0;

	virtual void SpawnEnemy() override;

	UFUNCTION()
	virtual void OnEnemyDied() override;

	TSubclassOf<ACombatEnemy> PickHordeClass() const;
};
