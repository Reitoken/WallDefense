// Automated tests for UWDHealthComponent (step 1 foundations).
// Run in editor:   Tools > Test Automation > filter "WallDefense"
// Run headless:    UnrealEditor-Cmd.exe <uproject> -ExecCmds="Automation RunTests WallDefense; Quit" -nullrhi -unattended

#include "Misc/AutomationTest.h"
#include "Combat/WDHealthComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace WDTests
{
	static UWDHealthComponent* MakeHealth(float MaxHealth = 100.f, float Defense = 0.f)
	{
		UWDHealthComponent* Health = NewObject<UWDHealthComponent>();
		Health->MaxHealth = MaxHealth;
		Health->Defense = Defense;
		Health->ResetToFull();
		return Health;
	}

	static FWDDamageEvent MakeDamage(float Amount, EWDElement Element = EWDElement::Normal)
	{
		FWDDamageEvent Event;
		Event.Amount = Amount;
		Event.Element = Element;
		return Event;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthBasicDamageTest,
	"WallDefense.Health.BasicDamage",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthBasicDamageTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f));

	TestEqual(TEXT("10 neutral damage on 0 defense deals 10"), Applied, 10.f);
	TestEqual(TEXT("Health drops to 90"), Health->GetHealth(), 90.f);
	TestFalse(TEXT("Not dead"), Health->IsDead());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthWeaknessTest,
	"WallDefense.Health.WeaknessMultiplier",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthWeaknessTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	Health->ElementalProfile.bHasWeakness = true;
	Health->ElementalProfile.Weakness = EWDElement::Fire;

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f, EWDElement::Fire));

	TestEqual(TEXT("Weakness deals x1.75"), Applied, 17.5f);
	TestEqual(TEXT("Match reported as weakness"), static_cast<int32>(Health->GetElementalMatch(EWDElement::Fire)), static_cast<int32>(EWDElementalMatch::Weakness));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthResistanceTest,
	"WallDefense.Health.ResistanceMultiplier",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthResistanceTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	Health->ElementalProfile.Resistances.Add(EWDElement::Ice);

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f, EWDElement::Ice));

	TestEqual(TEXT("Resistance deals x0.5"), Applied, 5.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthNormalNeverResistedTest,
	"WallDefense.Health.NormalNeverResisted",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthNormalNeverResistedTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	// Misconfigured data: Normal listed as a resistance. The rule says Normal is never resisted.
	Health->ElementalProfile.Resistances.Add(EWDElement::Normal);

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f, EWDElement::Normal));

	TestEqual(TEXT("Normal damage ignores resistances"), Applied, 10.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthDefenseTest,
	"WallDefense.Health.PercentDefense",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthDefenseTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth(100.f, /*Defense=*/100.f);

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f));

	TestEqual(TEXT("Defense 100 halves damage (100/(100+100))"), Applied, 5.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthShieldTest,
	"WallDefense.Health.ShieldAbsorbsFirst",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthShieldTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	Health->AddShield(15.f);

	// 20 damage: 15 absorbed by shield, 5 bleed into health.
	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(20.f));

	TestEqual(TEXT("Full damage applied across layers"), Applied, 20.f);
	TestEqual(TEXT("Shield is broken"), Health->GetShield(), 0.f);
	TestFalse(TEXT("HasShield is false"), Health->HasShield());
	TestEqual(TEXT("Health took the overflow (95)"), Health->GetHealth(), 95.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthDeathTest,
	"WallDefense.Health.DeathAndImmunity",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthDeathTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();

	Health->ApplyWDDamage(WDTests::MakeDamage(150.f));
	TestTrue(TEXT("Lethal damage kills"), Health->IsDead());
	TestEqual(TEXT("Health clamped at 0"), Health->GetHealth(), 0.f);

	const float AppliedAfterDeath = Health->ApplyWDDamage(WDTests::MakeDamage(10.f));
	TestEqual(TEXT("Dead targets take no damage"), AppliedAfterDeath, 0.f);

	Health->Heal(50.f);
	TestEqual(TEXT("Dead targets cannot be healed"), Health->GetHealth(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthHealTest,
	"WallDefense.Health.HealClampsAtMax",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthHealTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	Health->ApplyWDDamage(WDTests::MakeDamage(30.f));

	Health->Heal(1000.f);

	TestEqual(TEXT("Heal clamps at max health"), Health->GetHealth(), 100.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthResetTest,
	"WallDefense.Health.ResetToFull",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthResetTest::RunTest(const FString& Parameters)
{
	UWDHealthComponent* Health = WDTests::MakeHealth();
	Health->ApplyWDDamage(WDTests::MakeDamage(150.f));
	TestTrue(TEXT("Dead before reset"), Health->IsDead());

	Health->ResetToFull();

	TestFalse(TEXT("Alive after reset"), Health->IsDead());
	TestEqual(TEXT("Back to full health"), Health->GetHealth(), 100.f);
	TestEqual(TEXT("Shield cleared"), Health->GetShield(), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHealthStackedFormulaTest,
	"WallDefense.Health.FullFormulaStack",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHealthStackedFormulaTest::RunTest(const FString& Parameters)
{
	// Weakness + defense together: 10 * 1.75 * (100/(100+75)) = 10
	UWDHealthComponent* Health = WDTests::MakeHealth(100.f, /*Defense=*/75.f);
	Health->ElementalProfile.bHasWeakness = true;
	Health->ElementalProfile.Weakness = EWDElement::Lightning;

	const float Applied = Health->ApplyWDDamage(WDTests::MakeDamage(10.f, EWDElement::Lightning));

	TestEqual(TEXT("Element multiplier then percent defense"), Applied, 10.f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

