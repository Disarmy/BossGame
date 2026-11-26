#pragma once

#include "CoreMinimal.h"
#include "CEnemy.h"
#include "CBoss.generated.h"

UCLASS()
class BOSSGAME_API ACBoss : public ACEnemy
{
	GENERATED_BODY()
	
public:
	ACBoss();

	virtual void ApplyKnockback(const FVector& Origin, float Distance, float Duration) override;
};
