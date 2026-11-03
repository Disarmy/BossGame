// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"
#include "CUnit.h"
#include "CTypes.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

ACPlayerController::ACPlayerController()
{
	bShowMouseCursor = true;
}

bool ACPlayerController::GetAimLocation(FVector& OutLocation) const
{
	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Cursor, false, Hit)) {
		return false;
	}

	OutLocation = Hit.Location;
	return true;
}

void ACPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		SubSystem->AddMappingContext(MappingContext, 0);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void ACPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent)) {
		Input->BindAction(MoveAction, ETriggerEvent::Started, this, &ACPlayerController::OnMoveStarted);
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPlayerController::OnMove);
		
		for (const TPair<TObjectPtr<UInputAction>, FGameplayTag>& Pair : AbilityInputs) {
			if (Pair.Key) {
				Input->BindAction(Pair.Key, ETriggerEvent::Started, this, &ACPlayerController::OnAbilityInput, Pair.Value);
			}
		}
	}
}

void ACPlayerController::OnMove(const FInputActionValue & Value)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn())) {
		if (ASC->HasMatchingGameplayTag(CTags::Lock_Move)) {
			return;
		}
	}


	FHitResult Hit;
	// Cursor 채널
	if (GetHitResultUnderCursor(ECC_Cursor, false, Hit) == false)
		return;

	APawn* MyPawn = GetPawn();
	if (IsValid(MyPawn) == false)
		return;

	FVector Destination = Hit.Location;

	// 유닛(보스, 몹)을 클릭하면 그 둘레의 가장 가까운 지점으로
	ACUnit* Target = Cast<ACUnit>(Hit.GetActor());
	if (IsValid(Target) && Target != MyPawn)
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector Direction = (MyPawn->GetActorLocation() - TargetLocation).GetSafeNormal2D();

		const float Gap = Target->GetCapsuleComponent()->GetScaledCapsuleRadius()
			+ Cast<ACharacter>(MyPawn)->GetCapsuleComponent()->GetScaledCapsuleRadius()
			+ StopDistance;

		Destination = TargetLocation + Direction * Gap;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Destination);
}

void ACPlayerController::OnMoveStarted(const FInputActionValue& Value)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (ASC && ASC->HasMatchingGameplayTag(CTags::Lock_Move) && ASC->HasMatchingGameplayTag(CTags::State_CanCancel)) {
		FGameplayTagContainer CancelTags(CTags::Ability_Attack);
		ASC->CancelAbilities(&CancelTags);
	}
}

void ACPlayerController::OnAbilityInput(FGameplayTag InputTag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn());
	if (!IsValid(ASC)) return;

	TArray<FGameplayAbilitySpecHandle> Handles;
	bool bActiveFound = false;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities()) {
		const UCGameplayAbility* Ability = Cast<UCGameplayAbility>(Spec.Ability);

		if (!Ability || !Ability->GetInputTag().MatchesTagExact(InputTag)) {
			continue;
		}

		if (Spec.IsActive()) {
			bActiveFound = true;
		}
		else {
			Handles.Add(Spec.Handle);
		}
	}

	if (bActiveFound) {
		FGameplayEventData EventData;
		EventData.EventTag = InputTag;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), InputTag, EventData);
	}

	for (const FGameplayAbilitySpecHandle& Handle : Handles) {
		ASC->TryActivateAbility(Handle);
	}
}
