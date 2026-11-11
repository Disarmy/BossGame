#include "Animation/ANSC_SkillHit.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemBlueprintLibrary.h"

void UANSC_SkillHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	SendEvent(MeshComp, CTags::Event_Skill_HitBegin);
}

void UANSC_SkillHit::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	SendEvent(MeshComp, CTags::Event_Skill_Hit);
}

void UANSC_SkillHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	SendEvent(MeshComp, CTags::Event_Skill_HitEnd);
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UANSC_SkillHit::GetNotifyName_Implementation() const
{
	return TEXT("SkillHit");
}

void UANSC_SkillHit::SendEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& Tag) const
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner)) return;

	FGameplayEventData Payload;
	Payload.EventTag = Tag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, Tag, Payload);
}