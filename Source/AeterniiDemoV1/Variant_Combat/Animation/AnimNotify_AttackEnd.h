// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackEnd.generated.h"

/**
 *  Place at End section (or montage end). Closes hitboxes and clears attacking.
 *  Montage-complete on the GA also EndAbility / drops Ability.Attack.
 */
UCLASS(DisplayName = "AN_AttackEnd")
class UAnimNotify_AttackEnd : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
