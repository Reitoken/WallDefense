// Automated tests for the preload bundle collection (step 5).

#include "Misc/AutomationTest.h"
#include "Core/WDPreloadSubsystem.h"
#include "Stage/WDStageData.h"
#include "Wall/WDWallData.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPreloadCollectsSoftRefsTest,
	"WallDefense.Preload.CollectsSoftReferences",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPreloadCollectsSoftRefsTest::RunTest(const FString& Parameters)
{
	// The debug wall carries one soft mesh reference — the reflection walk must find it.
	UWDWallData* Wall = UWDWallData::MakeDebugWall(GetTransientPackage());
	TArray<FSoftObjectPath> Paths;
	UWDPreloadSubsystem::CollectSoftReferences(Wall, Paths);

	TestEqual(TEXT("Exactly the wall mesh is collected"), Paths.Num(), 1);
	if (Paths.Num() == 1)
	{
		TestEqual(TEXT("It is the cube soft path"), Paths[0].ToString(), FString(TEXT("/Engine/BasicShapes/Cube.Cube")));
	}

	// Null root: no crash, no path.
	UWDPreloadSubsystem::CollectSoftReferences(nullptr, Paths);
	TestEqual(TEXT("Null root adds nothing"), Paths.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWDPreloadStageBundleTest,
	"WallDefense.Preload.StageBundle",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FWDPreloadStageBundleTest::RunTest(const FString& Parameters)
{
	UWDStageData* Stage = UWDStageData::MakeDebugStage1(GetTransientPackage());
	UWDWallData* Wall = UWDWallData::MakeDebugWall(GetTransientPackage());

	// Debug sheets hold no soft refs yet; the wall contributes its mesh. No duplicates ever.
	const TArray<FSoftObjectPath> Bundle = UWDPreloadSubsystem::CollectStageAssets(Stage, { Wall, Wall });
	TestEqual(TEXT("Bundle = the wall mesh, deduplicated"), Bundle.Num(), 1);

	// A stage alone (no extra roots) currently collects nothing — and must not crash.
	const TArray<FSoftObjectPath> StageOnly = UWDPreloadSubsystem::CollectStageAssets(Stage, {});
	TestEqual(TEXT("Debug stage alone has no soft refs yet"), StageOnly.Num(), 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
