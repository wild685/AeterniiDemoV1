// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotifyState_DodgeIFrames.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CombatGameplayTags.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_DodgeIFrames::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			ASC->AddLooseGameplayTag(TAG_Status_Dodge_IFrames);
		}
	}
}

void UAnimNotifyState_DodgeIFrames::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
		{
			ASC->RemoveLooseGameplayTag(TAG_Status_Dodge_IFrames);
		}
	}
}

FString UAnimNotifyState_DodgeIFrames::GetNotifyName_Implementation() const
{
	return FString(TEXT("ANS_DodgeIFrames"));
}
