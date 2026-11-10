// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace CTags
{
	// Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Dodge);

	// State
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanCombo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_CanCancel);

	// Event
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit); 
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit_Flinch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit_Down);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Check);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Skill_Hit);

	// Lock
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lock_Move);

	// Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Down);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill);

	// Data
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_HitReact_Flinch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_HitReact_Down);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Cooldown);
}