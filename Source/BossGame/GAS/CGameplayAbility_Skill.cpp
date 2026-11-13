#include "GAS/CGameplayAbility_Skill.h"
#include "GAS/CGameplayTags.h"
#include "CUnit.h"
#include "CAoE.h"
#include "CAimIndicator.h"
#include "BossGame.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "DrawDebugHelpers.h"

UCGameplayAbility_Skill::UCGameplayAbility_Skill()
{
	bStopMovementOnActivate = true;
}

const FGameplayTagContainer* UCGameplayAbility_Skill::GetCooldownTags() const
{
	// GAS가 발동 전에 이 태그가 붙어 있는지로 쿨타임을 판정
	return &CooldownTags;
}

void UCGameplayAbility_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	if (CooldownTags.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("%s: CooldownTags is empty, cooldown not applied"), *GetName());
		return;
	}

	// 공용 GE_Cooldown_Skill 하나에 이 스킬의 태그와 시간을 실어서 적용
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	Spec.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Cooldown, CooldownDuration);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
}

void UCGameplayAbility_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	bInSkillHit = false;
	SkillHitActors.Reset();

	// 몽타주 타격 프레임의 노티파이(Event.Skill.Hit)마다 범위 판정
	UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_Hit, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHit);
	WaitHit->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitHitBegin = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitBegin, nullptr, false, true);
	WaitHitBegin->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHitBegin);
	WaitHitBegin->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitHitEnd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitEnd, nullptr, false, true);
	WaitHitEnd->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHitEnd);
	WaitHitEnd->ReadyForActivation();

	if (bUseTargeting) {
		BeginTargeting();
	}
	else if (!StartCast()) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);   // 이동 정지 + BP 그래프
}

void UCGameplayAbility_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bTargeting) {
		EndTargeting();
	}

	ClearActionStateTags();

	if (!WarpTargetName.IsNone()) {
		ACUnit* Unit = GetUnit();
		if (UMotionWarpingComponent* Warp = Unit ? Unit->FindComponentByClass<UMotionWarpingComponent>() : nullptr)
			Warp->RemoveWarpTarget(WarpTargetName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FCHitArea UCGameplayAbility_Skill::MakeHitArea() const
{
	FCHitArea Area;
	Area.Radius = HitRadius;
	Area.InnerRadius = HitInnerRadius;
	Area.Angle = HitAngle;
	return Area;
}

FVector UCGameplayAbility_Skill::GetAimPoint() const
{
	const ACUnit* Unit = GetUnit();
	if (!Unit) return FVector::ZeroVector;

	const FVector Start = Unit->GetActorLocation();

	// 조준 실패 시 정면 최대 거리
	FVector Point = Start + Unit->GetActorForwardVector() * FieldMaxDistance;

	FVector AimLocation;
	if (GetAimLocation(AimLocation)) {
		FVector ToAim = AimLocation - Start;
		ToAim.Z = 0.0f;
		Point = Start + ToAim.GetClampedToMaxSize(FieldMaxDistance);
	}

	// 바닥과 겹치지 않게
	Point.Z = Start.Z - Unit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f;
	return Point;
}

bool UCGameplayAbility_Skill::StartCast()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		return false;

	FaceAim();
	SetWarpTarget();

	if (FieldClass)
		FieldLocation = GetAimPoint();

	return true;
}

void UCGameplayAbility_Skill::BeginTargeting()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ACUnit* Unit = GetUnit();
	if (!ASC || !Unit) return;

	bTargeting = true;
	ASC->AddLooseGameplayTag(CTags::State_Targeting);
	ASC->AddLooseGameplayTag(CTags::State_CanCancel);   // 조준 중에는 언제든 취소 가능

	if (IndicatorClass) {
		FActorSpawnParameters Params;
		Params.Owner = Unit;

		Indicator = Unit->GetWorld()->SpawnActor<ACAimIndicator>(IndicatorClass, GetAimPoint(), FRotator::ZeroRotator, Params);
		if (Indicator)
			Indicator->Init(this, HitRadius);
	}

	UAbilityTask_WaitGameplayEvent* WaitConfirm = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Target_Confirm, nullptr, true, true);
	WaitConfirm->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnTargetConfirm);
	WaitConfirm->ReadyForActivation();
}

void UCGameplayAbility_Skill::EndTargeting()
{
	bTargeting = false;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo()) {
		ASC->SetLooseGameplayTagCount(CTags::State_Targeting, 0);
		ASC->SetLooseGameplayTagCount(CTags::State_CanCancel, 0);
	}

	if (Indicator) {
		Indicator->Destroy();
		Indicator = nullptr;
	}
}

void UCGameplayAbility_Skill::SpawnField()
{
	ACUnit* Unit = GetUnit();
	if (!Unit) return;

	FActorSpawnParameters Params;
	Params.Owner = Unit;
	Params.Instigator = Unit;

	const FRotator Rotation(0.0f, Unit->GetActorRotation().Yaw, 0.0f);
	if (ACAoE* Field = Unit->GetWorld()->SpawnActor<ACAoE>(FieldClass, FieldLocation, Rotation, Params))
		Field->Activate(MakeHitArea(), FieldDuration, FieldInterval, MakeDamageSpec(DamageEffect, Damage, HitReactTag));
}

void UCGameplayAbility_Skill::SetWarpTarget()
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

void UCGameplayAbility_Skill::ApplySkillDamage(AActor* Target)
{
	ApplyDamageEffect(Target, DamageEffect, Damage, HitReactTag);
}

void UCGameplayAbility_Skill::OnSkillHit(FGameplayEventData EventData)
{
	if (FieldClass) {
		SpawnField();
		return;
	}

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

	// Debug
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

void UCGameplayAbility_Skill::OnSkillHitBegin(FGameplayEventData EventData)
{
	bInSkillHit = true;
	SkillHitActors.Reset();
}

void UCGameplayAbility_Skill::OnSkillHitEnd(FGameplayEventData EventData)
{
	bInSkillHit = false;
}

void UCGameplayAbility_Skill::OnTargetConfirm(FGameplayEventData EventData)
{

	if (!bTargeting) return;

	EndTargeting();

	if (!StartCast()) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageJumpToSection(CastSection);
}
