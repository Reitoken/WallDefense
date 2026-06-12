#pragma once

#include "CoreMinimal.h"

/**
 * Pure stage-outcome math (GDD §2.2) — unit tested, no gameplay dependency.
 * 100% wall HP = 3 stars, the rest split in fair thirds; victory even at 0 stars.
 */
namespace WDStageMath
{
	/** Wall HP fraction at victory -> stars. */
	inline int32 StarsFromWallHealth(float HealthPercent)
	{
		if (HealthPercent >= 1.f - KINDA_SMALL_NUMBER)
		{
			return 3;
		}
		if (HealthPercent >= 0.66f)
		{
			return 2;
		}
		if (HealthPercent >= 0.33f)
		{
			return 1;
		}
		return 0;
	}

	/** Drop/reward multiplier per star count: ×1 / ×1.25 / ×1.5 / ×2 (GDD §2.2). */
	inline float RewardMultiplierForStars(int32 Stars)
	{
		switch (Stars)
		{
		case 3:  return 2.f;
		case 2:  return 1.5f;
		case 1:  return 1.25f;
		default: return 1.f;
		}
	}
}
