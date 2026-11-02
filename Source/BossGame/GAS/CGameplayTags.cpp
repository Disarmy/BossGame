// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameplayTags.h"

namespace CTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Attack, "Input.Attack", "기본 공격 입력");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Dodge, "Input.Dodge", "회피 입력");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "사망");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Invincible, "State.Invincible", "무적");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit, "Event.Hit", "피격이벤트");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lock_Move, "Lock.Move", "이동 차단");
}