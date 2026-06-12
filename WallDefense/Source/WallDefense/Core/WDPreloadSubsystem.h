#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "WDPreloadSubsystem.generated.h"

class UWDStageData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnPreloadProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWDOnPreloadFinished);

/**
 * Loads everything a stage will need BEFORE it starts (ArchitectureTechnique §5):
 * walks the stage's DataAssets (monster sheets, weapons, wall) by reflection and collects
 * every soft reference, streams the bundle asynchronously, then warms the projectile pool.
 * Guarantee: no asset hitch mid-fight. The bundle handle lives here until ReleaseBundle().
 */
UCLASS()
class WALLDEFENSE_API UWDPreloadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Starts the preload: stage sheets + extra roots (the owned weapons, the wall data). */
	void PreloadStage(const UWDStageData* Stage, const TArray<UObject*>& ExtraRoots, UWorld* World);

	/** Frees the bundle (back to menu). Safe to call anytime. */
	UFUNCTION(BlueprintCallable, Category = "WD|Preload")
	void ReleaseBundle();

	UFUNCTION(BlueprintPure, Category = "WD|Preload")
	bool IsPreloading() const { return bPreloading; }

	/** Every non-null soft reference reachable in this object's properties (recurses structs/arrays). */
	static void CollectSoftReferences(const UObject* Root, TArray<FSoftObjectPath>& OutPaths);

	/** The full bundle of a stage: the stage asset, its monster sheets, and the extra roots. */
	static TArray<FSoftObjectPath> CollectStageAssets(const UWDStageData* Stage, const TArray<UObject*>& ExtraRoots);

	/** The loading screen stays visible at least this long — readable, never a one-frame flash. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WD|Preload", meta = (ClampMin = "0.0"))
	float MinimumDuration = 0.8f;

	UPROPERTY(BlueprintAssignable, Category = "WD|Preload")
	FWDOnPreloadProgress OnPreloadProgress;

	UPROPERTY(BlueprintAssignable, Category = "WD|Preload")
	FWDOnPreloadFinished OnPreloadFinished;

private:
	void TickProgress();

	TSharedPtr<FStreamableHandle> BundleHandle;
	TWeakObjectPtr<UWorld> PreloadWorld;
	FTimerHandle ProgressTimer;
	float Elapsed = 0.f;
	bool bPreloading = false;
};
