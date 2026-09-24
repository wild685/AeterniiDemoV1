// Copyright Epic Games, Inc. All Rights Reserved.

#include "AeterniiAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

namespace AeterniiDepthTable
{
	struct FRow
	{
		const TCHAR* Numeral;
		const TCHAR* Name;
		float Aggro;
		float SpeedMultiplier;
		float DamageMultiplier;
		float PassiveCorruption;
		int32 MaxAlive;
		float SpawnInterval;
	};

	// Aeternii_Isometric_Demo.html DEPTHS[] and DEPTH_AT.
	// Prototype STRATA accuracy drifts are a different system and are not in this table.
	static const FRow Rows[AeterniiCorruptionMeter::StratumCount] = {
		{ TEXT("I"), TEXT("Lux Stratum"), 6.5f, 1.00f, 1.00f, 0.00f, 4, 4.6f },
		{ TEXT("II"), TEXT("Velum Unda"), 8.0f, 1.12f, 1.10f, 0.55f, 5, 3.5f },
		{ TEXT("III"), TEXT("Abyssii Nihil"), 10.5f, 1.22f, 1.20f, 1.30f, 6, 2.7f },
		{ TEXT("IV"), TEXT("Ruptura Originii"), 99.0f, 1.34f, 1.32f, 2.40f, 8, 2.2f }
	};
}

UAeterniiAttributeSet::UAeterniiAttributeSet()
{
	InitMaxCorruption(AeterniiCorruptionMeter::MeterMax);
	InitCorruption(0.0f);
	InitDepth(0.0f);
	InitMaxHealth(0.0f);
	InitHealth(0.0f);
	InitArmor(0.0f);
	InitMoveSpeed(0.0f);
	InitDamage(0.0f);
	InitAttackCooldown(0.0f);
	InitAttackRange(0.0f);
	InitCorruptionRate(0.0f);
}

void UAeterniiAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, AttackCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, CorruptionRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, Depth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, Corruption, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAeterniiAttributeSet, MaxCorruption, COND_None, REPNOTIFY_Always);
}

void UAeterniiAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
	else if (Attribute == GetMoveSpeedAttribute() || Attribute == GetDamageAttribute()
		|| Attribute == GetAttackCooldownAttribute() || Attribute == GetAttackRangeAttribute()
		|| Attribute == GetCorruptionRateAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetDepthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, static_cast<float>(AeterniiCorruptionMeter::StratumCount - 1));
	}
	else if (Attribute == GetMaxCorruptionAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetCorruptionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxCorruption());
	}
}

void UAeterniiAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute() || Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		ClampHealth();
		BroadcastHealth();
	}
	else if (Data.EvaluatedData.Attribute == GetCorruptionAttribute() || Data.EvaluatedData.Attribute == GetMaxCorruptionAttribute())
	{
		ClampCorruption();
		BroadcastCorruption();
	}
	else if (Data.EvaluatedData.Attribute == GetDepthAttribute())
	{
		SetDepth(FMath::Clamp(GetDepth(), 0.0f, static_cast<float>(AeterniiCorruptionMeter::StratumCount - 1)));
		BroadcastDepth();
	}
	else if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), 0.0f, 1.0f));
	}
}

void UAeterniiAttributeSet::InitializeDepthBaseline()
{
	InitMaxCorruption(AeterniiCorruptionMeter::MeterMax);
	InitCorruption(0.0f);
	InitDepth(0.0f);
}

float UAeterniiAttributeSet::ComputeIncomingHarm(float Amount, float DepthDamageMultiplier, float Armor, float Corruption, float DamageReduction)
{
	float Harm = Amount * DepthDamageMultiplier * (1.0f - Armor);
	Harm *= (1.0f - DamageReduction);
	if (Corruption > AeterniiCorruptionMeter::ThinSelfThreshold)
	{
		Harm *= 1.0f + (Corruption - AeterniiCorruptionMeter::ThinSelfThreshold) / AeterniiCorruptionMeter::ThinSelfDivisor;
	}
	return Harm;
}

float UAeterniiAttributeSet::ComputeIncomingHarmFromAttributes(float Amount, float DamageReduction) const
{
	FAeterniiDepthStratum Stratum;
	const int32 DepthIndex = GetDepthIndex();
	const float DepthDamage = GetDepthStratum(DepthIndex, Stratum) ? Stratum.DamageMultiplier : 1.0f;
	return ComputeIncomingHarm(Amount, DepthDamage, GetArmor(), GetCorruption(), DamageReduction);
}

float UAeterniiAttributeSet::AddCorruption(float CurrentCorruption, float Amount, float InCorruptionRate, bool bPurge)
{
	float ScaledAmount = Amount;
	if (bPurge)
	{
		ScaledAmount *= AeterniiCorruptionMeter::PurgeIncomingScale;
	}

	return FMath::Clamp(CurrentCorruption + ScaledAmount * InCorruptionRate, 0.0f, AeterniiCorruptionMeter::MeterMax);
}

float UAeterniiAttributeSet::AdvancePassiveCorruption(float CurrentCorruption, float PassiveCorruption, float DeltaSeconds, float InCorruptionRate, bool bPurge)
{
	float Next = CurrentCorruption;
	if (PassiveCorruption > 0.0f)
	{
		Next = AddCorruption(Next, PassiveCorruption * DeltaSeconds * AeterniiCorruptionMeter::PassiveCorruptionTimeScale, InCorruptionRate, bPurge);
	}
	if (bPurge)
	{
		Next = FMath::Max(0.0f, Next - AeterniiCorruptionMeter::PurgeDecayPerSecond * DeltaSeconds);
	}
	return FMath::Clamp(Next, 0.0f, AeterniiCorruptionMeter::MeterMax);
}

float UAeterniiAttributeSet::AdvanceCorruptionMeter(float DeltaSeconds, bool bPurge)
{
	FAeterniiDepthStratum Stratum;
	const float Passive = GetDepthStratum(GetDepthIndex(), Stratum) ? Stratum.PassiveCorruption : 0.0f;
	const float Next = AdvancePassiveCorruption(GetCorruption(), Passive, DeltaSeconds, GetCorruptionRate(), bPurge);
	SetCorruption(Next);
	BroadcastCorruption();
	return Next;
}

float UAeterniiAttributeSet::CorruptionSpeedScale(float Corruption)
{
	if (Corruption > AeterniiCorruptionMeter::SpeedPenaltyThreshold)
	{
		return 1.0f - (Corruption - AeterniiCorruptionMeter::SpeedPenaltyThreshold) / AeterniiCorruptionMeter::SpeedPenaltyDivisor;
	}
	return 1.0f;
}

float UAeterniiAttributeSet::CorruptionAfterUnmake(float CurrentCorruption, int32 DepthIndex)
{
	if (DepthIndex > 0)
	{
		return FMath::Max(0.0f, CurrentCorruption - AeterniiCorruptionMeter::UnmakeCorruptionRefund);
	}
	return CurrentCorruption;
}

int32 UAeterniiAttributeSet::DepthIndexFromProgress(float Progress)
{
	int32 Index = 0;
	for (int32 i = AeterniiCorruptionMeter::StratumCount - 1; i >= 0; --i)
	{
		if (Progress >= AeterniiCorruptionMeter::ProgressThresholds[i])
		{
			Index = i;
			break;
		}
	}
	return Index;
}

bool UAeterniiAttributeSet::GetDepthStratum(int32 DepthIndex, FAeterniiDepthStratum& OutStratum)
{
	if (DepthIndex < 0 || DepthIndex >= AeterniiCorruptionMeter::StratumCount)
	{
		return false;
	}

	const AeterniiDepthTable::FRow& Row = AeterniiDepthTable::Rows[DepthIndex];
	OutStratum.Index = DepthIndex;
	OutStratum.Numeral = Row.Numeral;
	OutStratum.Name = Row.Name;
	OutStratum.Aggro = Row.Aggro;
	OutStratum.SpeedMultiplier = Row.SpeedMultiplier;
	OutStratum.DamageMultiplier = Row.DamageMultiplier;
	OutStratum.PassiveCorruption = Row.PassiveCorruption;
	OutStratum.MaxAlive = Row.MaxAlive;
	OutStratum.SpawnInterval = Row.SpawnInterval;
	return true;
}

int32 UAeterniiAttributeSet::GetDepthIndex() const
{
	return FMath::Clamp(FMath::RoundToInt(GetDepth()), 0, AeterniiCorruptionMeter::StratumCount - 1);
}

void UAeterniiAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, Health, OldValue);
	BroadcastHealth();
}

void UAeterniiAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, MaxHealth, OldValue);
	BroadcastHealth();
}

void UAeterniiAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, Armor, OldValue);
}

void UAeterniiAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, MoveSpeed, OldValue);
}

void UAeterniiAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, Damage, OldValue);
}

void UAeterniiAttributeSet::OnRep_AttackCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, AttackCooldown, OldValue);
}

void UAeterniiAttributeSet::OnRep_AttackRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, AttackRange, OldValue);
}

void UAeterniiAttributeSet::OnRep_CorruptionRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, CorruptionRate, OldValue);
}

void UAeterniiAttributeSet::OnRep_Depth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, Depth, OldValue);
	BroadcastDepth();
}

void UAeterniiAttributeSet::OnRep_Corruption(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, Corruption, OldValue);
	BroadcastCorruption();
}

void UAeterniiAttributeSet::OnRep_MaxCorruption(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAeterniiAttributeSet, MaxCorruption, OldValue);
	BroadcastCorruption();
}

void UAeterniiAttributeSet::ClampHealth()
{
	SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
}

void UAeterniiAttributeSet::ClampCorruption()
{
	const float Cap = GetMaxCorruption() > 0.0f ? GetMaxCorruption() : AeterniiCorruptionMeter::MeterMax;
	SetCorruption(FMath::Clamp(GetCorruption(), 0.0f, Cap));
}

void UAeterniiAttributeSet::BroadcastHealth()
{
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}

void UAeterniiAttributeSet::BroadcastCorruption()
{
	OnCorruptionChanged.Broadcast(GetCorruption(), GetMaxCorruption());
}

void UAeterniiAttributeSet::BroadcastDepth()
{
	OnDepthChanged.Broadcast(GetDepth());
}
