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

	void ApplyDamageEffect(AActor* Target, TSubclassOf<UGameplayEffect> Effect, float Damage, const FGameplayTag& HitReactTag) const;

	void ClearActionStateTags() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	bool GetAimLocation(FVector& OutLocation) const;

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
