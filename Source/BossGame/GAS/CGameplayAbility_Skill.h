#pragma once

#include "CoreMinimal.h"
#include "CHitArea.h"
#include "GAS/CGameplayAbility.h"
#include "CGameplayAbility_Skill.generated.h"

class UGameplayEffect;
class ACAoE;
class ACAimIndicator;

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

	virtual bool BeginSkill();
	virtual bool StartCast();
	// Event.Skill.Hit 발동 하면
	virtual void OnSkillHit() {}

	// GA가 끝난 뒤에도 데미지를 주는 것(장판, 투사체)에 넘길 Spec
	FGameplayEffectSpecHandle MakeSkillDamageSpec() const;

protected:
	// 장판, 투사체처럼 범위 판정 밖에서 맞힌 대상에게 이 스킬의 데미지를 줄 때
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ApplySkillDamage(AActor* Target);

private:
	UFUNCTION()
	void OnSkillHitEvent(FGameplayEventData EventData);

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
};