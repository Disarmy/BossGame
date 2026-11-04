// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CGameplayAbility_Attack.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
class UGameplayEffect;

UCLASS()
class BOSSGAME_API UCGameplayAbility_Attack : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCGameplayAbility_Attack();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	UFUNCTION()
	void OnComboInput(FGameplayEventData EventData);

	UFUNCTION()
	void OnComboCheck(FGameplayEventData EventData);

	UFUNCTION()
	void OnMontageEnded();

	UFUNCTION()
	void OnAttackHit(FGameplayEventData EventData);

	void PlaySection(int32 Index);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 순서대로 재생할 섹션 (Attack1, Attack2, Attack3)
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FName> ComboSections;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// 콤보 단계별 데미지 (ComboSections와 같은 순서)
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TArray<float> ComboDamages;

private:
	// 실행 중 상태 (ActivateAbility에서 매번 초기화)
	int32 ComboIndex = 0;
	bool bComboInput = false;  // 판정 지점 전에 입력이 들어왔는지

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;
};
