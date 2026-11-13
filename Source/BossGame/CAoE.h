#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "CHitArea.h"
#include "CAoE.generated.h"

class UCStencilComponent;

UCLASS()
class BOSSGAME_API ACAoE : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACAoE();

	void Activate(const FCHitArea& InArea, float Duration, float Interval, const FGameplayEffectSpecHandle& InDamageSpec);

private:
	void Hit();
	void Deactivate();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;

	// 메시 원본의 반지름 (엔진 기본 Plane, Cylinder는 50)
	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	float MeshRadius = 50.0f;

private:
	FCHitArea Area;
	FGameplayEffectSpecHandle DamageSpec;
	FTimerHandle HitTimer;
	FTimerHandle EndTimer;
};
