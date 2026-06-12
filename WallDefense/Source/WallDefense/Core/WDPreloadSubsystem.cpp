#include "Core/WDPreloadSubsystem.h"
#include "Stage/WDStageData.h"
#include "Monsters/WDMonsterData.h"
#include "Weapons/WDProjectilePoolSubsystem.h"
#include "Engine/AssetManager.h"
#include "TimerManager.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyIterator.h"

namespace
{
	constexpr float ProgressTickInterval = 0.05f;
	constexpr int32 PoolWarmupCount = 48;
}

void UWDPreloadSubsystem::CollectSoftReferences(const UObject* Root, TArray<FSoftObjectPath>& OutPaths)
{
	if (!Root)
	{
		return;
	}
	// Reflection walk: any TSoftObjectPtr added to any DataAsset later is picked up for free.
	for (TPropertyValueIterator<FSoftObjectProperty> It(Root->GetClass(), Root); It; ++It)
	{
		const FSoftObjectPtr& Soft = It.Key()->GetPropertyValue(It.Value());
		if (!Soft.IsNull())
		{
			OutPaths.AddUnique(Soft.ToSoftObjectPath());
		}
	}
}

TArray<FSoftObjectPath> UWDPreloadSubsystem::CollectStageAssets(const UWDStageData* Stage, const TArray<UObject*>& ExtraRoots)
{
	TArray<FSoftObjectPath> Paths;
	if (Stage)
	{
		CollectSoftReferences(Stage, Paths);
		for (const FWDWaveDef& Wave : Stage->Waves)
		{
			for (const FWDWaveEntry& Entry : Wave.Entries)
			{
				CollectSoftReferences(Entry.Monster, Paths);
			}
		}
	}
	for (const UObject* Root : ExtraRoots)
	{
		CollectSoftReferences(Root, Paths);
	}
	return Paths;
}

void UWDPreloadSubsystem::PreloadStage(const UWDStageData* Stage, const TArray<UObject*>& ExtraRoots, UWorld* World)
{
	if (!World)
	{
		return;
	}
	ReleaseBundle();

	PreloadWorld = World;
	Elapsed = 0.f;
	bPreloading = true;

	const TArray<FSoftObjectPath> Paths = CollectStageAssets(Stage, ExtraRoots);
	if (Paths.Num() > 0)
	{
		BundleHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(Paths);
	}

	OnPreloadProgress.Broadcast(0.f);
	World->GetTimerManager().SetTimer(ProgressTimer, this, &UWDPreloadSubsystem::TickProgress, ProgressTickInterval, /*bLoop=*/true);
}

void UWDPreloadSubsystem::TickProgress()
{
	Elapsed += ProgressTickInterval;

	const float LoadProgress = BundleHandle.IsValid() ? BundleHandle->GetProgress() : 1.f;
	const float TimeProgress = MinimumDuration > 0.f ? FMath::Min(Elapsed / MinimumDuration, 1.f) : 1.f;
	const float Progress = FMath::Min(LoadProgress, TimeProgress);
	OnPreloadProgress.Broadcast(Progress);

	if (Progress < 1.f)
	{
		return;
	}

	if (UWorld* World = PreloadWorld.Get())
	{
		World->GetTimerManager().ClearTimer(ProgressTimer);
		// Warm the pools: no actor spawning at fire time once the fight starts.
		if (UWDProjectilePoolSubsystem* Pool = World->GetSubsystem<UWDProjectilePoolSubsystem>())
		{
			Pool->Warmup(PoolWarmupCount);
		}
	}

	bPreloading = false;
	OnPreloadFinished.Broadcast();
}

void UWDPreloadSubsystem::ReleaseBundle()
{
	if (UWorld* World = PreloadWorld.Get())
	{
		World->GetTimerManager().ClearTimer(ProgressTimer);
	}
	if (BundleHandle.IsValid())
	{
		BundleHandle->ReleaseHandle();
		BundleHandle.Reset();
	}
	bPreloading = false;
}
