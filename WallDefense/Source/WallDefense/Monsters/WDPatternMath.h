#pragma once

#include "CoreMinimal.h"

/**
 * Pure math behind the movement patterns (GDD §6.2) — unit-testable.
 * A monster advances along its spawn->target axis; patterns modulate
 * lateral offset, speed and height over time.
 */
namespace WDPatternMath
{
	/** Sinusoidal sway: smooth left-right. */
	inline float SinusoidOffset(float Time, float Amplitude, float Frequency)
	{
		return Amplitude * FMath::Sin(2.f * PI * Frequency * Time);
	}

	/** Zigzag: triangle wave between -Amplitude and +Amplitude (sharp turns). */
	inline float ZigzagOffset(float Time, float Amplitude, float Frequency)
	{
		const float Phase = FMath::Frac(Time * Frequency);
		const float Triangle = Phase < 0.5f ? (4.f * Phase - 1.f) : (3.f - 4.f * Phase);
		return Amplitude * Triangle;
	}

	/** Charge-pause: sprints then stops, in rhythm. Returns a speed multiplier. */
	inline float ChargePauseSpeed(float Time, float ChargeDuration, float PauseDuration)
	{
		const float Cycle = ChargeDuration + PauseDuration;
		const float Phase = FMath::Fmod(Time, Cycle);
		return Phase < ChargeDuration ? 1.6f : 0.f; // sprints faster, then freezes
	}

	/** Hopper: vertical arc height (0 on the ground between hops). */
	inline float HopHeight(float Time, float HopDuration, float HopHeightMax)
	{
		const float Phase = FMath::Frac(Time / FMath::Max(0.1f, HopDuration));
		return HopHeightMax * FMath::Sin(Phase * PI);
	}

	/** Burrower: underground (true) or surfaced, alternating. */
	inline bool IsBurrowed(float Time, float SurfacedDuration, float BurrowedDuration)
	{
		const float Cycle = SurfacedDuration + BurrowedDuration;
		return FMath::Fmod(Time, Cycle) >= SurfacedDuration;
	}

	/** Flanker: hugs the side, converges near the target. 1 = full lateral push, 0 = converged. */
	inline float FlankerBlend(float DistanceToTarget, float ConvergeDistance, float StartDistance)
	{
		if (StartDistance <= ConvergeDistance)
		{
			return 0.f;
		}
		return FMath::Clamp((DistanceToTarget - ConvergeDistance) / (StartDistance - ConvergeDistance), 0.f, 1.f);
	}
}
