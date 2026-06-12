// Automated tests for the meta: progression math, wallet, upgrades, rewards, save round-trip (step 6).

#include "Misc/AutomationTest.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDSaveGame.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace WDTests
{
	/** Subsystems demand a GameInstance outer (ClassWithin) — give them a transient one. */
	static UWDProgressionSubsystem* MakeProgression()
	{
		return NewObject<UWDProgressionSubsystem>(NewObject<UGameInstance>());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDProgressionMathTest,
	"WallDefense.Progression.MathCurves",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDProgressionMathTest::RunTest(const FString& Parameters)
{
	// XP curve: level 1 free, level 2 at 100, level 3 at 300.
	TestEqual(TEXT("Level 1 costs nothing"), WDProgressionMath::XPForLevel(1), 0);
	TestEqual(TEXT("Level 2 at 100 XP"), WDProgressionMath::XPForLevel(2), 100);
	TestEqual(TEXT("Level 3 at 300 XP"), WDProgressionMath::XPForLevel(3), 300);
	TestEqual(TEXT("0 XP = level 1"), WDProgressionMath::LevelForXP(0), 1);
	TestEqual(TEXT("99 XP still level 1"), WDProgressionMath::LevelForXP(99), 1);
	TestEqual(TEXT("100 XP = level 2"), WDProgressionMath::LevelForXP(100), 2);
	TestEqual(TEXT("300 XP = level 3"), WDProgressionMath::LevelForXP(300), 3);

	// The magnet grows with the level (GDD §4).
	TestTrue(TEXT("Magnet radius grows"),
		WDProgressionMath::MagnetRadiusForLevel(5) > WDProgressionMath::MagnetRadiusForLevel(1));

	// Material tiers structure the 1-100 climb (GDD §2.5).
	TestEqual(TEXT("Level 40 still costs Fragments"), static_cast<int32>(WDProgressionMath::TierForWeaponLevel(40)), static_cast<int32>(EWDResourceTier::Fragments));
	TestEqual(TEXT("Level 41 costs Crystals"), static_cast<int32>(WDProgressionMath::TierForWeaponLevel(41)), static_cast<int32>(EWDResourceTier::Crystals));
	TestEqual(TEXT("Level 71 costs Cores"), static_cast<int32>(WDProgressionMath::TierForWeaponLevel(71)), static_cast<int32>(EWDResourceTier::Cores));

	// Multiplier rounds UP — the player never loses a fraction.
	TestEqual(TEXT("10 x1.25 = 13 (ceil)"), WDProgressionMath::ApplyLootMultiplier(10, 1.25f), 13);
	TestEqual(TEXT("10 x1 = 10"), WDProgressionMath::ApplyLootMultiplier(10, 1.f), 10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDProgressionWalletTest,
	"WallDefense.Progression.WalletAndUpgrades",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDProgressionWalletTest::RunTest(const FString& Parameters)
{
	UWDProgressionSubsystem* Progression = WDTests::MakeProgression();

	// Wallet.
	Progression->AddGold(100);
	TestTrue(TEXT("Can spend within means"), Progression->TrySpendGold(40));
	TestEqual(TEXT("60 left"), Progression->GetGold(), 60);
	TestFalse(TEXT("Cannot overspend"), Progression->TrySpendGold(100));
	TestEqual(TEXT("Failed spend costs nothing"), Progression->GetGold(), 60);

	Progression->AddResource(EWDElement::Fire, EWDResourceTier::Fragments, 5);
	TestEqual(TEXT("5 Fire fragments"), Progression->GetResourceAmount(EWDElement::Fire, EWDResourceTier::Fragments), 5);
	TestFalse(TEXT("Tiers are separate currencies"), Progression->TrySpendResource(EWDElement::Fire, EWDResourceTier::Crystals, 1));

	// Weapon upgrade: needs gold AND the element's material (level 1 -> 2: 30 gold + 1 fragment).
	UWDProgressionSubsystem* Poor = WDTests::MakeProgression();
	Poor->AddResource(EWDElement::Fire, EWDResourceTier::Fragments, 10);
	TestFalse(TEXT("No gold, no upgrade"), Poor->TryLevelUpWeapon(EWDElement::Fire));
	TestEqual(TEXT("Failed upgrade spends NO resource"), Poor->GetResourceAmount(EWDElement::Fire, EWDResourceTier::Fragments), 10);

	TestTrue(TEXT("Funded upgrade succeeds"), Progression->TryLevelUpWeapon(EWDElement::Fire));
	TestEqual(TEXT("Weapon now level 2"), Progression->GetWeaponLevel(EWDElement::Fire), 2);
	TestEqual(TEXT("Gold spent (60-30)"), Progression->GetGold(), 30);
	TestEqual(TEXT("Fragment spent (5-1)"), Progression->GetResourceAmount(EWDElement::Fire, EWDResourceTier::Fragments), 4);

	// Wall upgrade caps at the data's max level.
	Progression->AddGold(10000);
	TestTrue(TEXT("Wall 1 -> 2"), Progression->TryUpgradeWall(4));
	TestTrue(TEXT("Wall 2 -> 3"), Progression->TryUpgradeWall(4));
	TestTrue(TEXT("Wall 3 -> 4"), Progression->TryUpgradeWall(4));
	TestFalse(TEXT("Wall capped at 4"), Progression->TryUpgradeWall(4));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDProgressionRewardsTest,
	"WallDefense.Progression.RunRewardsAndStars",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDProgressionRewardsTest::RunTest(const FString& Parameters)
{
	UWDProgressionSubsystem* Progression = WDTests::MakeProgression();

	FWDLootBundle Run;
	Run.Gold = 10;
	Run.XP = 10;
	Run.AddResource(EWDElement::Fire, EWDResourceTier::Fragments, 2);

	const FWDLootBundle Granted = Progression->ApplyRunRewards(Run, 1.5f);
	TestEqual(TEXT("Gold x1.5"), Granted.Gold, 15);
	TestEqual(TEXT("XP x1.5"), Granted.XP, 15);
	TestEqual(TEXT("Resource x1.5 (ceil)"), Granted.Resources[0].Amount, 3);
	TestEqual(TEXT("Banked in the wallet"), Progression->GetGold(), 15);
	TestEqual(TEXT("Banked XP"), Progression->GetXP(), 15);

	// Stars per stage x mode: keep the best, never regress.
	TestEqual(TEXT("Unplayed stage = -1"), Progression->GetBestStars(1, EWDDifficulty::Normal), -1);
	TestTrue(TEXT("First result is a record"), Progression->RegisterStageResult(1, EWDDifficulty::Normal, 2));
	TestFalse(TEXT("Worse result is no record"), Progression->RegisterStageResult(1, EWDDifficulty::Normal, 1));
	TestEqual(TEXT("Best stays 2"), Progression->GetBestStars(1, EWDDifficulty::Normal), 2);
	TestTrue(TEXT("Better result IS a record"), Progression->RegisterStageResult(1, EWDDifficulty::Normal, 3));
	TestEqual(TEXT("Modes are separate"), Progression->GetBestStars(1, EWDDifficulty::Hard), -1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDSaveRoundTripTest,
	"WallDefense.Save.RoundTrip",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDSaveRoundTripTest::RunTest(const FString& Parameters)
{
	// Build a lived-in state.
	UWDProgressionSubsystem* Progression = WDTests::MakeProgression();
	Progression->AddGold(123);
	Progression->AddXP(450);
	Progression->AddResource(EWDElement::Ice, EWDResourceTier::Fragments, 7);
	Progression->RegisterStageResult(1, EWDDifficulty::Normal, 3);

	// Serialize to memory and back (same path as the 5 disk slots, no file I/O in tests).
	UWDSaveGame* Save = NewObject<UWDSaveGame>();
	Save->State = Progression->GetState();
	TArray<uint8> Bytes;
	TestTrue(TEXT("Serializes"), UGameplayStatics::SaveGameToMemory(Save, Bytes));

	const UWDSaveGame* Loaded = Cast<UWDSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
	if (!TestNotNull(TEXT("Deserializes"), Loaded))
	{
		return false;
	}
	UWDProgressionSubsystem* Restored = WDTests::MakeProgression();
	Restored->SetState(Loaded->State);

	TestEqual(TEXT("Gold survives"), Restored->GetGold(), 123);
	TestEqual(TEXT("XP survives"), Restored->GetXP(), 450);
	TestEqual(TEXT("Resources survive"), Restored->GetResourceAmount(EWDElement::Ice, EWDResourceTier::Fragments), 7);
	TestEqual(TEXT("Stars survive"), Restored->GetBestStars(1, EWDDifficulty::Normal), 3);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
