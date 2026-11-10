// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayer.h"
#include "CStencilComponent.h"
#include "GAS/CGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

ACPlayer::ACPlayer()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 1200.0f;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	StencilComponent->SetStencil(EStencilType::Player);
}

void ACPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);   // 여기서 ASC 초기화, 기본 Ability 부여 CUnit

	if (bSkillsGiven) return; // 한번만
	bSkillsGiven = true;

	for (const TPair<FGameplayTag, TSubclassOf<UCGameplayAbility>>& Pair : DefaultSkills)
		EquipSkill(Pair.Key, Pair.Value);
}

void ACPlayer::EquipSkill(FGameplayTag SlotTag, TSubclassOf<UCGameplayAbility> SkillClass)
{
	if (!AbilitySystemComponent || !SlotTag.IsValid()) return;

	// 슬롯에 있던 스킬 찾기
	FGameplayAbilitySpecHandle OldHandle;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities()) {
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(SlotTag)) {
			OldHandle = Spec.Handle;
			break;
		}
	}

	// 순회가 끝난 뒤 해제
	if (OldHandle.IsValid())
		AbilitySystemComponent->ClearAbility(OldHandle);

	if (!SkillClass) return;

	FGameplayAbilitySpec NewSpec(SkillClass, 1, INDEX_NONE, this);
	NewSpec.GetDynamicSpecSourceTags().AddTag(SlotTag);
	AbilitySystemComponent->GiveAbility(NewSpec);
}
