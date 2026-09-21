// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TellStart.generated.h"

/**
 *  Place at Tell section start. Forwards to ACombatBoss::NotifyTellStart (VFX/audio stub).
 */
UCLASS(DisplayName = "AN_TellStart")
class UAnimNotify_TellStart : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
