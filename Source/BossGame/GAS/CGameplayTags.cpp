// Fill out your copyright notice in the Description page of Project Settings.


#include "CGameplayTags.h"

namespace CTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Attack, "Input.Attack", "기본 공격 입력");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Dodge, "Input.Dodge", "회피 입력");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "사망");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Invincible, "State.Invincible", "무적");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CanCombo, "State.CanCombo", "콤보 선입력 구간");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_CanCancel, "State.CanCancel", "후딜 캔슬 구간");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit, "Event.Hit", "피격이벤트");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit_Flinch, "Event.Hit.Flinch", "경직 피격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit_Down, "Event.Hit.Down", "다운 피격");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Combo_Check, "Event.Combo.Check", "콤보 판정 지점");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Attack_Hit, "Event.Attack.Hit", "공격 판정");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lock_Move, "Lock.Move", "이동 차단");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack, "Ability.Attack", "기본 공격 Ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Dodge, "Ability.Dodge", "회피 Ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Down, "Ability.Down", "다운 Ability");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_Damage, "Data.Damage", "데미지 수치");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_HitReact_Flinch, "Data.HitReact.Flinch", "경직 데미지");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Data_HitReact_Down, "Data.HitReact.Down", "다운 데미지");
}