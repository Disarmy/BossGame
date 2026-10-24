// Fill out your copyright notice in the Description page of Project Settings.


#include "CStencilComponent.h"
#include "Components/PrimitiveComponent.h"

// Sets default values for this component's properties
UCStencilComponent::UCStencilComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UCStencilComponent::BeginPlay()
{
	Super::BeginPlay();

	SetStencil(StencilType);
}

void UCStencilComponent::OnRegister()
{
	Super::OnRegister();

	SetStencil(StencilType);
}

void UCStencilComponent::SetStencil(EStencilType Type, bool bIncludeAttached)
{
	StencilType = Type;
	AActor* Owner = GetOwner();
	if (IsValid(Owner) == false)
		return;

	const uint8 Value = static_cast<uint8>(Type);
	const bool bRender = Value != 0;

	auto Apply = [Value, bRender](AActor* Target)
		{
			TArray<UPrimitiveComponent*> Prims;
			Target->GetComponents<UPrimitiveComponent>(Prims);
			for (UPrimitiveComponent* Prim : Prims)
			{
				Prim->SetRenderCustomDepth(bRender);
				Prim->SetCustomDepthStencilValue(Value);
			}
		};

	Apply(Owner);

	if (bIncludeAttached)
	{
		TArray<AActor*> Attached;
		Owner->GetAttachedActors(Attached, true, true);
		for (AActor* Child : Attached)
			Apply(Child);
	}
}
