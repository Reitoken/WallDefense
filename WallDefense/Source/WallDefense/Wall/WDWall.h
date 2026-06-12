#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/WDTypes.h"
#include "Wall/WDWallData.h"
#include "WDWall.generated.h"

class UStaticMeshComponent;
class UWDHealthComponent;
class UWDWallSkillsComponent;

/**
 * The objective to defend (GDD §3.2): ONE global HP bar, percent defense, one-shot skills.
 * The actor only assembles and wires its components (ArchitectureTechnique §6.2) — shield
 * and repair skills apply here; the repulsion push is applied by the GameMode (the assembler),
 * because the wall knows nothing about monsters. Debug body: wide grey block that reddens
 * as it suffers.
 */
UCLASS(Blueprintable)
class WALLDEFENSE_API AWDWall : public AActor
{
	GENERATED_BODY()

public:
	AWDWall();

	virtual void Tick(float DeltaSeconds) override;

	/** Applies the level stats and arms the skills unlocked at this level. */
	void InitFromData(UWDWallData* Data, int32 Level);

	/** Stage begins (waves start): the starting-shield skill fires here. */
	UFUNCTION(BlueprintCallable, Category = "WD|Wall")
	void HandleStageStarted();

	UFUNCTION(BlueprintPure, Category = "WD|Wall")
	float GetHealthPercent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Wall")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Wall")
	TObjectPtr<UWDHealthComponent> Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WD|Wall")
	TObjectPtr<UWDWallSkillsComponent> Skills;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDamaged(const FWDDamageEvent& DamageEvent, float AppliedDamage, EWDElementalMatch Match);

	UFUNCTION()
	void HandleSkillTriggered(const FWDWallSkillDef& Skill);

	UFUNCTION()
	void HandleDied(AActor* Killer);

private:
	void RefreshTint();

	UPROPERTY(Transient)
	TObjectPtr<UWDWallData> WallData;

	int32 WallLevel = 1;
};
