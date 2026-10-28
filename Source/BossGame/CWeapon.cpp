// Fill out your copyright notice in the Description page of Project Settings.


#include "CWeapon.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACWeapon::ACWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACWeapon::AttachWeapon(ACharacter* WeaponOwner)
{
	if (!IsValid(WeaponOwner)) return;

	AttachToComponent(WeaponOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
}



