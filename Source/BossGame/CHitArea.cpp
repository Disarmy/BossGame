#include "CHitArea.h"

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
