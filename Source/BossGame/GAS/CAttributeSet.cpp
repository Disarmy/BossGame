// Fill out your copyright notice in the Description page of Project Settings.


#include "CAttributeSet.h"
#include "GameplayEffectExtension.h"

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

	if (Data.EvaluatedData.Attribute == GetHPAttribute()) {
		SetHP(FMath::Clamp(GetHP(), 0.0f, GetMaxHP()));
	}
}
