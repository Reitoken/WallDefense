#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WDWallData.generated.h"

class UStaticMesh;

/** One-shot conditional wall skills (GDD §3.2) — unlocked by wall level, armed once per stage. */
UENUM(BlueprintType)
enum class EWDWallSkill : uint8
{
	StartingShield UMETA(DisplayName = "Bouclier de départ"),
	RepulsionWave  UMETA(DisplayName = "Onde de répulsion"),
	AutoRepair     UMETA(DisplayName = "Auto-réparation"),
};

USTRUCT(BlueprintType)
struct FWDWallSkillDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	EWDWallSkill Type = EWDWallSkill::StartingShield;

	/** Wall level required for this skill to be armed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "1"))
	int32 UnlockLevel = 1;

	/** StartingShield / AutoRepair: fraction of max health. RepulsionWave: push-back distance (uu). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float Value = 0.25f;

	/** AutoRepair: fires below this health fraction. RepulsionWave: attacker-to-wall contact distance (uu). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float TriggerThreshold = 0.25f;

	/** RepulsionWave only: monsters within this range of the wall get pushed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "100.0"))
	float Radius = 900.f;
};

USTRUCT(BlueprintType)
struct FWDWallLevelDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (ClampMin = "1.0"))
	float MaxHealth = 1000.f;

	/** Percent mitigation, same formula as everything: damage × 100 / (100 + Defense). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall", meta = (ClampMin = "0.0"))
	float Defense = 0.f;
};

/**
 * The wall's progression sheet (ArchitectureTechnique §4): HP/defense per level and the
 * conditional one-shot skills with their unlock levels. Upgrade costs arrive with the
 * Progression step. MakeDebugWall() = playable values when no asset is assigned.
 */
UCLASS(BlueprintType)
class WALLDEFENSE_API UWDWallData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Index 0 = wall level 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall")
	TArray<FWDWallLevelDef> Levels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall")
	TArray<FWDWallSkillDef> Skills;

	/** Optional visual; absent = debug grey block (debug-first §5). Preloaded before the stage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall")
	TSoftObjectPtr<UStaticMesh> WallMesh;

	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	int32 GetMaxLevel() const { return Levels.Num(); }

	/** Clamped to the available levels; safe defaults when the asset has none. */
	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	FWDWallLevelDef GetLevelDef(int32 Level) const;

	/** The skills armed at this wall level. */
	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	TArray<FWDWallSkillDef> GetUnlockedSkills(int32 Level) const;

	/** Debug wall: 4 levels, the 3 skills staggered across them (values 🔶 to balance). */
	static UWDWallData* MakeDebugWall(UObject* Outer);
};
