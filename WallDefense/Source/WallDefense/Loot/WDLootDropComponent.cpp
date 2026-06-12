#include "Loot/WDLootDropComponent.h"
#include "Loot/WDLootPickup.h"

void UWDLootDropComponent::Configure(int32 InGoldMin, int32 InGoldMax, int32 InXP, float InResourceChance,
	int32 InResourceAmount, EWDElement InResourceElement, EWDResourceTier InTier)
{
	GoldMin = InGoldMin;
	GoldMax = InGoldMax;
	XP = InXP;
	ResourceChance = InResourceChance;
	ResourceAmount = InResourceAmount;
	ResourceElement = InResourceElement;
	Tier = InTier;
}

TArray<FWDLootRoll> UWDLootDropComponent::BuildDrops(int32 GoldMin, int32 GoldMax, int32 XP, float ResourceChance,
	int32 ResourceAmount, EWDElement ResourceElement, EWDResourceTier Tier, FRandomStream& Rng)
{
	TArray<FWDLootRoll> Rolls;

	const int32 Gold = Rng.RandRange(GoldMin, GoldMax);
	if (Gold > 0)
	{
		Rolls.Add({ EWDLootType::Gold, EWDElement::Normal, Tier, Gold });
	}
	if (XP > 0)
	{
		Rolls.Add({ EWDLootType::XP, EWDElement::Normal, Tier, XP });
	}
	if (ResourceAmount > 0 && Rng.FRand() < ResourceChance)
	{
		Rolls.Add({ EWDLootType::Resource, ResourceElement, Tier, ResourceAmount });
	}
	return Rolls;
}

void UWDLootDropComponent::SpawnDrops()
{
	FRandomStream Rng(FMath::Rand());
	const TArray<FWDLootRoll> Rolls = BuildDrops(GoldMin, GoldMax, XP, ResourceChance, ResourceAmount, ResourceElement, Tier, Rng);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const FWDLootRoll& Roll : Rolls)
	{
		// Scatter around the corpse so piles never overlap perfectly.
		const float Angle = Rng.FRandRange(0.f, 2.f * PI);
		const float Distance = Rng.FRandRange(50.f, 140.f);
		const FVector Location = GetOwner()->GetActorLocation() * FVector(1.f, 1.f, 0.f)
			+ FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 40.f);

		if (AWDLootPickup* Pickup = GetWorld()->SpawnActor<AWDLootPickup>(Location, FRotator::ZeroRotator, Params))
		{
			Pickup->Init(Roll.Type, Roll.Element, Roll.Tier, Roll.Amount);
		}
	}
}
