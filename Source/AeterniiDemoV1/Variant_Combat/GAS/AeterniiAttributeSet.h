// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AeterniiAttributeSet.generated.h"

#define AETERNII_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAeterniiHealthChanged, float, NewHealth, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAeterniiCorruptionChanged, float, NewCorruption, float, NewMaxCorruption);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAeterniiDepthChanged, float, NewDepth);

/**
 *  One row of DEPTHS[] from Aeternii_Isometric_Demo.html.
 *  Index 0 is Lux Stratum. Visual-only fields (distort, wash) are not copied.
 */
USTRUCT(BlueprintType)
struct FAeterniiDepthStratum
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Depth")
	int32 Index = 0;

	/** HTML numeral: I, II, III, IV. */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	FString Numeral;

	UPROPERTY(BlueprintReadOnly, Category="Depth")
	FString Name;

	/** DEPTHS[i].aggro */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	float Aggro = 0.0f;

	/** DEPTHS[i].spd */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	float SpeedMultiplier = 1.0f;

	/** DEPTHS[i].dmg */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	float DamageMultiplier = 1.0f;

	/**
	 *  DEPTHS[i].corr. Passive meter input, before CorruptionRate and the 0.62 time scale.
	 *  Not the Corruption attribute.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	float PassiveCorruption = 0.0f;

	/** DEPTHS[i].maxAlive */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	int32 MaxAlive = 0;

	/** DEPTHS[i].spawn */
	UPROPERTY(BlueprintReadOnly, Category="Depth")
	float SpawnInterval = 0.0f;
};

/**
 *  Project attribute set for Noetic Arts and the Depth / Corruption meter.
 *
 *  The seven isometric CLASSES combat fields are:
 *  Health (hp), Armor (armor), MoveSpeed (speed), Damage (dmg),
 *  AttackCooldown (atkCd), AttackRange (range), CorruptionRate (corrRate).
 *  Depth and Corruption are the runtime meters. MaxHealth and MaxCorruption are their caps.
 *
 *  Pack and horde weapon damage stays on ICombatDamageable::ApplyDamage / TakeDamage
 *  (ACombatCharacter::CurrentHP / ACombatEnemy::CurrentHP). Do not write those from here.
 *
 *  UCombatAttributeSet remains the legacy boss health/stamina set used by GE_Damage_Melee.
 *  It is not this class and does not carry Depth or Corruption.
 *
 *  Formulas below are the isometric demo (hurtPlayer, addCorr, and the update-loop
 *  passive). The tactical prototype HTML uses a different depth and corruption model
 *  (accuracy drift, per-round flat corruption). That model is not implemented here.
 */

/** Numeric constants from Aeternii_Isometric_Demo.html. Not the tactical prototype. */
namespace AeterniiCorruptionMeter
{
	inline constexpr float MeterMax = 100.0f;
	inline constexpr float ThinSelfThreshold = 50.0f;
	inline constexpr float ThinSelfDivisor = 250.0f;
	inline constexpr float SpeedPenaltyThreshold = 60.0f;
	inline constexpr float SpeedPenaltyDivisor = 400.0f;
	inline constexpr float PassiveCorruptionTimeScale = 0.62f;
	inline constexpr float PurgeIncomingScale = 0.15f;
	inline constexpr float PurgeDecayPerSecond = 6.0f;
	inline constexpr float ChantCorruptionAmount = 9.0f;
	inline constexpr float UnmakeCorruptionRefund = 1.0f;
	inline constexpr int32 StratumCount = 4;
	inline constexpr float ProgressThresholds[StratumCount] = { 0.0f, 0.20f, 0.46f, 0.74f };
}

UCLASS(BlueprintType)
class UAeterniiAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UAeterniiAttributeSet();

	/** Isometric hp. Cap is MaxHealth. Not pack CurrentHP. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, MaxHealth)

	/** Isometric armor. Fraction in 0–1, for example 0.32. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_Armor)
	FGameplayAttributeData Armor;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, Armor)

	/** Isometric speed. Demo units, not Unreal cm/s. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, MoveSpeed)

	/** Isometric dmg. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_Damage)
	FGameplayAttributeData Damage;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, Damage)

	/** Isometric atkCd, in seconds. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_AttackCooldown)
	FGameplayAttributeData AttackCooldown;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, AttackCooldown)

	/** Isometric range. Demo units, not Unreal cm. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, AttackRange)

	/** Isometric corrRate. Multiplies incoming corruption in AddCorruption. */
	UPROPERTY(BlueprintReadOnly, Category="Combat", ReplicatedUsing=OnRep_CorruptionRate)
	FGameplayAttributeData CorruptionRate;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, CorruptionRate)

	/**
	 *  Stratum index 0–3 (Lux, Velum, Abyssii, Ruptura).
	 *  Matches isometric G.depth, not the prototype's 1–4 index.
	 */
	UPROPERTY(BlueprintReadOnly, Category="Depth", ReplicatedUsing=OnRep_Depth)
	FGameplayAttributeData Depth;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, Depth)

	/** Identity corruption meter. Clamped to 0–MaxCorruption (100 in the demo). */
	UPROPERTY(BlueprintReadOnly, Category="Corruption", ReplicatedUsing=OnRep_Corruption)
	FGameplayAttributeData Corruption;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, Corruption)

	UPROPERTY(BlueprintReadOnly, Category="Corruption", ReplicatedUsing=OnRep_MaxCorruption)
	FGameplayAttributeData MaxCorruption;
	AETERNII_ATTRIBUTE_ACCESSORS(UAeterniiAttributeSet, MaxCorruption)

	UPROPERTY(BlueprintAssignable, Category="Combat")
	FOnAeterniiHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Corruption")
	FOnAeterniiCorruptionChanged OnCorruptionChanged;

	UPROPERTY(BlueprintAssignable, Category="Depth")
	FOnAeterniiDepthChanged OnDepthChanged;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** Corruption 0, Depth 0 (Lux Stratum), MaxCorruption 100. Does not touch the seven combat fields. */
	UFUNCTION(BlueprintCallable, Category="Aeternii")
	void InitializeDepthBaseline();

	/**
	 *  hurtPlayer, before the HP write:
	 *  amount * stratum.dmg * (1 - armor), then optional damage reduction,
	 *  then if corruption > 50 multiply by 1 + (corruption - 50) / 250.
	 *  DamageReduction is the demo's buffs.dr.amt (0 when the buff is absent).
	 */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	static float ComputeIncomingHarm(float Amount, float DepthDamageMultiplier, float Armor, float Corruption, float DamageReduction);

	/** Same formula using this set's Depth, Armor, and Corruption. Does not modify Health. */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	float ComputeIncomingHarmFromAttributes(float Amount, float DamageReduction) const;

	/**
	 *  addCorr: if bPurge, amount is scaled by 0.15 first.
	 *  Result is clamp(current + amount * corruptionRate, 0, 100).
	 */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	static float AddCorruption(float CurrentCorruption, float Amount, float InCorruptionRate, bool bPurge);

	/**
	 *  One update-loop step of passive strata corruption, then purge decay.
	 *  If PassiveCorruption > 0, applies AddCorruption(PassiveCorruption * dt * 0.62).
	 *  If bPurge, then subtracts 6 * dt. Not called from character Tick.
	 */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	static float AdvancePassiveCorruption(float CurrentCorruption, float PassiveCorruption, float DeltaSeconds, float InCorruptionRate, bool bPurge);

	/** Writes Corruption from AdvancePassiveCorruption using this set's Depth and CorruptionRate. */
	UFUNCTION(BlueprintCallable, Category="Aeternii|Corruption")
	float AdvanceCorruptionMeter(float DeltaSeconds, bool bPurge);

	/** If corruption > 60, returns 1 - (corruption - 60) / 400. Otherwise 1. */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	static float CorruptionSpeedScale(float Corruption);

	/** If DepthIndex > 0, subtracts 1 corruption (unmake refund). Otherwise unchanged. */
	UFUNCTION(BlueprintPure, Category="Aeternii|Corruption")
	static float CorruptionAfterUnmake(float CurrentCorruption, int32 DepthIndex);

	/**
	 *  Isometric depth scan. Thresholds are 0, 0.20, 0.46, 0.74.
	 *  Returns 0 when progress is below the first threshold.
	 */
	UFUNCTION(BlueprintPure, Category="Aeternii|Depth")
	static int32 DepthIndexFromProgress(float Progress);

	/** False when DepthIndex is outside 0–3. */
	UFUNCTION(BlueprintPure, Category="Aeternii|Depth")
	static bool GetDepthStratum(int32 DepthIndex, FAeterniiDepthStratum& OutStratum);

	UFUNCTION(BlueprintPure, Category="Aeternii|Depth")
	int32 GetDepthIndex() const;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackCooldown(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CorruptionRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Depth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Corruption(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxCorruption(const FGameplayAttributeData& OldValue);

	void ClampHealth();
	void ClampCorruption();
	void BroadcastHealth();
	void BroadcastCorruption();
	void BroadcastDepth();
};
