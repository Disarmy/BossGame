#include "GAS/CGameplayAbility_Skill.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UCGameplayAbility_Skill::UCGameplayAbility_Skill()
{
	bStopMovementOnActivate = true;

	// 스킬 중에는 공격과 다른 스킬을 막고 이동을 잠금
	BlockAbilitiesWithTag.AddTag(CTags::Ability_Attack);
	BlockAbilitiesWithTag.AddTag(CTags::Ability_Skill);
	ActivationOwnedTags.AddTag(CTags::Lock_Move);
}

const FGameplayTagContainer* UCGameplayAbility_Skill::GetCooldownTags() const
{
	// GAS가 발동 전에 이 태그가 붙어 있는지로 쿨타임을 판정
	return &CooldownTags;
}

void UCGameplayAbility_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	if (CooldownTags.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("%s: CooldownTags is empty, cooldown not applied"), *GetName());
		return;
	}

	// 공용 GE_Cooldown_Skill 하나에 이 스킬의 태그와 시간을 실어서 적용
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	Spec.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Cooldown, CooldownDuration);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
}

void UCGameplayAbility_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 몽타주 타격 프레임의 노티파이(Event.Skill.Hit)마다 범위 판정
	UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Skill_Hit, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UCGameplayAbility_Skill::OnSkillHitEvent);
	WaitHit->ReadyForActivation();

	if (!BeginSkill()) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);   // 이동 정지 + BP 그래프
}

void UCGameplayAbility_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ClearActionStateTags();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCGameplayAbility_Skill::BeginSkill()
{
	return StartCast();
}

bool UCGameplayAbility_Skill::StartCast()
{
	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		return false;

	FaceAim();
	return true;
}

FGameplayEffectSpecHandle UCGameplayAbility_Skill::MakeSkillDamageSpec() const
{
	return MakeDamageSpec(DamageEffect, Attack);
}

void UCGameplayAbility_Skill::ApplySkillDamage(AActor* Target)
{
	ApplyDamageEffect(Target, DamageEffect, Attack);
}

void UCGameplayAbility_Skill::OnSkillHitEvent(FGameplayEventData EventData)
{
	OnSkillHit();
}
