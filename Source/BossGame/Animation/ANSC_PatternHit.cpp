#include "Animation/ANSC_PatternHit.h"
#include "GAS/CGameplayTags.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemBlueprintLibrary.h"

void UANSC_PatternHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// 실제 시간으로 변환
	float PlayRate = 1.0f;
	const UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	UAnimInstance* AnimInstance = MeshComp ? MeshComp->GetAnimInstance() : nullptr;
	if (Montage && AnimInstance)
		PlayRate = AnimInstance->Montage_GetPlayRate(Montage) * Montage->RateScale;

	const float Duration = PlayRate > 0.0f ? TotalDuration / PlayRate : TotalDuration;
	SendEvent(MeshComp, CTags::Event_Pattern_HitBegin, Duration);
}

void UANSC_PatternHit::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	SendEvent(MeshComp, CTags::Event_Pattern_Hit, 0.0f);
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UANSC_PatternHit::GetNotifyName_Implementation() const
{
	return TEXT("PatternHit");
}

void UANSC_PatternHit::SendEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& Tag, float Magnitude) const
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner)) return;

	FGameplayEventData Payload;
	Payload.EventTag = Tag;
	Payload.EventMagnitude = Magnitude;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, Tag, Payload);
}