#include "AI/CStateTree_Task_RandomPattern.h"
#include "GAS/CGameplayTags.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

const UStruct* FCStateTree_Task_RandomPattern::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FCStateTree_Task_RandomPattern::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstanceData.AIController->GetPawn());
	if (!ASC) return EStateTreeRunStatus::Failed;
	InstanceData.AbilitySystemComponent = ASC;

	// 쿨타임, 차단 태그 등을 통과한 이 페이즈의 패턴만 후보
	TArray<FGameplayAbilitySpecHandle> Candidates;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		const UGameplayAbility* Ability = Spec.Ability;
		if (!Ability) continue;

		const FGameplayTagContainer& Tags = Ability->GetAssetTags();
		if (!Tags.HasTagExact(CTags::Ability_Boss_Pattern)) continue;
		if (InstanceData.PhaseTag.IsValid() && !Tags.HasTagExact(InstanceData.PhaseTag)) continue;
		if (!Ability->CanActivateAbility(Spec.Handle, ASC->AbilityActorInfo.Get())) continue;

		Candidates.Add(Spec.Handle);
	}

	// 전부 쿨타임이면 실패, 휴식 뒤 다시 고름
	if (Candidates.IsEmpty()) return EStateTreeRunStatus::Failed;

	const FGameplayAbilitySpecHandle Handle = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	if (!ASC->TryActivateAbility(Handle)) return EStateTreeRunStatus::Failed;

	InstanceData.ActiveHandle = Handle;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FCStateTree_Task_RandomPattern::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	const FGameplayAbilitySpec* Spec = InstanceData.AbilitySystemComponent->FindAbilitySpecFromHandle(InstanceData.ActiveHandle);
	return (Spec && Spec->IsActive()) ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded;
}

void FCStateTree_Task_RandomPattern::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 패턴 도중에 상태가 바뀌면 패턴도 취소
	if (InstanceData.AbilitySystemComponent && InstanceData.ActiveHandle.IsValid())
		InstanceData.AbilitySystemComponent->CancelAbilityHandle(InstanceData.ActiveHandle);

	InstanceData.ActiveHandle = FGameplayAbilitySpecHandle();
}