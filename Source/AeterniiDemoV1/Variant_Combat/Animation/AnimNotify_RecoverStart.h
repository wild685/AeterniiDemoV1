// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_RecoverStart.generated.h"

/**
 *  Place at Recover section start. Forwards to ACombatBoss::NotifyRecoverStart.
 */
UCLASS(DisplayName = "AN_RecoverStart")
class UAnimNotify_RecoverStart : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
