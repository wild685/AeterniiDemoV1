// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_AetherBlight.h"
#include "CombatCharacter.h"
#include "CombatGameplayTags.h"
#include "NoeticEffigy.h"

UGA_AetherBlight::UGA_AetherBlight()
{
	AbilityTags.AddTag(TAG_Ability_Noetic_AetherBlight);
	ActivationOwnedTags.AddTag(TAG_Ability_Noetic_AetherBlight);
	ActivationBlockedTags.AddTag(TAG_Ability_Noetic_AetherBlight);
	BlockAbilitiesWithTag.AddTag(TAG_Ability_Noetic_AetherBlight);

	ArtCooldownTags.AddTag(TAG_Cooldown_Noetic_EffigyVow);
}

bool UGA_AetherBlight::ExecuteKit(const FNoeticKitSpec& Kit)
{
	ACombatCharacter* Character = GetCombatAvatar();
	UWorld* World = GetWorld();
	if (!Character || !World || Kit.Id != FName(TEXT("effigy_vow")))
	{
		return false;
	}

	FVector AimPoint = FVector::ZeroVector;
	if (!ResolveAimPoint(Kit.Range, AimPoint))
	{
		return false;
	}

	const FVector Start = Character->GetActorLocation();
	const FVector Direction = (AimPoint - Start).GetSafeNormal2D();
	const float Distance = Direction.IsNearlyZero()
		? 0.0f
		: FMath::Min(ToWorld(Kit.Range), FVector::Dist2D(Start, AimPoint));
	const FVector Desired = Start + Direction * Distance;
	const FVector SpawnLocation = ClearSpot(Desired);

	if (ANoeticEffigy* Existing = ANoeticEffigy::FindForOwner(Character))
	{
		Existing->Destroy();
	}

	FActorSpawnParameters Params;
	Params.Owner = Character;
	Params.Instigator = Character;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FRotator SpawnRotation = Direction.IsNearlyZero()
		? Character->GetActorRotation()
		: FRotator(0.0f, Direction.Rotation().Yaw, 0.0f);
	ANoeticEffigy* Effigy = World->SpawnActor<ANoeticEffigy>(ANoeticEffigy::StaticClass(), SpawnLocation, SpawnRotation, Params);
	if (!Effigy)
	{
		return false;
	}

	Effigy->ActivateEffigy(ToWorld(AeterniiNoeticKits::EntityRadius));
	return true;
}
