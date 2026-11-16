#include "GAS/CGameplayAbility_Skill_Area.h"
#include "GAS/CGameplayTags.h"
#include "CUnit.h"
#include "BossGame.h"
#include "MotionWarpingComponent.h"
#include "Engine/OverlapResult.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "DrawDebugHelpers.h"


void UCGameplayAbility_Skill_Area::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!WarpTargetName.IsNone()) {
		ACUnit* Unit = GetUnit();
		if (UMotionWarpingComponent* Warp = Unit ? Unit->FindComponentByClass<UMotionWarpingComponent>() : nullptr)
			Warp->RemoveWarpTarget(WarpTargetName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCGameplayAbility_Skill_Area::BeginSkill()
{
	bInSkillHit = false;
	SkillHitActors.Reset();

	UAbilityTask_WaitGameplayEvent* WaitHitBegin = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitBegin, nullptr, false, true);
	WaitHitBegin->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill_Area::OnSkillHitBegin);
	WaitHitBegin->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitHitEnd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitEnd, nullptr, false, true);
	WaitHitEnd->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill_Area::OnSkillHitEnd);
	WaitHitEnd->ReadyForActivation();

	return Super::BeginSkill();
}

bool UCGameplayAbility_Skill_Area::StartCast()
{
	if (!Super::StartCast()) return false;

	SetWarpTarget();
	return true;
}

void UCGameplayAbility_Skill_Area::OnSkillHit()
{
	ACUnit* Unit = GetUnit();
	if (!Unit) return;

	const FCHitArea Area = MakeHitArea();

	UWorld* World = Unit->GetWorld();
	const FVector Forward = Unit->GetActorForwardVector().GetSafeNormal2D();
	const FVector Center = Unit->GetActorLocation() + Forward * HitForwardOffset;

	// 바깥 반지름 구로 후보를 모은 뒤 수학 검사 (자기 자신 제외)
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SkillHit), false, Unit);
	World->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(HitRadius), Params);

	TSet<AActor*> HitActors;   // 컴포넌트가 여러 개 걸려도 대상당 한 번
	for (const FOverlapResult& Overlap : Overlaps) {
		AActor* Target = Overlap.GetActor();
		if (!Target || HitActors.Contains(Target)) continue;
		if (bInSkillHit && SkillHitActors.Contains(Target)) continue;

		if (!Area.Contains(Center, Forward, Target->GetActorLocation(), Target->GetSimpleCollisionRadius()))
			continue;

		HitActors.Add(Target);
		if (bInSkillHit)
			SkillHitActors.Add(Target);

		ApplySkillDamage(Target);
	}

#if ENABLE_DRAW_DEBUG
	if (CDebug::bDrawHit) {
		const float LifeTime = bInSkillHit ? -1.0f : 1.0f;
		const FColor Color = HitActors.IsEmpty() ? FColor::Green : FColor::Red;
		DrawDebugCircle(World, Center, HitRadius, 32, Color, false, LifeTime, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
		if (HitInnerRadius > 0.0f)
			DrawDebugCircle(World, Center, HitInnerRadius, 32, Color, false, LifeTime, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
		if (HitAngle < 360.0f) {
			for (const float Sign : { -1.0f, 1.0f }) {
				const FVector Edge = Forward.RotateAngleAxis(Sign * HitAngle * 0.5f, FVector::UpVector);
				DrawDebugLine(World, Center, Center + Edge * HitRadius, Color, false, LifeTime, 0, 2.0f);
			}
		}
	}
#endif

}

FCHitArea UCGameplayAbility_Skill_Area::MakeHitArea() const
{
	FCHitArea Area;
	Area.Radius = HitRadius;
	Area.InnerRadius = HitInnerRadius;
	Area.Angle = HitAngle;

	return Area;
}

void UCGameplayAbility_Skill_Area::SetWarpTarget()
{
	if (WarpTargetName.IsNone()) return;

	ACUnit* Unit = GetUnit();
	UMotionWarpingComponent* Warp = Unit ? Unit->FindComponentByClass<UMotionWarpingComponent>() : nullptr;
	if (!Warp) return;

	const FVector Start = Unit->GetActorLocation();

	// 조준 실패 시 정면 최대 거리
	FVector Target = Start + Unit->GetActorForwardVector() * WarpMaxDistance;

	FVector AimLocation;
	if (GetAimLocation(AimLocation)) {
		FVector ToAim = AimLocation - Start;
		ToAim.Z = 0.0f;
		Target = Start + ToAim.GetClampedToMaxSize(WarpMaxDistance);
	}

	Warp->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, Target, Unit->GetActorRotation());
}

void UCGameplayAbility_Skill_Area::OnSkillHitBegin(FGameplayEventData EventData)
{
	bInSkillHit = true;
	SkillHitActors.Reset();
}

void UCGameplayAbility_Skill_Area::OnSkillHitEnd(FGameplayEventData EventData)
{
	bInSkillHit = false;
}
