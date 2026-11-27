#include "GAS/CGameplayAbility_Pattern_Area.h"
#include "BossGame.h"
#include "CAoE.h"
#include "CAttackData.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

void UCGameplayAbility_Pattern_Area::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 타격 전에 패턴이 끝나거나 취소되면 장판도 같이 사라짐
	RemoveAoE();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCGameplayAbility_Pattern_Area::OnPatternHitBegin(float Duration)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!AoEClass || !Avatar) return;

	RemoveAoE();

	FActorSpawnParameters Params;
	Params.Owner = Avatar;
	Params.Instigator = Cast<APawn>(Avatar);

	// 바닥과 겹치지 않게 살짝 띄움
	const FVector Location = GetHitCenter() + FVector(0.0f, 0.0f, 1.0f);
	const FRotator Rotation(0.0f, Avatar->GetActorRotation().Yaw, 0.0f);

	AoE = Avatar->GetWorld()->SpawnActor<ACAoE>(AoEClass, Location, Rotation, Params);
	if (!AoE) return;

	const FCBossAttackData* Row = Attack.GetRow<FCBossAttackData>(GetName());
	AoE->Telegraph(HitArea, Duration, Row ? Row->ResponseTag : FGameplayTag());
}

void UCGameplayAbility_Pattern_Area::OnPatternHit()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	UWorld* World = Avatar->GetWorld();
	const FVector Center = GetHitCenter();
	const FVector Forward = Avatar->GetActorForwardVector().GetSafeNormal2D();

	TArray<AActor*> Targets;
	HitArea.GetTargets(World, Center, Forward, Avatar, Targets);

	for (AActor* Target : Targets)
		ApplyDamageEffect(Target, DamageEffect, Attack);

	if (IsValid(AoE))
		AoE->FinishTelegraph();
	AoE = nullptr;

	if (CDebug::bDrawHit) {
		const FColor Color = Targets.IsEmpty() ? FColor::Green : FColor::Red;
		DrawDebugCircle(World, Center, HitArea.Radius, 32, Color, false, 1.0f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
		if (HitArea.InnerRadius > 0.0f)
			DrawDebugCircle(World, Center, HitArea.InnerRadius, 32, Color, false, 1.0f, 0, 2.0f, FVector::ForwardVector, FVector::RightVector, false);
	}
}

FVector UCGameplayAbility_Pattern_Area::GetHitCenter() const
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	FVector Center = Avatar ? Avatar->GetActorLocation() : FVector::ZeroVector;

	if (const ACharacter* Character = Cast<ACharacter>(Avatar))
		Center.Z -= Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	return Center;
}

void UCGameplayAbility_Pattern_Area::RemoveAoE()
{
	if (IsValid(AoE))
		AoE->Deactivate();

	AoE = nullptr;
}
