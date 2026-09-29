// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_NoeticArtBase.h"
#include "AeterniiAttributeSet.h"
#include "CombatCharacter.h"
#include "CombatEnemy.h"
#include "CombatGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

namespace NoeticArtMath
{
	static float DistSqPointToSegment2D(const FVector& Point, const FVector& A, const FVector& B)
	{
		const FVector2D P(Point.X, Point.Y);
		const FVector2D A2(A.X, A.Y);
		const FVector2D B2(B.X, B.Y);
		const FVector2D AB = B2 - A2;
		const float Denom = AB.SizeSquared();
		float T = 0.0f;
		if (Denom > KINDA_SMALL_NUMBER)
		{
			T = FMath::Clamp(FVector2D::DotProduct(P - A2, AB) / Denom, 0.0f, 1.0f);
		}
		const FVector2D Closest = A2 + AB * T;
		return FVector2D::DistSquared(P, Closest);
	}
}

UGA_NoeticArtBase::UGA_NoeticArtBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

ENoeticCanonArt UGA_NoeticArtBase::GetArt() const
{
	return ENoeticCanonArt::ExistenceShift;
}

bool UGA_NoeticArtBase::ExecuteKit(const FNoeticKitSpec&)
{
	return false;
}

bool UGA_NoeticArtBase::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	FNoeticKitSpec Kit;
	return ResolveKit(ActorInfo, Kit);
}

const FGameplayTagContainer* UGA_NoeticArtBase::GetCooldownTags() const
{
	return &ArtCooldownTags;
}

void UGA_NoeticArtBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	UWorld* World = GetWorld();
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	if (World)
	{
		for (TPair<FGameplayTag, FTimerHandle>& Pair : LooseTagTimers)
		{
			if (AbilitySystem && World->GetTimerManager().IsTimerActive(Pair.Value))
			{
				AbilitySystem->RemoveLooseGameplayTag(Pair.Key);
			}
			World->GetTimerManager().ClearTimer(Pair.Value);
		}
	}
	LooseTagTimers.Reset();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGA_NoeticArtBase::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FNoeticKitSpec Kit;
	if (!ResolveKit(ActorInfo, Kit) || !ExecuteKit(Kit))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	StartKitCooldown(Kit);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

bool UGA_NoeticArtBase::ResolveKit(const FGameplayAbilityActorInfo* ActorInfo, FNoeticKitSpec& OutKit) const
{
	const ACombatCharacter* Character = ActorInfo ? Cast<ACombatCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	if (!Character)
	{
		return false;
	}

	const FNoeticKitSpec* Kit = AeterniiNoeticKits::FindKit(Character->GetAppliedPlayerClassId(), GetArt());
	if (!Kit)
	{
		return false;
	}

	OutKit = *Kit;
	return true;
}

ACombatCharacter* UGA_NoeticArtBase::GetCombatAvatar() const
{
	return Cast<ACombatCharacter>(GetAvatarActorFromActorInfo());
}

float UGA_NoeticArtBase::ToWorld(float DemoUnits) const
{
	const ACombatCharacter* Character = GetCombatAvatar();
	const float Scale = Character ? Character->GetDemoUnitsToCentimeters() : 1.0f;
	return DemoUnits * Scale;
}

bool UGA_NoeticArtBase::ResolveAimPoint(float DemoRange, FVector& OutPoint) const
{
	ACombatCharacter* Character = GetCombatAvatar();
	if (!Character)
	{
		return false;
	}

	const FVector OriginLocation = Character->GetActorLocation();
	const float WorldRange = ToWorld(DemoRange);
	FVector RayOrigin = OriginLocation;
	FVector RayDirection = Character->GetActorForwardVector();

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		float MouseX = 0.0f;
		float MouseY = 0.0f;
		FVector WorldOrigin = FVector::ZeroVector;
		FVector WorldDirection = FVector::ZeroVector;
		if (PlayerController->GetMousePosition(MouseX, MouseY)
			&& PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldOrigin, WorldDirection))
		{
			RayOrigin = WorldOrigin;
			RayDirection = WorldDirection;
		}
		else if (UCameraComponent* Camera = Character->GetFollowCamera())
		{
			RayOrigin = Camera->GetComponentLocation();
			RayDirection = PlayerController->GetControlRotation().Vector();
		}
	}

	const float PlaneZ = OriginLocation.Z - Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	if (FMath::Abs(RayDirection.Z) > KINDA_SMALL_NUMBER)
	{
		const float T = (PlaneZ - RayOrigin.Z) / RayDirection.Z;
		if (T > 0.0f)
		{
			OutPoint = RayOrigin + RayDirection * T;
			OutPoint.Z = OriginLocation.Z;
			return true;
		}
	}

	OutPoint = OriginLocation + RayDirection.GetSafeNormal2D() * WorldRange;
	OutPoint.Z = OriginLocation.Z;
	return true;
}

bool UGA_NoeticArtBase::ProbeClear(const FVector& From, const FVector& To, float DemoProbeRadius) const
{
	const ACombatCharacter* Character = GetCombatAvatar();
	UWorld* World = GetWorld();
	if (!Character || !World)
	{
		return false;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(NoeticProbe), false, Character);
	const float Radius = ToWorld(DemoProbeRadius);
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	FHitResult Hit;
	return !World->SweepSingleByChannel(Hit, From, To, FQuat::Identity, ECC_WorldStatic, Shape, Params);
}

FVector UGA_NoeticArtBase::BlinkTo(const FVector& AimPoint, float DemoRange) const
{
	ACombatCharacter* Character = GetCombatAvatar();
	if (!Character)
	{
		return FVector::ZeroVector;
	}

	const FVector Start = Character->GetActorLocation();
	const FVector Direction = (AimPoint - Start).GetSafeNormal2D();
	if (Direction.IsNearlyZero())
	{
		return Start;
	}

	const float Want = FMath::Min(ToWorld(DemoRange), FVector::Dist2D(Start, AimPoint));
	const float Step = ToWorld(AeterniiNoeticKits::BlinkStep);
	FVector Accepted = Start;
	const UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	const FCollisionShape Shape = FCollisionShape::MakeCapsule(Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight());
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NoeticBlink), false, Character);

	if (Step <= KINDA_SMALL_NUMBER)
	{
		return Start;
	}

	for (float Distance = Step; Distance <= Want + KINDA_SMALL_NUMBER; Distance += Step)
	{
		const FVector Next = Start + Direction * FMath::Min(Distance, Want);
		FHitResult Hit;
		const bool bBlocked = GetWorld()->SweepSingleByChannel(Hit, Accepted, Next, FQuat::Identity, ECC_WorldStatic, Shape, Params);
		if (bBlocked)
		{
			break;
		}
		Accepted = Next;
	}

	Character->SetActorLocation(Accepted, false, nullptr, ETeleportType::TeleportPhysics);
	Character->SetActorRotation(FRotator(0.0f, Direction.Rotation().Yaw, 0.0f));
	return Accepted;
}

FVector UGA_NoeticArtBase::ClearSpot(const FVector& Desired) const
{
	const ACombatCharacter* Character = GetCombatAvatar();
	if (!Character)
	{
		return Desired;
	}

	auto SpotClear = [this, Character](const FVector& Spot)
	{
		return ProbeClear(Spot, Spot + FVector(0.0f, 0.0f, 1.0f), AeterniiNoeticKits::EntityRadius);
	};

	if (SpotClear(Desired))
	{
		return Desired;
	}

	const float RingStep = ToWorld(AeterniiNoeticKits::ClearSpotRingStep);
	const float MaxRadius = ToWorld(AeterniiNoeticKits::ClearSpotMaxRadius);
	for (float Radius = RingStep; Radius <= MaxRadius + KINDA_SMALL_NUMBER; Radius += RingStep)
	{
		for (int32 Spoke = 0; Spoke < AeterniiNoeticKits::ClearSpotSpokes; ++Spoke)
		{
			const float Angle = (static_cast<float>(Spoke) / static_cast<float>(AeterniiNoeticKits::ClearSpotSpokes)) * TWO_PI;
			const FVector Candidate = Desired + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;
			if (SpotClear(Candidate))
			{
				return Candidate;
			}
		}
	}

	// The demo falls back to a nave coordinate. That point is not a general spawn, so stay at the request.
	UE_LOG(LogCombatGAS, Verbose, TEXT("ClearSpot found no open ground near %s."), *Desired.ToCompactString());
	return Desired;
}

void UGA_NoeticArtBase::ForEachLivingEnemy(TFunctionRef<void(ACombatEnemy*)> Callback) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ACombatEnemy> It(World); It; ++It)
	{
		ACombatEnemy* Enemy = *It;
		if (Enemy && Enemy->CurrentHP > 0.0f)
		{
			Callback(Enemy);
		}
	}
}

void UGA_NoeticArtBase::DamageEnemiesNearSegment(const FVector& Start, const FVector& End, float DemoRadius, float Amount) const
{
	ACombatCharacter* Character = GetCombatAvatar();
	if (!Character || Amount <= 0.0f)
	{
		return;
	}

	const float RadiusSq = FMath::Square(ToWorld(DemoRadius));
	ForEachLivingEnemy([&](ACombatEnemy* Enemy)
	{
		if (NoeticArtMath::DistSqPointToSegment2D(Enemy->GetActorLocation(), Start, End) < RadiusSq)
		{
			Enemy->ApplyDamage(Amount, Character, Enemy->GetActorLocation(), FVector::ZeroVector);
		}
	});
}

void UGA_NoeticArtBase::DamageEnemiesInRadius(const FVector& Center, float DemoRadius, float Amount) const
{
	ACombatCharacter* Character = GetCombatAvatar();
	if (!Character || Amount <= 0.0f)
	{
		return;
	}

	const float RadiusSq = FMath::Square(ToWorld(DemoRadius));
	ForEachLivingEnemy([&](ACombatEnemy* Enemy)
	{
		if (FVector::DistSquared2D(Enemy->GetActorLocation(), Center) < RadiusSq)
		{
			Enemy->ApplyDamage(Amount, Character, Enemy->GetActorLocation(), FVector::ZeroVector);
		}
	});
}

void UGA_NoeticArtBase::SlowEnemiesInRadius(const FVector& Center, float DemoRadius, float Duration, float SlowAmount) const
{
	const float RadiusSq = FMath::Square(ToWorld(DemoRadius));
	ForEachLivingEnemy([&](ACombatEnemy* Enemy)
	{
		if (FVector::DistSquared2D(Enemy->GetActorLocation(), Center) < RadiusSq)
		{
			Enemy->ApplyNoeticSlow(Duration, SlowAmount);
		}
	});
}

void UGA_NoeticArtBase::AddAeterniiAttribute(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude) const
{
	if (!EffectClass || FMath::IsNearlyZero(Magnitude))
	{
		return;
	}

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(EffectClass, 1.0f);
	if (!Spec.IsValid())
	{
		return;
	}

	Spec.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
	ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec);
}

void UGA_NoeticArtBase::HoldLooseTag(FGameplayTag Tag, float Duration)
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo();
	UWorld* World = GetWorld();
	if (!AbilitySystem || !World || !Tag.IsValid() || Duration <= 0.0f)
	{
		return;
	}

	AbilitySystem->AddLooseGameplayTag(Tag);
	FTimerHandle& Handle = LooseTagTimers.FindOrAdd(Tag);
	World->GetTimerManager().SetTimer(
		Handle,
		FTimerDelegate::CreateUObject(this, &UGA_NoeticArtBase::ReleaseLooseTag, Tag),
		Duration,
		false);
}

void UGA_NoeticArtBase::ReleaseLooseTag(FGameplayTag Tag)
{
	if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
	{
		AbilitySystem->RemoveLooseGameplayTag(Tag);
	}
}

void UGA_NoeticArtBase::StartKitCooldown(const FNoeticKitSpec& Kit)
{
	const FGameplayTag Tag = AeterniiNoeticKits::CooldownTagForKit(Kit.Id);
	HoldLooseTag(Tag, Kit.CooldownSeconds);
	UE_LOG(LogCombatGAS, Log, TEXT("Noetic kit %s cooldown %.2fs."), *Kit.Id.ToString(), Kit.CooldownSeconds);
}
