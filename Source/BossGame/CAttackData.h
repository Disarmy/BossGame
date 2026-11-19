#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "CAttackData.generated.h"

// 공격 한 번의 데미지와 반응. DT_Attack의 행
USTRUCT(BlueprintType)
struct FCAttackData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 0.0f;

	// 맞은 대상의 반응 (비우면 데미지만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Data.HitReact"))
	FGameplayTag HitReactTag;
};