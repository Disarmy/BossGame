#include "CAoE.h"
#include "BossGame.h"
#include "CStencilComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

ACAoE::ACAoE()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

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

void ACAoE::ApplyArea(const FCHitArea& InArea)
{
	Area = InArea;

	const float Scale = Area.Radius / MeshRadius;
	Mesh->SetWorldScale3D(FVector(Scale, Scale, 1.0f));

	if (MaterialInstance) {
		MaterialInstance->SetScalarParameterValue(TEXT("InnerRatio"), Area.Radius > 0.0f ? Area.InnerRadius / Area.Radius : 0.0f);
		MaterialInstance->SetScalarParameterValue(TEXT("Angle"), Area.Angle);
	}
}

void ACAoE::Telegraph(const FCHitArea& InArea, float InFillDuration, const FGameplayTag& ResponseTag)
{
	ApplyArea(InArea);

	if (MaterialInstance) {
		const FLinearColor* Color = ResponseColors.Find(ResponseTag);
		MaterialInstance->SetVectorParameterValue(TEXT("Color"), Color ? *Color : DefaultColor);
		MaterialInstance->SetScalarParameterValue(TEXT("Progress"), 0.0f);
	}

	FillDuration = FMath::Max(InFillDuration, KINDA_SMALL_NUMBER);
	FillElapsed = 0.0f;
	SetActorTickEnabled(true);
}

void ACAoE::FinishTelegraph()
{
	SetActorTickEnabled(false);
	if (MaterialInstance)
		MaterialInstance->SetScalarParameterValue(TEXT("Progress"), 1.0f);

	if (HitLingerTime > 0.0f)
		GetWorldTimerManager().SetTimer(EndTimer, this, &ACAoE::Deactivate, HitLingerTime, false);
	else
		Deactivate();
}

void ACAoE::Deactivate()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Destroy();
}

void ACAoE::BeginPlay()
{
	Super::BeginPlay();

	MaterialInstance = Mesh->CreateDynamicMaterialInstance(0);
}

void ACAoE::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FillElapsed += DeltaSeconds;
	const float Progress = FMath::Clamp(FillElapsed / FillDuration, 0.0f, 1.0f);

	if (MaterialInstance)
		MaterialInstance->SetScalarParameterValue(TEXT("Progress"), Progress);

	// 다 차면 타격 노티파이까지 가득 찬 채로 대기
	if (Progress >= 1.0f)
		SetActorTickEnabled(false);
}
