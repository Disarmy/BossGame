#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CAttackData.generated.h"

// 공격 한 번의 데미지와 반응. DT_Attack_Player의 행
USTRUCT(BlueprintType)
struct FCAttackData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.0f;

	// 맞은 대상의 반응 (비우면 데미지만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Data.HitReact"))
	FGameplayTag HitReactTag;

	// 공격자 중심에서 바깥으로 밀려나는 거리 cm
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float Knockback = 0.0f;

	// 밀려나는 데 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.05"))
	float KnockbackDuration = 0.3f;
};

// DT_Attack_Boss의 행
USTRUCT(BlueprintType)
struct FCBossAttackData : public FCAttackData
{
	GENERATED_BODY()

	// 가드 카운터 패링 태그 (비우면 데미지만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Data.Response"))
	FGameplayTag ResponseTag;
};