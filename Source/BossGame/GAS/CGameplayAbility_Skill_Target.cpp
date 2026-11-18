#include "GAS/CGameplayAbility_Skill_Target.h"
#include "GAS/CGameplayTags.h"
#include "CUnit.h"
#include "CAoE.h"
#include "CAimIndicator.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

FVector UCGameplayAbility_Skill_Target::GetAimPoint() const
{
	const ACUnit* Unit = GetUnit();
	if (!Unit) return FVector::ZeroVector;

	const FVector Start = Unit->GetActorLocation();

	// 조준 실패 시 정면 최대 거리
	FVector Point = Start + Unit->GetActorForwardVector() * TargetMaxDistance;

	FVector AimLocation;
	if (GetAimLocation(AimLocation)) {
		FVector ToAim = AimLocation - Start;
		ToAim.Z = 0.0f;
		Point = Start + ToAim.GetClampedToMaxSize(TargetMaxDistance);
	}

	// 바닥과 겹치지 않게
	Point.Z = Start.Z - Unit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 2.0f;
	return Point;
}

void UCGameplayAbility_Skill_Target::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (bTargeting)
		EndTargeting();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCGameplayAbility_Skill_Target::BeginSkill()
{
	// 쿨타임은 확정할 때 시작
	BeginTargeting();
	return true;
}

bool UCGameplayAbility_Skill_Target::StartCast()
{
	if (!Super::StartCast()) return false;

	TargetLocation = GetAimPoint();
	return true;
}

void UCGameplayAbility_Skill_Target::OnSkillHit()
{
	ACUnit* Unit = GetUnit();
	if (!Unit || !AoEClass) return;

	FActorSpawnParameters Params;
	Params.Owner = Unit;
	Params.Instigator = Unit;

	const FRotator Rotation(0.0f, Unit->GetActorRotation().Yaw, 0.0f);
	if (ACAoE* AoE = Unit->GetWorld()->SpawnActor<ACAoE>(AoEClass, TargetLocation, Rotation, Params))
		AoE->Activate(AoEArea, AoEDuration, AoEInterval, MakeSkillDamageSpec());
}

void UCGameplayAbility_Skill_Target::BeginTargeting()
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
			Indicator->Init(this, AoEArea.Radius);
	}

	UAbilityTask_WaitGameplayEvent* WaitConfirm = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Target_Confirm, nullptr, true, true);
	WaitConfirm->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill_Target::OnTargetConfirm);
	WaitConfirm->ReadyForActivation();
}

void UCGameplayAbility_Skill_Target::EndTargeting()
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

void UCGameplayAbility_Skill_Target::OnTargetConfirm(FGameplayEventData EventData)
{
	if (!bTargeting) return;

	EndTargeting();

	if (!StartCast()) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MontageJumpToSection(CastSection);
}
