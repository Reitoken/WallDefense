#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WDHeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

/**
 * The top-down heroine (free movement, Zelda-like camera).
 * Only moves and faces — weapons, magnet and outfit are separate components (steps 3+).
 * She cannot die: only the wall matters (GDD §4).
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWDHeroCharacter();

	virtual void Tick(float DeltaSeconds) override;

	/** Move on the ground plane. Input convention: X = right, Y = up (screen space). */
	UFUNCTION(BlueprintCallable, Category = "WD|Hero")
	void Move(const FVector2D& Input);

	/** Face a world direction (from right stick or mouse cursor). Movement does NOT rotate her. */
	UFUNCTION(BlueprintCallable, Category = "WD|Hero")
	void SetAimDirection(const FVector& WorldDirection);

	// --- Animation state (read by UWDHeroAnimInstance) ---

	UFUNCTION(BlueprintPure, Category = "WD|Hero|Animation")
	float GetSpeed() const;

	UFUNCTION(BlueprintPure, Category = "WD|Hero|Animation")
	bool IsMoving() const;

	/** Signed angle velocity-vs-facing for the strafe blendspace (-180..180). */
	UFUNCTION(BlueprintPure, Category = "WD|Hero|Animation")
	float GetMoveDirectionAngle() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Hero")
	TObjectPtr<USpringArmComponent> CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Hero")
	TObjectPtr<UCameraComponent> Camera;

	/** Debug body (engine capsule mesh): the collision capsule is invisible in game. Replaced by the real mesh later. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Hero")
	TObjectPtr<UStaticMeshComponent> DebugBody;

	/** Debug nose: shows the facing at a glance from above. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Hero")
	TObjectPtr<UStaticMeshComponent> DebugNose;

	/** Higher = snappier turn toward the aim direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Hero", meta = (ClampMin = "0.0"))
	float FacingInterpSpeed = 14.f;

	/** Camera distance (Zelda-like top-down). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Hero|Camera", meta = (ClampMin = "200.0"))
	float CameraDistance = 1800.f;

	/** Camera pitch — -90 = straight down, -55..-65 = slight Zelda tilt. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Hero|Camera", meta = (ClampMin = "-90.0", ClampMax = "-30.0"))
	float CameraPitch = -62.f;

	/** Below this speed she is considered idle (uu/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Hero|Animation", meta = (ClampMin = "0.0"))
	float IdleSpeedThreshold = 5.f;

private:
	float DesiredYaw = 0.f;
};
