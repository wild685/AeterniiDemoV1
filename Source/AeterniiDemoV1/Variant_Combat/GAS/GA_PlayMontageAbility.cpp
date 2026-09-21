// Copyright Epic Games, Inc. All Rights Reserved.

#include "GA_PlayMontageAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "CombatGameplayTags.h"

UGA_PlayMontageAbility::UGA_PlayMontageAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UGA_PlayMontageAbility::ActivateAbility(
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

	if (!MontageToPlay)
	{
		UE_LOG(LogCombatGAS, Warning, TEXT("%s has no montage assigned. Ending ability so AI can continue. Assign AM_* in the editor (see Docs/CombatP1.md)."), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MontageToPlay,
		1.0f,
		StartSection,
		true);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// End only on completed / interrupt / cancel so Ability.Attack lasts the full montage
	// (including Recover). Do not bind OnBlendOut.
	MontageTask->OnCompleted.AddDynamic(this, &UGA_PlayMontageAbility::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_PlayMontageAbility::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_PlayMontageAbility::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

void UGA_PlayMontageAbility::OnMontageCompleted()
{
	FinishAbility(false);
}

void UGA_PlayMontageAbility::OnMontageInterrupted()
{
	FinishAbility(true);
}

void UGA_PlayMontageAbility::FinishAbility(bool bWasCancelled)
{
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
	}
}
