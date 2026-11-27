#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "CHitArea.h"
#include "CAoE.generated.h"

class UCStencilComponent;

UCLASS()
class BOSSGAME_API ACAoE : public AActor
{
	GENERATED_BODY()
	
public:	
	ACAoE();

	// 주기적 타격 장판
	void Activate(const FCHitArea& InArea, float Duration, float Interval, const FGameplayEffectSpecHandle& InDamageSpec);

	// 예고 장판
	void Telegraph(const FCHitArea& InArea, float InFillDuration, const FGameplayTag& ResponseTag);

	void FinishTelegraph();
	void Deactivate();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void Hit();
	void ApplyArea(const FCHitArea& InArea);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;

	// 메시 원본의 반지름 (엔진 기본 Plane, Cylinder는 50)
	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	float MeshRadius = 50.0f;

	// 대응 종류가 없는 일반 공격 색
	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	FLinearColor DefaultColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "AoE", meta = (Categories = "Data.Response"))
	TMap<FGameplayTag, FLinearColor> ResponseColors;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;

	// 타격 뒤 장판이 남아 있는 시간
	UPROPERTY(EditDefaultsOnly, Category = "AoE")
	float HitLingerTime = 0.2f;

	FCHitArea Area;
	FGameplayEffectSpecHandle DamageSpec;
	FTimerHandle HitTimer;
	FTimerHandle EndTimer;
	float FillDuration = 0.0f;
	float FillElapsed = 0.0f;
};
