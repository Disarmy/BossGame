#include "GAS/CGameplayAbility_Skill_Summon.h"
#include "CUnit.h"
#include "CSummon.h"
#include "BossGame.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

void UCGameplayAbility_Skill_Summon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 폭발 전에 끝나면(사망 등) 소환물 제거
	if (IsValid(Summon))
		Summon->Destroy();
	Summon = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCGameplayAbility_Skill_Summon::StartCast()
{
	if (!Super::StartCast()) return false;

	ACUnit* Unit = GetUnit();
	if (!Unit) return false;

	HitCount = 0;

	SummonForward = Unit->GetActorForwardVector().GetSafeNormal2D();
	SummonLocation = Unit->GetActorLocation() + SummonForward * SummonDistance;
	SummonLocation.Z -= Unit->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if (SummonClass) {
		FActorSpawnParameters Params;
		Params.Owner = Unit;
		Params.Instigator = Unit;

		Summon = Unit->GetWorld()->SpawnActor<ACSummon>(SummonClass, SummonLocation, SummonForward.Rotation(), Params);
	}

	return true;
}

void UCGameplayAbility_Skill_Summon::OnSkillHit()
{
	if (HitCount++ == 0) {
		HitArea(ImpactArea, Damage, HitReactTag);

		if (IsValid(Summon))
			Summon->ReceiveImpact();
		return;
	}

	HitArea(ExplosionArea, ExplosionDamage, ExplosionHitReactTag);

	if (IsValid(Summon))
		Summon->ReceiveExplode();
	Summon = nullptr;   // 이후 제거는 소환물 BP가 맡음
}

void UCGameplayAbility_Skill_Summon::HitArea(const FCHitArea& Area, float InDamage, const FGameplayTag& InHitReactTag)
{
	ACUnit* Unit = GetUnit();
	if (!Unit) return;

	TArray<AActor*> Targets;
	Area.GetTargets(Unit->GetWorld(), SummonLocation, SummonForward, Unit, Targets);

	for (AActor* Target : Targets)
		ApplyDamageEffect(Target, DamageEffect, InDamage, InHitReactTag);

#if ENABLE_DRAW_DEBUG
	if (CDebug::bDrawHit) {
		const FColor Color = Targets.IsEmpty() ? FColor::Green : FColor::Red;
		DrawDebugCircle(Unit->GetWorld(), SummonLocation, Area.Radius, 32, Color, false, 1.0f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
	}
#endif
}
