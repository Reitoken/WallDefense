#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapons/WDWeaponData.h"
#include "WDProjectile.generated.h"

/**
 * THE generic projectile: behaviors (pierce, bounce, fragments, chain, homing) are data,
 * not subclasses. No collision component — manual sphere sweeps each tick (deterministic,
 * pool-friendly). Debug rendering is the reference visual (step 3); Niagara plugs in later.
 */
UCLASS()
class WALLDEFENSE_API AWDProjectile : public AActor
{
	GENERATED_BODY()

public:
	AWDProjectile();

	virtual void Tick(float DeltaSeconds) override;

	/** Arms and launches the projectile from the pool. */
	void Launch(const FWDShotParams& InParams, const FVector& Start, const FVector& Direction);

	bool IsInFlight() const { return bInFlight; }

private:
	void Consume(const FVector& Location, bool bSpawnFragments);
	void ApplyHit(AActor* Victim, const FVector& ImpactPoint);
	void ApplyChain(AActor* FirstVictim);

	FWDShotParams Params;
	FVector Velocity = FVector::ZeroVector;
	float TraveledDistance = 0.f;
	int32 PierceLeft = 0;
	int32 BounceLeft = 0;
	bool bInFlight = false;

	/** Already-damaged actors (piercing must not re-hit). */
	TSet<TObjectPtr<AActor>> HitActors;
};
