#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "CStateTree_Task_FaceTarget.generated.h"

class AAIController;
class UAbilitySystemComponent;

USTRUCT()
struct FCStateTree_Task_FaceTargetInstanceData
{
	GENERATED_BODY()

	// Context 카테고리는 스키마의 AIController에 자동 바인딩
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController;

	UPROPERTY()
	TObjectPtr<AActor> Target;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};

USTRUCT(meta = (DisplayName = "Face Target", Category = "Boss"))
struct BOSSGAME_API FCStateTree_Task_FaceTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCStateTree_Task_FaceTargetInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
