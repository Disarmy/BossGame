#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANSC_PatternHit.generated.h"

struct FGameplayTag;

// 구간 시작: 장판 예고 시작(길이 전달), 구간 끝: 타격
UCLASS(meta = (DisplayName = "Pattern Hit"))
class BOSSGAME_API UANSC_PatternHit : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

private:
	void SendEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& Tag, float Magnitude) const;
};
