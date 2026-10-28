// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWeapon.generated.h"

UCLASS()
class BOSSGAME_API ACWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACWeapon();
	
	void AttachWeapon(ACharacter* Owner);
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName AttachSocketName = TEXT("WeaponSocket");

};
