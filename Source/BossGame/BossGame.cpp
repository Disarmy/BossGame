#include "BossGame.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BossGame, "BossGame" );

namespace CDebug
{
	bool bDrawHit = false;

	static FAutoConsoleVariableRef CVarDrawHit(
		TEXT("BossGame.DrawHit"), bDrawHit, TEXT("Draw weapon trace and skill hit area"));
}