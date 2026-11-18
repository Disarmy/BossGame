#pragma once

#include "CoreMinimal.h"
#include "CHitArea.h"
#include "GAS/CGameplayAbility_Skill.h"
#include "CGameplayAbility_Skill_Area.generated.h"

// 캐릭터 기준 범위 스킬
// 단발  판정, 구간 판정, 조준 위치로 캐릭터 이동 포함
UCLASS()
class BOSSGAME_API UCGameplayAbility_Skill_Area : public UCGameplayAbility_Skill
{
	GENERATED_BODY()

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual bool BeginSkill() override;
	virtual bool StartCast() override;
	virtual void OnSkillHit() override;

private:
	FCHitArea MakeHitArea() const;

	// 조준 위치를 워프 목표로 등록
	void SetWarpTarget();

private:
	UFUNCTION()
	void OnSkillHitBegin(FGameplayEventData EventData);

	UFUNCTION()
	void OnSkillHitEnd(FGameplayEventData EventData);

protected:
	// 판정 범위: 캐릭터 앞쪽으로 Offset만큼 옮긴 지점 기준
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitRadius = 200.0f;

	// 0이면 원, 크면 도넛
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitInnerRadius = 0.0f;

	// 360이면 원, 작으면 정면 부채꼴
	UPROPERTY(EditDefaultsOnly, Category = "Hit", meta = (ClampMin = "0", ClampMax = "360"))
	float HitAngle = 360.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float HitForwardOffset = 0.0f;

	// 몽타주의 Motion Warping 노티파이와 같은 이름 비우면 워프X
	UPROPERTY(EditDefaultsOnly, Category = "Warp")
	FName WarpTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Warp")
	float WarpMaxDistance = 600.0f;

private:
	bool bInSkillHit = false;
	TSet<TWeakObjectPtr<AActor>> SkillHitActors;
};
