// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CombatBossTypes.h"
#include "AnimNotifyState_DamageWindow.generated.h"

/**
 *  Span the Active section only. Begin opens the hitbox, End closes it.
 *  Sweep/Closer: Hitbox_Melee. Slam: Hitbox_AOE.
 */
UCLASS(DisplayName = "ANS_DamageWindow")
class UAnimNotifyState_DamageWindow : public UAnimNotifyState
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Hitbox")
	EBossHitbox Hitbox = EBossHitbox::Melee;

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
