// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotify_AttackEnd.h"
#include "CombatBoss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_AttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ACombatBoss* Boss = Cast<ACombatBoss>(MeshComp->GetOwner()))
	{
		Boss->NotifyAttackEnd();
	}
}

FString UAnimNotify_AttackEnd::GetNotifyName_Implementation() const
{
	return FString(TEXT("AN_AttackEnd"));
}
