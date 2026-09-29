// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_MemoryBurn.h"
#include "CombatCharacter.h"
#include "CombatGameplayTags.h"
#include "TimerManager.h"

UGA_MemoryBurn::UGA_MemoryBurn()
{
	AbilityTags.AddTag(TAG_Ability_Noetic_MemoryBurn);
	ActivationOwnedTags.AddTag(TAG_Ability_Noetic_MemoryBurn);
	ActivationBlockedTags.AddTag(TAG_Ability_Noetic_MemoryBurn);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Noetic_MemoryBurn);

	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_AshenRecitation);
	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_CensureLance);
	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_Unwriting);
}

void UGA_MemoryBurn::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(UnwriteTimer);
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_MemoryBurn::ExecuteKit(const FNoeticKitSpec& Kit)
{
	ACombatCharacter* Character = GetCombatAvatar();
	if (!Character)
	{
		return false;
	}

	if (Kit.Id == FName(TEXT("ashen_recitation")))
	{
		const FVector Center = Character->GetActorLocation();
		DamageEnemiesInRadius(Center, AeterniiNoeticKits::AshenRecitationRadius, AeterniiNoeticKits::AshenRecitationDamage);
		SlowEnemiesInRadius(Center, AeterniiNoeticKits::AshenRecitationRadius, AeterniiNoeticKits::AshenSlowDuration, AeterniiNoeticKits::AshenSlowAmount);
		return true;
	}

	FVector AimPoint = FVector::ZeroVector;
	if (!ResolveAimPoint(Kit.Range, AimPoint))
	{
		return false;
	}

	const FVector Start = Character->GetActorLocation();
	const FVector Direction = (AimPoint - Start).GetSafeNormal2D();
	if (!Direction.IsNearlyZero())
	{
		Character->SetActorRotation(FRotator(0.0f, Direction.Rotation().Yaw, 0.0f));
	}

	if (Kit.Id == FName(TEXT("censure_lance")))
	{
		const float MaxLength = ToWorld(Kit.Range);
		const float Step = ToWorld(AeterniiNoeticKits::LanceStep);
		FVector End = Start;
		if (!Direction.IsNearlyZero() && Step > KINDA_SMALL_NUMBER)
		{
			float Walked = 0.0f;
			for (float Distance = Step; Distance <= MaxLength + KINDA_SMALL_NUMBER; Distance += Step)
			{
				const float Clamped = FMath::Min(Distance, MaxLength);
				const FVector Next = Start + Direction * Clamped;
				if (!ProbeClear(Start + Direction * Walked, Next, AeterniiNoeticKits::LanceProbeRadius))
				{
					break;
				}
				Walked = Clamped;
				End = Next;
			}
		}
		DamageEnemiesNearSegment(Start, End, AeterniiNoeticKits::CensureLanceWidth, AeterniiNoeticKits::CensureLanceDamage);
		return true;
	}

	if (Kit.Id == FName(TEXT("unwriting")))
	{
		const float Distance = Direction.IsNearlyZero()
			? 0.0f
			: FMath::Min(ToWorld(Kit.Range), FVector::Dist2D(Start, AimPoint));
		PendingUnwriteLocation = Start + Direction * Distance;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				UnwriteTimer,
				this,
				&UGA_MemoryBurn::ResolveUnwriting,
				AeterniiNoeticKits::UnwritingDelay,
				false);
		}
		return true;
	}

	return false;
}

void UGA_MemoryBurn::ResolveUnwriting()
{
	DamageEnemiesInRadius(PendingUnwriteLocation, AeterniiNoeticKits::UnwritingDamageRadius, AeterniiNoeticKits::UnwritingDamage);
}
