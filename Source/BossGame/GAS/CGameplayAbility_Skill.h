#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CGameplayAbility_Skill.generated.h"

class UGameplayEffect;

// 스킬 공통 부모: 스킬 이름 기준 쿨타임, 캐릭터 기준 범위 판정
UCLASS()
class BOSSGAME_API UCGameplayAbility_Skill : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UCGameplayAbility_Skill();

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	// 대상이 판정 범위(안쪽~바깥 반지름, 각도) 안에 있는지. 대상 캡슐 반지름만큼 여유를 줌
	bool IsInHitArea(const FVector& Center, const FVector& Forward, const FVector& Point, float TargetRadius) const;

protected:
	// 장판, 투사체처럼 범위 판정 밖에서 맞힌 대상에게 이 스킬의 데미지를 줄 때
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplySkillDamage(AActor* Target);

private:
	UFUNCTION()
	void OnSkillHit(FGameplayEventData EventData);

	UFUNCTION()
	void OnSkillHitBegin(FGameplayEventData EventData);

	UFUNCTION()
	void OnSkillHitEnd(FGameplayEventData EventData);

protected:
	// 스킬 이름 쿨타임 태그 (예: Cooldown.Skill.SlideAttack). 슬롯과 무관
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (Categories = "Cooldown.Skill"))
	FGameplayTagContainer CooldownTags;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 0.0f;

	// 맞은 대상의 반응 (비우면 데미지만)
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (Categories = "Data.HitReact"))
	FGameplayTag HitReactTag;

	// 판정 범위: 캐릭터 앞쪽으로 Offset만큼 옮긴 지점 기준
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitRadius = 200.0f;

	// 0이면 원, 크면 도넛 (안쪽은 안 맞음)
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitInnerRadius = 0.0f;

	// 360이면 원, 작으면 정면 부채꼴
	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (ClampMin = "0", ClampMax = "360"))
	float HitAngle = 360.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitForwardOffset = 0.0f;

private:
	bool bInSkillHit = false;
	TSet<TWeakObjectPtr<AActor>> SkillHitActors;
};