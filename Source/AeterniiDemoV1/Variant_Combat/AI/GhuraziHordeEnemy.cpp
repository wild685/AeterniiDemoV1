// Copyright Epic Games, Inc. All Rights Reserved.


#include "GhuraziHordeEnemy.h"
#include "CombatDamageable.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogGhuraziHorde, Log, All);

AGhuraziHordeEnemy::AGhuraziHordeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Ghurazi"));

	ApplyArchetypeDefaults();
}

void AGhuraziHordeEnemy::ApplyArchetypeDefaults()
{
	switch (Archetype)
	{
	case EGhuraziHordeArchetype::Raider:
		MaxHP = 3.0f;
		MeleeDamage = 1.0f;
		MeleeKnockbackImpulse = 150.0f;
		MeleeLaunchImpulse = 280.0f;
		MeleeTraceDistance = 105.0f;
		MeleeTraceRadius = 50.0f;
		TelegraphDuration = 0.34f;
		AttackRecoveryTime = 1.35f;
		PreferredEngageRange = 105.0f;
		MinChargeLoops = 1;
		MaxChargeLoops = 1;
		FallbackStrikeDelay = 0.12f;
		ChantStrikeRadius = 0.0f;
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 500.0f;
		}
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->InitCapsuleSize(35.0f, 90.0f);
		}
		break;

	case EGhuraziHordeArchetype::Chanter:
		MaxHP = 2.0f;
		MeleeDamage = 1.5f;
		MeleeKnockbackImpulse = 80.0f;
		MeleeLaunchImpulse = 220.0f;
		MeleeTraceDistance = 640.0f;
		MeleeTraceRadius = 80.0f;
		TelegraphDuration = 1.35f;
		AttackRecoveryTime = 3.1f;
		PreferredEngageRange = 640.0f;
		MinChargeLoops = 3;
		MaxChargeLoops = 5;
		FallbackStrikeDelay = 0.20f;
		ChantStrikeRadius = 190.0f;
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 350.0f;
		}
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->InitCapsuleSize(30.0f, 95.0f);
		}
		break;

	case EGhuraziHordeArchetype::Mauler:
		MaxHP = 8.0f;
		MeleeDamage = 2.0f;
		MeleeKnockbackImpulse = 280.0f;
		MeleeLaunchImpulse = 420.0f;
		MeleeTraceDistance = 145.0f;
		MeleeTraceRadius = 70.0f;
		TelegraphDuration = 0.72f;
		AttackRecoveryTime = 2.3f;
		PreferredEngageRange = 145.0f;
		MinChargeLoops = 2;
		MaxChargeLoops = 3;
		FallbackStrikeDelay = 0.16f;
		ChantStrikeRadius = 0.0f;
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 280.0f;
		}
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->InitCapsuleSize(45.0f, 100.0f);
		}
		break;

	default:
		break;
	}

	CurrentHP = MaxHP;
}

float AGhuraziHordeEnemy::GetTelegraphAlpha() const
{
	if (!bIsTelegraphing || TelegraphDuration <= KINDA_SMALL_NUMBER)
	{
		return 0.0f;
	}

	const float Elapsed = GetWorld() ? (GetWorld()->GetTimeSeconds() - TelegraphStartTime) : 0.0f;
	return FMath::Clamp(Elapsed / TelegraphDuration, 0.0f, 1.0f);
}

int32 AGhuraziHordeEnemy::ChooseComboHitCount() const
{
	const int32 BaseCount = Super::ChooseComboHitCount();

	// Raiders stay readable as a short swarm string; Maulers rarely chain.
	switch (Archetype)
	{
	case EGhuraziHordeArchetype::Raider:
		return FMath::Clamp(BaseCount, 1, 2);
	case EGhuraziHordeArchetype::Mauler:
		return 1;
	case EGhuraziHordeArchetype::Chanter:
		return 1;
	default:
		return BaseCount;
	}
}

int32 AGhuraziHordeEnemy::ChooseChargeLoopCount() const
{
	const float LoopDuration = FMath::Max(0.05f, ChargeLoopDuration);
	const int32 FromTelegraph = FMath::Max(1, FMath::RoundToInt(TelegraphDuration / LoopDuration));
	const int32 MinLoops = FMath::Max(1, MinChargeLoops);
	const int32 MaxLoops = FMath::Max(MinLoops, MaxChargeLoops);
	return FMath::Clamp(FromTelegraph, MinLoops, MaxLoops);
}

void AGhuraziHordeEnemy::DoAIComboAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;
	bDidStrikeThisAttack = false;
	bPendingChargedAttack = false;
	BeginTelegraph(/*bChargedAttack=*/false);
}

void AGhuraziHordeEnemy::DoAIChargedAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;
	bDidStrikeThisAttack = false;
	bPendingChargedAttack = true;

	TargetChargeLoops = ChooseChargeLoopCount();
	CurrentChargeLoop = 0;

	PlayChargedMontage();
	BeginTelegraph(/*bChargedAttack=*/true);
}

void AGhuraziHordeEnemy::BeginTelegraph(bool bChargedAttack)
{
	bIsTelegraphing = true;
	TelegraphStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	CacheCastLocationFromPlayer();

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		if (!bMovementFrozenForTelegraph)
		{
			WalkSpeedBeforeTelegraph = Movement->MaxWalkSpeed;
			bMovementFrozenForTelegraph = true;
		}
		Movement->StopMovementImmediately();
		Movement->MaxWalkSpeed = 0.0f;
	}

	NotifyPlayerOfTelegraph();
	BP_OnTelegraphStarted(Archetype, TelegraphDuration);

	UE_LOG(LogGhuraziHorde, Verbose, TEXT("%s telegraph start (%s, %.2fs, charged=%s)"),
		*GetName(),
		*UEnum::GetValueAsString(Archetype),
		TelegraphDuration,
		bChargedAttack ? TEXT("true") : TEXT("false"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TelegraphTimer, this, &AGhuraziHordeEnemy::CommitTelegraph, TelegraphDuration, false);
	}
	else
	{
		CommitTelegraph();
	}
}

void AGhuraziHordeEnemy::CommitTelegraph()
{
	if (!bIsTelegraphing)
	{
		return;
	}

	bIsTelegraphing = false;
	RestoreMovementFromTelegraph();
	BP_OnTelegraphCommitted(Archetype);

	if (bPendingChargedAttack)
	{
		// Force the existing charged-attack montage out of its windup loop.
		CurrentChargeLoop = TargetChargeLoops;
		CheckChargedAttack();
	}
	else
	{
		PlayComboMontage();
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	const bool bMontagePlaying = AnimInstance && (
		(ComboAttackMontage && AnimInstance->Montage_IsPlaying(ComboAttackMontage)) ||
		(ChargedAttackMontage && AnimInstance->Montage_IsPlaying(ChargedAttackMontage)));

	if (!bMontagePlaying)
	{
		TryFallbackStrike();
		return;
	}

	// Give montage AnimNotify_DoAttackTrace a chance to land before the C++ fallback.
	const float NotifyGrace = FMath::Max(FallbackStrikeDelay, 0.45f);
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(FallbackStrikeTimer, this, &AGhuraziHordeEnemy::TryFallbackStrike, NotifyGrace, false);
	}
}

void AGhuraziHordeEnemy::TryFallbackStrike()
{
	if (!bIsAttacking || CurrentHP <= 0.0f)
	{
		return;
	}

	if (!bDidStrikeThisAttack)
	{
		UE_LOG(LogGhuraziHorde, Verbose, TEXT("%s fallback strike (no AnimNotify_DoAttackTrace on montage yet)"), *GetName());
		DoAttackTrace(NAME_None);
	}

	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	const bool bMontagePlaying = AnimInstance && (
		(ComboAttackMontage && AnimInstance->Montage_IsPlaying(ComboAttackMontage)) ||
		(ChargedAttackMontage && AnimInstance->Montage_IsPlaying(ChargedAttackMontage)));

	if (!bMontagePlaying)
	{
		AttackMontageEnded(nullptr, true);
	}
}

void AGhuraziHordeEnemy::PlayComboMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		if (ComboAttackMontage)
		{
			TargetComboCount = ChooseComboHitCount();
			CurrentComboAttack = 0;

			const float MontageLength = AnimInstance->Montage_Play(ComboAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
			if (MontageLength > 0.0f)
			{
				AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
				return;
			}
		}
	}

	// No combo montage assigned yet — fallback strike completes the StateTree task.
	TryFallbackStrike();
}

void AGhuraziHordeEnemy::PlayChargedMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		if (ChargedAttackMontage)
		{
			const float MontageLength = AnimInstance->Montage_Play(ChargedAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
			if (MontageLength > 0.0f)
			{
				AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ChargedAttackMontage);
				return;
			}
		}
	}

	UE_LOG(LogGhuraziHorde, Warning, TEXT("%s charged telegraph has no ChargedAttackMontage — will fallback-strike on commit."), *GetName());
}

void AGhuraziHordeEnemy::DoAttackTrace(FName DamageSourceBone)
{
	bDidStrikeThisAttack = true;

	if (Archetype == EGhuraziHordeArchetype::Chanter)
	{
		ApplyChantStrike();
		return;
	}

	Super::DoAttackTrace(DamageSourceBone);
}

void AGhuraziHordeEnemy::ApplyChantStrike()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(ChantStrikeRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const FVector StrikeLocation = CachedCastLocation.IsNearlyZero() ? GetActorLocation() : CachedCastLocation;

	if (World->OverlapMultiByObjectType(Overlaps, StrikeLocation, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* HitActor = Overlap.GetActor();
			if (!HitActor || !HitActor->ActorHasTag(FName("Player")))
			{
				continue;
			}

			const FVector Away = (HitActor->GetActorLocation() - StrikeLocation).GetSafeNormal();
			const FVector Impulse = (Away * MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);
			DealHordeDamage(HitActor, StrikeLocation, Impulse);
		}
	}
}

void AGhuraziHordeEnemy::DealHordeDamage(AActor* HitActor, const FVector& DamageLocation, const FVector& Impulse)
{
	ICombatDamageable* Damageable = Cast<ICombatDamageable>(HitActor);
	if (!Damageable)
	{
		return;
	}

	// TODO(Gary): keep routing through ICombatDamageable::ApplyDamage until the GAS
	// attribute set is in. If UCombatAttributeSet should own horde melee, swap this
	// call site — do not add a second health attribute here.
	Damageable->ApplyDamage(MeleeDamage, this, DamageLocation, Impulse);
}

void AGhuraziHordeEnemy::CacheCastLocationFromPlayer()
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		CachedCastLocation = PlayerPawn->GetActorLocation();

		const FVector ToPlayer = (CachedCastLocation - GetActorLocation()).GetSafeNormal2D();
		if (!ToPlayer.IsNearlyZero())
		{
			SetActorRotation(ToPlayer.Rotation());
		}
	}
	else
	{
		CachedCastLocation = GetActorLocation() + (GetActorForwardVector() * PreferredEngageRange);
	}
}

void AGhuraziHordeEnemy::NotifyPlayerOfTelegraph()
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		if (ICombatDamageable* Damageable = Cast<ICombatDamageable>(PlayerPawn))
		{
			const FVector DangerPoint = (Archetype == EGhuraziHordeArchetype::Chanter) ? CachedCastLocation : GetActorLocation();
			Damageable->NotifyDanger(DangerPoint, this);
		}
	}
}

void AGhuraziHordeEnemy::RestoreMovementFromTelegraph()
{
	if (!bMovementFrozenForTelegraph)
	{
		return;
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeedBeforeTelegraph;
	}

	bMovementFrozenForTelegraph = false;
}

void AGhuraziHordeEnemy::ClearTelegraphState()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TelegraphTimer);
		World->GetTimerManager().ClearTimer(FallbackStrikeTimer);
	}

	const bool bHadOpenTelegraph = bIsTelegraphing;
	bIsTelegraphing = false;
	RestoreMovementFromTelegraph();

	if (bHadOpenTelegraph)
	{
		UE_LOG(LogGhuraziHorde, Verbose, TEXT("%s telegraph cleared"), *GetName());
	}
}

void AGhuraziHordeEnemy::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ClearTelegraphState();
	if (!bIsAttacking)
	{
		return;
	}

	Super::AttackMontageEnded(Montage, bInterrupted);
}

void AGhuraziHordeEnemy::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	const bool bComboTelegraphOpen = bIsTelegraphing && !bPendingChargedAttack;
	ClearTelegraphState();
	Super::ApplyDamage(Damage, DamageCauser, DamageLocation, DamageImpulse);

	// Combo windup has not started a montage yet, so Super cannot complete the StateTree task via montage end.
	if (bComboTelegraphOpen && bIsAttacking && CurrentHP > 0.0f)
	{
		AttackMontageEnded(nullptr, true);
	}
}

void AGhuraziHordeEnemy::HandleDeath()
{
	ClearTelegraphState();
	const bool bWasAttacking = bIsAttacking;
	Super::HandleDeath();

	if (bWasAttacking)
	{
		AttackMontageEnded(nullptr, true);
	}
}

void AGhuraziHordeEnemy::ApplyHordeSeparation(float DeltaSeconds)
{
	if (HordeSeparationDistance <= KINDA_SMALL_NUMBER || CurrentHP <= 0.0f)
	{
		return;
	}

	const float MaxPush = HordeSeparationSpeed * DeltaSeconds;
	const FVector MyLocation = GetActorLocation();

	for (TActorIterator<AGhuraziHordeEnemy> It(GetWorld()); It; ++It)
	{
		AGhuraziHordeEnemy* Other = *It;
		if (!Other || Other == this || Other->CurrentHP <= 0.0f)
		{
			continue;
		}

		FVector Offset = MyLocation - Other->GetActorLocation();
		Offset.Z = 0.0f;
		const float Distance = Offset.Size();
		if (Distance >= HordeSeparationDistance || Distance <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const FVector PushDir = Offset / Distance;
		const float PushAmount = FMath::Min((HordeSeparationDistance - Distance) * 0.5f, MaxPush);
		AddActorWorldOffset(FVector(PushDir.X, PushDir.Y, 0.0f) * PushAmount, true);
	}
}

void AGhuraziHordeEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;

	UE_LOG(LogGhuraziHorde, Log, TEXT("%s BeginPlay archetype=%s telegraph=%.2fs engage=%.0fcm recovery=%.2fs"),
		*GetName(),
		*UEnum::GetValueAsString(Archetype),
		TelegraphDuration,
		PreferredEngageRange,
		AttackRecoveryTime);
}

void AGhuraziHordeEnemy::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ClearTelegraphState();
	Super::EndPlay(EndPlayReason);
}

void AGhuraziHordeEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentHP <= 0.0f)
	{
		return;
	}

	ApplyHordeSeparation(DeltaSeconds);
}

#if WITH_EDITOR
void AGhuraziHordeEnemy::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AGhuraziHordeEnemy, Archetype))
	{
		ApplyArchetypeDefaults();
	}
}
#endif
