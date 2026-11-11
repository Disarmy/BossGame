#include "CWeapon.h"
#include "BossGame.h"
#include "GAS/CGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ACWeapon::ACWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickEnabled(false);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
	TraceStart->SetupAttachment(Mesh);

	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
	TraceEnd->SetupAttachment(Mesh);
}

void ACWeapon::AttachWeapon(ACharacter* WeaponOwner)
{
	if (!IsValid(WeaponOwner)) return;

	AttachToComponent(WeaponOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
}

void ACWeapon::BeginTrace()
{
	HitActors.Reset();
	SetActorTickEnabled(true);
}

void ACWeapon::EndTrace()
{
	SetActorTickEnabled(false);
	HitActors.Reset();
}

void ACWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* WeaponOwner = GetOwner();
	if (!WeaponOwner)
		return;

	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(WeaponTrace), false);
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(WeaponOwner);

	// 유닛 캡슐(Pawn)만 대상
	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(TraceRadius), Params);

	if (CDebug::bDrawHit)
		DrawDebugLine(GetWorld(), Start, End, Hits.IsEmpty() ? FColor::Green : FColor::Red, false, 0.5f, 0, 2.0f);

	for (const FHitResult& Hit : Hits) {
		AActor* Target = Hit.GetActor();
		if (!Target || HitActors.Contains(Target))
			continue;

		HitActors.Add(Target);

		FGameplayEventData EventData;
		EventData.EventTag = CTags::Event_Attack_Hit;
		EventData.Instigator = WeaponOwner;
		EventData.Target = Target;
		EventData.OptionalObject = this;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(WeaponOwner, CTags::Event_Attack_Hit, EventData);
	}
}

