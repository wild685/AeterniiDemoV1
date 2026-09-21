// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatBossStateTreeUtility.h"
#include "AIController.h"
#include "CombatBoss.h"
#include "CombatGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"

static ACombatBoss* CombatBossFromInstanceData(const FStateTreeBossCombatInstanceData& InstanceData)
{
	if (InstanceData.Boss)
	{
		return InstanceData.Boss;
	}

	if (InstanceData.Controller)
	{
		return Cast<ACombatBoss>(InstanceData.Controller->GetPawn());
	}

	return nullptr;
}

static ACombatBoss* CombatBossFromControllerAndPawn(AAIController* Controller, ACombatBoss* Boss)
{
	if (Boss)
	{
		return Boss;
	}

	if (Controller)
	{
		return Cast<ACombatBoss>(Controller->GetPawn());
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FStateTreeAcquireBossTargetTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACombatBoss* Boss = CombatBossFromInstanceData(InstanceData);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Boss, 0);
	if (!PlayerPawn)
	{
		Boss->SetAcquiredTarget(nullptr);
		return EStateTreeRunStatus::Failed;
	}

	Boss->SetAcquiredTarget(PlayerPawn);
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FStateTreeAcquireBossTargetTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Acquire Boss Target</b>"));
}
#endif

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FStateTreeChooseBossAttackTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACombatBoss* Boss = CombatBossFromInstanceData(InstanceData);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}

	const float Dist2D = Boss->GetAcquiredTargetDist2D();
	if (Dist2D < 0.0f)
	{
		Boss->SetNextAttack(EBossAttack::None);
		return EStateTreeRunStatus::Failed;
	}

	const EBossAttack Chosen = ACombatBoss::ChooseAttack(Dist2D, Boss->GetLastAttack());
	Boss->SetNextAttack(Chosen);

	if (Chosen == EBossAttack::None)
	{
		UE_LOG(LogCombatGAS, Verbose, TEXT("%s ChooseAttack Dist2D=%.1f -> None (MoveCloser)."), *Boss->GetName(), Dist2D);
		return EStateTreeRunStatus::Failed;
	}

	Boss->SetLastAttack(Chosen);
	UE_LOG(LogCombatGAS, Log, TEXT("%s ChooseAttack Dist2D=%.1f -> %s"), *Boss->GetName(), Dist2D, *UEnum::GetValueAsString(Chosen));
	return EStateTreeRunStatus::Succeeded;
}

#if WITH_EDITOR
FText FStateTreeChooseBossAttackTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Choose Boss Attack</b>"));
}
#endif

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FStateTreeTryActivateBossGATask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACombatBoss* Boss = CombatBossFromInstanceData(InstanceData);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (AActor* Target = Boss->GetAcquiredTarget())
	{
		if (AAIController* Controller = InstanceData.Controller ? InstanceData.Controller.Get() : Cast<AAIController>(Boss->GetController()))
		{
			Controller->SetFocus(Target);
		}
	}

	const bool bActivated = Boss->TryActivateBossAttack(Boss->GetNextAttack());
	return bActivated ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText FStateTreeTryActivateBossGATask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Try Activate Boss GA</b>"));
}
#endif

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FStateTreeWaitWhileAttackingTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedSeconds = 0.0f;

	ACombatBoss* Boss = CombatBossFromControllerAndPawn(InstanceData.Controller, InstanceData.Boss);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!Boss->IsAbilityAttackActive())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeWaitWhileAttackingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedSeconds += DeltaTime;

	ACombatBoss* Boss = CombatBossFromControllerAndPawn(InstanceData.Controller, InstanceData.Boss);
	if (!Boss)
	{
		return EStateTreeRunStatus::Failed;
	}

	if (!Boss->IsAbilityAttackActive())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	if (InstanceData.ElapsedSeconds >= InstanceData.TimeoutSeconds)
	{
		UE_LOG(LogCombatGAS, Warning, TEXT("%s WaitWhileAttacking timed out after %.1fs."), *Boss->GetName(), InstanceData.TimeoutSeconds);
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeWaitWhileAttackingTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Wait While Attacking</b>"));
}
#endif

////////////////////////////////////////////////////////////////////

EStateTreeRunStatus FStateTreeMoveCloserTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedSeconds = 0.0f;

	ACombatBoss* Boss = CombatBossFromControllerAndPawn(InstanceData.Controller, InstanceData.Boss);
	if (!Boss || !Boss->GetAcquiredTarget())
	{
		return EStateTreeRunStatus::Failed;
	}

	if (AAIController* Controller = InstanceData.Controller ? InstanceData.Controller.Get() : Cast<AAIController>(Boss->GetController()))
	{
		Controller->SetFocus(Boss->GetAcquiredTarget());
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FStateTreeMoveCloserTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedSeconds += DeltaTime;

	ACombatBoss* Boss = CombatBossFromControllerAndPawn(InstanceData.Controller, InstanceData.Boss);
	AActor* Target = Boss ? Boss->GetAcquiredTarget() : nullptr;
	if (!Boss || !Target)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector Direction = (Target->GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal2D();
	if (!Direction.IsNearlyZero())
	{
		Boss->AddMovementInput(Direction, 1.0f);
	}

	if (InstanceData.ElapsedSeconds >= InstanceData.Duration)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FStateTreeMoveCloserTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (AAIController* Controller = InstanceData.Controller)
	{
		Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FStateTreeMoveCloserTask::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Move Closer</b>"));
}
#endif

////////////////////////////////////////////////////////////////////

bool FStateTreeBossHasQueuedAttackCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	ACombatBoss* Boss = CombatBossFromControllerAndPawn(InstanceData.Controller, InstanceData.Boss);
	const bool bHasAttack = Boss && Boss->GetNextAttack() != EBossAttack::None;
	return InstanceData.bMustHaveAttack ? bHasAttack : !bHasAttack;
}

#if WITH_EDITOR
FText FStateTreeBossHasQueuedAttackCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>Boss Has Queued Attack</b>"));
}
#endif
