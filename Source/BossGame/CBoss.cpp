#include "CBoss.h"
#include "CTypes.h"
#include "CStencilComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACBoss::ACBoss()
{
	StencilComponent->SetStencil(EStencilType::Boss);

	// 위치 고정
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	// AI 포커스 방향으로 RotationRate만큼 회전
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 120.0f, 0.0f);

	// 커서 트레이스가 보스 몸을 통과해 바닥에 찍히도록
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Cursor, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Cursor, ECR_Ignore);

	// 플레이어 클릭 이동 경로가 보스를 돌아가게 함
	GetCapsuleComponent()->SetCanEverAffectNavigation(true);
	GetCapsuleComponent()->bDynamicObstacle = true;
}
