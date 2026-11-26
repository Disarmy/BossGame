#include "GAS/CGameplayAbility_Pattern_Area.h"
#include "BossGame.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

void UCGameplayAbility_Pattern_Area::OnPatternHit()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	const FVector Forward = Avatar->GetActorForwardVector().GetSafeNormal2D();

	FVector Center = Avatar->GetActorLocation();
	if (const ACharacter* Character = Cast<ACharacter>(Avatar)) {
		Center.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	TArray<AActor*> Targets;
	HitArea.GetTargets(World, Center, Forward, Avatar, Targets);

	for (AActor* Target : Targets)
		ApplyDamageEffect(Target, DamageEffect, Attack);

	if (CDebug::bDrawHit) {
		const FColor Color = Targets.IsEmpty() ? FColor::Green : FColor::Red;
		DrawDebugCircle(World, Center, HitArea.Radius, 32, Color, false, 1.0f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
		if (HitArea.InnerRadius > 0.0f)
			DrawDebugCircle(World, Center, HitArea.InnerRadius, 32, Color, false, 1.0f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
	}
}
