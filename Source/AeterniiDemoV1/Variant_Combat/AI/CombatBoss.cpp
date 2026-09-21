// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatBoss.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "CombatAbilitySystemComponent.h"
#include "CombatAttributeSet.h"
#include "CombatBossAIController.h"
#include "CombatGameplayTags.h"
#include "CombatLifeBar.h"
#include "Components/CapsuleComponent.h"
#include "GA_BossAttack.h"
#include "GE_Damage_Melee.h"

ACombatBoss::ACombatBoss()
{
	AIControllerClass = ACombatBossAIController::StaticClass();

	// Existing combo enemies default to 3 HP; bosses should survive a few player strings.
	MaxHP = 20.0f;
	CurrentHP = MaxHP;

	AbilitySystemComponent = CreateDefaultSubobject<UCombatAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("AttributeSet"));

	HitboxMelee = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Hitbox_Melee"));
	HitboxMelee->SetupAttachment(RootComponent);
	HitboxMelee->SetCapsuleSize(70.0f, 90.0f);
	HitboxMelee->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));
	HitboxMelee->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitboxMelee->SetGenerateOverlapEvents(false);
	HitboxMelee->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitboxMelee->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitboxMelee->SetCollisionObjectType(ECC_WorldDynamic);

	HitboxAOE = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Hitbox_AOE"));
	HitboxAOE->SetupAttachment(RootComponent);
	HitboxAOE->SetCapsuleSize(220.0f, 80.0f);
	HitboxAOE->SetRelativeLocation(FVector::ZeroVector);
	HitboxAOE->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitboxAOE->SetGenerateOverlapEvents(false);
	HitboxAOE->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitboxAOE->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitboxAOE->SetCollisionObjectType(ECC_WorldDynamic);

	SweepAbilityClass = UGA_Boss_Sweep::StaticClass();
	SlamAbilityClass = UGA_Boss_Slam::StaticClass();
	CloserAbilityClass = UGA_Boss_Closer::StaticClass();
	MeleeDamageEffect = UGE_Damage_Melee::StaticClass();
}

UAbilitySystemComponent* ACombatBoss::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACombatBoss::SetNextAttack(EBossAttack InAttack)
{
	NextAttack = InAttack;
}

void ACombatBoss::SetLastAttack(EBossAttack InAttack)
{
	LastAttack = InAttack;
}

void ACombatBoss::SetAcquiredTarget(AActor* InTarget)
{
	AcquiredTarget = InTarget;
}

float ACombatBoss::GetAcquiredTargetDist2D() const
{
	if (!AcquiredTarget.IsValid())
	{
		return -1.0f;
	}

	return FVector::Dist2D(GetActorLocation(), AcquiredTarget->GetActorLocation());
}

EBossAttack ACombatBoss::ChooseAttack(float Dist2D, EBossAttack InLastAttack)
{
	struct FCandidate
	{
		EBossAttack Attack = EBossAttack::None;
		float Weight = 0.0f;
	};

	TArray<FCandidate, TInlineAllocator<3>> Candidates;

	auto AddIfInRange = [&Candidates, Dist2D](EBossAttack Attack, bool bInRange, float BaseWeight)
	{
		if (bInRange && BaseWeight > 0.0f)
		{
			Candidates.Add({Attack, BaseWeight});
		}
	};

	// Dist2D gates
	AddIfInRange(EBossAttack::Sweep, Dist2D >= 150.0f && Dist2D <= 400.0f, 0.50f);
	AddIfInRange(EBossAttack::Slam, Dist2D >= 100.0f && Dist2D <= 300.0f, 0.30f);
	AddIfInRange(EBossAttack::Closer, Dist2D > 400.0f, 0.20f);

	auto FindWeight = [&Candidates](EBossAttack Attack) -> float*
	{
		for (FCandidate& Candidate : Candidates)
		{
			if (Candidate.Attack == Attack)
			{
				return &Candidate.Weight;
			}
		}
		return nullptr;
	};

	// last Sweep → Sweep×0.4
	if (InLastAttack == EBossAttack::Sweep)
	{
		if (float* Weight = FindWeight(EBossAttack::Sweep))
		{
			*Weight *= 0.4f;
		}
	}

	// last Slam → Slam×0.5
	if (InLastAttack == EBossAttack::Slam)
	{
		if (float* Weight = FindWeight(EBossAttack::Slam))
		{
			*Weight *= 0.5f;
		}
	}

	// never Closer twice
	if (InLastAttack == EBossAttack::Closer)
	{
		Candidates.RemoveAll([](const FCandidate& Candidate)
		{
			return Candidate.Attack == EBossAttack::Closer;
		});
	}

	// Dist>450 → Closer×2 Sweep×0.5
	if (Dist2D > 450.0f)
	{
		if (float* Weight = FindWeight(EBossAttack::Closer))
		{
			*Weight *= 2.0f;
		}
		if (float* Weight = FindWeight(EBossAttack::Sweep))
		{
			*Weight *= 0.5f;
		}
	}

	// Dist<180 → Slam×1.5 Closer×0
	if (Dist2D < 180.0f)
	{
		if (float* Weight = FindWeight(EBossAttack::Slam))
		{
			*Weight *= 1.5f;
		}
		if (float* Weight = FindWeight(EBossAttack::Closer))
		{
			*Weight *= 0.0f;
		}
	}

	float TotalWeight = 0.0f;
	for (const FCandidate& Candidate : Candidates)
	{
		if (Candidate.Weight > 0.0f)
		{
			TotalWeight += Candidate.Weight;
		}
	}

	if (TotalWeight <= 0.0f)
	{
		return EBossAttack::None;
	}

	float Roll = FMath::FRand() * TotalWeight;
	for (const FCandidate& Candidate : Candidates)
	{
		if (Candidate.Weight <= 0.0f)
		{
			continue;
		}

		Roll -= Candidate.Weight;
		if (Roll <= 0.0f)
		{
			return Candidate.Attack;
		}
	}

	return Candidates.Last().Attack;
}

bool ACombatBoss::TryActivateBossAttack(EBossAttack Attack)
{
	if (Attack == EBossAttack::None || !AbilitySystemComponent)
	{
		return false;
	}

	const TSubclassOf<UGameplayAbility> AbilityClass = GetAbilityClassForAttack(Attack);
	if (!AbilityClass)
	{
		UE_LOG(LogCombatGAS, Warning, TEXT("%s: no GA class mapped for %s."), *GetName(), *UEnum::GetValueAsString(Attack));
		return false;
	}

	const bool bActivated = AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass);
	UE_LOG(LogCombatGAS, Log, TEXT("%s TryActivateAbility %s -> %s"),
		*GetName(),
		*AbilityClass->GetName(),
		bActivated ? TEXT("ok") : TEXT("failed"));
	return bActivated;
}

bool ACombatBoss::IsAbilityAttackActive() const
{
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(TAG_Ability_Attack);
}

void ACombatBoss::OpenHitbox(EBossHitbox Hitbox)
{
	HitActorsThisSwing.Reset();

	if (UCapsuleComponent* Capsule = GetHitbox(Hitbox))
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Capsule->SetGenerateOverlapEvents(true);

		TArray<AActor*> Overlaps;
		Capsule->GetOverlappingActors(Overlaps, APawn::StaticClass());
		for (AActor* Overlap : Overlaps)
		{
			TryHitActor(Overlap);
		}
	}
}

void ACombatBoss::CloseHitbox(EBossHitbox Hitbox)
{
	if (UCapsuleComponent* Capsule = GetHitbox(Hitbox))
	{
		Capsule->SetGenerateOverlapEvents(false);
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACombatBoss::CloseAllHitboxes()
{
	CloseHitbox(EBossHitbox::Melee);
	CloseHitbox(EBossHitbox::AOE);
	HitActorsThisSwing.Reset();
}

void ACombatBoss::NotifyTellStart()
{
	UE_LOG(LogCombatGAS, Verbose, TEXT("%s Tell start (VFX/audio hook)."), *GetName());
	BP_OnTellStart();
}

void ACombatBoss::NotifyRecoverStart()
{
	CloseAllHitboxes();
	UE_LOG(LogCombatGAS, Verbose, TEXT("%s Recover start."), *GetName());
	BP_OnRecoverStart();
}

void ACombatBoss::NotifyAttackEnd()
{
	CloseAllHitboxes();

	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(TAG_Ability_Attack))
	{
		FGameplayTagContainer AttackTags;
		AttackTags.AddTag(TAG_Ability_Attack);
		AbilitySystemComponent->CancelAbilities(&AttackTags);
	}

	BP_OnAttackEnd();
}

void ACombatBoss::SyncHealthFromGAS(float NewHealth, float NewMaxHealth)
{
	CurrentHP = NewHealth;
	MaxHP = NewMaxHealth;

	if (LifeBarWidget && NewMaxHealth > 0.0f)
	{
		LifeBarWidget->SetLifePercentage(NewHealth / NewMaxHealth);
	}

	if (NewHealth <= 0.0f)
	{
		HandleDeath();
	}
}

void ACombatBoss::BeginPlay()
{
	Super::BeginPlay();

	if (HitboxMelee)
	{
		HitboxMelee->OnComponentBeginOverlap.AddDynamic(this, &ACombatBoss::OnHitboxBeginOverlap);
	}
	if (HitboxAOE)
	{
		HitboxAOE->OnComponentBeginOverlap.AddDynamic(this, &ACombatBoss::OnHitboxBeginOverlap);
	}

	InitializeAbilitySystem();
}

void ACombatBoss::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilitySystem();
}

void ACombatBoss::OnRep_Controller()
{
	Super::OnRep_Controller();
	InitializeAbilitySystem();
}

void ACombatBoss::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	CloseAllHitboxes();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}

	Super::HandleDeath();
}

float ACombatBoss::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}

	if (AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(TAG_Status_Dodge_IFrames))
	{
		return 0.0f;
	}

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (AttributeSet)
	{
		AttributeSet->SetHealth(CurrentHP);
		AttributeSet->SetMaxHealth(MaxHP);
	}

	return ActualDamage;
}

void ACombatBoss::InitializeAbilitySystem()
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (!bHealthDelegateBound)
	{
		AttributeSet->OnHealthChanged.AddDynamic(this, &ACombatBoss::HandleGASHealthChanged);
		bHealthDelegateBound = true;
	}

	AttributeSet->InitMaxHealth(MaxHP);
	AttributeSet->InitHealth(CurrentHP > 0.0f ? CurrentHP : MaxHP);
	AttributeSet->InitMaxStamina(100.0f);
	AttributeSet->InitStamina(100.0f);

	GrantDefaultAbilities();
}

void ACombatBoss::GrantDefaultAbilities()
{
	if (bDefaultAbilitiesGranted || !AbilitySystemComponent)
	{
		return;
	}

	TArray<TSubclassOf<UGameplayAbility>> ToGrant = DefaultAbilities;
	if (ToGrant.Num() == 0)
	{
		ToGrant.Add(SweepAbilityClass);
		ToGrant.Add(SlamAbilityClass);
		ToGrant.Add(CloserAbilityClass);
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : ToGrant)
	{
		AbilitySystemComponent->GrantAbilityIfMissing(AbilityClass);
	}

	bDefaultAbilitiesGranted = true;
}

void ACombatBoss::HandleGASHealthChanged(float NewHealth, float NewMaxHealth)
{
	SyncHealthFromGAS(NewHealth, NewMaxHealth);
}

void ACombatBoss::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryHitActor(OtherActor);
}

void ACombatBoss::TryHitActor(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == this || HitActorsThisSwing.Contains(OtherActor))
	{
		return;
	}

	HitActorsThisSwing.Add(OtherActor);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ApplyMeleeDamageTo(OtherActor, MeleeDamage, MeleeDamageEffect);
	}
}

UCapsuleComponent* ACombatBoss::GetHitbox(EBossHitbox Hitbox) const
{
	switch (Hitbox)
	{
	case EBossHitbox::Melee:
		return HitboxMelee;
	case EBossHitbox::AOE:
		return HitboxAOE;
	default:
		return nullptr;
	}
}

TSubclassOf<UGameplayAbility> ACombatBoss::GetAbilityClassForAttack(EBossAttack Attack) const
{
	switch (Attack)
	{
	case EBossAttack::Sweep:
		return SweepAbilityClass;
	case EBossAttack::Slam:
		return SlamAbilityClass;
	case EBossAttack::Closer:
		return CloserAbilityClass;
	default:
		return nullptr;
	}
}
