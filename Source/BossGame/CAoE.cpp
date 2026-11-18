#include "CAoE.h"
#include "BossGame.h"
#include "CStencilComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

ACAoE::ACAoE()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetCastShadow(false);

	StencilComponent = CreateDefaultSubobject<UCStencilComponent>(TEXT("StencilComponent"));
	StencilComponent->SetStencil(EStencilType::KeepColor);
}

void ACAoE::Activate(const FCHitArea& InArea, float Duration, float Interval, const FGameplayEffectSpecHandle& InDamageSpec)
{
	Area = InArea;
	DamageSpec = InDamageSpec;

	const float Scale = Area.Radius / MeshRadius;
	Mesh->SetWorldScale3D(FVector(Scale, Scale, 1.0f));

	Hit();

	FTimerManager& Timers = GetWorldTimerManager();
	Timers.SetTimer(HitTimer, this, &ACAoE::Hit, Interval, true);
	Timers.SetTimer(EndTimer, this, &ACAoE::Deactivate, Duration, false);
}

void ACAoE::Hit()
{
	AActor* Source = GetInstigator();
	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Source);
	if (!SourceASC || !DamageSpec.IsValid()) return;

	const FVector Center = GetActorLocation();
	const FVector Forward = GetActorForwardVector();

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AoEHit), false, Source);
	GetWorld()->OverlapMultiByObjectType(Overlaps, Center, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(Area.Radius), Params);

	TSet<AActor*> HitActors;   // 컴포넌트가 여러 개 걸려도 대상당 한 번
	for (const FOverlapResult& Overlap : Overlaps) {
		AActor* Target = Overlap.GetActor();
		if (!Target || HitActors.Contains(Target)) continue;
		if (!Area.Contains(Center, Forward, Target->GetActorLocation(), Target->GetSimpleCollisionRadius())) continue;

		HitActors.Add(Target);

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target))
			SourceASC->ApplyGameplayEffectSpecToTarget(*DamageSpec.Data.Get(), TargetASC);
	}

#if ENABLE_DRAW_DEBUG
	if (CDebug::bDrawHit) {
		const FColor Color = HitActors.IsEmpty() ? FColor::Green : FColor::Red;
		DrawDebugCircle(GetWorld(), Center, Area.Radius, 32, Color, false, 0.3f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
	}
#endif
}

void ACAoE::Deactivate()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Destroy();
}