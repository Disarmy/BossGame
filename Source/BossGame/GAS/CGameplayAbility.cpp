#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayTags.h"
#include "CPlayerController.h"
#include "CUnit.h"
#include "CAttackData.h"
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

FGameplayEffectSpecHandle UCGameplayAbility::MakeDamageSpec(TSubclassOf<UGameplayEffect> Effect, const FDataTableRowHandle& AttackRow) const
{
	const FCAttackData* Attack = AttackRow.GetRow<FCAttackData>(GetName());
	if (!Effect || !Attack) return FGameplayEffectSpecHandle();

	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(Effect, GetAbilityLevel());
	Spec.Data->SetSetByCallerMagnitude(CTags::Data_Damage, Attack->Damage);
	if (Attack->HitReactTag.IsValid())
		Spec.Data->AddDynamicAssetTag(Attack->HitReactTag);

	return Spec;
}

void UCGameplayAbility::ApplyDamageEffect(AActor* Target, TSubclassOf<UGameplayEffect> Effect, const FDataTableRowHandle& AttackRow) const
{
	const FGameplayEffectSpecHandle Spec = MakeDamageSpec(Effect, AttackRow);
	if (!Target || !Spec.IsValid()) return;

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

bool UCGameplayAbility::GetAimLocation(FVector& OutLocation) const
{
	const ACPlayerController* PC = Cast<ACPlayerController>(GetCurrentActorInfo()->PlayerController.Get());
	return PC && PC->GetAimLocation(OutLocation);
}


ACUnit* UCGameplayAbility::GetUnit() const
{
	return Cast<ACUnit>(GetAvatarActorFromActorInfo());
}

void UCGameplayAbility::FaceAim()
{
	ACUnit* Unit = GetUnit();
	FVector AimLocation;
	if (!Unit || !GetAimLocation(AimLocation)) return;

	FVector Direction = AimLocation - Unit->GetActorLocation();
	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
		Unit->SetActorRotation(Direction.Rotation());
}
