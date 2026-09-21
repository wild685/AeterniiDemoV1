// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotifyState_DamageWindow.h"
#include "CombatBoss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_DamageWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (ACombatBoss* Boss = Cast<ACombatBoss>(MeshComp->GetOwner()))
	{
		Boss->OpenHitbox(Hitbox);
	}
}

void UAnimNotifyState_DamageWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ACombatBoss* Boss = Cast<ACombatBoss>(MeshComp->GetOwner()))
	{
		Boss->CloseHitbox(Hitbox);
	}
}

FString UAnimNotifyState_DamageWindow::GetNotifyName_Implementation() const
{
	return FString(TEXT("ANS_DamageWindow"));
}
