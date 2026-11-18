#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSummon.generated.h"

class UCStencilComponent;

UCLASS()
class BOSSGAME_API ACSummon : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSummon();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Impact"))
	void ReceiveImpact();

	// 폭발 연출 후 제거는 BP에서
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Explode"))
	void ReceiveExplode();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UCStencilComponent> StencilComponent;
};
