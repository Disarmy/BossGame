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
class UAnimMontage;

UCLASS()
class BOSSGAME_API ACUnit : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACUnit();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void Die(AActor* Killer);
	virtual void ApplyKnockback(const FVector& Origin, float Distance, float Duration);

	FORCEINLINE ACWeapon* GetWeapon() const { return Weapon; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void InitAbilitySystem();

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<ACWeapon> EquipWeaponClass);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DropWeapon();

	UFUNCTION(BlueprintPure, Category = "Animation")
	UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }

	UFUNCTION(BlueprintPure, Category = "Animation")
	UAnimMontage* GetDownMontage() const { return DownMontage; }

	UFUNCTION(BlueprintPure, Category = "Unit")
	bool IsDead() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Unit", meta = (DisplayName = "On Die"))
	void ReceiveDie(AActor* Killer);

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
	 
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DownMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

private:
	UPROPERTY()
	TObjectPtr<ACWeapon> Weapon;

	bool bDefaultGiven = false;
	// 넉백
	FVector KnockbackVelocity = FVector::ZeroVector;
	float KnockbackTimeLeft = 0.0f;
};
