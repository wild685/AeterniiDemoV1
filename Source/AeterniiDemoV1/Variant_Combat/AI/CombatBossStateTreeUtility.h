// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatBossTypes.h"
#include "StateTreeConditionBase.h"
#include "StateTreeTaskBase.h"
#include "CombatBossStateTreeUtility.generated.h"

class AAIController;
class ACombatBoss;

/**
 *  Shared instance data: bind Controller (AI schema) and/or Boss pawn.
 *  Tasks resolve Boss from Controller->GetPawn() when the Boss binding is empty.
 */
USTRUCT()
struct FStateTreeBossCombatInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> Controller;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<ACombatBoss> Boss;
};

////////////////////////////////////////////////////////////////////

/**
 *  Idle_AcquireTarget: resolve the local player pawn onto the boss.
 *  Succeeds when a pawn is found; fails so the tree can retry.
 */
USTRUCT(meta = (DisplayName = "Acquire Boss Target", Category = "Combat|Boss"))
struct FStateTreeAcquireBossTargetTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FStateTreeAcquireBossTargetTask()
	{
		bShouldCallTick = false;
		bShouldStateChangeOnReselect = false;
	}

	using FInstanceDataType = FStateTreeBossCombatInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

////////////////////////////////////////////////////////////////////

/**
 *  STT_ChooseBossAttack
 *  Weighted pick from Dist2D + last attack. Succeeds with a real attack,
 *  fails with None so the tree can MoveCloser.
 */
USTRUCT(meta = (DisplayName = "Choose Boss Attack", Category = "Combat|Boss"))
struct FStateTreeChooseBossAttackTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FStateTreeChooseBossAttackTask()
	{
		bShouldCallTick = false;
		bShouldStateChangeOnReselect = false;
	}

	using FInstanceDataType = FStateTreeBossCombatInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

////////////////////////////////////////////////////////////////////

/**
 *  STT_TryActivateBossGA
 *  Maps NextAttack → GA class and TryActivateAbility. Never PlayMontage.
 */
USTRUCT(meta = (DisplayName = "Try Activate Boss GA", Category = "Combat|Boss"))
struct FStateTreeTryActivateBossGATask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FStateTreeTryActivateBossGATask()
	{
		bShouldCallTick = false;
		bShouldStateChangeOnReselect = false;
	}

	using FInstanceDataType = FStateTreeBossCombatInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct FStateTreeWaitWhileAttackingInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> Controller;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<ACombatBoss> Boss;

	/** Safety timeout so a missing montage / missing EndAbility cannot stall the tree. */
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = 0.1, Units = "s"))
	float TimeoutSeconds = 5.0f;

	UPROPERTY()
	float ElapsedSeconds = 0.0f;
};

/**
 *  STT_WaitWhileAttacking
 *  Ticks until Ability.Attack is cleared, or TimeoutSeconds (default 5s).
 */
USTRUCT(meta = (DisplayName = "Wait While Attacking", Category = "Combat|Boss"))
struct FStateTreeWaitWhileAttackingTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FStateTreeWaitWhileAttackingTask()
	{
		bShouldCallTick = true;
		bShouldStateChangeOnReselect = false;
	}

	using FInstanceDataType = FStateTreeWaitWhileAttackingInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct FStateTreeMoveCloserInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> Controller;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<ACombatBoss> Boss;

	UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = 0.05, Units = "s"))
	float Duration = 0.75f;

	UPROPERTY()
	float ElapsedSeconds = 0.0f;
};

/**
 *  MoveCloser (~0.75s) when ChooseAttack yields None.
 */
USTRUCT(meta = (DisplayName = "Move Closer", Category = "Combat|Boss"))
struct FStateTreeMoveCloserTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FStateTreeMoveCloserTask()
	{
		bShouldCallTick = true;
		bShouldStateChangeOnReselect = false;
	}

	using FInstanceDataType = FStateTreeMoveCloserInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};

////////////////////////////////////////////////////////////////////

USTRUCT()
struct FStateTreeBossHasQueuedAttackInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AAIController> Controller;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<ACombatBoss> Boss;

	/** If true, passes when NextAttack != None. If false, passes when NextAttack == None. */
	UPROPERTY(EditAnywhere, Category = Condition)
	bool bMustHaveAttack = true;
};

/**
 *  Optional transition condition if you prefer a condition over Choose succeed/fail.
 */
USTRUCT(DisplayName = "Boss Has Queued Attack")
struct FStateTreeBossHasQueuedAttackCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeBossHasQueuedAttackInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
