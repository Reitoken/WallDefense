#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Weapons/WDWeaponData.h"
#include "WDProjectilePoolSubsystem.generated.h"

class AWDProjectile;

/**
 * Pool of generic projectiles, world-scoped. Anything can fire a shot through it
 * (weapons, fragments, later monster shooters) — no actor spawning at fire time after warmup.
 */
UCLASS()
class WALLDEFENSE_API UWDProjectilePoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Fires one shot. Acquires from the pool (or spawns when empty). */
	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	AWDProjectile* FireShot(const FWDShotParams& Params, const FVector& Start, const FVector& Direction);

	/** Returns a projectile to the free list. */
	void Release(AWDProjectile* Projectile);

	/** Pre-spawns projectiles (preload step, before a stage). */
	UFUNCTION(BlueprintCallable, Category = "WD|Weapons")
	void Warmup(int32 Count);

	UFUNCTION(BlueprintPure, Category = "WD|Weapons")
	int32 GetFreeCount() const { return FreeProjectiles.Num(); }

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AWDProjectile>> FreeProjectiles;
};
