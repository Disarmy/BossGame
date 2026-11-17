#include "CHitArea.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

bool FCHitArea::Contains(const FVector& Center, const FVector& Forward, const FVector& Point, float TargetRadius) const
{
	FVector ToPoint = Point - Center;
	ToPoint.Z = 0.0f;
	const float Dist = ToPoint.Size();

	if (Dist - TargetRadius > Radius) return false;
	if (Dist + TargetRadius < InnerRadius) return false;
	if (Angle >= 360.0f || Dist <= KINDA_SMALL_NUMBER) return true;

	const float CosHalf = FMath::Cos(FMath::DegreesToRadians(Angle * 0.5f));
	return FVector::DotProduct(Forward, ToPoint / Dist) >= CosHalf;

}

void FCHitArea::GetTargets(const UWorld* World, const FVector& Center, const FVector& Forward, const AActor* Ignore, TArray<AActor*>& OutTargets) const
{
	if (!World) return;

	// 바깥 반지름 구로 후보를 모은 뒤 수학 검사
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HitArea), false, Ignore);
	World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(Radius), Params);

	for (const FOverlapResult& Overlap : Overlaps) {
		AActor* Target = Overlap.GetActor();
		if (!Target || OutTargets.Contains(Target)) continue;

		if (Contains(Center, Forward, Target->GetActorLocation(), Target->GetSimpleCollisionRadius()))
			OutTargets.Add(Target);
	}
}
