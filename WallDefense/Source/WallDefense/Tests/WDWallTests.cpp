// Automated tests for the wall, its one-shot skills and the stage outcome math (step 5).

#include "Misc/AutomationTest.h"
#include "Wall/WDWallData.h"
#include "Wall/WDWallSkillsComponent.h"
#include "Stage/WDStageMath.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDStarsThresholdsTest,
	"WallDefense.Stage.StarsThresholds",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDStarsThresholdsTest::RunTest(const FString& Parameters)
{
	// GDD §2.2: 100% = 3, >= 66% = 2, >= 33% = 1, below = 0 (victory anyway).
	TestEqual(TEXT("Untouched wall = 3 stars"), WDStageMath::StarsFromWallHealth(1.f), 3);
	TestEqual(TEXT("99% is no longer perfect"), WDStageMath::StarsFromWallHealth(0.99f), 2);
	TestEqual(TEXT("66% exactly = 2 stars"), WDStageMath::StarsFromWallHealth(0.66f), 2);
	TestEqual(TEXT("65% = 1 star"), WDStageMath::StarsFromWallHealth(0.65f), 1);
	TestEqual(TEXT("33% exactly = 1 star"), WDStageMath::StarsFromWallHealth(0.33f), 1);
	TestEqual(TEXT("Barely standing = 0 star"), WDStageMath::StarsFromWallHealth(0.05f), 0);

	// Drop multipliers: ×1 / ×1.25 / ×1.5 / ×2.
	TestEqual(TEXT("0 star x1"), WDStageMath::RewardMultiplierForStars(0), 1.f);
	TestEqual(TEXT("1 star x1.25"), WDStageMath::RewardMultiplierForStars(1), 1.25f);
	TestEqual(TEXT("2 stars x1.5"), WDStageMath::RewardMultiplierForStars(2), 1.5f);
	TestEqual(TEXT("3 stars x2"), WDStageMath::RewardMultiplierForStars(3), 2.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWallDataTest,
	"WallDefense.Wall.DebugWallData",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWallDataTest::RunTest(const FString& Parameters)
{
	const UWDWallData* Wall = UWDWallData::MakeDebugWall(GetTransientPackage());

	TestTrue(TEXT("Several wall levels"), Wall->GetMaxLevel() >= 2);
	for (int32 i = 1; i < Wall->Levels.Num(); ++i)
	{
		TestTrue(TEXT("Each level has more HP than the previous"), Wall->Levels[i].MaxHealth > Wall->Levels[i - 1].MaxHealth);
	}

	TestEqual(TEXT("The 3 designed skills exist"), Wall->Skills.Num(), 3);
	for (const FWDWallSkillDef& Skill : Wall->Skills)
	{
		TestTrue(TEXT("Skill unlock level within the wall levels"), Skill.UnlockLevel >= 1 && Skill.UnlockLevel <= Wall->GetMaxLevel());
		TestTrue(TEXT("Skill has a positive effect value"), Skill.Value > 0.f);
	}

	// Level clamping: below = level 1, above = max level.
	TestEqual(TEXT("Level 0 clamps to level 1"), Wall->GetLevelDef(0).MaxHealth, Wall->Levels[0].MaxHealth);
	TestEqual(TEXT("Level 99 clamps to max"), Wall->GetLevelDef(99).MaxHealth, Wall->Levels.Last().MaxHealth);

	// Unlock filtering: level 1 arms nothing, max level arms everything.
	TestEqual(TEXT("Level 1: no skill yet"), Wall->GetUnlockedSkills(1).Num(), 0);
	TestEqual(TEXT("Max level: all skills armed"), Wall->GetUnlockedSkills(Wall->GetMaxLevel()).Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWallSkillsOneShotTest,
	"WallDefense.Wall.SkillsFireOnce",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWallSkillsOneShotTest::RunTest(const FString& Parameters)
{
	UWDWallData* Data = UWDWallData::MakeDebugWall(GetTransientPackage());
	UWDWallSkillsComponent* Skills = NewObject<UWDWallSkillsComponent>();
	Skills->Configure(Data->GetUnlockedSkills(Data->GetMaxLevel()));

	TestEqual(TEXT("3 skills armed"), Skills->GetArmedCount(), 3);
	TestTrue(TEXT("Starting shield armed"), Skills->IsArmed(EWDWallSkill::StartingShield));

	// Stage start: the shield fires, once.
	Skills->NotifyStageStarted();
	TestTrue(TEXT("Starting shield consumed at stage start"), Skills->HasTriggered(EWDWallSkill::StartingShield));
	TestFalse(TEXT("Starting shield no longer armed"), Skills->IsArmed(EWDWallSkill::StartingShield));
	Skills->NotifyStageStarted();
	TestEqual(TEXT("A second stage-start changes nothing"), Skills->GetArmedCount(), 2);

	// Healthy wall hit from afar: nothing triggers.
	Skills->NotifyWallDamaged(0.8f, /*AttackerDistance=*/2000.f);
	TestFalse(TEXT("No auto-repair above the threshold"), Skills->HasTriggered(EWDWallSkill::AutoRepair));
	TestFalse(TEXT("No repulsion against ranged attackers"), Skills->HasTriggered(EWDWallSkill::RepulsionWave));

	// First melee contact: the repulsion wave fires.
	Skills->NotifyWallDamaged(0.8f, /*AttackerDistance=*/120.f);
	TestTrue(TEXT("Repulsion fires on first contact"), Skills->HasTriggered(EWDWallSkill::RepulsionWave));

	// HP collapse: the auto-repair fires, once.
	Skills->NotifyWallDamaged(0.2f, 120.f);
	TestTrue(TEXT("Auto-repair fires under 25%"), Skills->HasTriggered(EWDWallSkill::AutoRepair));
	TestEqual(TEXT("Everything is consumed"), Skills->GetArmedCount(), 0);
	Skills->NotifyWallDamaged(0.05f, 50.f);
	TestEqual(TEXT("One-shot: nothing fires twice"), Skills->GetArmedCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWallSkillsLockedTest,
	"WallDefense.Wall.SkillsLockedByLevel",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWallSkillsLockedTest::RunTest(const FString& Parameters)
{
	UWDWallData* Data = UWDWallData::MakeDebugWall(GetTransientPackage());
	UWDWallSkillsComponent* Skills = NewObject<UWDWallSkillsComponent>();

	// Level 1 wall: nothing unlocked, conditions can rage all they want.
	Skills->Configure(Data->GetUnlockedSkills(1));
	Skills->NotifyStageStarted();
	Skills->NotifyWallDamaged(0.01f, 10.f);
	TestEqual(TEXT("No skill at level 1"), Skills->GetArmedCount(), 0);
	TestFalse(TEXT("Nothing triggered either"), Skills->HasTriggered(EWDWallSkill::AutoRepair));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
