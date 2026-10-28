// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerController.h"
#include "CUnit.h"
#include "CTypes.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ACPlayerController::ACPlayerController()
{
	bShowMouseCursor = true;
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
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACPlayerController::OnMove);
	}
}

void ACPlayerController::OnMove(const FInputActionValue & Value)
{
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
