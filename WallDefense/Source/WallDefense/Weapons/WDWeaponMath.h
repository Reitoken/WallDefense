#pragma once

#include "CoreMinimal.h"

/** Pure math for projectiles and shots — fully unit-testable. */
namespace WDWeaponMath
{
	/** Bounce a direction off a surface normal. */
	inline FVector Reflect(const FVector& Direction, const FVector& Normal)
	{
		return FMath::GetReflectionVector(Direction, Normal).GetSafeNormal();
	}

	/** Evenly spaced horizontal directions in a fan of TotalAngleDeg centered on Base. Count 1 = Base itself. */
	inline TArray<FVector> ComputeSpreadDirections(const FVector& Base, int32 Count, float TotalAngleDeg)
	{
		TArray<FVector> Result;
		if (Count <= 0)
		{
			return Result;
		}
		const FVector Flat = Base.GetSafeNormal2D();
		if (Count == 1)
		{
			Result.Add(Flat);
			return Result;
		}
		const float Step = TotalAngleDeg / (Count - 1);
		const float Start = -TotalAngleDeg * 0.5f;
		for (int32 i = 0; i < Count; ++i)
		{
			Result.Add(Flat.RotateAngleAxis(Start + Step * i, FVector::UpVector));
		}
		return Result;
	}

	/** Full 360° ring of horizontal directions (fragments). */
	inline TArray<FVector> ComputeRingDirections(int32 Count)
	{
		TArray<FVector> Result;
		for (int32 i = 0; i < Count; ++i)
		{
			const float AngleRad = 2.f * PI * i / FMath::Max(1, Count);
			Result.Add(FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.f));
		}
		return Result;
	}

	/** Steer Current toward Desired, turning at most MaxTurnDeg. Both flattened/normalized. */
	inline FVector ComputeHomingDirection(const FVector& Current, const FVector& Desired, float MaxTurnDeg)
	{
		const FVector From = Current.GetSafeNormal2D();
		const FVector To = Desired.GetSafeNormal2D();
		if (From.IsNearlyZero() || To.IsNearlyZero())
		{
			return From;
		}
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(From, To), -1.f, 1.f)));
		if (AngleDeg <= MaxTurnDeg)
		{
			return To;
		}
		const float Sign = (From.X * To.Y - From.Y * To.X) >= 0.f ? 1.f : -1.f;
		return From.RotateAngleAxis(Sign * MaxTurnDeg, FVector::UpVector);
	}
}
