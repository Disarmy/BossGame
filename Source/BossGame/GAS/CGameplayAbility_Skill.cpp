// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CGameplayAbility_Skill.h"
#include "GAS/CGameplayTags.h"
#include "CUnit.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Engine/OverlapResult.h"
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
	if (!CooldownGE || CooldownTags.IsEmpty()) return;

	// 공용 GE_Cooldown_Skill 하나에 이 스킬의 태그와 시간을 실어서 적용
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	Spec.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Cooldown, CooldownDuration);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
}

void UCGameplayAbility_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 쿨타임 시작 (실패하면 발동 취소)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FaceAim();

	// 몽타주 타격 프레임의 노티파이(Event.Skill.Hit)마다 범위 판정
	UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_Hit, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHit);
	WaitHit->ReadyForActivation();

	bInSkillHit = false;
	SkillHitActors.Reset();

	UAbilityTask_WaitGameplayEvent* WaitHitBegin = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitBegin, nullptr, false, true);
	WaitHitBegin->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHitBegin);
	WaitHitBegin->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitHitEnd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_HitEnd, nullptr, false, true);
	WaitHitEnd->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHitEnd);
	WaitHitEnd->ReadyForActivation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);   // 이동 정지 + BP 그래프
}

void UCGameplayAbility_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 몽타주가 중간에 끊겨도 캔슬 구간 태그가 남지 않게
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		ASC->SetLooseGameplayTagCount(CTags::State_CanCancel, 0);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCGameplayAbility_Skill::IsInHitArea(const FVector& Center, const FVector& Forward, const FVector& Point, float TargetRadius) const
{
	FVector ToPoint = Point - Center;
	ToPoint.Z = 0.0f;
	const float Dist = ToPoint.Size();

	// 거리: 대상 캡슐이 바깥 원에 걸치면 맞음, 안쪽 원에 완전히 들어가 있으면 안 맞음
	if (Dist - TargetRadius > HitRadius) return false;
	if (Dist + TargetRadius < HitInnerRadius) return false;

	// 각도: 360이면 원, 대상이 중심에 겹쳐 있으면 맞음
	if (HitAngle >= 360.0f || Dist <= KINDA_SMALL_NUMBER) return true;

	const float CosHalf = FMath::Cos(FMath::DegreesToRadians(HitAngle * 0.5f));
	return FVector::DotProduct(Forward, ToPoint / Dist) >= CosHalf;
}

void UCGameplayAbility_Skill::ApplySkillDamage(AActor* Target)
{
	ApplyDamageEffect(Target, DamageEffect, Damage, HitReactTag);
}

void UCGameplayAbility_Skill::OnSkillHit(FGameplayEventData EventData)
{
	ACUnit* Unit = GetUnit();
	if (!Unit) return;

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

		if (!IsInHitArea(Center, Forward, Target->GetActorLocation(), Target->GetSimpleCollisionRadius()))
			continue;

		HitActors.Add(Target);
		if (bInSkillHit)
			SkillHitActors.Add(Target);

		ApplySkillDamage(Target);
	}

	// Debug
#if ENABLE_DRAW_DEBUG
	if (bDrawDebugHit) {
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
