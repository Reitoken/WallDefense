#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/WDTypes.h"
#include "WDProgressionSubsystem.generated.h"

/** Best result on one stage in one mode (stars counted per mode, GDD §2.5). */
USTRUCT(BlueprintType)
struct FWDStageRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 StageNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	EWDDifficulty Mode = EWDDifficulty::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 Stars = 0;
};

/** The whole meta state of one save slot — snapshot serialized by the SaveSubsystem. */
USTRUCT(BlueprintType)
struct FWDProgressionState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 Gold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 XP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<FWDResourceStack> Resources;

	/** Weapon = its element (one weapon per element, GDD §5). Absent = level 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TMap<EWDElement, int32> WeaponLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 WallLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<FWDStageRecord> StageRecords;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnGoldChanged, int32, NewGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnXPChanged, int32, NewXP, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnResourceChanged, const FWDResourceStack&, Stack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnWallUpgraded, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnWeaponLeveledUp, EWDElement, Element, int32, NewLevel);

/**
 * The player's meta state (ArchitectureTechnique §5): gold, elemental resources,
 * XP/level, weapon levels, wall level, stars per stage×mode. Pure state + rules +
 * events — reads nothing from disk (the SaveSubsystem owns persistence).
 */
UCLASS()
class WALLDEFENSE_API UWDProgressionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// --- Wallet ---
	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetGold() const { return State.Gold; }

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	bool TrySpendGold(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetResourceAmount(EWDElement Element, EWDResourceTier Tier) const;

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	void AddResource(EWDElement Element, EWDResourceTier Tier, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	bool TrySpendResource(EWDElement Element, EWDResourceTier Tier, int32 Amount);

	// --- Character XP ---
	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetXP() const { return State.XP; }

	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	void AddXP(int32 Amount);

	// --- Upgrades (costs from WDProgressionMath until the economy pass) ---
	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetWeaponLevel(EWDElement Element) const;

	/** Spends gold + the element's material; fails untouched if either is missing. */
	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	bool TryLevelUpWeapon(EWDElement Element);

	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetWallLevel() const { return State.WallLevel; }

	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	bool TryUpgradeWall(int32 MaxLevel);

	// --- Stage results ---
	UFUNCTION(BlueprintPure, Category = "WD|Progression")
	int32 GetBestStars(int32 StageNumber, EWDDifficulty Mode) const;

	/** Keeps the best score. Returns true when it is a new record. */
	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	bool RegisterStageResult(int32 StageNumber, EWDDifficulty Mode, int32 Stars);

	/** Applies a run's loot × star multiplier (each entry rounded up). Returns what was granted. */
	UFUNCTION(BlueprintCallable, Category = "WD|Progression")
	FWDLootBundle ApplyRunRewards(const FWDLootBundle& RunLoot, float Multiplier);

	// --- Snapshot (SaveSubsystem only) ---
	const FWDProgressionState& GetState() const { return State; }
	void SetState(const FWDProgressionState& NewState);
	void ResetState();

	// --- Events ---
	UPROPERTY(BlueprintAssignable, Category = "WD|Progression")
	FWDOnGoldChanged OnGoldChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Progression")
	FWDOnXPChanged OnXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Progression")
	FWDOnResourceChanged OnResourceChanged;

	UPROPERTY(BlueprintAssignable, Category = "WD|Progression")
	FWDOnWallUpgraded OnWallUpgraded;

	UPROPERTY(BlueprintAssignable, Category = "WD|Progression")
	FWDOnWeaponLeveledUp OnWeaponLeveledUp;

private:
	FWDResourceStack* FindStack(EWDElement Element, EWDResourceTier Tier);
	void BroadcastAll();

	FWDProgressionState State;
};
