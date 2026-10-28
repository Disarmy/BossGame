// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class BOSSGAME_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnMove(const FInputActionValue& Value);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float StopDistance = 30.f;   // 캡슐끼리 닿기 직전 여유

};
