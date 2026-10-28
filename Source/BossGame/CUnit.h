// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CUnit.generated.h"

class UCStencilComponent;
class ACWeapon;

UCLASS()
class BOSSGAME_API ACUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<ACWeapon> EquipWeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DropWeapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACWeapon> WeaponClass;

private:
	UPROPERTY()
	TObjectPtr<ACWeapon> Weapon;
};
