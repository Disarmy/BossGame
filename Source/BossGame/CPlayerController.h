#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "CPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
struct FInputActionValue;

UCLASS()
class BOSSGAME_API ACPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACPlayerController();

	bool GetAimLocation(FVector& OutLocation) const;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void OnMove(const FInputActionValue& Value);
	void OnMoveStarted(const FInputActionValue& Value);
	void OnAbilityInput(FGameplayTag InputTag);
	bool TryCancelCurAction(UAbilitySystemComponent* ASC);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<TObjectPtr<UInputAction>, FGameplayTag> AbilityInputs;

	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float StopDistance = 30.0f;   // 캡슐끼리 닿기 직전 여유

private:
	// 캔슬 구간(State.CanCancel)에서 끊을 수 있는 동작
	FGameplayTagContainer CancelTags;
};
