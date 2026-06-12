#pragma once

#include "CoreMinimal.h"

/**
 * Pure math for the top-down hero — no engine state, fully unit-testable.
 * Screen convention (camera at fixed yaw 0 looking down):
 *   screen UP    = world +X
 *   screen RIGHT = world +Y
 * Input convention: X = right, Y = up (sticks and WASD are mapped to this).
 */
namespace WDHeroMath
{
	/** 2D input (X right, Y up) → normalized world direction on the ground plane. */
	inline FVector InputToWorldDirection(const FVector2D& Input)
	{
		const FVector Dir(Input.Y, Input.X, 0.f);
		return Dir.IsNearlyZero() ? FVector::ZeroVector : Dir.GetSafeNormal();
	}

	/** World direction → yaw in degrees. */
	inline float DirectionToYaw(const FVector& WorldDirection)
	{
		return FMath::RadiansToDegrees(FMath::Atan2(WorldDirection.Y, WorldDirection.X));
	}

	/**
	 * Signed angle (degrees, -180..180) between facing and velocity.
	 * Feeds the strafe blendspace: 0 = running forward, 90 = strafing right,
	 * ±180 = backpedaling, -90 = strafing left.
	 */
	inline float ComputeMoveDirectionAngle(const FVector& Velocity, const FVector& Forward)
	{
		if (Velocity.IsNearlyZero())
		{
			return 0.f;
		}
		const FVector V = Velocity.GetSafeNormal2D();
		const FVector F = Forward.GetSafeNormal2D();
		const float Dot = FVector::DotProduct(F, V);
		const float Det = F.X * V.Y - F.Y * V.X;
		return FMath::RadiansToDegrees(FMath::Atan2(Det, Dot));
	}
}
