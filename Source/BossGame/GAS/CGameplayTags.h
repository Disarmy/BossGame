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
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combo_Check);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Hit);

	// Lock
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Lock_Move);

	// Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge);

	// Data
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_Damage);
}