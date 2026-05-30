#pragma once

#include "CoreMinimal.h"
#include "Player/BasePlayerCharacter.h"
#include "LanePlayerCharacter.generated.h"

class ALaneGrid;

/**
 * Second mobility test (Mega Man Battle Network style).
 * - Left/Right: jump between lanes (one lane per press).
 * - Up/Down: move along the current lane, facing the move direction.
 * The player stays snapped to the lane centerline. Inherits weapon / everything else
 * from ABasePlayerCharacter; only mobility is overridden.
 */
UCLASS(Abstract, Blueprintable)
class WALLDEFENSE_API ALanePlayerCharacter : public ABasePlayerCharacter
{
	GENERATED_BODY()

public:
	ALanePlayerCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Lane")
	int32 GetCurrentLaneIndex() const { return CurrentLaneIndex; }

protected:
	virtual void BeginPlay() override;
	virtual void Move(const FInputActionValue& Value) override;

	void MoveCompleted(const FInputActionValue& Value);
	void ChangeLane(int32 Direction);
	ALaneGrid* ResolveLaneGrid();

	/** Left empty = auto-find the first ALaneGrid in the level. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Lane")
	TObjectPtr<ALaneGrid> LaneGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane", meta = (ClampMin = "0.0"))
	float AlongLaneSpeed = 600.f;

	/** Higher = snappier lane changes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane", meta = (ClampMin = "0.0"))
	float LaneChangeInterpSpeed = 14.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane", meta = (ClampMin = "0.0"))
	float FacingInterpSpeed = 14.f;

	/** Min absolute horizontal input to trigger a lane jump. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float LaneInputThreshold = 0.5f;

	/** Flip if Left/Right end up moving lanes the wrong way for your camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lane")
	bool bInvertLaneDirection = false;

	UPROPERTY(BlueprintReadOnly, Category = "Lane")
	int32 CurrentLaneIndex = 1;

private:
	bool bLaneChangeArmed = true;
	float AlongInput = 0.f;
	float DesiredYaw = 0.f;
	float FixedZ = 0.f;
};
