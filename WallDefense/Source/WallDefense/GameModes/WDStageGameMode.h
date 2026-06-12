#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Core/WDTypes.h"
#include "Wall/WDWallData.h"
#include "WDStageGameMode.generated.h"

class AWDWall;
class AWDStageDirector;
class UWDStageData;
class UWDLoadingScreenWidget;
class UWDStageSummaryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnStageFinished, bool, bVictory, int32, Stars);

/**
 * THE assembler of a playable stage (ArchitectureTechnique §6.6): spawns heroine + wall +
 * director, preloads the stage bundle behind the loading screen, fades in, then arbitrates —
 * wall dead = defeat, waves cleared = victory -> stars from the wall's remaining HP (GDD §2.2).
 * Tallies the run loot from the heroine's magnet; at the verdict, applies the rewards
 * (× star multiplier, ALWAYS kept on defeat) to the Progression, auto-saves, and shows
 * the summary screen (replay / back to the hub). Use as GameMode Override in any map.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDStageGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWDStageGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;

	/** Pause menu "abandon": counts as a defeat, the run loot is kept (GDD §11). */
	UFUNCTION(BlueprintCallable, Category = "WD|Stage")
	void AbandonStage();

	UPROPERTY(BlueprintAssignable, Category = "WD|Stage")
	FWDOnStageFinished OnStageFinished;

	/** Stage to play; none = generated zone 1 stage (number from the ?WDStage= travel option). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage")
	TObjectPtr<UWDStageData> Stage;

	/** Wall sheet; none = the debug wall. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage")
	TObjectPtr<UWDWallData> WallData;

	/** 0 = the wall level bought in the hub (Progression). Set >0 to force a level in tests. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WD|Stage", meta = (ClampMin = "0"))
	int32 WallLevelOverride = 0;

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

	UFUNCTION()
	void HandleLootCollected(EWDLootType Type, EWDElement Element, EWDResourceTier Tier, int32 Amount);

	UFUNCTION()
	void HandleSummaryReplay();

	UFUNCTION()
	void HandleSummaryMenu();

private:
	/** Shared verdict path: rewards -> Progression -> auto-save -> summary screen. */
	void FinishStage(bool bVictory);
	void TravelTo(const TCHAR* GameModeOption);

	UPROPERTY(Transient)
	TObjectPtr<AWDWall> Wall;

	UPROPERTY(Transient)
	TObjectPtr<AWDStageDirector> Director;

	UPROPERTY(Transient)
	TObjectPtr<UWDLoadingScreenWidget> LoadingScreen;

	UPROPERTY(Transient)
	TObjectPtr<UWDStageSummaryWidget> SummaryWidget;

	/** Everything the magnet grabbed this run (GDD §7). */
	FWDLootBundle RunLoot;

	/** From the travel options (?WDStage=N?WDMode=M) — the hub picks them. */
	int32 StageNumber = 1;
	EWDDifficulty Mode = EWDDifficulty::Normal;

	FTimerHandle StartTimer;
	bool bStageOver = false;
};
