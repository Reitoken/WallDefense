#pragma once

#include "CoreMinimal.h"
#include "Core/WDTypes.h"

/**
 * Pure progression math — unit tested, debug values (real curves land with the
 * economy pass, GDD §13.2; they become UCurveFloat in DataAssets later §11.4).
 */
namespace WDProgressionMath
{
	/** Cumulative XP required to BE character level L (level 1 = 0, level 2 = 100, level 3 = 300...). */
	inline int32 XPForLevel(int32 Level)
	{
		return Level <= 1 ? 0 : 50 * Level * (Level - 1);
	}

	inline int32 LevelForXP(int32 XP)
	{
		int32 Level = 1;
		while (Level < 100 && XP >= XPForLevel(Level + 1))
		{
			++Level;
		}
		return Level;
	}

	/** Loot attraction range grows with the character level (GDD §4). */
	inline float MagnetRadiusForLevel(int32 Level)
	{
		return 350.f + 30.f * (Level - 1);
	}

	/** Each weapon level adds flat damage (the 1-100 milestone behaviors arrive at step 7). */
	inline float WeaponDamageMultiplier(int32 WeaponLevel)
	{
		return 1.f + 0.08f * (WeaponLevel - 1);
	}

	/** Which material tier the next weapon level costs (GDD §2.5: ~1-40 / 41-70 / 71-100). */
	inline EWDResourceTier TierForWeaponLevel(int32 Level)
	{
		if (Level <= 40)
		{
			return EWDResourceTier::Fragments;
		}
		return Level <= 70 ? EWDResourceTier::Crystals : EWDResourceTier::Cores;
	}

	inline int32 WeaponUpgradeGoldCost(int32 CurrentLevel)     { return 20 + 10 * CurrentLevel; }
	inline int32 WeaponUpgradeResourceCost(int32 CurrentLevel) { return 1 + CurrentLevel / 4; }
	inline int32 WallUpgradeGoldCost(int32 CurrentLevel)       { return 120 * CurrentLevel; }

	/** Star multiplier on drops, rounded UP — the player never loses a fraction (GDD §2.2). */
	inline int32 ApplyLootMultiplier(int32 Amount, float Multiplier)
	{
		return FMath::CeilToInt32(Amount * Multiplier);
	}

	/** Fixed end-of-stage reward per star (GDD §2.2) — NOT multiplied. */
	inline int32 StarBonusGold(int32 Stars, int32 StageNumber)
	{
		return 30 * Stars * StageNumber;
	}
}
