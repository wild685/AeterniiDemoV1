// Copyright Epic Games, Inc. All Rights Reserved.


#include "GhuraziHordeSpawner.h"
#include "CombatEnemy.h"
#include "Engine/World.h"
#include "TimerManager.h"

TSubclassOf<ACombatEnemy> AGhuraziHordeSpawner::PickHordeClass() const
{
	float TotalWeight = 0.0f;
	for (const FGhuraziHordeSpawnEntry& Entry : HordeClasses)
	{
		if (Entry.EnemyClass && Entry.Weight > 0.0f)
		{
			TotalWeight += Entry.Weight;
		}
	}

	if (TotalWeight <= KINDA_SMALL_NUMBER)
	{
		return EnemyClass;
	}

	float Roll = FMath::FRandRange(0.0f, TotalWeight);
	for (const FGhuraziHordeSpawnEntry& Entry : HordeClasses)
	{
		if (!Entry.EnemyClass || Entry.Weight <= 0.0f)
		{
			continue;
		}

		Roll -= Entry.Weight;
		if (Roll <= 0.0f)
		{
			return Entry.EnemyClass;
		}
	}

	return EnemyClass;
}

void AGhuraziHordeSpawner::SpawnEnemy()
{
	const int32 Remaining = FMath::Max(0, SpawnCount - AliveInCurrentPack);
	const int32 ToSpawn = FMath::Min(PackSize, Remaining);
	if (ToSpawn <= 0)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FTransform CapsuleTransform = GetSpawnCapsuleTransform();

	for (int32 Index = 0; Index < ToSpawn; ++Index)
	{
		TSubclassOf<ACombatEnemy> ClassToSpawn = PickHordeClass();
		if (!IsValid(ClassToSpawn))
		{
			continue;
		}

		FTransform SpawnTransform = CapsuleTransform;
		if (PackSpreadRadius > KINDA_SMALL_NUMBER && ToSpawn > 1)
		{
			const float Angle = (2.0f * PI * static_cast<float>(Index)) / static_cast<float>(ToSpawn);
			const FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * PackSpreadRadius;
			SpawnTransform.AddToTranslation(Offset);
		}

		ACombatEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ACombatEnemy>(ClassToSpawn, SpawnTransform, SpawnParams);
		if (SpawnedEnemy)
		{
			SpawnedEnemy->OnEnemyDied.AddDynamic(this, &AGhuraziHordeSpawner::OnEnemyDied);
			++AliveInCurrentPack;
		}
	}
}

void AGhuraziHordeSpawner::OnEnemyDied()
{
	--SpawnCount;
	AliveInCurrentPack = FMath::Max(0, AliveInCurrentPack - 1);

	if (SpawnCount <= 0)
	{
		if (AliveInCurrentPack <= 0)
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AGhuraziHordeSpawner::SpawnerDepleted, ActivationDelay);
		}
		return;
	}

	if (AliveInCurrentPack <= 0)
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AGhuraziHordeSpawner::SpawnEnemy, RespawnDelay);
	}
}
