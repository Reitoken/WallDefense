#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaneGrid.generated.h"

class USceneComponent;

/**
 * Defines a set of parallel lanes (Mega Man Battle Network style).
 * Lanes run along the actor's forward (local +X) axis and are separated along the
 * actor's right (local +Y) axis. Columns subdivide each lane along its length.
 * Rotate/move this actor to orient the whole lane system; all queries respect it.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API ALaneGrid : public AActor
{
	GENERATED_BODY()

public:
	ALaneGrid();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	int32 GetNumLanes() const { return FMath::Max(1, NumLanes); }

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	int32 GetNumColumns() const { return FMath::Max(1, NumColumns); }

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	float GetLaneSpacing() const { return LaneSpacing; }

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	float GetLaneLength() const { return LaneLength; }

	/** World direction the lanes run along (monsters travel this; player moves up/down this). */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	FVector GetLaneForwardVector() const;

	/** World direction separating the lanes (left/right lane changes). */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	FVector GetLaneRightVector() const;

	/** Lateral offset (signed) of a lane from the grid center, along the right vector. */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	float GetLaneLateralOffset(int32 LaneIndex) const;

	/** World point on a lane centerline at along-distance 0 (grid origin projected to the lane). */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	FVector GetLaneCenterWorld(int32 LaneIndex) const;

	/** World point on a lane at a given along-distance from the grid center. */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	FVector GetLanePointWorld(int32 LaneIndex, float AlongDistance) const;

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	int32 GetNearestLaneIndex(const FVector& WorldLocation) const;

	/** Clamp an along-distance to the playable lane length. */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	float ClampAlong(float AlongDistance) const;

	/** Along-distance of a column's center (for future column jumps). */
	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	float GetColumnAlongDistance(int32 ColumnIndex) const;

	UFUNCTION(BlueprintPure, Category = "LaneGrid")
	int32 GetNearestColumnIndex(float AlongDistance) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LaneGrid")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid", meta = (ClampMin = "1"))
	int32 NumLanes = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid", meta = (ClampMin = "1.0"))
	float LaneSpacing = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid", meta = (ClampMin = "1.0"))
	float LaneLength = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid", meta = (ClampMin = "1"))
	int32 NumColumns = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid|Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaneGrid|Debug")
	float DebugZOffset = 5.f;
};
