// Fill out your copyright notice in the Description page of Project Settings.


#include "CUnit.h"
#include "CStencilComponent.h"
#include "CWeapon.h"
#include "GAS/CAttributeSet.h"
#include "AbilitySystemComponent.h"

// Sets default values
ACUnit::ACUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StencilComponent = CreateDefaultSubobject<UCStencilComponent>(TEXT("StencilComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UCAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ACUnit::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void ACUnit::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponClass) {
		EquipWeapon(WeaponClass);
	}
}

void ACUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystem();
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

void ACUnit::InitAbilitySystem()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (bDefaultGiven) return;
	bDefaultGiven = true;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (AbilityClass)
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		if (EffectClass == nullptr)
			continue;

		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (Spec.IsValid())
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}


