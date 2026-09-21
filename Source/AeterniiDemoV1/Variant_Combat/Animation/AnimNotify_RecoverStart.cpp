// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotify_RecoverStart.h"
#include "CombatBoss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_RecoverStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ACombatBoss* Boss = Cast<ACombatBoss>(MeshComp->GetOwner()))
	{
		Boss->NotifyRecoverStart();
	}
}

FString UAnimNotify_RecoverStart::GetNotifyName_Implementation() const
{
	return FString(TEXT("AN_RecoverStart"));
}
