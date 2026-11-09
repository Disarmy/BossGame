// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANSC_GameplayTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"

void UANSC_GameplayTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !Tag.IsValid())
		return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
		ASC->AddLooseGameplayTag(Tag);
}

void UANSC_GameplayTag::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference & EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !Tag.IsValid())
		return;

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
		ASC->RemoveLooseGameplayTag(Tag);
}

FString UANSC_GameplayTag::GetNotifyName_Implementation() const
{
	return Tag.IsValid() ? Tag.ToString() : TEXT("Gameplay Tag");
}
