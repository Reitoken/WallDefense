#include "Core/WDProgressionSubsystem.h"
#include "Core/WDProgressionMath.h"

void UWDProgressionSubsystem::AddGold(int32 Amount)
{
	if (Amount > 0)
	{
		State.Gold += Amount;
		OnGoldChanged.Broadcast(State.Gold);
	}
}

bool UWDProgressionSubsystem::TrySpendGold(int32 Amount)
{
	if (Amount < 0 || State.Gold < Amount)
	{
		return false;
	}
	State.Gold -= Amount;
	OnGoldChanged.Broadcast(State.Gold);
	return true;
}

int32 UWDProgressionSubsystem::GetResourceAmount(EWDElement Element, EWDResourceTier Tier) const
{
	for (const FWDResourceStack& Stack : State.Resources)
	{
		if (Stack.Element == Element && Stack.Tier == Tier)
		{
			return Stack.Amount;
		}
	}
	return 0;
}

FWDResourceStack* UWDProgressionSubsystem::FindStack(EWDElement Element, EWDResourceTier Tier)
{
	for (FWDResourceStack& Stack : State.Resources)
	{
		if (Stack.Element == Element && Stack.Tier == Tier)
		{
			return &Stack;
		}
	}
	return nullptr;
}

void UWDProgressionSubsystem::AddResource(EWDElement Element, EWDResourceTier Tier, int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}
	if (FWDResourceStack* Stack = FindStack(Element, Tier))
	{
		Stack->Amount += Amount;
		OnResourceChanged.Broadcast(*Stack);
	}
	else
	{
		const FWDResourceStack NewStack{ Element, Tier, Amount };
		State.Resources.Add(NewStack);
		OnResourceChanged.Broadcast(NewStack);
	}
}

bool UWDProgressionSubsystem::TrySpendResource(EWDElement Element, EWDResourceTier Tier, int32 Amount)
{
	FWDResourceStack* Stack = FindStack(Element, Tier);
	if (Amount < 0 || !Stack || Stack->Amount < Amount)
	{
		return false;
	}
	Stack->Amount -= Amount;
	OnResourceChanged.Broadcast(*Stack);
	return true;
}

int32 UWDProgressionSubsystem::GetCharacterLevel() const
{
	return WDProgressionMath::LevelForXP(State.XP);
}

void UWDProgressionSubsystem::AddXP(int32 Amount)
{
	if (Amount > 0)
	{
		State.XP += Amount;
		OnXPChanged.Broadcast(State.XP, GetCharacterLevel());
	}
}

int32 UWDProgressionSubsystem::GetWeaponLevel(EWDElement Element) const
{
	const int32* Level = State.WeaponLevels.Find(Element);
	return Level ? *Level : 1;
}

bool UWDProgressionSubsystem::TryLevelUpWeapon(EWDElement Element)
{
	const int32 Level = GetWeaponLevel(Element);
	if (Level >= 100)
	{
		return false;
	}

	const int32 GoldCost = WDProgressionMath::WeaponUpgradeGoldCost(Level);
	const int32 ResourceCost = WDProgressionMath::WeaponUpgradeResourceCost(Level);
	const EWDResourceTier Tier = WDProgressionMath::TierForWeaponLevel(Level + 1);

	// Check both BEFORE spending either — a failed upgrade never costs anything.
	if (State.Gold < GoldCost || GetResourceAmount(Element, Tier) < ResourceCost)
	{
		return false;
	}
	TrySpendGold(GoldCost);
	TrySpendResource(Element, Tier, ResourceCost);

	State.WeaponLevels.Add(Element, Level + 1);
	OnWeaponLeveledUp.Broadcast(Element, Level + 1);
	return true;
}

bool UWDProgressionSubsystem::TryUpgradeWall(int32 MaxLevel)
{
	if (State.WallLevel >= MaxLevel)
	{
		return false;
	}
	if (!TrySpendGold(WDProgressionMath::WallUpgradeGoldCost(State.WallLevel)))
	{
		return false;
	}
	++State.WallLevel;
	OnWallUpgraded.Broadcast(State.WallLevel);
	return true;
}

int32 UWDProgressionSubsystem::GetBestStars(int32 StageNumber, EWDDifficulty Mode) const
{
	for (const FWDStageRecord& Record : State.StageRecords)
	{
		if (Record.StageNumber == StageNumber && Record.Mode == Mode)
		{
			return Record.Stars;
		}
	}
	return -1; // never finished
}

bool UWDProgressionSubsystem::RegisterStageResult(int32 StageNumber, EWDDifficulty Mode, int32 Stars)
{
	for (FWDStageRecord& Record : State.StageRecords)
	{
		if (Record.StageNumber == StageNumber && Record.Mode == Mode)
		{
			if (Stars <= Record.Stars)
			{
				return false;
			}
			Record.Stars = Stars;
			return true;
		}
	}
	State.StageRecords.Add({ StageNumber, Mode, Stars });
	return true;
}

FWDLootBundle UWDProgressionSubsystem::ApplyRunRewards(const FWDLootBundle& RunLoot, float Multiplier)
{
	FWDLootBundle Granted;
	Granted.Gold = WDProgressionMath::ApplyLootMultiplier(RunLoot.Gold, Multiplier);
	Granted.XP = WDProgressionMath::ApplyLootMultiplier(RunLoot.XP, Multiplier);
	for (const FWDResourceStack& Stack : RunLoot.Resources)
	{
		Granted.AddResource(Stack.Element, Stack.Tier, WDProgressionMath::ApplyLootMultiplier(Stack.Amount, Multiplier));
	}

	AddGold(Granted.Gold);
	AddXP(Granted.XP);
	for (const FWDResourceStack& Stack : Granted.Resources)
	{
		AddResource(Stack.Element, Stack.Tier, Stack.Amount);
	}
	return Granted;
}

bool UWDProgressionSubsystem::IsStageUnlocked(int32 StageNumber, EWDDifficulty Mode) const
{
	// Within a mode: the previous stage must be finished.
	if (StageNumber > 1 && GetBestStars(StageNumber - 1, Mode) < 0)
	{
		return false;
	}
	// Across modes: Hard needs the stage done in Normal, Hell needs it done in Hard (GDD §2.5).
	if (Mode == EWDDifficulty::Hard && GetBestStars(StageNumber, EWDDifficulty::Normal) < 0)
	{
		return false;
	}
	if (Mode == EWDDifficulty::Hell && GetBestStars(StageNumber, EWDDifficulty::Hard) < 0)
	{
		return false;
	}
	return true;
}

bool UWDProgressionSubsystem::RegisterMonsterDiscovered(FName MonsterName)
{
	if (MonsterName.IsNone() || State.KnownMonsters.Contains(MonsterName))
	{
		return false;
	}
	State.KnownMonsters.Add(MonsterName);
	OnDiscovery.Broadcast(MonsterName, /*bWeakness=*/false);
	return true;
}

bool UWDProgressionSubsystem::RegisterWeaknessDiscovered(FName MonsterName)
{
	if (MonsterName.IsNone() || State.KnownWeaknesses.Contains(MonsterName))
	{
		return false;
	}
	State.KnownWeaknesses.Add(MonsterName);
	OnDiscovery.Broadcast(MonsterName, /*bWeakness=*/true);
	return true;
}

void UWDProgressionSubsystem::SetState(const FWDProgressionState& NewState)
{
	State = NewState;
	BroadcastAll();
}

void UWDProgressionSubsystem::ResetState()
{
	State = FWDProgressionState();
	BroadcastAll();
}

void UWDProgressionSubsystem::BroadcastAll()
{
	OnGoldChanged.Broadcast(State.Gold);
	OnXPChanged.Broadcast(State.XP, GetCharacterLevel());
	for (const FWDResourceStack& Stack : State.Resources)
	{
		OnResourceChanged.Broadcast(Stack);
	}
	OnWallUpgraded.Broadcast(State.WallLevel);
}
