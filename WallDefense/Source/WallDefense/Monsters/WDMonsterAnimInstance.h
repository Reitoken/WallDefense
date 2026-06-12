#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WDMonsterAnimInstance.generated.h"

/**
 * Base AnimInstance for monsters: C++ exposes the state, the AnimBP only does transitions
 * (ArchitectureTechnique §6.5). Becomes relevant once monsters get skeletal meshes —
 * the variables already work today.
 */
UCLASS()
class WALLDEFENSE_API UWDMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bHasShield = false;
};
