#include "MonsterSpawnZone.h"

#include "BaseMonster.h"
#include "Combat/MonsterMovementZone.h"
#include "Combat/Target.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Lanes/LaneGrid.h"
#include "TimerManager.h"

AMonsterSpawnZone::AMonsterSpawnZone()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	SetRootComponent(SpawnBox);
	SpawnBox->SetBoxExtent(FVector(200.f, 200.f, 50.f));
	SpawnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnBox->SetHiddenInGame(true);
}

void AMonsterSpawnZone::BeginPlay()
{
	Super::BeginPlay();

	if (!Target)
	{
		for (TActorIterator<ATarget> It(GetWorld()); It; ++It)
		{
			Target = *It;
			UE_LOG(LogTemp, Log, TEXT("[%s] Auto-bound Target: %s"), *GetName(), *Target->GetName());
			break;
		}
	}
	if (!MovementZone)
	{
		for (TActorIterator<AMonsterMovementZone> It(GetWorld()); It; ++It)
		{
			MovementZone = *It;
			UE_LOG(LogTemp, Log, TEXT("[%s] Auto-bound MovementZone: %s"), *GetName(), *MovementZone->GetName());
			break;
		}
	}
	if (!LaneGrid)
	{
		for (TActorIterator<ALaneGrid> It(GetWorld()); It; ++It)
		{
			LaneGrid = *It;
			UE_LOG(LogTemp, Log, TEXT("[%s] Auto-bound LaneGrid: %s"), *GetName(), *LaneGrid->GetName());
			break;
		}
	}

	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No Target assigned and none found in level — monsters will spawn but won't move/attack."), *GetName());
	}
	if (!MovementZone)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] No MovementZone assigned — monsters will roam without bounds."), *GetName());
	}

	if (bAutoStart)
	{
		StartSpawning();
	}
}

void AMonsterSpawnZone::StartSpawning()
{
	if (SpawnEntries.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] MonsterSpawnZone has no entries — nothing to spawn."), *GetName());
		return;
	}

	CurrentEntryIndex = 0;
	SpawnedInCurrentEntry = 0;
	ScheduleNext(SpawnEntries[0].WaveStartDelay);
}

void AMonsterSpawnZone::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void AMonsterSpawnZone::ScheduleNext(float Delay)
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterSpawnZone::SpawnNext, FMath::Max(0.01f, Delay), false);
}

void AMonsterSpawnZone::SpawnNext()
{
	if (!SpawnEntries.IsValidIndex(CurrentEntryIndex))
	{
		if (bLoopWaves && SpawnEntries.Num() > 0)
		{
			CurrentEntryIndex = 0;
			SpawnedInCurrentEntry = 0;
			ScheduleNext(SpawnEntries[0].WaveStartDelay);
		}
		else
		{
			OnAllWavesComplete.Broadcast();
		}
		return;
	}

	const FMonsterSpawnEntry& Entry = SpawnEntries[CurrentEntryIndex];

	if (Entry.MonsterClass)
	{
		SpawnOne(Entry.MonsterClass);
	}
	SpawnedInCurrentEntry++;

	if (SpawnedInCurrentEntry >= Entry.Count)
	{
		CurrentEntryIndex++;
		SpawnedInCurrentEntry = 0;
		if (SpawnEntries.IsValidIndex(CurrentEntryIndex))
		{
			ScheduleNext(SpawnEntries[CurrentEntryIndex].WaveStartDelay);
		}
		else if (bLoopWaves && SpawnEntries.Num() > 0)
		{
			CurrentEntryIndex = 0;
			ScheduleNext(SpawnEntries[0].WaveStartDelay);
		}
		else
		{
			OnAllWavesComplete.Broadcast();
		}
	}
	else
	{
		ScheduleNext(Entry.SpawnInterval);
	}
}

ABaseMonster* AMonsterSpawnZone::SpawnOne(TSubclassOf<ABaseMonster> MonsterClass)
{
	if (!MonsterClass)
	{
		return nullptr;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// Choose lane + spawn location.
	int32 ChosenLane = INDEX_NONE;
	FVector SpawnLoc;
	FRotator SpawnRot = GetActorRotation();

	if (LaneGrid)
	{
		const int32 NumLanes = LaneGrid->GetNumLanes();
		ChosenLane = bRandomLane ? FMath::RandRange(0, NumLanes - 1) : FMath::Clamp(FixedLaneIndex, 0, NumLanes - 1);

		// Spawn at the lane end farthest from the target so monsters travel down the lane toward it.
		const FVector Forward = LaneGrid->GetLaneForwardVector();
		const FVector LaneCenter = LaneGrid->GetLaneCenterWorld(ChosenLane);
		const float HalfLen = LaneGrid->GetLaneLength() * 0.5f;

		float TargetAlong = 0.f;
		if (Target)
		{
			TargetAlong = FVector::DotProduct(Target->GetActorLocation() - LaneCenter, Forward);
		}
		// Far end = opposite sign of the target's along position.
		const float SpawnAlong = (TargetAlong >= 0.f) ? -HalfLen : HalfLen;

		SpawnLoc = LaneGrid->GetLanePointWorld(ChosenLane, SpawnAlong);
		SpawnLoc.Z = GetActorLocation().Z;
		// Face down the lane toward the target.
		SpawnRot = ((TargetAlong >= 0.f) ? Forward : -Forward).Rotation();
	}
	else
	{
		SpawnLoc = GetRandomSpawnLocation();
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseMonster* Monster = World->SpawnActor<ABaseMonster>(MonsterClass, SpawnLoc, SpawnRot, Params);
	if (!Monster)
	{
		return nullptr;
	}

	if (Target)
	{
		Monster->SetTarget(Target);
	}
	if (MovementZone)
	{
		Monster->SetMovementZone(MovementZone);
	}
	if (LaneGrid && ChosenLane != INDEX_NONE)
	{
		Monster->SetLane(LaneGrid, ChosenLane);
	}

	OnMonsterSpawned.Broadcast(Monster);
	return Monster;
}

FVector AMonsterSpawnZone::GetRandomSpawnLocation() const
{
	if (!bSpawnAtRandomPoint || !SpawnBox)
	{
		return GetActorLocation();
	}

	const FVector Extent = SpawnBox->GetScaledBoxExtent();
	const FVector Center = SpawnBox->GetComponentLocation();
	return FVector(
		FMath::FRandRange(Center.X - Extent.X, Center.X + Extent.X),
		FMath::FRandRange(Center.Y - Extent.Y, Center.Y + Extent.Y),
		Center.Z);
}
