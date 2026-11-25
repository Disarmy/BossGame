#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CGameplayAbility_Pattern.generated.h"

UCLASS()
class BOSSGAME_API UCGameplayAbility_Pattern : public UCGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCGameplayAbility_Pattern();

	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// 패턴 이름 쿨타임 태그
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown", meta = (Categories = "Cooldown.Boss"))
	FGameplayTagContainer CooldownTags;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownDuration = 10.0f;
};
