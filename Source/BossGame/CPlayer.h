// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CUnit.h"
#include "GameplayTagContainer.h"
#include "CPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCGameplayAbility;

UCLASS()
class BOSSGAME_API ACPlayer : public ACUnit
{
	GENERATED_BODY()
	
public:
	ACPlayer();

protected:
	virtual void PossessedBy(AController* NewController) override;

public:
	// 슬롯에 스킬 장착 (기존 스킬은 해제). 쿨타임은 스킬 이름 기준 태그라 바꿔 껴도 유지됨
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void EquipSkill(FGameplayTag SlotTag, TSubclassOf<UCGameplayAbility> SkillClass);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	// 스킬 목록 (Input.Skill.SlotN → 스킬 GA)
	UPROPERTY(EditDefaultsOnly, Category = "Ability", meta = (Categories = "Input.Skill"))
	TMap<FGameplayTag, TSubclassOf<UCGameplayAbility>> DefaultSkills;

private:
	bool bSkillsGiven = false;
};
