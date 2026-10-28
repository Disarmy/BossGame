// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CUnit.h"
#include "CPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class BOSSGAME_API ACPlayer : public ACUnit
{
	GENERATED_BODY()
	
public:
	ACPlayer();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;


};
