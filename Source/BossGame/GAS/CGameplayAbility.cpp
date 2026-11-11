#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayTags.h"
#include "CPlayerController.h"
#include "CUnit.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 죽으면 어떤 Ability도 발동하지 않음
	ActivationBlockedTags.AddTag(CTags::State_Dead);
}

void UCGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (bStopMovementOnActivate) {
		if (ACUnit* Unit = GetUnit()) {
			if (AController* Controller = Unit->GetController()) {
				Controller->StopMovement();
			}
		}
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCGameplayAbility::ApplyDamageEffect(AActor* Target, TSubclassOf<UGameplayEffect> Effect, float Damage, const FGameplayTag& HitReactTag) const
{
	if (!Target || !Effect) return;

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(Effect, GetAbilityLevel());
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Damage, Damage);
	if (HitReactTag.IsValid())
		Spec.Data->AddDynamicAssetTag(HitReactTag);

	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, Spec,
		UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Target));
}

void UCGameplayAbility::ClearActionStateTags() const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo()) {
		ASC->SetLooseGameplayTagCount(CTags::State_CanCombo, 0);
		ASC->SetLooseGameplayTagCount(CTags::State_CanCancel, 0);
	}
}

ACUnit* UCGameplayAbility::GetUnit() const
{
	return Cast<ACUnit>(GetAvatarActorFromActorInfo());
}

void UCGameplayAbility::FaceAim()
{
	ACUnit* Unit = GetUnit();
	if (Unit == nullptr)
		return;

	const ACPlayerController* PC = Cast<ACPlayerController>(GetCurrentActorInfo()->PlayerController.Get());
	if (PC == nullptr)
		return;

	FVector AimLocation;
	if (PC->GetAimLocation(AimLocation) == false)
		return;

	FVector Direction = AimLocation - Unit->GetActorLocation();
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero() == false)
		Unit->SetActorRotation(Direction.Rotation());
}
