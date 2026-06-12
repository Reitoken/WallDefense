// Automated tests for the weapon math and the debug arsenal (step 3).

#include "Misc/AutomationTest.h"
#include "Weapons/WDWeaponMath.h"
#include "Weapons/WDWeaponData.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWeaponReflectTest,
	"WallDefense.Weapons.Reflect",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWeaponReflectTest::RunTest(const FString& Parameters)
{
	// Head-on into a wall facing back: straight bounce back.
	TestTrue(TEXT("Head-on bounce reverses direction"),
		WDWeaponMath::Reflect(FVector(1, 0, 0), FVector(-1, 0, 0)).Equals(FVector(-1, 0, 0), 0.001f));

	// 45 degrees onto a wall whose normal is -X: X flips, Y stays.
	const FVector Bounced = WDWeaponMath::Reflect(FVector(1, 1, 0).GetSafeNormal(), FVector(-1, 0, 0));
	TestTrue(TEXT("45 degree bounce mirrors X"),
		Bounced.Equals(FVector(-1, 1, 0).GetSafeNormal(), 0.001f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWeaponRingTest,
	"WallDefense.Weapons.RingDirections",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWeaponRingTest::RunTest(const FString& Parameters)
{
	const TArray<FVector> Ring = WDWeaponMath::ComputeRingDirections(4);

	TestEqual(TEXT("Requested count delivered"), Ring.Num(), 4);
	TestTrue(TEXT("First fragment flies +X"), Ring[0].Equals(FVector(1, 0, 0), 0.001f));
	TestTrue(TEXT("Second fragment flies +Y (90 degrees apart)"), Ring[1].Equals(FVector(0, 1, 0), 0.001f));
	for (const FVector& Direction : Ring)
	{
		TestTrue(TEXT("Every direction is normalized and flat"),
			FMath::IsNearlyEqual(Direction.Size(), 1.f, 0.001f) && FMath::IsNearlyZero(Direction.Z));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWeaponSpreadTest,
	"WallDefense.Weapons.SpreadDirections",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWeaponSpreadTest::RunTest(const FString& Parameters)
{
	const FVector Base(1, 0, 0);
	const TArray<FVector> Fan = WDWeaponMath::ComputeSpreadDirections(Base, 3, 60.f);

	TestEqual(TEXT("Requested count delivered"), Fan.Num(), 3);
	TestTrue(TEXT("Middle shot follows the base direction"), Fan[1].Equals(Base, 0.001f));
	TestTrue(TEXT("Edges sit at ±30 degrees"),
		Fan[0].Equals(Base.RotateAngleAxis(-30.f, FVector::UpVector), 0.001f) &&
		Fan[2].Equals(Base.RotateAngleAxis(30.f, FVector::UpVector), 0.001f));

	const TArray<FVector> Single = WDWeaponMath::ComputeSpreadDirections(Base, 1, 60.f);
	TestTrue(TEXT("Count 1 = the base direction"), Single.Num() == 1 && Single[0].Equals(Base, 0.001f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWeaponHomingTest,
	"WallDefense.Weapons.HomingTurnClamp",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWeaponHomingTest::RunTest(const FString& Parameters)
{
	const FVector Current(1, 0, 0);
	const FVector Desired(0, 1, 0); // 90 degrees away

	// Turn is clamped to 30 degrees per call.
	const FVector Clamped = WDWeaponMath::ComputeHomingDirection(Current, Desired, 30.f);
	TestTrue(TEXT("Turn clamped to 30 degrees"),
		Clamped.Equals(Current.RotateAngleAxis(30.f, FVector::UpVector), 0.001f));

	// Generous budget: snaps onto the target direction.
	TestTrue(TEXT("Within budget the projectile locks on"),
		WDWeaponMath::ComputeHomingDirection(Current, Desired, 120.f).Equals(Desired, 0.001f));

	// Left side turns left (sign is honored).
	const FVector Left = WDWeaponMath::ComputeHomingDirection(Current, FVector(0, -1, 0), 30.f);
	TestTrue(TEXT("Turns toward the correct side"),
		Left.Equals(Current.RotateAngleAxis(-30.f, FVector::UpVector), 0.001f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDWeaponArsenalTest,
	"WallDefense.Weapons.DebugArsenal",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDWeaponArsenalTest::RunTest(const FString& Parameters)
{
	const TArray<UWDWeaponData*> Arsenal = UWDWeaponData::MakeDebugArsenal(GetTransientPackage());

	TestEqual(TEXT("7 weapons, one per element"), Arsenal.Num(), 7);

	TSet<EWDElement> SeenElements;
	for (const UWDWeaponData* Weapon : Arsenal)
	{
		SeenElements.Add(Weapon->Element);
		TestTrue(TEXT("Sane stats"), Weapon->Damage > 0.f && Weapon->FireInterval > 0.f && Weapon->Range > 0.f);
	}
	TestEqual(TEXT("All 7 elements are distinct"), SeenElements.Num(), 7);

	// The dark laser punishes spamming: by design the longest fire interval of the arsenal.
	float MaxInterval = 0.f; EWDElement Slowest = EWDElement::Normal;
	for (const UWDWeaponData* Weapon : Arsenal)
	{
		if (Weapon->FireInterval > MaxInterval) { MaxInterval = Weapon->FireInterval; Slowest = Weapon->Element; }
	}
	TestEqual(TEXT("Dark laser has the longest cooldown (switch while it cools)"),
		static_cast<int32>(Slowest), static_cast<int32>(EWDElement::Dark));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
