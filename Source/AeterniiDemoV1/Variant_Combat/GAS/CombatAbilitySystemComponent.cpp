// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CombatAttributeSet.h"
#include "CombatDamageable.h"
#include "CombatGameplayTags.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

UCombatAbilitySystemComponent::UCombatAbilitySystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCombatAbilitySystemComponent::GrantAbilityIfMissing(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!AbilityClass)
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetClass() == AbilityClass)
		{
			return;
		}
	}

	GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, GetOwnerActor()));
}

bool UCombatAbilitySystemComponent::ApplyMeleeDamageTo(AActor* Target, float Damage, TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!Target || Damage <= 0.0f)
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = nullptr;
	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Target))
	{
		TargetASC = ASI->GetAbilitySystemComponent();
	}

	if (TargetASC && TargetASC->HasMatchingGameplayTag(TAG_Status_Dodge_IFrames))
	{
		UE_LOG(LogCombatGAS, Verbose, TEXT("Melee damage skipped on %s (Status.Dodge.IFrames)."), *Target->GetName());
		return false;
	}

	// GE_Damage_Melee writes UCombatAttributeSet::Health (legacy boss mirror).
	// Player and horde pawns carry UAeterniiAttributeSet only, so pack hits stay on the interface.
	if (TargetASC && EffectClass && TargetASC->GetSet<UCombatAttributeSet>())
	{
		FGameplayEffectContextHandle Context = MakeEffectContext();
		Context.AddSourceObject(GetAvatarActor());

		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(TAG_Data_Damage, -FMath::Abs(Damage));
			ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			return true;
		}
	}

	if (ICombatDamageable* Damageable = Cast<ICombatDamageable>(Target))
	{
		const FVector Impulse = FVector::ZeroVector;
		Damageable->ApplyDamage(Damage, GetAvatarActor(), Target->GetActorLocation(), Impulse);
		return true;
	}

	return false;
}

bool UCombatAbilitySystemComponent::HasCombatTag(FGameplayTag Tag) const
{
	return HasMatchingGameplayTag(Tag);
}
