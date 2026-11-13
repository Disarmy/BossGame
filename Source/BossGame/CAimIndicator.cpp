#include "CAimIndicator.h"
#include "CStencilComponent.h"
#include "GAS/CGameplayAbility_Skill.h"
#include "Components/StaticMeshComponent.h"

ACAimIndicator::ACAimIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCastShadow(false);

	StencilComponent = CreateDefaultSubobject<UCStencilComponent>(TEXT("StencilComponent"));
	StencilComponent->SetStencil(EStencilType::KeepColor);
}

void ACAimIndicator::Init(const UCGameplayAbility_Skill* InAbility, float Radius)
{
	Ability = InAbility;

	FVector Scale = Mesh->GetComponentScale();
	Scale.X = Scale.Y = Radius / MeshRadius;
	Mesh->SetWorldScale3D(Scale);

	if (Ability.IsValid())
		SetActorLocation(Ability->GetAimPoint());
}

// Called every frame
void ACAimIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Ability.IsValid())
		SetActorLocation(Ability->GetAimPoint());
}

