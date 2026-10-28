// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CTypes.generated.h"   // 항상 마지막 include

// Cursor 채널용
#define ECC_Cursor ECC_GameTraceChannel1

// CustomDepth Stencil 값. 포스트 프로세스 머티리얼의 Custom 노드 비교값과 일치해야 함
UENUM(BlueprintType)
enum class EStencilType : uint8
{
	None = 0 UMETA(DisplayName = "None"),       // 흑백
	KeepColor = 1 UMETA(DisplayName = "Keep Color"), // 장판·무기·공격 이펙트
	Enemy = 2 UMETA(DisplayName = "Enemy"),      // 붉은 외곽선
	Chest = 3 UMETA(DisplayName = "Chest"),      // 푸른 외곽선
	Player = 4 UMETA(DisplayName = "Player"),     // 흰 외곽선 (항상)
	Boss = 5 UMETA(DisplayName = "Boss"),       // 어두운 실루엣
};