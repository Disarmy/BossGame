#pragma once

#include "CoreMinimal.h"
#include "CHitArea.h"
#include "GAS/CGameplayAbility_Skill.h"
#include "CGameplayAbility_Skill_Summon.generated.h"

class ACSummon;

// 정면에 소환물을 떨어뜨리는 스킬
// 첫 번째 Event.Skill.Hit은 박힘, 두 번째는 폭발
UCLASS()
class BOSSGAME_API UCGameplayAbility_Skill_Summon : public UCGameplayAbility_Skill
{
	GENERATED_BODY()

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual bool StartCast() override;
	virtual void OnSkillHit() override;

private:
	void HitArea(const FCHitArea& Area, const FDataTableRowHandle& AttackRow);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	TSubclassOf<ACSummon> SummonClass;

	// 캐릭터 정면으로 떨어진 거리
	UPROPERTY(EditDefaultsOnly, Category = "Summon")
	float SummonDistance = 400.0f;

	// 박힘의 데미지와 반응은 Damage 카테고리 값
	UPROPERTY(EditDefaultsOnly, Category = "Impact")
	FCHitArea ImpactArea;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	FCHitArea ExplosionArea;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion", meta = (RowType = "/Script/BossGame.CAttackData"))
	FDataTableRowHandle ExplosionAttack;

private:
	UPROPERTY()
	TObjectPtr<ACSummon> Summon;

	FVector SummonLocation;
	FVector SummonForward;
	int32 HitCount = 0;
};
