#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "CStateTree_Task_RandomPattern.generated.h"

class AAIController;
class UAbilitySystemComponent;

USTRUCT()
struct FCStateTree_Task_RandomPatternInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController;

	// 이 페이즈 태그가 에셋 태그에 있는 패턴만 후보
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Categories = "Ability.Boss.Phase"))
	FGameplayTag PhaseTag;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	FGameplayAbilitySpecHandle ActiveHandle;
};

USTRUCT(meta = (DisplayName = "Random Pattern", Category = "Boss"))
struct BOSSGAME_API FCStateTree_Task_RandomPattern : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCStateTree_Task_RandomPatternInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};