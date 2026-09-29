// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatDamageable.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NoeticEffigy.generated.h"

/**
 *  Aether Blight decoy (isometric effigy_vow).
 *  95 HP and a 7.5s life. Hits use ICombatDamageable::ApplyDamage on this actor.
 *  They do not write UAeterniiAttributeSet.
 */
UCLASS()
class ANoeticEffigy : public AActor, public ICombatDamageable
{
	GENERATED_BODY()

public:

	ANoeticEffigy();

	void ActivateEffigy(float WorldRadius);

	static ANoeticEffigy* FindForOwner(const AActor* InOwner);

	bool IsLive() const;

	virtual void ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse) override;
	virtual void HandleDeath() override;
	virtual void ApplyHealing(float Healing, AActor* Healer) override;
	virtual void NotifyDanger(const FVector& DangerLocation, AActor* DangerSource) override;

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere, Category="Effigy")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category="Effigy")
	float CurrentHP = 0.0f;

	FTimerHandle LifeTimer;

	UFUNCTION()
	void Expire();
};
