// Fill out your copyright notice in the Description page of Project Settings.


#include "CUnit.h"
#include "CStencilComponent.h"
#include "CWeapon.h"

// Sets default values
ACUnit::ACUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StencilComponent = CreateDefaultSubobject<UCStencilComponent>(TEXT("StencilComponent"));
}

// Called when the game starts or when spawned
void ACUnit::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponClass) {
		EquipWeapon(WeaponClass);
	}
}

void ACUnit::EquipWeapon(TSubclassOf<ACWeapon> EquipWeaponClass)
{
	if (EquipWeaponClass == nullptr) return;

	DropWeapon(); // 이전 무기 정리

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	Weapon = GetWorld()->SpawnActor<ACWeapon>(EquipWeaponClass, GetActorTransform(), Params);
	if (!IsValid(Weapon)) return;

	Weapon->AttachWeapon(this);

	StencilComponent->Refresh();
}

void ACUnit::DropWeapon()
{
	if (IsValid(Weapon))
		Weapon->Destroy();

	Weapon = nullptr;
}


