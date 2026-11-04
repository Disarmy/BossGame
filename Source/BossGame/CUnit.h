// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CUnit.generated.h"

class UCStencilComponent;
class ACWeapon;
class UAbilitySystemComponent;
class UCAttributeSet;
class UGameplayAbility;
class UGameplayEffect;


UCLASS()
class BOSSGAME_API ACUnit : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACUnit();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<ACWeapon> EquipWeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DropWeapon();

	FORCEINLINE ACWeapon* GetWeapon() const { return Weapon; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ACWeapon> WeaponClass;

private:
	UPROPERTY()
	TObjectPtr<ACWeapon> Weapon;

private:
	void InitAbilitySystem();
	bool bDefaultGiven = false;
};
