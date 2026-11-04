// Fill out your copyright notice in the Description page of Project Settings.


#include "CEnemy.h"
#include "CStencilComponent.h"
#include "AIController.h"

ACEnemy::ACEnemy()
{
	StencilComponent->SetStencil(EStencilType::Enemy);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// 추후 몬스터 AI 만들면 수정
	AIControllerClass = AAIController::StaticClass();
}
