// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"
#include "CUnit.h"
#include "CPlayerController.h"
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
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero() == false)
		Unit->SetActorRotation(Direction.Rotation());
}
