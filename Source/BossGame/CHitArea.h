#pragma once

#include "CoreMinimal.h"
#include "CHitArea.generated.h"

// 판정 범위: 원, 도넛, 부채꼴. 스킬과 장판이 같이 씀
USTRUCT(BlueprintType)
struct FCHitArea
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 200.0f;

	// 0이면 원, 크면 도넛 (안쪽은 안 맞음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InnerRadius = 0.0f;

	// 360이면 원, 작으면 정면 부채꼴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "360"))
	float Angle = 360.0f;

	// 대상 캡슐 반지름만큼 여유를 줌
	bool Contains(const FVector& Center, const FVector& Forward, const FVector& Point, float TargetRadius) const;
};