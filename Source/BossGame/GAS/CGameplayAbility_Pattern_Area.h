#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility_Pattern.h"
#include "CHitArea.h"
#include "CGameplayAbility_Pattern_Area.generated.h"


UCLASS()
class BOSSGAME_API UCGameplayAbility_Pattern_Area : public UCGameplayAbility_Pattern
{
	GENERATED_BODY()

protected:
	virtual void OnPatternHit() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	FCHitArea HitArea;
};
