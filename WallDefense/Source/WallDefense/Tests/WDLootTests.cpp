// Automated tests for the loot chain: drop rolls and run bundles (step 6).

#include "Misc/AutomationTest.h"
#include "Loot/WDLootDropComponent.h"
#include "Core/WDTypes.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDLootRollsTest,
	"WallDefense.Loot.DeterministicRolls",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDLootRollsTest::RunTest(const FString& Parameters)
{
	// Guaranteed resource (chance 1): gold within bounds + XP + the element's material.
	FRandomStream Rng(42);
	const TArray<FWDLootRoll> Rolls = UWDLootDropComponent::BuildDrops(
		/*GoldMin=*/3, /*GoldMax=*/6, /*XP=*/4, /*ResourceChance=*/1.f,
		/*ResourceAmount=*/2, EWDElement::Fire, EWDResourceTier::Fragments, Rng);

	TestEqual(TEXT("Gold + XP + resource"), Rolls.Num(), 3);
	int32 GoldAmount = -1, XPAmount = -1, ResourceAmount = -1;
	for (const FWDLootRoll& Roll : Rolls)
	{
		switch (Roll.Type)
		{
		case EWDLootType::Gold:     GoldAmount = Roll.Amount; break;
		case EWDLootType::XP:       XPAmount = Roll.Amount; break;
		case EWDLootType::Resource:
			ResourceAmount = Roll.Amount;
			TestEqual(TEXT("Resource carries the monster's element"), static_cast<int32>(Roll.Element), static_cast<int32>(EWDElement::Fire));
			break;
		}
	}
	TestTrue(TEXT("Gold within [3,6]"), GoldAmount >= 3 && GoldAmount <= 6);
	TestEqual(TEXT("XP as configured"), XPAmount, 4);
	TestEqual(TEXT("Resource amount as configured"), ResourceAmount, 2);

	// Chance 0: never a resource. Zero gold/XP: no empty piles either.
	FRandomStream Rng2(7);
	const TArray<FWDLootRoll> NoLoot = UWDLootDropComponent::BuildDrops(0, 0, 0, 0.f, 5, EWDElement::Ice, EWDResourceTier::Fragments, Rng2);
	TestEqual(TEXT("Nothing configured, nothing dropped"), NoLoot.Num(), 0);

	// Same seed = same rolls (the math is testable by design).
	FRandomStream RngA(123), RngB(123);
	const TArray<FWDLootRoll> A = UWDLootDropComponent::BuildDrops(1, 100, 5, 0.5f, 1, EWDElement::Fire, EWDResourceTier::Fragments, RngA);
	const TArray<FWDLootRoll> B = UWDLootDropComponent::BuildDrops(1, 100, 5, 0.5f, 1, EWDElement::Fire, EWDResourceTier::Fragments, RngB);
	TestEqual(TEXT("Deterministic with the same seed"), A.Num(), B.Num());
	for (int32 i = 0; i < A.Num() && i < B.Num(); ++i)
	{
		TestEqual(TEXT("Same amounts"), A[i].Amount, B[i].Amount);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDLootBundleTest,
	"WallDefense.Loot.BundleMerge",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDLootBundleTest::RunTest(const FString& Parameters)
{
	FWDLootBundle Bundle;
	TestTrue(TEXT("Starts empty"), Bundle.IsEmpty());

	Bundle.Add(EWDLootType::Gold, EWDElement::Normal, EWDResourceTier::Fragments, 10);
	Bundle.Add(EWDLootType::XP, EWDElement::Normal, EWDResourceTier::Fragments, 5);
	Bundle.Add(EWDLootType::Resource, EWDElement::Fire, EWDResourceTier::Fragments, 2);
	Bundle.Add(EWDLootType::Resource, EWDElement::Fire, EWDResourceTier::Fragments, 3);
	Bundle.Add(EWDLootType::Resource, EWDElement::Ice, EWDResourceTier::Fragments, 1);
	Bundle.Add(EWDLootType::Resource, EWDElement::Fire, EWDResourceTier::Crystals, 4);

	TestEqual(TEXT("Gold accumulates"), Bundle.Gold, 10);
	TestEqual(TEXT("XP accumulates"), Bundle.XP, 5);
	TestEqual(TEXT("Same element+tier merges, others stay separate"), Bundle.Resources.Num(), 3);
	for (const FWDResourceStack& Stack : Bundle.Resources)
	{
		if (Stack.Element == EWDElement::Fire && Stack.Tier == EWDResourceTier::Fragments)
		{
			TestEqual(TEXT("Fire fragments merged 2+3"), Stack.Amount, 5);
		}
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
