// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility_Attack.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UCGameplayAbility_Attack::UCGameplayAbility_Attack()
{
	bStopMovementOnActivate = true;
}

void UCGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData * TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage || ComboSections.IsEmpty()) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ComboIndex = 0;
	bComboInput = false;
	MontageTask = nullptr;
	FaceAim();

	// 공격 중 들어오는 입력 (컨트롤러가 보낸 Input.Attack 이벤트)
	UAbilityTask_WaitGameplayEvent* WaitInput = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Input_Attack, nullptr, false, true);
	WaitInput->EventReceived.AddDynamic(this, &UCGameplayAbility_Attack::OnComboInput);
	WaitInput->ReadyForActivation();

	// 몽타주의 콤보 판정 지점 (노티파이가 보낸 Event.Combo.Check)
	UAbilityTask_WaitGameplayEvent* WaitCheck = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Combo_Check, nullptr, false, true);
	WaitCheck->EventReceived.AddDynamic(this, &UCGameplayAbility_Attack::OnComboCheck);
	WaitCheck->ReadyForActivation();

	// 무기 적중 (ANSC_WeaponTrace 구간 동안 무기가 보냄)
	UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Attack_Hit, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UCGameplayAbility_Attack::OnAttackHit);
	WaitHit->ReadyForActivation();

	// Attack1 재생
	PlaySection(0);
}

void UCGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 몽타주가 중간에 끊겨도 구간 태그가 남지 않게
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo()) {
		ASC->SetLooseGameplayTagCount(CTags::State_CanCombo, 0);
		ASC->SetLooseGameplayTagCount(CTags::State_CanCancel, 0);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCGameplayAbility_Attack::OnComboInput(FGameplayEventData EventData)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC && ASC->HasMatchingGameplayTag(CTags::State_CanCombo)) {
		bComboInput = true;
	}
}

void UCGameplayAbility_Attack::OnComboCheck(FGameplayEventData EventData)
{
	// 콤보가 아니거나 콤보 수가 가지고 있는 수 보다 많아지면
	if (!bComboInput || ComboIndex + 1 >= ComboSections.Num()) {
		return;
	}

	ComboIndex++;
	bComboInput = false;

	FaceAim();
	PlaySection(ComboIndex);
}

void UCGameplayAbility_Attack::OnMontageEnded()
{
	// 콤보 전환 시 이전 재생이 밀려나며 오는 신호는 무시 (새 재생이 이미 진행 중)
	if (UAnimInstance* AnimInstance = GetActorInfo().GetAnimInstance()) {
		if (AnimInstance->Montage_IsPlaying(AttackMontage))
			return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UCGameplayAbility_Attack::OnAttackHit(FGameplayEventData EventData)
{
	AActor* Target = const_cast<AActor*>(EventData.Target.Get());
	if (!Target || !DamageEffect)
		return;

	const float Damage = ComboDamages.IsValidIndex(ComboIndex) ? ComboDamages[ComboIndex] : 0.f;

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffect, GetAbilityLevel());
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Damage, Damage);
	if (HitReactTag.IsValid()) {
		Spec.Data->AddDynamicAssetTag(HitReactTag);
	}

	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec,
		UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Target));
}

void UCGameplayAbility_Attack::PlaySection(int32 Index)
{
	if (MontageTask) {
		MontageTask->OnCompleted.RemoveAll(this);
		MontageTask->OnInterrupted.RemoveAll(this);
		MontageTask->OnCancelled.RemoveAll(this);
		MontageTask->EndTask();
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, 1.f, ComboSections[Index]);
	MontageTask->OnCompleted.AddDynamic(this, &UCGameplayAbility_Attack::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UCGameplayAbility_Attack::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UCGameplayAbility_Attack::OnMontageEnded);
	MontageTask->ReadyForActivation();
}
