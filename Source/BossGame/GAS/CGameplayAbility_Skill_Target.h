#pragma once

#include "CoreMinimal.h"
#include "CHitArea.h"
#include "GAS/CGameplayAbility_Skill.h"
#include "CGameplayAbility_Skill_Target.generated.h"

// 위치 조준 후 시전 스킬
// 스킬 확정할 때 쿨타임 시작
UCLASS()
class BOSSGAME_API UCGameplayAbility_Skill_Target : public UCGameplayAbility_Skill
{
	GENERATED_BODY()
	
public:
	FVector GetAimPoint() const;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual bool BeginSkill() override;
	virtual bool StartCast() override;
	virtual void OnSkillHit() override;

private:
	void BeginTargeting();
	void EndTargeting();

private:
	UFUNCTION()
	void OnTargetConfirm(FGameplayEventData EventData);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ACAimIndicator> IndicatorClass;

	// 확정 시 넘어갈 몽타주 섹션
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName CastSection = TEXT("Cast");

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetMaxDistance = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	TSubclassOf<ACAoE> AoEClass;

	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	FCHitArea AoEArea;

	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	float AoEDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	float AoEInterval = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<ACAimIndicator> Indicator;

	bool bTargeting = false;
	FVector TargetLocation;
};
