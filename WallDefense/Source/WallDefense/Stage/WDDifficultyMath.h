#pragma once

#include "CoreMinimal.h"
#include "Core/WDTypes.h"

/**
 * Normal / Hard / Enfer multipliers (GDD §2.5) — pure, unit tested. Debug values;
 * the real DT_DifficultyModes DataTable replaces these at the balancing pass.
 * The modes structure the 1-100 climb: each drops its own material tier.
 */
namespace WDDifficultyMath
{
	inline float HealthMultiplier(EWDDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EWDDifficulty::Hard: return 2.2f;
		case EWDDifficulty::Hell: return 4.5f;
		default:                  return 1.f;
		}
	}

	inline float WallDamageMultiplier(EWDDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EWDDifficulty::Hard: return 1.4f;
		case EWDDifficulty::Hell: return 2.f;
		default:                  return 1.f;
		}
	}

	/** Harder modes add elemental resistances on top of each sheet (GDD §2.5). */
	inline int32 ExtraResistances(EWDDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EWDDifficulty::Hard: return 1;
		case EWDDifficulty::Hell: return 2;
		default:                  return 0;
		}
	}

	inline float DropAmountMultiplier(EWDDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EWDDifficulty::Hard: return 1.6f;
		case EWDDifficulty::Hell: return 2.4f;
		default:                  return 1.f;
		}
	}

	/** Each mode drops its own tier: Fragments / Cristaux / Noyaux. */
	inline EWDResourceTier DropTier(EWDDifficulty Difficulty)
	{
		switch (Difficulty)
		{
		case EWDDifficulty::Hard: return EWDResourceTier::Crystals;
		case EWDDifficulty::Hell: return EWDResourceTier::Cores;
		default:                  return EWDResourceTier::Fragments;
		}
	}
}
