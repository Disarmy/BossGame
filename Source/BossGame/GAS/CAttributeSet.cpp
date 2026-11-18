#include "CAttributeSet.h"
#include "CUnit.h"
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
		SetDamage(0.0f);

		if (LocalDamage > 0.0f) {
			SetHP(FMath::Clamp(GetHP() - LocalDamage, 0.0f, GetMaxHP()));
			
			// HP 0 → 사망 (마지막 일격에는 피격 반응을 보내지 않음)
			if (GetHP() <= 0.0f) {
				if (ACUnit* Unit = Cast<ACUnit>(GetOwningActor()))
					Unit->Die(Data.EffectSpec.GetContext().GetInstigator());
				return;
			}

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
