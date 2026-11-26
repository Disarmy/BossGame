#include "GAS/CGameplayAbility_Pattern.h"
#include "GAS/CGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UCGameplayAbility_Pattern::UCGameplayAbility_Pattern()
{
	// 랜덤 패턴 태스크가 이 태그로 후보를 찾음
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(CTags::Ability_Boss_Pattern);
	SetAssetTags(Tags);

	// 패턴은 한 번에 하나, 패턴 중에는 회전 고정
	BlockAbilitiesWithTag.AddTag(CTags::Ability_Boss_Pattern);
	ActivationOwnedTags.AddTag(CTags::Lock_Rotate);
}

const FGameplayTagContainer* UCGameplayAbility_Pattern::GetCooldownTags() const
{
	return &CooldownTags;
}

void UCGameplayAbility_Pattern::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	if (CooldownTags.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("%s: CooldownTags is empty, cooldown not applied"), *GetName());
		return;
	}

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	Spec.Data->DynamicGrantedTags.AppendTags(CooldownTags);
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Cooldown, CooldownDuration);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
}

void UCGameplayAbility_Pattern::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 타격 프레임의 노티파이(Event.Pattern.Hit)마다 판정
	UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, CTags::Event_Pattern_Hit, nullptr, false, true);
	WaitHit->EventReceived.AddDynamic(this, &UCGameplayAbility_Pattern::OnPatternHitEvent);
	WaitHit->ReadyForActivation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCGameplayAbility_Pattern::OnPatternHitEvent(FGameplayEventData EventData)
{
	OnPatternHit();
}
