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

	void BeginTrace();
	void EndTrace();

protected:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName AttachSocketName = TEXT("WeaponSocket");

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> TraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> TraceEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius = 20.0f;

private:
	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActors;
};
