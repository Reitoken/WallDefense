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

	const FVector DeathSpot = GetOwner()->GetActorLocation();
	for (const FWDLootRoll& Roll : Rolls)
	{
		// Each drop pops out of the corpse and bounces to its own landing spot around it.
		const float Angle = Rng.FRandRange(0.f, 2.f * PI);
		const float Distance = Rng.FRandRange(90.f, 230.f);
		const FVector LandingSpot(DeathSpot.X + FMath::Cos(Angle) * Distance, DeathSpot.Y + FMath::Sin(Angle) * Distance, 40.f);
		const FVector SpawnSpot(DeathSpot.X, DeathSpot.Y, FMath::Max(DeathSpot.Z, 60.f));

		if (AWDLootPickup* Pickup = GetWorld()->SpawnActor<AWDLootPickup>(SpawnSpot, FRotator::ZeroRotator, Params))
		{
			Pickup->Init(Roll.Type, Roll.Element, Roll.Tier, Roll.Amount);
			Pickup->StartBounceTo(LandingSpot);
		}
	}
}
