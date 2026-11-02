// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"
#include "CUnit.h"
#include "GameFramework/Controller.h"

UCGameplayAbility::UCGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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

ACUnit* UCGameplayAbility::GetUnit() const
{
	return Cast<ACUnit>(GetAvatarActorFromActorInfo());
}
