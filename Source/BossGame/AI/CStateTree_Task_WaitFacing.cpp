#include "AI/CStateTree_Task_WaitFacing.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"

const UStruct* FCStateTree_Task_WaitFacing::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FCStateTree_Task_WaitFacing::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime = 0.0f;

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FCStateTree_Task_WaitFacing::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.ElapsedTime += DeltaTime;

	if (InstanceData.ElapsedTime < InstanceData.MinTime)
		return EStateTreeRunStatus::Running;

	if (InstanceData.ElapsedTime >= InstanceData.MaxTime)
		return EStateTreeRunStatus::Succeeded;

	// 대상은 Face Target이 걸어 둔 포커스를 그대로 씀
	const APawn* Pawn = InstanceData.AIController->GetPawn();
	const AActor* Target = InstanceData.AIController->GetFocusActor();
	if (!Pawn || !Target)
		return EStateTreeRunStatus::Running;

	FVector ToTarget = Target->GetActorLocation() - Pawn->GetActorLocation();
	ToTarget.Z = 0.0f;
	if (ToTarget.IsNearlyZero())
		return EStateTreeRunStatus::Succeeded;

	const float Dot = FVector::DotProduct(Pawn->GetActorForwardVector().GetSafeNormal2D(), ToTarget.GetSafeNormal());
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

	return Angle <= InstanceData.AcceptAngle ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}