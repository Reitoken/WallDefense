#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WDStageData.generated.h"

class UWDMonsterData;

USTRUCT(BlueprintType)
struct FWDWaveEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TObjectPtr<UWDMonsterData> Monster;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "1"))
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FWDWaveDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	TArray<FWDWaveEntry> Entries;

	/** Inter-wave timer. The next wave starts EARLIER if this one is cleared (no dead time, GDD §2.3). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave", meta = (ClampMin = "1.0"))
	float TimeBeforeNextWave = 12.f;
};

/**
 * One stage = one DataAsset (GDD §10): the waves, the monster list per wave, the stage bases.
 * MakeDebugStage1() builds zone 1 / stage demo content with the debug bestiary.
 */
UCLASS(BlueprintType)
class WALLDEFENSE_API UWDStageData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage", meta = (ClampMin = "1"))
	int32 StageNumber = 1;

	/** Monster stat bases for this stage (multiplied by each sheet — MonsterScaling.md). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage", meta = (ClampMin = "1.0"))
	float BaseHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage", meta = (ClampMin = "0.0"))
	float BaseWallDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stage")
	TArray<FWDWaveDef> Waves;

	/** Zone 1, stages 1-5 (GDD §2.1): 2 monster types at stage 1, +1 new per stage, boss at stage 5. */
	static UWDStageData* MakeZone1Stage(UObject* Outer, int32 StageNumber);

	/** Back-compat alias for the demo flow: zone 1, stage 1. */
	static UWDStageData* MakeDebugStage1(UObject* Outer) { return MakeZone1Stage(Outer, 1); }
};
