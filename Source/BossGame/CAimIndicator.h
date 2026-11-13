#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CAimIndicator.generated.h"

class UCStencilComponent;
class UCGameplayAbility_Skill;

UCLASS()
class BOSSGAME_API ACAimIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	ACAimIndicator();

	void Init(const UCGameplayAbility_Skill* InAbility, float Radius);

protected:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;

	// 메시 원본의 반지름 (엔진 기본 Plane, Cylinder는 50)
	UPROPERTY(EditDefaultsOnly, Category = "Indicator")
	float MeshRadius = 50.0f;
	
private:
	TWeakObjectPtr<const UCGameplayAbility_Skill> Ability;
};
