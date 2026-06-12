#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Wall/WDWallData.h"
#include "WDStageGameMode.generated.h"

class AWDWall;
class AWDStageDirector;
class UWDStageData;
class UWDLoadingScreenWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnStageFinished, bool, bVictory, int32, Stars);

/**
 * THE assembler of a playable stage (ArchitectureTechnique §6.6): spawns heroine + wall +
 * director, preloads the stage bundle behind the loading screen, fades in, then arbitrates —
 * wall dead = defeat, waves cleared = victory -> stars from the wall's remaining HP (GDD §2.2).
 * Debug loop: the level reloads a few seconds after the verdict (the menu arrives at step 6).
 * Use as GameMode Override in any map with a floor.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDStageGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDStageGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "WD|Stage")
	FWDOnStageFinished OnStageFinished;

	/** Stage to play; none = the zone 1 debug stage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage")
	TObjectPtr<UWDStageData> Stage;

	/** Wall sheet; none = the debug wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage")
	TObjectPtr<UWDWallData> WallData;

	/** Debug default = max level so every wall skill shows up in playtests. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage", meta = (ClampMin = "1"))
	int32 WallLevel = 4;

	/** Breath between the fade-in and the first wave. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage", meta = (ClampMin = "0.0"))
	float StartDelay = 3.f;

protected:
	UFUNCTION()
	void HandlePreloadFinished();

	UFUNCTION()
	void HandleWallSkill(const FWDWallSkillDef& Skill);

	UFUNCTION()
	void HandleWallDestroyed(AActor* Killer);

	UFUNCTION()
	void HandleStageCompleted();

private:
	void ScheduleRestart(float Delay);
	void RestartStageLevel();

	UPROPERTY(Transient)
	TObjectPtr<AWDWall> Wall;

	UPROPERTY(Transient)
	TObjectPtr<AWDStageDirector> Director;

	UPROPERTY(Transient)
	TObjectPtr<UWDLoadingScreenWidget> LoadingScreen;

	FTimerHandle StartTimer;
	FTimerHandle RestartTimer;
	bool bStageOver = false;
};
