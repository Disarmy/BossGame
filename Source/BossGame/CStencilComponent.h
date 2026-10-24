// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CTypes.h"
#include "CStencilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSGAME_API UCStencilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCStencilComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void OnRegister() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Stencil")
	void SetStencil(EStencilType Type, bool bIncludeAttached = true);


private:
	UPROPERTY(EditAnywhere, Category = "Stencil")
	EStencilType StencilType = EStencilType::None;
};
