#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "CStateTree_Task_WaitFacing.generated.h"

class AAIController;

USTRUCT()
struct FCStateTree_Task_WaitFacingInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MinTime = 0.3f;

	// 플레이어가 회전 속도보다 빠르게 돌때를 위함
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float MaxTime = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float AcceptAngle = 15.0f;

	float ElapsedTime = 0.0f;
};

USTRUCT(meta = (DisplayName = "Wait Facing", Category = "Boss"))
struct BOSSGAME_API FCStateTree_Task_WaitFacing : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCStateTree_Task_WaitFacingInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};