// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatBossTypes.h"
#include "CombatEnemy.h"
#include "CombatBoss.generated.h"

class UAnimMontage;
class UCapsuleComponent;
class UCombatAttributeSet;
class UGameplayAbility;
class UGameplayEffect;

/**
 *  GAS-lite boss pawn. Extends ACombatEnemy so existing combat levels can swap
 *  the enemy class without a parallel character stack.
 *
 *  Attacks are Gameplay Abilities (TryActivateAbility), never raw PlayMontage.
 */
UCLASS(abstract)
class ACombatBoss : public ACombatEnemy
{
	GENERATED_BODY()

	/** Forward melee capsule. Disabled until the Active damage window. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> HitboxMelee;

	/** Optional slam impact capsule. Disabled until the Active damage window. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> HitboxAOE;

public:

	ACombatBoss();

	/** Legacy boss health/stamina set. Depth attributes are ACombatEnemy::GetAeterniiAttributeSet. */
	UFUNCTION(BlueprintPure, Category = "GAS")
	UCombatAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure, Category = "Boss")
	EBossAttack GetNextAttack() const { return NextAttack; }

	UFUNCTION(BlueprintPure, Category = "Boss")
	EBossAttack GetLastAttack() const { return LastAttack; }

	void SetNextAttack(EBossAttack InAttack);
	void SetLastAttack(EBossAttack InAttack);

	UFUNCTION(BlueprintPure, Category = "Boss")
	AActor* GetAcquiredTarget() const { return AcquiredTarget.Get(); }

	void SetAcquiredTarget(AActor* InTarget);

	/** 2D distance to the acquired target, or -1 if none. */
	UFUNCTION(BlueprintPure, Category = "Boss")
	float GetAcquiredTargetDist2D() const;

	/**
	 *  Weighted attack pick used by STT_ChooseBossAttack.
	 *  Dist2D gates: Sweep 150-400, Slam 100-300, Closer >400.
	 *  Base weights: Sweep 0.50 / Slam 0.30 / Closer 0.20.
	 *  Returns None when no candidates remain.
	 */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	static EBossAttack ChooseAttack(float Dist2D, EBossAttack InLastAttack);

	/** Maps EBossAttack → granted GA and calls TryActivateAbility. Never PlayMontage. */
	UFUNCTION(BlueprintCallable, Category = "Boss")
	bool TryActivateBossAttack(EBossAttack Attack);

	UFUNCTION(BlueprintPure, Category = "Boss")
	bool IsAbilityAttackActive() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|Hitbox")
	void OpenHitbox(EBossHitbox Hitbox);

	UFUNCTION(BlueprintCallable, Category = "Boss|Hitbox")
	void CloseHitbox(EBossHitbox Hitbox);

	UFUNCTION(BlueprintCallable, Category = "Boss|Hitbox")
	void CloseAllHitboxes();

	void NotifyTellStart();
	void NotifyRecoverStart();
	void NotifyAttackEnd();

	void SyncHealthFromGAS(float NewHealth, float NewMaxHealth);

protected:

	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> AttributeSet;

	/** Granted on possess if the editor array is empty. */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> MeleeDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> SweepAbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> SlamAbilityClass;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayAbility> CloserAbilityClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	EBossAttack NextAttack = EBossAttack::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	EBossAttack LastAttack = EBossAttack::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	TWeakObjectPtr<AActor> AcquiredTarget;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActorsThisSwing;

	bool bDefaultAbilitiesGranted = false;
	bool bHealthDelegateBound = false;
	bool bIsDead = false;

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void HandleDeath() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void InitializeAbilitySystem();
	void GrantDefaultAbilities();

	UFUNCTION()
	void HandleGASHealthChanged(float NewHealth, float NewMaxHealth);

	UFUNCTION()
	void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void TryHitActor(AActor* OtherActor);
	UCapsuleComponent* GetHitbox(EBossHitbox Hitbox) const;
	TSubclassOf<UGameplayAbility> GetAbilityClassForAttack(EBossAttack Attack) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Attack")
	void BP_OnTellStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Attack")
	void BP_OnRecoverStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss|Attack")
	void BP_OnAttackEnd();
};
