#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawnZone.generated.h"

class UBoxComponent;
class ABaseMonster;
class AMonsterMovementZone;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterSpawned, ABaseMonster*, Monster);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesComplete);

USTRUCT(BlueprintType)
struct FMonsterSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ABaseMonster> MonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "1"))
	int32 Count = 5;

	/** Seconds between each spawn of this entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0.0"))
	float SpawnInterval = 0.5f;

	/** Seconds to wait before starting this entry (after previous entry completed). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn", meta = (ClampMin = "0.0"))
	float WaveStartDelay = 0.f;
};

UCLASS(Blueprintable)
class WALLDEFENSE_API AMonsterSpawnZone : public AActor
{
	GENERATED_BODY()

public:
	AMonsterSpawnZone();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	ABaseMonster* SpawnOne(TSubclassOf<ABaseMonster> MonsterClass);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn|Components")
	TObjectPtr<UBoxComponent> SpawnBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Waves")
	TArray<FMonsterSpawnEntry> SpawnEntries;

	/** Assign on the level instance (not the BP defaults). Left empty = auto-find the first ATarget in the world at BeginPlay. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Spawn|Wiring")
	TObjectPtr<AActor> Target;

	/** Assign on the level instance (not the BP defaults). Left empty = auto-find the first AMonsterMovementZone in the world at BeginPlay. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Spawn|Wiring")
	TObjectPtr<AMonsterMovementZone> MovementZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Behavior")
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Behavior")
	bool bLoopWaves = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Behavior")
	bool bSpawnAtRandomPoint = true;

	UPROPERTY(BlueprintAssignable, Category = "Spawn|Events")
	FOnMonsterSpawned OnMonsterSpawned;

	UPROPERTY(BlueprintAssignable, Category = "Spawn|Events")
	FOnAllWavesComplete OnAllWavesComplete;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnNext();

	void ScheduleNext(float Delay);
	FVector GetRandomSpawnLocation() const;

	int32 CurrentEntryIndex = 0;
	int32 SpawnedInCurrentEntry = 0;

	FTimerHandle SpawnTimerHandle;
};
