// Automated tests for WDHeroMath (step 2 — top-down hero).
// The math that turns inputs into world movement/aim, and velocity into anim data.

#include "Misc/AutomationTest.h"
#include "Player/WDHeroMath.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHeroInputToWorldTest,
	"WallDefense.Hero.InputToWorldDirection",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHeroInputToWorldTest::RunTest(const FString& Parameters)
{
	// Screen up (0,1) → world +X
	TestTrue(TEXT("Up input goes to world +X"),
		WDHeroMath::InputToWorldDirection(FVector2D(0.f, 1.f)).Equals(FVector(1.f, 0.f, 0.f), 0.001f));

	// Screen right (1,0) → world +Y
	TestTrue(TEXT("Right input goes to world +Y"),
		WDHeroMath::InputToWorldDirection(FVector2D(1.f, 0.f)).Equals(FVector(0.f, 1.f, 0.f), 0.001f));

	// Zero stays zero (no drift)
	TestTrue(TEXT("Zero input gives zero vector"),
		WDHeroMath::InputToWorldDirection(FVector2D::ZeroVector).IsNearlyZero());

	// Diagonals are normalized
	const FVector Diagonal = WDHeroMath::InputToWorldDirection(FVector2D(1.f, 1.f));
	TestTrue(TEXT("Diagonal input is normalized"), FMath::IsNearlyEqual(Diagonal.Size(), 1.f, 0.001f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHeroDirectionToYawTest,
	"WallDefense.Hero.DirectionToYaw",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHeroDirectionToYawTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("+X is yaw 0"), WDHeroMath::DirectionToYaw(FVector(1, 0, 0)), 0.f);
	TestEqual(TEXT("+Y is yaw 90"), WDHeroMath::DirectionToYaw(FVector(0, 1, 0)), 90.f);
	TestEqual(TEXT("-Y is yaw -90"), WDHeroMath::DirectionToYaw(FVector(0, -1, 0)), -90.f);
	TestEqual(TEXT("-X is yaw 180"), WDHeroMath::DirectionToYaw(FVector(-1, 0, 0)), 180.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHeroMoveDirectionAngleTest,
	"WallDefense.Hero.MoveDirectionAngle",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHeroMoveDirectionAngleTest::RunTest(const FString& Parameters)
{
	const FVector Forward(1.f, 0.f, 0.f); // facing +X

	TestEqual(TEXT("Running forward = 0"),
		WDHeroMath::ComputeMoveDirectionAngle(FVector(600, 0, 0), Forward), 0.f);

	TestEqual(TEXT("Strafing right = 90"),
		WDHeroMath::ComputeMoveDirectionAngle(FVector(0, 600, 0), Forward), 90.f);

	TestEqual(TEXT("Strafing left = -90"),
		WDHeroMath::ComputeMoveDirectionAngle(FVector(0, -600, 0), Forward), -90.f);

	TestEqual(TEXT("Backpedaling = ±180"),
		FMath::Abs(WDHeroMath::ComputeMoveDirectionAngle(FVector(-600, 0, 0), Forward)), 180.f);

	TestEqual(TEXT("Idle = 0 (no noise)"),
		WDHeroMath::ComputeMoveDirectionAngle(FVector::ZeroVector, Forward), 0.f);

	// Velocity Z must not pollute the planar angle (jumps, slopes).
	TestEqual(TEXT("Vertical velocity is ignored"),
		WDHeroMath::ComputeMoveDirectionAngle(FVector(600, 0, 300), Forward), 0.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDHeroAimRoundTripTest,
	"WallDefense.Hero.AimStickRoundTrip",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDHeroAimRoundTripTest::RunTest(const FString& Parameters)
{
	// Pushing the right stick up-right must aim up-right on screen: world (X=Y stick) yaw 45.
	const FVector Aim = WDHeroMath::InputToWorldDirection(FVector2D(1.f, 1.f));
	TestEqual(TEXT("Stick up-right aims at yaw 45"), WDHeroMath::DirectionToYaw(Aim), 45.f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
