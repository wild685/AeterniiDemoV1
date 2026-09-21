// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DodgeIFrames.generated.h"

/**
 *  Adds Status.Dodge.IFrames for the notify duration.
 *  Expected placement on a ~0.55s dodge montage: 0.05s → 0.35s (≈0.30s i-frames).
 */
UCLASS(DisplayName = "ANS_DodgeIFrames")
class UAnimNotifyState_DodgeIFrames : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
