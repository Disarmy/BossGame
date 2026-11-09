// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ANSC_WeaponTrace.h"
#include "CUnit.h"
#include "CWeapon.h"
#include "Components/SkeletalMeshComponent.h"


void UANSC_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ACUnit* Unit = Cast<ACUnit>(MeshComp ? MeshComp->GetOwner() : nullptr)) {
		if (ACWeapon* Weapon = Unit->GetWeapon()) {
			Weapon->BeginTrace();
		}
	}
}

void UANSC_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ACUnit* Unit = Cast<ACUnit>(MeshComp ? MeshComp->GetOwner() : nullptr)) {
		if (ACWeapon* Weapon = Unit->GetWeapon()) {
			Weapon->EndTrace();
		}
	}
}