#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/WDTypes.h"
#include "WDLootDropComponent.generated.h"

/** One rolled drop, ready to spawn. Plain data so the roll logic is unit-testable. */
struct FWDLootRoll
{
	EWDLootType Type = EWDLootType::Gold;
	EWDElement Element = EWDElement::Normal;
	EWDResourceTier Tier = EWDResourceTier::Fragments;
	int32 Amount = 0;
};

/**
 * Spawns the owner's drops when it dies (GDD §7: gold, XP, the monster's elemental
 * resource). The owner wires it (calls SpawnDrops from its death handler) — this
 * component knows no other component (ArchitectureTechnique §10).
 */
UCLASS(ClassGroup = (Loot), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDLootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void Configure(int32 InGoldMin, int32 InGoldMax, int32 InXP, float InResourceChance,
		int32 InResourceAmount, EWDElement InResourceElement, EWDResourceTier InTier);

	/** Pure roll logic — deterministic with a seeded stream (unit tested). */
	static TArray<FWDLootRoll> BuildDrops(int32 GoldMin, int32 GoldMax, int32 XP, float ResourceChance,
		int32 ResourceAmount, EWDElement ResourceElement, EWDResourceTier Tier, FRandomStream& Rng);

	/** Rolls and scatters the pickups around the owner. Call once, at death. */
	UFUNCTION(BlueprintCallable, Category = "WD|Loot")
	void SpawnDrops();

private:
	int32 GoldMin = 0;
	int32 GoldMax = 0;
	int32 XP = 0;
	float ResourceChance = 0.f;
	int32 ResourceAmount = 0;
	EWDElement ResourceElement = EWDElement::Normal;
	EWDResourceTier Tier = EWDResourceTier::Fragments;
};
