#include "AI/CStateTree_Task_FaceTarget.h"
#include "GAS/CGameplayTags.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

const UStruct* FCStateTree_Task_FaceTarget::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FCStateTree_Task_FaceTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstanceData.AIController->GetPawn());

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FCStateTree_Task_FaceTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	AAIController* AIController = InstanceData.AIController;

	// 플레이어 한 명이라 Perception 없이 찾음. 시작 시점에 플레이어가 아직 없을 수 있어 Tick에서 찾음
	if (InstanceData.Target == nullptr)
		InstanceData.Target = UGameplayStatics::GetPlayerPawn(Context.GetWorld(), 0);

	const bool bLocked = InstanceData.AbilitySystemComponent && InstanceData.AbilitySystemComponent->HasMatchingGameplayTag(CTags::Lock_Rotate);

	if (bLocked)
	{
		// 포커스가 없으면 컨트롤 회전이 폰 방향을 따라가서 그 자리에서 멈춤
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
	else if (AIController->GetFocusActor() != InstanceData.Target)
	{
		AIController->SetFocus(InstanceData.Target);
	}

	return EStateTreeRunStatus::Running;
}

void FCStateTree_Task_FaceTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.AIController->ClearFocus(EAIFocusPriority::Gameplay);
}