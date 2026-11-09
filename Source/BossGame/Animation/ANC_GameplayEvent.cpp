// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANC_GameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"

void UANC_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !EventTag.IsValid())
		return;

	AActor* Owner = MeshComp->GetOwner();

	// 에디터 미리보기 액터처럼 ASC가 없는 대상이면 무시 (에러 로그 방지)
	if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		return;

	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = Owner;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);

}

FString UANC_GameplayEvent::GetNotifyName_Implementation() const
{
	return EventTag.IsValid() ? EventTag.ToString() : TEXT("Gameplay Event");
}