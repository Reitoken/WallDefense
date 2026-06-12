#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WDHeroAnimInstance.generated.h"

/**
 * Base AnimInstance for the heroine: C++ exposes the state, the AnimBP only does transitions
 * (ArchitectureTechnique §6.5). Create the AnimBP with this as parent class — the variables
 * below are immediately available in the AnimGraph.
 * Weapon-related variables (bIsFiring, ActiveElement) are fed from step 3 on.
 */
UCLASS()
class WALLDEFENSE_API UWDHeroAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** Planar speed (uu/s). */
	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bIsMoving = false;

	/** Velocity vs facing, -180..180 — drives the strafe blendspace. */
	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	float MoveDirection = 0.f;

	/** Fed by the weapon component from step 3. */
	UPROPERTY(BlueprintReadOnly, Category = "WD|Animation")
	bool bIsFiring = false;
};
