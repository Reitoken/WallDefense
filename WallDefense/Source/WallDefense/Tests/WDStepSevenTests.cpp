// Automated tests for step 7: difficulty modes, unlock chain, encyclopedia, live language.

#include "Misc/AutomationTest.h"
#include "Stage/WDDifficultyMath.h"
#include "Core/WDProgressionMath.h"
#include "Core/WDProgressionSubsystem.h"
#include "Core/WDSettingsSubsystem.h"
#include "Engine/GameInstance.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace WDTests
{
	static UWDProgressionSubsystem* MakeProgression7()
	{
		return NewObject<UWDProgressionSubsystem>(NewObject<UGameInstance>());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDDifficultyMathTest,
	"WallDefense.Difficulty.ModeMultipliers",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDDifficultyMathTest::RunTest(const FString& Parameters)
{
	// Harder = tougher monsters, harder wall hits, better drops (GDD §2.5).
	TestEqual(TEXT("Normal is the baseline"), WDDifficultyMath::HealthMultiplier(EWDDifficulty::Normal), 1.f);
	TestTrue(TEXT("Hell out-healths Hard out-healths Normal"),
		WDDifficultyMath::HealthMultiplier(EWDDifficulty::Hell) > WDDifficultyMath::HealthMultiplier(EWDDifficulty::Hard)
		&& WDDifficultyMath::HealthMultiplier(EWDDifficulty::Hard) > 1.f);
	TestTrue(TEXT("Wall damage scales the same way"),
		WDDifficultyMath::WallDamageMultiplier(EWDDifficulty::Hell) > WDDifficultyMath::WallDamageMultiplier(EWDDifficulty::Hard));
	TestTrue(TEXT("Drops pay more in harder modes"),
		WDDifficultyMath::DropAmountMultiplier(EWDDifficulty::Hell) > WDDifficultyMath::DropAmountMultiplier(EWDDifficulty::Hard));

	// Extra resistances: 0 / 1 / 2.
	TestEqual(TEXT("Normal adds none"), WDDifficultyMath::ExtraResistances(EWDDifficulty::Normal), 0);
	TestEqual(TEXT("Hard adds one"), WDDifficultyMath::ExtraResistances(EWDDifficulty::Hard), 1);
	TestEqual(TEXT("Hell adds two"), WDDifficultyMath::ExtraResistances(EWDDifficulty::Hell), 2);

	// Each mode drops its own material tier.
	TestEqual(TEXT("Normal drops Fragments"), static_cast<int32>(WDDifficultyMath::DropTier(EWDDifficulty::Normal)), static_cast<int32>(EWDResourceTier::Fragments));
	TestEqual(TEXT("Hard drops Crystals"), static_cast<int32>(WDDifficultyMath::DropTier(EWDDifficulty::Hard)), static_cast<int32>(EWDResourceTier::Crystals));
	TestEqual(TEXT("Hell drops Cores"), static_cast<int32>(WDDifficultyMath::DropTier(EWDDifficulty::Hell)), static_cast<int32>(EWDResourceTier::Cores));

	// The Special scales with the weapon level.
	TestTrue(TEXT("Special grows with the weapon"),
		WDProgressionMath::SpecialDamageMultiplier(50) > WDProgressionMath::SpecialDamageMultiplier(1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDUnlockChainTest,
	"WallDefense.Progression.UnlockChain",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDUnlockChainTest::RunTest(const FString& Parameters)
{
	UWDProgressionSubsystem* Progression = WDTests::MakeProgression7();

	// Fresh save: only stage 1 Normal is open.
	TestTrue(TEXT("Stage 1 Normal always open"), Progression->IsStageUnlocked(1, EWDDifficulty::Normal));
	TestFalse(TEXT("Stage 2 locked before stage 1 is done"), Progression->IsStageUnlocked(2, EWDDifficulty::Normal));
	TestFalse(TEXT("Hard locked before Normal is done"), Progression->IsStageUnlocked(1, EWDDifficulty::Hard));

	// Finish stage 1 Normal: stage 2 Normal and stage 1 Hard open up.
	Progression->RegisterStageResult(1, EWDDifficulty::Normal, 2);
	TestTrue(TEXT("Stage 2 Normal opens"), Progression->IsStageUnlocked(2, EWDDifficulty::Normal));
	TestTrue(TEXT("Stage 1 Hard opens"), Progression->IsStageUnlocked(1, EWDDifficulty::Hard));
	TestFalse(TEXT("Stage 1 Hell still needs Hard"), Progression->IsStageUnlocked(1, EWDDifficulty::Hell));
	TestFalse(TEXT("Stage 2 Hard needs stage 2 Normal"), Progression->IsStageUnlocked(2, EWDDifficulty::Hard));

	// Finish stage 1 Hard: Hell opens on stage 1.
	Progression->RegisterStageResult(1, EWDDifficulty::Hard, 1);
	TestTrue(TEXT("Stage 1 Hell opens after Hard"), Progression->IsStageUnlocked(1, EWDDifficulty::Hell));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDDiscoveriesTest,
	"WallDefense.Progression.EncyclopediaDiscoveries",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDDiscoveriesTest::RunTest(const FString& Parameters)
{
	UWDProgressionSubsystem* Progression = WDTests::MakeProgression7();
	const FName Ombrillon(TEXT("Ombrillon"));

	TestFalse(TEXT("Unknown before meeting it"), Progression->IsMonsterDiscovered(Ombrillon));
	TestTrue(TEXT("First meeting is a discovery"), Progression->RegisterMonsterDiscovered(Ombrillon));
	TestFalse(TEXT("Second meeting is not"), Progression->RegisterMonsterDiscovered(Ombrillon));
	TestTrue(TEXT("Now known"), Progression->IsMonsterDiscovered(Ombrillon));

	TestFalse(TEXT("Weakness still hidden"), Progression->IsWeaknessDiscovered(Ombrillon));
	TestTrue(TEXT("A weakness hit reveals it"), Progression->RegisterWeaknessDiscovered(Ombrillon));
	TestFalse(TEXT("Only once"), Progression->RegisterWeaknessDiscovered(Ombrillon));
	TestTrue(TEXT("Confirmed"), Progression->IsWeaknessDiscovered(Ombrillon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDLiveLanguageTest,
	"WallDefense.Settings.LiveLanguageSwitch",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDLiveLanguageTest::RunTest(const FString& Parameters)
{
	// English ON: the WDUI namespace resolves to English, the process culture is untouched.
	UWDSettingsSubsystem::ApplyLanguage(TEXT("en"));
	TestEqual(TEXT("EN applied to game texts"),
		NSLOCTEXT("WDUI", "PauseResume", "Reprendre").ToString(), FString(TEXT("Resume")));
	TestEqual(TEXT("Formatted patterns translate too"),
		NSLOCTEXT("WDUI", "HubWallUp", "Améliorer — {0} or").ToString(), FString(TEXT("Upgrade — {0} gold")));

	// Back to French: sources restored.
	UWDSettingsSubsystem::ApplyLanguage(TEXT("fr"));
	TestEqual(TEXT("FR restored"),
		NSLOCTEXT("WDUI", "PauseResume", "Reprendre").ToString(), FString(TEXT("Reprendre")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
