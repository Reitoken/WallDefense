#include "Stage/WDStageDirector.h"
#include "Stage/WDStageData.h"
#include "Monsters/WDMonster.h"
#include "Monsters/WDMonsterData.h"
#include "Core/WDDebugSubsystem.h"

AWDStageDirector::AWDStageDirector()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AWDStageDirector::Configure(UWDStageData* InStage, AActor* InTarget, const FVector& InSpawnCenter, float InSpawnHalfWidth)
{
	Stage = InStage;
	Target = InTarget;
	SpawnCenter = InSpawnCenter;
	SpawnHalfWidth = InSpawnHalfWidth;
}

void AWDStageDirector::StartStage()
{
	if (!Stage || Stage->Waves.IsEmpty() || !Target.IsValid())
	{
		return;
	}
	CurrentWaveIndex = -1;
	AliveMonsters.Reset();
	bAllWavesSpawned = false;
	bRunning = true;
	StartNextWave();
}

void AWDStageDirector::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bRunning)
	{
		return;
	}

	if (!bAllWavesSpawned)
	{
		NextWaveCountdown -= DeltaSeconds;
		// Early start: field cleared OR timer elapsed (never both waiting).
		if (NextWaveCountdown <= 0.f || AliveMonsters.IsEmpty())
		{
			StartNextWave();
		}
	}
	else if (AliveMonsters.IsEmpty())
	{
		CompleteStage();
	}

	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		FString Status = FString::Printf(TEXT("Vague %d/%d — %d ennemis"), CurrentWaveIndex + 1, Stage->Waves.Num(), AliveMonsters.Num());
		if (!bAllWavesSpawned)
		{
			Status += FString::Printf(TEXT(" — prochaine : %.0fs"), FMath::Max(0.f, NextWaveCountdown));
		}
		Debug->DrawText(Target->GetActorLocation() + FVector(0, 0, 320.f), Status, EWDElement::Normal, 0.f);
	}
}

void AWDStageDirector::StartNextWave()
{
	++CurrentWaveIndex;
	if (!Stage->Waves.IsValidIndex(CurrentWaveIndex))
	{
		bAllWavesSpawned = true;
		return;
	}

	const FWDWaveDef& Wave = Stage->Waves[CurrentWaveIndex];
	NextWaveCountdown = Wave.TimeBeforeNextWave;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (const FWDWaveEntry& Entry : Wave.Entries)
	{
		if (!Entry.Monster)
		{
			continue;
		}
		for (int32 i = 0; i < Entry.Count; ++i)
		{
			const FVector SpawnLocation = SpawnCenter + FVector(FMath::FRandRange(0.f, 400.f), FMath::FRandRange(-SpawnHalfWidth, SpawnHalfWidth), 90.f);
			AWDMonster* Monster = GetWorld()->SpawnActor<AWDMonster>(SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			if (Monster)
			{
				Monster->InitFromData(Entry.Monster, Stage->BaseHealth, Stage->BaseWallDamage, Target.Get());
				Monster->OnKilled.AddDynamic(this, &AWDStageDirector::HandleMonsterKilled);
				AliveMonsters.Add(Monster);
			}
		}
	}

	if (bAllWavesSpawned == false && Stage->Waves.IsValidIndex(CurrentWaveIndex))
	{
		bAllWavesSpawned = (CurrentWaveIndex == Stage->Waves.Num() - 1);
	}

	OnWaveStarted.Broadcast(CurrentWaveIndex + 1, Stage->Waves.Num());
}

void AWDStageDirector::HandleMonsterKilled(AWDMonster* Monster)
{
	AliveMonsters.Remove(Monster);
}

void AWDStageDirector::CompleteStage()
{
	bRunning = false;
	OnStageCompleted.Broadcast();
	if (UWDDebugSubsystem* Debug = GetWorld()->GetSubsystem<UWDDebugSubsystem>())
	{
		Debug->DrawText(Target->GetActorLocation() + FVector(0, 0, 400.f), TEXT("STAGE TERMINÉ !"), EWDElement::Light, 8.f);
	}
}
