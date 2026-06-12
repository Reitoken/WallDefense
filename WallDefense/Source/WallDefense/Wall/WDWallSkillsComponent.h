#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Wall/WDWallData.h"
#include "WDWallSkillsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWDOnWallSkillTriggered, const FWDWallSkillDef&, Skill);

/**
 * The wall's one-shot conditional skills (GDD §3.2): each armed skill fires ONCE per stage
 * when its condition is met. Pure decision logic — the owner pushes state in
 * (NotifyStageStarted / NotifyWallDamaged) and applies the effects on OnSkillTriggered.
 * Knows no other component (ArchitectureTechnique §10), only its DataAsset types.
 */
UCLASS(ClassGroup = (Wall), meta = (BlueprintSpawnableComponent))
class WALLDEFENSE_API UWDWallSkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Arms the skills unlocked at the current wall level. Resets any previous state. */
	UFUNCTION(BlueprintCallable, Category = "WD|Wall")
	void Configure(const TArray<FWDWallSkillDef>& UnlockedSkills);

	/** Stage begins: StartingShield fires here. */
	UFUNCTION(BlueprintCallable, Category = "WD|Wall")
	void NotifyStageStarted();

	/** Wall took a hit: AutoRepair (health fraction) and RepulsionWave (attacker distance) check here. */
	UFUNCTION(BlueprintCallable, Category = "WD|Wall")
	void NotifyWallDamaged(float HealthPercent, float AttackerDistance);

	/** Armed = unlocked and not consumed yet. */
	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	bool IsArmed(EWDWallSkill Type) const;

	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	bool HasTriggered(EWDWallSkill Type) const;

	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	int32 GetArmedCount() const;

	UPROPERTY(BlueprintAssignable, Category = "WD|Wall")
	FWDOnWallSkillTriggered OnSkillTriggered;

private:
	void Trigger(int32 Index);

	TArray<FWDWallSkillDef> Armed;
	TArray<bool> Consumed;
};
