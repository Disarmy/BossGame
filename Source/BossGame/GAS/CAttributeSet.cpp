// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

void UCAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHPAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHP());
	}
}

void UCAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData & Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) {
		const float LocalDamage = GetDamage();
		SetDamage(0.f);

		if (LocalDamage > 0.f) {
			SetHP(FMath::Clamp(GetHP() - LocalDamage, 0.f, GetMaxHP()));

			// 피격 이벤트 (피격 반응 Ability가 받음)
			FGameplayEventData EventData;
			EventData.EventTag = CTags::Event_Hit;
			EventData.Instigator = Data.EffectSpec.GetContext().GetInstigator();
			EventData.EventMagnitude = LocalDamage;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), CTags::Event_Hit, EventData);

			// HP 0 → 사망 처리 (다음 단계)
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHPAttribute()) {
		SetHP(FMath::Clamp(GetHP(), 0.0f, GetMaxHP()));
	}
}
