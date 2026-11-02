// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CGameplayAbility.generated.h"

class ACUnit;

UCLASS()
class BOSSGAME_API UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCGameplayAbility();

	FORCEINLINE const FGameplayTag& GetInputTag() const { return InputTag; }

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintPure, Category = "Ability")
	ACUnit* GetUnit() const;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void FaceAim();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bStopMovementOnActivate = false;
};
