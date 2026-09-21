// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotify_TellStart.h"
#include "CombatBoss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_TellStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ACombatBoss* Boss = Cast<ACombatBoss>(MeshComp->GetOwner()))
	{
		Boss->NotifyTellStart();
	}
}

FString UAnimNotify_TellStart::GetNotifyName_Implementation() const
{
	return FString(TEXT("AN_TellStart"));
}
