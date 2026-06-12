#pragma once

#include "CoreMinimal.h"
#include "UI/WDMenuScreen.h"
#include "Core/WDTypes.h"
#include "WDAdventureScreen.generated.h"

/**
 * The adventure board (GDD §2.1): the 6 zones as a grid, stars per stage for the
 * selected difficulty, the unlock chain, and the launch. Zone 1 is playable;
 * zones 2-6 show as "coming soon" until their bestiaries land.
 */
UCLASS()
class WALLDEFENSE_API UWDAdventureScreen : public UWDMenuScreen
{
	GENERATED_BODY()

protected:
	virtual void RebuildContent() override;

	UFUNCTION() void HandleModeNormal() { SelectMode(EWDDifficulty::Normal); }
	UFUNCTION() void HandleModeHard()   { SelectMode(EWDDifficulty::Hard); }
	UFUNCTION() void HandleModeHell()   { SelectMode(EWDDifficulty::Hell); }

	UFUNCTION() void HandleStage1() { LaunchStage(1); }
	UFUNCTION() void HandleStage2() { LaunchStage(2); }
	UFUNCTION() void HandleStage3() { LaunchStage(3); }
	UFUNCTION() void HandleStage4() { LaunchStage(4); }
	UFUNCTION() void HandleStage5() { LaunchStage(5); }

private:
	void SelectMode(EWDDifficulty Mode);
	void LaunchStage(int32 StageNumber);

	EWDDifficulty SelectedMode = EWDDifficulty::Normal;
};
