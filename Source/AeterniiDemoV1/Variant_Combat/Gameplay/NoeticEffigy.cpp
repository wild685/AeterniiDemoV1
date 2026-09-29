// Copyright Epic Games, Inc. All Rights Reserved.

#include "NoeticEffigy.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "NoeticArtKits.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ANoeticEffigy::ANoeticEffigy()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Pawn);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ANoeticEffigy::ActivateEffigy(float WorldRadius)
{
	CurrentHP = AeterniiNoeticKits::EffigyHitPoints;
	Sphere->SetSphereRadius(WorldRadius);

	// Engine basic sphere is 50 units in radius. Scale the visual to the collision radius.
	if (Mesh->GetStaticMesh())
	{
		const float MeshScale = Sphere->GetScaledSphereRadius() / 50.0f;
		Mesh->SetRelativeScale3D(FVector(MeshScale));
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(LifeTimer, this, &ANoeticEffigy::Expire, AeterniiNoeticKits::EffigyLifeSeconds, false);
	}
}

ANoeticEffigy* ANoeticEffigy::FindForOwner(const AActor* InOwner)
{
	if (!InOwner)
	{
		return nullptr;
	}

	UWorld* World = InOwner->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	for (TActorIterator<ANoeticEffigy> It(World); It; ++It)
	{
		if (It->GetOwner() == InOwner && It->IsLive())
		{
			return *It;
		}
	}
	return nullptr;
}

bool ANoeticEffigy::IsLive() const
{
	return CurrentHP > 0.0f && !IsActorBeingDestroyed();
}

void ANoeticEffigy::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	if (!IsLive() || Damage <= 0.0f)
	{
		return;
	}

	// The caster's melee also sweeps ECC_Pawn. The decoy only takes foe hits.
	if (const AActor* OwnerActor = GetOwner())
	{
		if (DamageCauser == OwnerActor || (DamageCauser && DamageCauser->GetInstigator() == OwnerActor))
		{
			return;
		}
	}

	CurrentHP -= Damage;
	if (CurrentHP <= 0.0f)
	{
		CurrentHP = 0.0f;
		HandleDeath();
	}
}

void ANoeticEffigy::HandleDeath()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LifeTimer);
	}
	Destroy();
}

void ANoeticEffigy::ApplyHealing(float Healing, AActor* Healer)
{
}

void ANoeticEffigy::NotifyDanger(const FVector& DangerLocation, AActor* DangerSource)
{
}

void ANoeticEffigy::Expire()
{
	HandleDeath();
}
