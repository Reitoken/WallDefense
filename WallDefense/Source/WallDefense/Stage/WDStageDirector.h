#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "WDStageDirector.generated.h"

class UWDStageData;
class AWDMonster;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWDOnWaveStarted, int32, WaveNumber, int32, TotalWaves);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnStageCompleted);

/**
 * Unrolls a stage's waves (ArchitectureTechnique §6.6): spawns the monsters,
 * counts the living, runs the inter-wave timer — and starts the next wave EARLY
 * when the field is cleared (zero dead time, GDD §2.3).
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDStageDirector : public AActor
{
	GENERATED_BODY()

public:
	AWDStageDirector();

	virtual void Tick(float DeltaSeconds) override;

	/** Arms the director. Target = what the monsters attack (the wall). */
	void Configure(UWDStageData* InStage, AActor* InTarget, const FVector& InSpawnCenter, float InSpawnHalfWidth,
		EWDDifficulty InDifficulty = EWDDifficulty::Normal);

	UFUNCTION(BlueprintCallable, Category = "WD|Stage")
	void StartStage();

	/** Halts the stage (defeat): no more waves, optionally clears the field. */
	UFUNCTION(BlueprintCallable, Category = "WD|Stage")
	void StopStage(bool bDespawnMonsters);

	UFUNCTION(BlueprintPure, Category = "WD|Stage")
	int32 GetAliveCount() const { return AliveMonsters.Num(); }

	UFUNCTION(BlueprintPure, Category = "WD|Stage")
	bool IsStageRunning() const { return bRunning; }

	UPROPERTY(BlueprintAssignable, Category = "WD|Stage")
	FWDOnWaveStarted OnWaveStarted;

	UPROPERTY(BlueprintAssignable, Category = "WD|Stage")
	FWDOnStageCompleted OnStageCompleted;

protected:
	UFUNCTION()
	void HandleMonsterKilled(AWDMonster* Monster);

private:
	void StartNextWave();
	void CompleteStage();

	UPROPERTY(Transient)
	TObjectPtr<UWDStageData> Stage;

	UPROPERTY(Transient)
	TSet<TObjectPtr<AWDMonster>> AliveMonsters;

	TWeakObjectPtr<AActor> Target;
	EWDDifficulty Difficulty = EWDDifficulty::Normal;
	FVector SpawnCenter = FVector::ZeroVector;
	float SpawnHalfWidth = 800.f;
	int32 CurrentWaveIndex = -1;
	float NextWaveCountdown = 0.f;
	bool bRunning = false;
	bool bAllWavesSpawned = false;
};
