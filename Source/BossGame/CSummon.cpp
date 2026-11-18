#include "CSummon.h"
#include "CStencilComponent.h"

ACSummon::ACSummon()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	StencilComponent = CreateDefaultSubobject<UCStencilComponent>(TEXT("StencilComponent"));
	StencilComponent->SetStencil(EStencilType::KeepColor);
}