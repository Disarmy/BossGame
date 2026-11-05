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

			// 피격 이벤트: 세부 태그를 보내면 부모 Event.Hit 트리거도 같이 발동
			const FGameplayTagContainer& SpecTags = Data.EffectSpec.GetDynamicAssetTags();
			FGameplayTag HitTag = CTags::Event_Hit;
			if (SpecTags.HasTagExact(CTags::Data_HitReact_Down))
				HitTag = CTags::Event_Hit_Down;
			else if (SpecTags.HasTagExact(CTags::Data_HitReact_Flinch))
				HitTag = CTags::Event_Hit_Flinch;

			FGameplayEventData EventData;
			EventData.EventTag = HitTag;
			EventData.Instigator = Data.EffectSpec.GetContext().GetInstigator();
			EventData.EventMagnitude = LocalDamage;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(), HitTag, EventData);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHPAttribute()) {
		SetHP(FMath::Clamp(GetHP(), 0.0f, GetMaxHP()));
	}
}
