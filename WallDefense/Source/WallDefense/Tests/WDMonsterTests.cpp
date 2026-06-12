// Automated tests for the monster patterns, the zone 1 bestiary and the debug stage (step 4).

#include "Misc/AutomationTest.h"
#include "Monsters/WDPatternMath.h"
#include "Monsters/WDMonsterData.h"
#include "Stage/WDStageData.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPatternSinusoidTest,
	"WallDefense.Patterns.SinusoidBounds",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPatternSinusoidTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Starts centered"), WDPatternMath::SinusoidOffset(0.f, 200.f, 0.5f), 0.f);
	for (float Time = 0.f; Time < 10.f; Time += 0.1f)
	{
		if (FMath::Abs(WDPatternMath::SinusoidOffset(Time, 200.f, 0.5f)) > 200.001f)
		{
			AddError(TEXT("Sinusoid exceeded its amplitude"));
			return false;
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPatternZigzagTest,
	"WallDefense.Patterns.ZigzagShape",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPatternZigzagTest::RunTest(const FString& Parameters)
{
	const float A = 200.f, F = 1.f;
	TestEqual(TEXT("Phase 0 sits at -A"), WDPatternMath::ZigzagOffset(0.f, A, F), -A);
	TestEqual(TEXT("Quarter phase crosses 0"), WDPatternMath::ZigzagOffset(0.25f, A, F), 0.f);
	TestEqual(TEXT("Half phase peaks at +A"), WDPatternMath::ZigzagOffset(0.4999f, A, F), A * 0.9996f, 0.5f);
	TestEqual(TEXT("Periodic: t and t+1/F match"), WDPatternMath::ZigzagOffset(0.3f, A, F), WDPatternMath::ZigzagOffset(1.3f, A, F), 0.01f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPatternChargePauseTest,
	"WallDefense.Patterns.ChargePauseDuty",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPatternChargePauseTest::RunTest(const FString& Parameters)
{
	// 1.0s charge, 0.5s pause.
	TestEqual(TEXT("Charging at t=0.5"), WDPatternMath::ChargePauseSpeed(0.5f, 1.f, 0.5f), 1.6f);
	TestEqual(TEXT("Paused at t=1.2"), WDPatternMath::ChargePauseSpeed(1.2f, 1.f, 0.5f), 0.f);
	TestEqual(TEXT("Charging again next cycle (t=1.6)"), WDPatternMath::ChargePauseSpeed(1.6f, 1.f, 0.5f), 1.6f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPatternBurrowFlankTest,
	"WallDefense.Patterns.BurrowAndFlank",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPatternBurrowFlankTest::RunTest(const FString& Parameters)
{
	// Burrower: 1.5s surfaced, 1.2s underground.
	TestFalse(TEXT("Surfaced at start"), WDPatternMath::IsBurrowed(0.5f, 1.5f, 1.2f));
	TestTrue(TEXT("Underground after the surfaced phase"), WDPatternMath::IsBurrowed(2.f, 1.5f, 1.2f));
	TestFalse(TEXT("Back on the surface next cycle"), WDPatternMath::IsBurrowed(2.8f, 1.5f, 1.2f));

	// Flanker: full lateral far away, converged near the wall, clamped.
	TestEqual(TEXT("Far away hugs the side"), WDPatternMath::FlankerBlend(2000.f, 600.f, 2000.f), 1.f);
	TestEqual(TEXT("Converged at the wall"), WDPatternMath::FlankerBlend(600.f, 600.f, 2000.f), 0.f);
	TestEqual(TEXT("Clamped below the converge point"), WDPatternMath::FlankerBlend(100.f, 600.f, 2000.f), 0.f);

	// Hopper: never digs into the ground.
	for (float Time = 0.f; Time < 3.f; Time += 0.05f)
	{
		if (WDPatternMath::HopHeight(Time, 0.9f, 140.f) < -0.001f)
		{
			AddError(TEXT("Hop height went negative"));
			return false;
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDZone1BestiaryTest,
	"WallDefense.Monsters.Zone1Bestiary",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDZone1BestiaryTest::RunTest(const FString& Parameters)
{
	const TArray<UWDMonsterData*> Bestiary = UWDMonsterData::MakeZone1Bestiary(GetTransientPackage());

	TestEqual(TEXT("5 monsters + 1 boss"), Bestiary.Num(), 6);

	int32 BossCount = 0;
	float BossHealth = 0.f, MaxOtherHealth = 0.f;
	for (const UWDMonsterData* Monster : Bestiary)
	{
		TestTrue(TEXT("Every sheet has a weakness (core rule)"), Monster->ElementalProfile.bHasWeakness);
		TestTrue(TEXT("Sane speed"), Monster->Speed >= 10.f);
		if (Monster->Role == EWDMonsterRole::Boss)
		{
			++BossCount;
			BossHealth = Monster->HealthMultiplier;
		}
		else
		{
			MaxOtherHealth = FMath::Max(MaxOtherHealth, Monster->HealthMultiplier);
		}
		if (Monster->Role == EWDMonsterRole::Healer)
		{
			TestTrue(TEXT("The healer heals"), Monster->HealPulseAmount > 0.f);
			TestEqual(TEXT("The healer does not hurt the wall"), Monster->WallDamageMultiplier, 0.f);
		}
		if (Monster->Role == EWDMonsterRole::Shooter)
		{
			TestTrue(TEXT("The shooter attacks from afar"), Monster->bRangedAttack && Monster->AttackRange > 500.f);
		}
	}
	TestEqual(TEXT("Exactly one boss"), BossCount, 1);
	TestTrue(TEXT("The boss out-healths everyone"), BossHealth > MaxOtherHealth);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDZone1StagesTest,
	"WallDefense.Stage.Zone1Stages",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDZone1StagesTest::RunTest(const FString& Parameters)
{
	// GDD §2.1/§6.3: 2 types at stage 1, +1 new per stage, the whole cast + boss at stage 5.
	for (int32 StageNumber = 1; StageNumber <= 5; ++StageNumber)
	{
		const UWDStageData* Stage = UWDStageData::MakeZone1Stage(GetTransientPackage(), StageNumber);
		TestEqual(TEXT("Stage number kept"), Stage->StageNumber, StageNumber);
		TestTrue(TEXT("At least 3 waves"), Stage->Waves.Num() >= 3);

		TSet<FString> DistinctTypes;
		bool bHasBoss = false;
		bool bBossInLastWave = false;
		for (int32 i = 0; i < Stage->Waves.Num(); ++i)
		{
			const FWDWaveDef& Wave = Stage->Waves[i];
			TestTrue(TEXT("No empty wave"), Wave.Entries.Num() > 0);
			TestTrue(TEXT("Sane inter-wave timer"), Wave.TimeBeforeNextWave >= 1.f);
			for (const FWDWaveEntry& Entry : Wave.Entries)
			{
				TestTrue(TEXT("Valid monster and count"), Entry.Monster != nullptr && Entry.Count >= 1);
				DistinctTypes.Add(Entry.Monster->DisplayName.ToString());
				if (Entry.Monster->Role == EWDMonsterRole::Boss)
				{
					bHasBoss = true;
					bBossInLastWave = (i == Stage->Waves.Num() - 1);
				}
			}
		}
		// +1 new type per stage: stage 1 = 2 types ... stage 5 = 5 types + the boss = 6.
		TestEqual(TEXT("Type count follows the +1 per stage rule"), DistinctTypes.Num(), StageNumber + 1);
		TestEqual(TEXT("Boss only at stage 5"), bHasBoss, StageNumber == 5);
		if (StageNumber == 5)
		{
			TestTrue(TEXT("The boss closes the zone"), bBossInLastWave);
		}
	}

	// Bases scale up with the stage number.
	const UWDStageData* First = UWDStageData::MakeZone1Stage(GetTransientPackage(), 1);
	const UWDStageData* Last = UWDStageData::MakeZone1Stage(GetTransientPackage(), 5);
	TestTrue(TEXT("Stage 5 monsters out-health stage 1"), Last->BaseHealth > First->BaseHealth);
	TestTrue(TEXT("Stage 5 hits the wall harder"), Last->BaseWallDamage > First->BaseWallDamage);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
