#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility_Pattern.h"
#include "CHitArea.h"
#include "CGameplayAbility_Pattern_Area.generated.h"

class ACAoE;

UCLASS()
class BOSSGAME_API UCGameplayAbility_Pattern_Area : public UCGameplayAbility_Pattern
{
	GENERATED_BODY()

protected:	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnPatternHitBegin(float Duration) override;
	virtual void OnPatternHit() override;

private:
	FVector GetHitCenter() const;
	void RemoveAoE();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	FCHitArea HitArea;

	// 비우면 예고 없이 타격만
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TSubclassOf<ACAoE> AoEClass;

private:
	UPROPERTY()
	TObjectPtr<ACAoE> AoE;
};
